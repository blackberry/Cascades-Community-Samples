/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "applicationui.hpp"
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>
#include <QSettings>
#include <btapi/btgatt.h>
#include <btapi/btle.h>
#include <unistd.h>

static ApplicationUI *appInstance = 0;
static AbstractPane *_root;

static const QString HEX_CHARS = QString("0123456789ABCDEF");

// SensorTag service UUID
static const char* SENSORTAG_IBEACON_SERVICE_UUID = "0xF000BCC0-0451-4000-B000-000000000000";

// Characteristic UUIDs
static const char* SENSORTAG_BEACON_UUID_UUID = "0xF000BCC1-0451-4000-B000-000000000000";
static const char* SENSORTAG_BEACON_MAJOR_UUID = "0xF000BCC2-0451-4000-B000-000000000000";
static const char* SENSORTAG_BEACON_MINOR_UUID = "0xF000BCC3-0451-4000-B000-000000000000";
static const char* SENSORTAG_BEACON_POWER_UUID = "0xF000BCC4-0451-4000-B000-000000000000";
static const char* SENSORTAG_BEACON_INTERVAL_UUID = "0xF000BCC5-0451-4000-B000-000000000000";
static const char* SENSORTAG_BEACON_PIN_UUID = "0xF000BCC6-0451-4000-B000-000000000000";

uint16_t _beacon_uuid_handle;
uint16_t _beacon_uuid_value_handle;
QString _beacon_uuid;
QString _new_beacon_uuid;

uint16_t _beacon_major_handle;
uint16_t _beacon_major_value_handle;
uint16_t _beacon_major;
uint16_t _new_beacon_major;

uint16_t _beacon_minor_handle;
uint16_t _beacon_minor_value_handle;
uint16_t _beacon_minor;
uint16_t _new_beacon_minor;

uint16_t _beacon_power_handle;
uint16_t _beacon_power_value_handle;
uint8_t _beacon_power;
uint8_t _new_beacon_power;

uint16_t _beacon_interval_handle;
uint16_t _beacon_interval_value_handle;
uint16_t _beacon_interval;
uint16_t _new_beacon_interval;

uint16_t _beacon_pin_handle;
uint16_t _beacon_pin_value_handle;
uint32_t _beacon_pin;
uint32_t _new_beacon_pin;

static const char *_service;
static const char *_bdaddr;
static int _service_instance;

QString _mac;

bool _beacon_attributes_available;

bool _connect_to_write;

static void btEvent(const int event, const char *btAddr, const char *eventData);

/*
 * Don't use deprecated API if we're building for >= 10.3.0
 */
#if BBNDK_VERSION_AT_LEAST(10,3,0)
static void btLeAdvertisementCbExt(const char *bdaddr, int8_t rssi, bt_le_advert_packet_event_t eventType, const char *data, int len, void *userData);
#else
static void btLeAdvertisementCb(const char *bdaddr, int8_t rssi, const char *data, int len, void *userData);
#endif

static bool readBeaconAttributes(int service_instance);
static bool writeBeaconAttributes(int service_instance);
static bool establishHandles(int service_instance);
static void alert(QString msg);
static uint8_t hexToInt(QChar c1, QChar c2);
static void hexToIntArray(QString hex_string, uint8_t* output);

uint8_t hexToInt(QChar c1, QChar c2) {
    c1 = c1.toUpper();
    c2 = c2.toUpper();
    uint8_t result = 0;
    result = HEX_CHARS.indexOf(c1) * 16;
    result = result + HEX_CHARS.indexOf(c2);
    return result;
}

void hexToIntArray(QString hex_string, uint8_t* output) {
    const size_t numdigits = hex_string.length() / 2;
    for (size_t i = 0; i != numdigits; ++i) {
        output[i] = hexToInt(hex_string.at(2 * i), hex_string.at(2 * i + 1));
    }
}

static void alert(QString msg)
{
    bb::system::SystemDialog *dial;
    dial = new bb::system::SystemDialog("OK", NULL);
    dial->setTitle("Alert");
    dial->setBody(msg);
    dial->setDismissAutomatically(true);
    dial->show();
}

void gatt_service_connected(const char *bdaddr, const char *service, int instance, int err, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{

    Q_UNUSED(superTimeout)
    Q_UNUSED(userData)
    Q_UNUSED(latency)
    Q_UNUSED(connInt)
    Q_UNUSED(err)

    qDebug() << "BBBB gatt_service_connected. instance=" << instance << " service=" << service << " err=" << err;

    if (err != 0) {
        alert(QString("Error connecting to GATT service:%1").arg(err));
        return;
    }

    _mac = QString(bdaddr);
    _service = service;
    _bdaddr = bdaddr;
    _service_instance = instance;

    if (!_connect_to_write) {
        _beacon_attributes_available = false;
        _beacon_attributes_available = readBeaconAttributes(instance);
    } else {
        writeBeaconAttributes(instance);
    }

    qDebug() << "BBBB disconnecting from sensortag iBeacon service";
    if (bt_gatt_disconnect_service(_bdaddr, SENSORTAG_IBEACON_SERVICE_UUID) < 0) {
        qDebug() << "BBBB GATT disconnect service request failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
    } else {
        qDebug() << "BBBB disconnected from SensorTag iBeacon service OK";
    }


}

void gatt_service_disconnected(const char *bdaddr, const char *service, int instance, int reason, void *userData)
{

    Q_UNUSED(userData)
    Q_UNUSED(instance)
    Q_UNUSED(reason)

    QString bdaddr_str = QString(bdaddr);
    QString service_str = QString(service);
    qDebug() << "BBBB gatt_service_disconnected";
}

void gatt_service_updated(const char *bdaddr, int instance, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{

    Q_UNUSED(superTimeout)
    Q_UNUSED(userData)
    Q_UNUSED(latency)
    Q_UNUSED(connInt)
    Q_UNUSED(instance)

    QString bdaddr_str = QString(bdaddr);
    qDebug() << QString("BBBB gatt_service_updated: bdaddr:%1").arg(bdaddr_str);
}

bt_gatt_callbacks_t gatt_callbacks = { gatt_service_connected, gatt_service_disconnected, gatt_service_updated };

bool readBeaconAttributes(int service_instance)
{

    if (!establishHandles(service_instance)) {
        return false;
    }

    bool result = false;

    // Read beacon UUID
    if (_beacon_uuid_value_handle != 0) {
        qDebug() << "BBBB reading beacon UUID";
        int uuid_max_len = 80;
        uint8_t *uuid_bytes;
        uuid_bytes = (uint8_t*) calloc(uuid_max_len, sizeof(uint8_t));
        errno = 0;
        int result = bt_gatt_read_value(service_instance, _beacon_uuid_value_handle, 0, uuid_bytes, uuid_max_len, 0);
        if (result < 0) {
            qDebug() << "BBBB bt_gatt_read_value beacon UUID - errno=(" << errno << ") :" << strerror(errno);
            alert(QString("Error reading beacon UUID: %1 %2").arg(errno).arg(strerror(errno)));
        } else {
            char * beacon_uuid = (char*) uuid_bytes;
            _beacon_uuid = QByteArray(beacon_uuid).toHex();
            qDebug() << "BBBB UUID: " << _beacon_uuid;
        }
        free(uuid_bytes);
        result = true;
    } else {
        qDebug() << "BBBB Beacon UUID:not present";
        _beacon_uuid = "";
    }

    // Read beacon major attribute

    if (_beacon_major_value_handle != 0) {
        qDebug() << "BBBB reading beacon MAJOR";
        int major_max_len = 2;
        uint8_t *major_bytes;
        major_bytes = (uint8_t*) calloc(major_max_len, sizeof(uint8_t));
        errno = 0;
        int result = bt_gatt_read_value(service_instance, _beacon_major_value_handle, 0, major_bytes, major_max_len, 0);
        if (result < 0) {
            qDebug() << "BBBB bt_gatt_read_value MAJOR - errno=(" << errno << ") :" << strerror(errno);
            alert(QString("Error reading beacon MAJOR: %1 %2").arg(errno).arg(strerror(errno)));
        } else {
            _beacon_major = (major_bytes[0] << 8) + (major_bytes[1]);
            qDebug() << "BBBB MAJOR: " << _beacon_major;
        }
        free(major_bytes);
        result = true;
    } else {
        qDebug() << "BBBB Beacon MAJOR:not present";
        _beacon_major = 0;
    }

    // Read beacon minor attribute

    if (_beacon_minor_value_handle != 0) {
        qDebug() << "BBBB reading beacon MINOR";
        int minor_max_len = 2;
        uint8_t *minor_bytes;
        minor_bytes = (uint8_t*) calloc(minor_max_len, sizeof(uint8_t));
        errno = 0;
        int result = bt_gatt_read_value(service_instance, _beacon_minor_value_handle, 0, minor_bytes, minor_max_len, 0);
        if (result < 0) {
            qDebug() << "BBBB bt_gatt_read_value MINOR - errno=(" << errno << ") :" << strerror(errno);
            alert(QString("Error reading beacon MINOR: %1 %2").arg(errno).arg(strerror(errno)));
        } else {
            _beacon_minor = (minor_bytes[0] << 8) + (minor_bytes[1]);
            qDebug() << "BBBB MINOR: " << _beacon_minor;
        }
        free(minor_bytes);
        result = true;
    } else {
        qDebug() << "BBBB Beacon MINOR:not present";
        _beacon_minor = 0;
    }

    // Read beacon power attribute

    if (_beacon_power_value_handle != 0) {
        qDebug() << "BBBB reading beacon POWER";
        int power_max_len = 1;
        uint8_t *power_bytes;
        power_bytes = (uint8_t*) calloc(power_max_len, sizeof(uint8_t));
        errno = 0;
        int result = bt_gatt_read_value(service_instance, _beacon_power_value_handle, 0, power_bytes, power_max_len, 0);
        if (result < 0) {
            qDebug() << "BBBB bt_gatt_read_value POWER - errno=(" << errno << ") :" << strerror(errno);
            alert(QString("Error reading beacon POWER: %1 %2").arg(errno).arg(strerror(errno)));
        } else {
            _beacon_power = power_bytes[0];
            qDebug() << "BBBB POWER: " << _beacon_power;
        }
        free(power_bytes);
        result = true;
    } else {
        qDebug() << "BBBB Beacon POWER:not present";
        _beacon_power = 0;
    }

    // Read beacon interval attribute

    if (_beacon_interval_value_handle != 0) {
        qDebug() << "BBBB reading beacon INTERVAL";
        int interval_max_len = 2;
        uint8_t *interval_bytes;
        interval_bytes = (uint8_t*) calloc(interval_max_len, sizeof(uint8_t));
        errno = 0;
        int result = bt_gatt_read_value(service_instance, _beacon_interval_value_handle, 0, interval_bytes, interval_max_len, 0);
        if (result < 0) {
            qDebug() << "BBBB bt_gatt_read_value INTERVAL - errno=(" << errno << ") :" << strerror(errno);
            alert(QString("Error reading beacon INTERVAL: %1 %2").arg(errno).arg(strerror(errno)));
        } else {
            _beacon_interval = (interval_bytes[0] << 8) + (interval_bytes[1]);
            qDebug() << "BBBB INTERVAL: " << _beacon_interval;
        }
        free(interval_bytes);
        result = true;
    } else {
        qDebug() << "BBBB Beacon INTERVAL:not present";
        _beacon_interval = 0;
    }

    // Read beacon PIN attribute

    if (_beacon_pin_value_handle != 0) {
        qDebug() << "BBBB reading beacon PIN";
        int pin_max_len = 4;
        uint8_t *pin_bytes;
        pin_bytes = (uint8_t*) calloc(pin_max_len, sizeof(uint8_t));
        errno = 0;
        int result = bt_gatt_read_value(service_instance, _beacon_pin_value_handle, 0, pin_bytes, pin_max_len, 0);
        if (result < 0) {
            qDebug() << "BBBB bt_gatt_read_value PIN - errno=(" << errno << ") :" << strerror(errno);
            alert(QString("Error reading beacon PIN: %1 %2").arg(errno).arg(strerror(errno)));
        } else {
            _beacon_pin = (pin_bytes[0] << 8) + (pin_bytes[1]);
            qDebug() << "BBBB PIN: " << _beacon_pin;
        }
        free(pin_bytes);
        result = true;
    } else {
        qDebug() << "BBBB Beacon PIN:not present";
        _beacon_pin = 0;
    }

    appInstance->emitOpenConfig(_mac);

    return result;
}

bool writeBeaconAttributes(int service_instance)
{
    qDebug() << "BBBB writeBeaconAttributes: instance=" << service_instance;

    qDebug() << "XXXX writeBeaconAttributes - new UUID=" << _new_beacon_uuid << " vs current UUID=" << _beacon_uuid;
    if (_beacon_uuid.compare(_new_beacon_uuid) != 0) {
        qDebug() << "BBBB writing new UUID=" << _new_beacon_uuid.toAscii().constData() << " to handle " << _beacon_uuid_value_handle;
        uint8_t *uuid_bytes;
        int uuid_len = _new_beacon_uuid.length() / 2;
        uuid_bytes = (uint8_t*) calloc(uuid_len, sizeof(uint8_t));
        hexToIntArray(_new_beacon_uuid,uuid_bytes);
        if (bt_gatt_write_value(service_instance, _beacon_uuid_value_handle, 0, uuid_bytes , uuid_len) == EOK) {
            qDebug() << "XXXX writeBeaconAttributes - new UUID written successfully";
            _beacon_uuid = _new_beacon_uuid;
        } else {
            qDebug() << "XXXX writeBeaconAttributes failed to update UUID - errno=(" << errno<< ") :" << strerror(errno);
            alert(QString("failed to update UUID - errno=(%1) : %2").arg(errno).arg(strerror(errno)));
            return false;
        }
    } else {
        qDebug() << "XXXX writeBeaconAttributes - UUID has not been changed";
    }


    qDebug() << "XXXX writeBeaconAttributes - new major=" << _new_beacon_major << " vs current major=" << _beacon_major;
    if (_beacon_major != _new_beacon_major) {
        qDebug() << "BBBB writing new major=" << _new_beacon_major << " to handle " << _beacon_major_value_handle;
        int major_len = 2;
        uint8_t *major_bytes;
        major_bytes = (uint8_t*) calloc(major_len, sizeof(uint8_t));
        major_bytes[1] = _new_beacon_major & 0xFF;
        major_bytes[0] = (_new_beacon_major >> 8);
        qDebug() << "BBBB major_bytes[0]=" << major_bytes[0];
        qDebug() << "BBBB major_bytes[1]=" << major_bytes[1];
        if (bt_gatt_write_value(service_instance, _beacon_major_value_handle, 0, major_bytes , major_len) == EOK) {
            qDebug() << "XXXX writeBeaconAttributes - new MAJOR written successfully";
            _beacon_major = _new_beacon_major;
        } else {
            qDebug() << "XXXX writeBeaconAttributes failed to update MAJOR - errno=(" << errno<< ") :" << strerror(errno);
            alert(QString("failed to update MAJOR - errno=(%1) : %2").arg(errno).arg(strerror(errno)));
            return false;
        }
    } else {
        qDebug() << "XXXX writeBeaconAttributes - MAJOR has not been changed";
    }

    qDebug() << "XXXX writeBeaconAttributes - new minor=" << _new_beacon_minor << " vs current minor=" << _beacon_minor;
    if (_beacon_minor != _new_beacon_minor) {
        qDebug() << "BBBB writing new minor=" << _new_beacon_minor << " to handle " << _beacon_minor_value_handle;
        int minor_len = 2;
        uint8_t *minor_bytes;
        minor_bytes = (uint8_t*) calloc(minor_len, sizeof(uint8_t));
        minor_bytes[1] = _new_beacon_minor & 0xFF;
        minor_bytes[0] = (_new_beacon_minor >> 8);
        qDebug() << "BBBB minor_bytes[0]=" << minor_bytes[0];
        qDebug() << "BBBB minor_bytes[1]=" << minor_bytes[1];
        if (bt_gatt_write_value(service_instance, _beacon_minor_value_handle, 0, minor_bytes , minor_len) == EOK) {
            qDebug() << "XXXX writeBeaconAttributes - new MINOR written successfully";
            _beacon_minor = _new_beacon_minor;
        } else {
            qDebug() << "XXXX writeBeaconAttributes failed to update MINOR - errno=(" << errno<< ") :" << strerror(errno);
            alert(QString("failed to update MINOR - errno=(%1) : %2").arg(errno).arg(strerror(errno)));
            return false;
        }
    } else {
        qDebug() << "XXXX writeBeaconAttributes - MINOR has not been changed";
    }

    qDebug() << "XXXX writeBeaconAttributes - new power=" << _new_beacon_power << " vs current power=" << _beacon_power;
    if (_beacon_power != _new_beacon_power) {
        qDebug() << "BBBB writing new power=" << _new_beacon_power << " to handle " << _beacon_power_value_handle;
        int power_len = 1;
        uint8_t *power_bytes;
        power_bytes = (uint8_t*) calloc(power_len, sizeof(uint8_t));
        power_bytes[0] = _new_beacon_power & 0xFF;
        qDebug() << "BBBB power_bytes[0]=" << power_bytes[0];
        if (bt_gatt_write_value(service_instance, _beacon_power_value_handle, 0, power_bytes , power_len) == EOK) {
            qDebug() << "XXXX writeBeaconAttributes - new POWER written successfully";
            _beacon_power = _new_beacon_power;
        } else {
            qDebug() << "XXXX writeBeaconAttributes failed to update POWER - errno=(" << errno<< ") :" << strerror(errno);
            alert(QString("failed to update POWER - errno=(%1) : %2").arg(errno).arg(strerror(errno)));
            return false;
        }
    } else {
        qDebug() << "XXXX writeBeaconAttributes - POWER has not been changed";
    }

    qDebug() << "XXXX writeBeaconAttributes - new interval=" << _new_beacon_interval << " vs current interval=" << _beacon_interval;
    if (_beacon_interval != _new_beacon_interval) {
        qDebug() << "BBBB writing new interval=" << _new_beacon_interval << " to handle " << _beacon_interval_value_handle;
        int interval_len = 2;
        uint8_t *interval_bytes;
        interval_bytes = (uint8_t*) calloc(interval_len, sizeof(uint8_t));
        interval_bytes[1] = _new_beacon_interval & 0xFF;
        interval_bytes[0] = (_new_beacon_interval >> 8);
        qDebug() << "BBBB interval_bytes[0]=" << interval_bytes[0];
        qDebug() << "BBBB interval_bytes[1]=" << interval_bytes[1];
        if (bt_gatt_write_value(service_instance, _beacon_interval_value_handle, 0, interval_bytes , interval_len) == EOK) {
            qDebug() << "XXXX writeBeaconAttributes - new INTERVAL written successfully";
            _beacon_interval = _new_beacon_interval;
        } else {
            qDebug() << "XXXX writeBeaconAttributes failed to update INTERVAL - errno=(" << errno<< ") :" << strerror(errno);
            alert(QString("failed to update INTERVAL - errno=(%1) : %2").arg(errno).arg(strerror(errno)));
            return false;
        }
    } else {
        qDebug() << "XXXX writeBeaconAttributes - INTERVAL has not been changed";
    }

    qDebug() << "XXXX writeBeaconAttributes - new pin=" << _new_beacon_pin << " vs current pin=" << _beacon_pin;
    if (_beacon_pin != _new_beacon_pin) {
        qDebug() << "BBBB writing new pin=" << _new_beacon_pin << " to handle " << _beacon_pin_value_handle;
        int pin_len = 4;
        uint8_t *pin_bytes;
        pin_bytes = (uint8_t*) calloc(pin_len, sizeof(uint8_t));
        pin_bytes[3] = _new_beacon_pin & 0xFF;
        pin_bytes[2] = (_new_beacon_pin >> 8);
        pin_bytes[1] = (_new_beacon_pin >> 16);
        pin_bytes[0] = (_new_beacon_pin >> 24);
        qDebug() << "BBBB pin_bytes[0]=" << pin_bytes[0];
        qDebug() << "BBBB pin_bytes[1]=" << pin_bytes[1];
        qDebug() << "BBBB pin_bytes[2]=" << pin_bytes[2];
        qDebug() << "BBBB pin_bytes[3]=" << pin_bytes[3];
        if (bt_gatt_write_value(service_instance, _beacon_pin_value_handle, 0, pin_bytes , pin_len) == EOK) {
            qDebug() << "XXXX writeBeaconAttributes - new PIN written successfully";
            _beacon_pin = _new_beacon_pin;
        } else {
            qDebug() << "XXXX writeBeaconAttributes failed to update PIN - errno=(" << errno<< ") :" << strerror(errno);
            alert(QString("failed to update PIN - errno=(%1) : %2").arg(errno).arg(strerror(errno)));
            return false;
        }
    } else {
        qDebug() << "XXXX writeBeaconAttributes - PIN has not been changed";
    }

    return true;
}

bool establishHandles(int service_instance)
{
    errno = 0;
    int num_characteristics = bt_gatt_characteristics_count(service_instance);

    if (num_characteristics == -1) {
        qDebug() << "BBBB bt_gatt_characteristics_count failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
        alert("bt_gatt_characteristics_count failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")");
        return false;
    }

    if (num_characteristics > -1) {
        qDebug() << QString("BBBB # characteristics: %1").arg(num_characteristics);
        bt_gatt_characteristic_t *characteristicList;

        characteristicList = (bt_gatt_characteristic_t*) malloc(num_characteristics * sizeof(bt_gatt_characteristic_t));
        if (NULL == characteristicList) {
            qDebug() << QString("BBBB GATT characteristics: Not enough memory");
            alert("Error: not enough memory");
            bt_gatt_disconnect_instance(service_instance);
            return false;
        }

        /* BEGIN WORKAROUND - Temporary fix to address race condition */
        int number = 0;
        do {
            number = bt_gatt_characteristics(service_instance, characteristicList, num_characteristics);
        } while ((number == -1) && (errno == EBUSY));

        int characteristicListSize = number;

        for (int i = 0; i < characteristicListSize; i++) {
            qDebug() << "BBBB characteristic: uuid,handle,value_handle:" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
            if (strcmp(characteristicList[i].uuid, SENSORTAG_BEACON_UUID_UUID) == 0) {
                qDebug() << "BBBB found beacon UUID characteristic";
                _beacon_uuid_handle = characteristicList[i].handle;
                _beacon_uuid_value_handle = characteristicList[i].value_handle;
                continue;
            }
            if (strcmp(characteristicList[i].uuid, SENSORTAG_BEACON_MAJOR_UUID) == 0) {
                qDebug() << "BBBB found beacon MAJOR characteristic";
                _beacon_major_handle = characteristicList[i].handle;
                _beacon_major_value_handle = characteristicList[i].value_handle;
                continue;
            }
            if (strcmp(characteristicList[i].uuid, SENSORTAG_BEACON_MINOR_UUID) == 0) {
                qDebug() << "BBBB found beacon MINOR characteristic";
                _beacon_minor_handle = characteristicList[i].handle;
                _beacon_minor_value_handle = characteristicList[i].value_handle;
                continue;
            }
            if (strcmp(characteristicList[i].uuid, SENSORTAG_BEACON_POWER_UUID) == 0) {
                qDebug() << "BBBB found beacon POWER characteristic";
                _beacon_power_handle = characteristicList[i].handle;
                _beacon_power_value_handle = characteristicList[i].value_handle;
                continue;
            }
            if (strcmp(characteristicList[i].uuid, SENSORTAG_BEACON_INTERVAL_UUID) == 0) {
                qDebug() << "BBBB found beacon INTERVAL characteristic";
                _beacon_interval_handle = characteristicList[i].handle;
                _beacon_interval_value_handle = characteristicList[i].value_handle;
                continue;
            }
            if (strcmp(characteristicList[i].uuid, SENSORTAG_BEACON_PIN_UUID) == 0) {
                qDebug() << "BBBB found beacon PIN characteristic";
                _beacon_pin_handle = characteristicList[i].handle;
                _beacon_pin_value_handle = characteristicList[i].value_handle;
                continue;
            }
            qDebug() << "BBBB other characteristic: uuid,handle,value_handle=" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
        }

        if (characteristicList != NULL) {
            free(characteristicList);
            characteristicList = NULL;
        }

        /* END WORKAROUND */
    }

    return true;
}

/*
 * Don't use deprecated API if we're building for >= 10.3.0
 */
#if BBNDK_VERSION_AT_LEAST(10,3,0)
static void btLeAdvertisementCbExt(const char *bdaddr, int8_t rssi, bt_le_advert_packet_event_t eventType, const char *data, int len, void *userData)
{
    Q_UNUSED(eventType)
    Q_UNUSED(userData)

    if (appInstance) {
        appInstance->parseBeaconData(data, len, rssi, bdaddr);
    }
}
#else
static void btLeAdvertisementCb(const char *bdaddr, int8_t rssi, const char *data, int len, void *userData)
{
    Q_UNUSED(userData)

    if (appInstance) {
        appInstance->parseBeaconData(data, len, rssi, bdaddr);
    }
}
#endif

static void btEvent(const int event, const char *btAddr, const char *eventData)
{
    Q_UNUSED(btAddr)

    QString eventName;

    if (appInstance) {
        eventName = appInstance->btEventName(event);
        appInstance->emitMessageForUi(QString("BT event_name=%1").arg(eventName));
    }

    qDebug() << "BBBB BT event_name=" << eventName << endl;

    if (eventData != NULL) {
        QString eventDataStr = QString(*eventData);
        qDebug() << "BBBB BT event_data=" << eventDataStr;
    } else {
        qDebug() << "BBBB BT event_data=NULL";
    }
}

/*
 * Don't use deprecated API if we're building for >= 10.3.0
 */
#if BBNDK_VERSION_AT_LEAST(10,3,0)
bt_le_callbacks_t leCallbacks = { NULL, NULL, NULL, btLeAdvertisementCbExt };
#else
bt_le_callbacks_t leCallbacks = { btLeAdvertisementCb, NULL, NULL };
#endif

ApplicationUI::ApplicationUI()
	: QObject()
	, _model(new GroupDataModel(this))
	, _config_model(new GroupDataModel(this))
	, _state(STATE_STOPPED)
	, _scan_state(STATE_STOPPED)
	, _orgName("BlackBerry Ltd")
	, _appName("WheresMyBeacon Settings")
	, _bt_initialised(false)
{
    appInstance = this;

    _bdaddr = 0;

    _pTranslator = new QTranslator(this);
    _pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    Q_ASSERT(res);
    Q_UNUSED(res);

    onSystemLanguageChanged();

    _qml = QmlDocument::create("asset:///main.qml").parent(this);

    QDeclarativeContext *root_context;
    root_context = _qml->defaultDeclarativeEngine()->rootContext();
    root_context->setContextProperty("app", this);

    _root = _qml->createRootObject<AbstractPane>();
    _mainPage = _root->findChild<QObject*>("mainPage");

    // ============== Message to be sent to page

    QObject::connect(this, SIGNAL(message(QVariant)), _mainPage, SLOT(onMessage(QVariant)));

    // ============== Hook up buttons

    QObject::connect(_mainPage, SIGNAL(startListening()), this, SLOT(onStartListening()));
    QObject::connect(_mainPage, SIGNAL(stopListening()), this, SLOT(onStopListening()));
    QObject::connect(this, SIGNAL(openConfig(QVariant)), _mainPage, SLOT(onOpenConfig(QVariant)), Qt::QueuedConnection);

    Application::instance()->setScene(_root);
}

ApplicationUI::~ApplicationUI()
{
    deinitBluetooth();
    appInstance = NULL;
}

void ApplicationUI::prepForAdvertisements()
{
    _model->clear();

    if (!_bt_initialised) {
        initBluetooth();
    }

    if (bt_le_set_scan_params(1000, 100, BT_LE_ADVERT_SCAN_PASSIVE) == EOK) {
        qDebug() << "BBBB bt_le_set_scan_params() OK" << endl;
    } else {
        qDebug() << "BBBB bt_le_set_scan_params() FAIL " << strerror(errno) << endl;
        return;
    }

    if (bt_le_add_scan_device(BT_LE_BDADDR_ANY, NULL) == EOK) {
        qDebug() << "BBBB bt_le_add_scan_device() OK" << endl;
    } else {
        qDebug() << "BBBB bt_le_add_scan_device() FAIL " << strerror(errno) << endl;
    }

}

void ApplicationUI::initBluetooth()
{
    if (bt_device_init(btEvent) == EOK) {
        qDebug() << "BBBB bt_device_init() OK" << endl;
    } else {
        qDebug() << "BBBB bt_device_init() FAIL " << strerror(errno) << endl;
        return;
    }

    if (bt_le_init(&leCallbacks) == EOK) {
        qDebug() << "BBBB bt_le_init() OK" << endl;
    } else {
        qDebug() << "BBBB bt_le_init() FAIL " << strerror(errno) << endl;
        return;
    }

    _bt_initialised = true;

}
void ApplicationUI::deinitBluetooth()
{
    qDebug() << "BBBB Calling bt_le_deinit()" << endl;
    bt_le_deinit();
    qDebug() << "BBBB Calling bt_device_deinit()" << endl;
    bt_device_deinit();

    _bt_initialised = false;
}

void ApplicationUI::onStartListening()
{
    qDebug() << "BBBB onStartListening";
    _state = STATE_STARTED;
    emit stateChanged();

    bb::system::SystemToast toast;
    toast.setBody(QString("Listening - advertising data will appear shortly"));
    toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast.exec();

    prepForAdvertisements();

}

void ApplicationUI::onStopListening()
{
    qDebug() << "BBBB onStopListening";
    _state = STATE_STOPPED;
    emit stateChanged();
    deinitBluetooth();
}

void ApplicationUI::startScanning()
{
    qDebug() << "BBBB startScanning";
    _config_model->clear();
    _scan_state = STATE_STARTED;
    emit scanStateChanged();

    bb::system::SystemToast toast;
    toast.setBody(QString("Scanning - configurable beacons will appear shortly"));
    toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast.exec();

    bt_remote_device_t **remoteDeviceArray;
    bt_remote_device_t *nextRemoteDevice = 0;

    if (!_bt_initialised) {
        initBluetooth();
    }

    // look for SensorTag devices
    remoteDeviceArray = bt_disc_retrieve_devices(BT_DISCOVERY_ALL, 0);

    int beacons = 0;
    if (remoteDeviceArray) {
        for (int i = 0; (nextRemoteDevice = remoteDeviceArray[i]); ++i) {
            qDebug() << "BBBB processing device #" << (i + 1);

            char **servicesArray = bt_rdev_get_services_gatt(remoteDeviceArray[i]);
            qDebug() << "BBBB checking services";
            if (servicesArray) {
                qDebug() << "BBBB found services";
                for (int i = 0; servicesArray[i]; i++) {
                    qDebug() << "BBBB Checking type = " << servicesArray[i];
                    if (strcasecmp(servicesArray[i], SENSORTAG_IBEACON_SERVICE_UUID) == 0) {
                        qDebug() << "BBBB SensorTag iBeacon service found";
                        beacons++;
                        QVariantMap map;
                        char buffer[128];
                        const int bufferSize = sizeof(buffer);
                        bt_rdev_get_friendly_name(nextRemoteDevice, buffer, bufferSize);
                        map["DEVICE_NAME"] = QString::fromLatin1(buffer);
                        qDebug() << "BBBB DEVICE_NAME=" << QString::fromLatin1(buffer);
                        bt_rdev_get_address(nextRemoteDevice, buffer);
                        map["MAC"] = QString::fromLatin1(buffer);
                        qDebug() << "BBBB MAC=" << QString::fromLatin1(buffer);
                        _config_model->insert(map);
                        break;
                    }
                }
                bt_rdev_free_services(servicesArray);
            } else {
                qDebug() << "BBBB no services found";
            }
        }
        qDebug() << "BBBB freeing buffer";
        bt_rdev_free_array(remoteDeviceArray);
    } else {
        qDebug() << "BBBB no devices with the SensorTag iBeacon service found";
    }

    if (beacons == 0) {
        toast.setBody(QString("No supported, configurable beacons were found"));
        toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
        toast.exec();
    } else {
        toast.setBody(QString("Found %1 SensorTag iBeacons").arg(beacons));
        toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
        toast.exec();
    }

    _scan_state = STATE_STOPPED;
    emit scanStateChanged();

}

void ApplicationUI::readBeaconCharacteristics(QString mac)
{

    qDebug() << "BBBB readBeaconCharacteristics:" << mac;

    _mac = mac;

    _selected_device = bt_rdev_get_le_device(mac.toAscii().constData());

    bb::system::SystemToast toast;
    toast.setBody(QString("Connecting to beacon service - data will appear shortly"));
    toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast.exec();

    errno = 0;

    bool done_connecting = false;
    int attempt_count = 0;

    bt_gatt_conn_parm_t conParm;
    conParm.minConn = 0x30;
    conParm.maxConn = 0x50;
    conParm.latency = 0;
    conParm.superTimeout = 50;

    _connect_to_write = false;

    bt_gatt_init(&gatt_callbacks);

    while (!done_connecting) {

        if (bt_gatt_connect_service(mac.toAscii().constData(), SENSORTAG_IBEACON_SERVICE_UUID, NULL, &conParm, this) < 0) {
            qDebug() << "BBBB GATT connect service request failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
            attempt_count++;
            if (attempt_count > 5) {
                done_connecting = true;
                qDebug() << "BBBB given up attempting to connect to GATT service";
            } else {
                delay(1);
                qDebug() << "BBBB trying again to connect to GATT service";
            }
        } else {
            qDebug() << "BBBB requested connection to SensorTag iBeacon service OK";
            done_connecting = true;
        }
    }

}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(_pTranslator);
    QString locale_string = QLocale().name();
    QString file_name = QString("TestRandom_%1").arg(locale_string);
    if (_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(_pTranslator);
    }
}

void ApplicationUI::emitMessageForUi(const QVariant &text)
{
    emit message(text);
}

QString ApplicationUI::btEventName(int event)
{
    switch (event) {
        case BT_EVT_ACCESS_CHANGED:
            return QString("BT_EVT_ACCESS_CHANGED");
        case BT_EVT_RADIO_SHUTDOWN:
            return QString("BT_EVT_RADIO_SHUTDOWN");
        case BT_EVT_RADIO_INIT:
            return QString("BT_EVT_RADIO_INIT");
        case BT_EVT_CONFIRM_NUMERIC_REQUEST:
            return QString("BT_EVT_CONFIRM_NUMERIC_REQUEST");
        case BT_EVT_PAIRING_COMPLETE:
            return QString("BT_EVT_PAIRING_COMPLETE");
        case BT_EVT_DEVICE_ADDED:
            return QString("BT_EVT_DEVICE_ADDED");
        case BT_EVT_DEVICE_DELETED:
            return QString("BT_EVT_DEVICE_DELETED");
        case BT_EVT_SERVICE_CONNECTED:
            return QString("BT_EVT_SERVICE_CONNECTED");
        case BT_EVT_SERVICE_DISCONNECTED:
            return QString("BT_EVT_SERVICE_DISCONNECTED");
        case BT_EVT_FAULT:
            return QString("BT_EVT_FAULT");
        case BT_EVT_NAME_UPDATED:
            return QString("BT_EVT_NAME_UPDATED");
        case BT_EVT_UNDEFINED_EVENT:
            return QString("BT_EVT_UNDEFINED_EVENT");
        default:
            return QString("UNKNOWN EVENT:%1").arg(event);
    }
}

void ApplicationUI::parseBeaconData(const char *data, int len, int8_t rssi, const char *bdaddr)
{
    BtleAdvertData advertData;
    if (advertData.parse(QByteArray(data, len))) {
        qDebug() << "BBBB advert data parsed OK" << endl;
        if (advertData.hasBeaconData()) {
            QVariantMap entry;

            if (advertData.hasIBeaconData()) {
                qDebug() << "BBBB iBeacon data present in advert data" << endl;
                qDebug() << "BBBB beacon UUID    " << advertData.beaconUuid().toHex() << endl;
                qDebug() << "BBBB beacon Major   " << advertData.beaconMajor() << endl;
                qDebug() << "BBBB beacon Minor   " << advertData.beaconMinor() << endl;
                qDebug() << "BBBB beacon Strength" << advertData.calibratedStrength() << "dBm" << endl;
                qDebug() << "BBBB beacon path loss" << advertData.calibratedStrength() - rssi << "dBm" << endl;

                entry["TYPE"] = "IBEACON";
                entry["UUID"] = advertData.beaconUuidAsString();
                entry["MAJOR"] = advertData.beaconMajor();
                entry["MINOR"] = advertData.beaconMinor();

            } else if(advertData.hasAltBeaconData()) {
                qDebug() << "BBBB Alt Beacon data present in advert data" << endl;
                qDebug() << "BBBB beacon Id    " << advertData.beaconId().toHex() << endl;
                qDebug() << "BBBB beacon Strength" << advertData.calibratedStrength() << "dBm" << endl;
                qDebug() << "BBBB beacon path loss" << advertData.calibratedStrength() - rssi << "dBm" << endl;

                entry["TYPE"] = "ALTBEACON";
                entry["COMPANY"] = advertData.companyCode();
                entry["COMPANYNAME"] = advertData.companyCodeAsString();
                entry["ID"] = advertData.beaconIdAsString();
                entry["RESV"] = advertData.altBeaconReserved();
            }
            entry["MAC"] = QString(bdaddr);
            entry["RSSI"] = advertData.calibratedStrength();
            entry["LOSS"] = advertData.calibratedStrength() - rssi;

            qDebug() << "BBBB entry[\"MAC\"]" << entry["MAC"] << endl;

            if (!_model->isEmpty()) {
                bool duplicate = false;
                for (QVariantList indexPath = _model->first(); !indexPath.isEmpty(); indexPath = _model->after(indexPath)) {
                    QVariantMap targetEntry = _model->data(indexPath).toMap();
                    if (targetEntry["MAC"].toString().compare(entry["MAC"].toString()) == 0) {
                        qDebug() << "BBBB duplicate found in model" << endl;
                        duplicate = true;
                        _model->updateItem(indexPath, entry);
                        break;
                    }
                }
                if (!duplicate) {
                    qDebug() << "BBBB Inserting new item in model" << endl;
                    _model->insert(entry);
                }
            } else {
                qDebug() << "BBBB Adding first item in model" << endl;
                _model->insert(entry);
            }
        } else {
            qDebug() << "BBBB no beacon present data in advert data" << endl;
        }
    } else {
        qDebug() << "BBBB advert data parse FAIL" << endl;
    }
}

bb::cascades::DataModel* ApplicationUI::model() const
{
    return _model;
}

bb::cascades::DataModel* ApplicationUI::configModel() const
{
    return _config_model;
}

int ApplicationUI::state()
{
    return _state;
}

int ApplicationUI::scanState()
{
    return _scan_state;
}

void ApplicationUI::save(QString mac, QString name, QVariant type)
{

    qDebug() << "BBBB save: mac=" << mac << " name=" << name << " type=" << type;

    QSettings qSettings(_orgName, _appName);

    QString name_key("");
    name_key.append(mac).append("_").append("BEACON_NAME");
    qSettings.setValue(name_key, QVariant(name.trimmed()));

    QString type_key("");
    type_key.append(mac).append("_").append("BEACON_TYPE");
    qSettings.setValue(type_key, QVariant(type.toString().trimmed()));

    qSettings.sync();

}

void ApplicationUI::updateBeaconAttributes(QString uuid, int major, int minor, int power, int interval,  int pin) {
    uuid.remove(QChar('-'));
    uuid.remove(QChar(' '));
    qDebug() << "BBBB updateBeaconAttributes: uuid=" << uuid << " major=" << major << " minor=" << minor << " power=" << power <<" interval=" << interval << " pin=" << pin;
    _connect_to_write = true;
    _new_beacon_uuid = uuid;
    _new_beacon_major = major;
    _new_beacon_minor = minor;
    _new_beacon_power = power;
    _new_beacon_interval = interval;
    _new_beacon_pin = pin;

    bb::system::SystemToast toast;
    toast.setBody(QString("Connecting to beacon service and updating characteristics"));
    toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast.exec();

    errno = 0;

    bool done_connecting = false;
    int attempt_count = 0;

    bt_gatt_conn_parm_t conParm;
    conParm.minConn = 0x30;
    conParm.maxConn = 0x50;
    conParm.latency = 0;
    conParm.superTimeout = 50;

    bt_gatt_init(&gatt_callbacks);

    while (!done_connecting) {

        if (bt_gatt_connect_service(_mac.toAscii().constData(), SENSORTAG_IBEACON_SERVICE_UUID, NULL, &conParm, this) < 0) {
            qDebug() << "BBBB GATT connect service request failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
            attempt_count++;
            if (attempt_count > 5) {
                done_connecting = true;
                qDebug() << "BBBB given up attempting to connect to GATT service";
            } else {
                delay(1);
                qDebug() << "BBBB trying again to connect to GATT service";
            }
        } else {
            qDebug() << "BBBB requested connection to SensorTag iBeacon service OK";
            done_connecting = true;
        }
    }

}

QString ApplicationUI::getBeaconType(QString mac)
{

    QSettings qSettings(_orgName, _appName);

    QString key("");
    key.append(mac).append("_").append("BEACON_TYPE");
    if (qSettings.value(key).isNull()) {
        return QString("");
    }

    QString btype = qSettings.value(key).toString();
    return btype;
}

QString ApplicationUI::getBeaconName(QString mac)
{

    QSettings qSettings(_orgName, _appName);
    QString key("");
    key.append(mac).append("_").append("BEACON_NAME");
    if (qSettings.value(key).isNull()) {
        return QString("");
    }
    return qSettings.value(key).toString();
}

void ApplicationUI::updateSensorTag(QString mac, QString uuid, int major, int minor, int power, int interval)
{
    qDebug() << "BBBB updateSensorTag: mac=" << mac << " uuid=" << uuid << " major=" << major << " minor=" << minor << " power=" << power << " interval=" << interval;

    bt_remote_device_t *remoteDevice = bt_rdev_get_device(mac.toAscii());

    if (!remoteDevice) {
        qDebug() << "BBBB updateSensorTag: bad device mac - exiting";
        return;
    }

    char **servicesArray = bt_rdev_get_services_gatt(remoteDevice);
    if (servicesArray) {
        for (int i = 0; servicesArray[i]; i++) {
            qDebug() << "BBBB servicesArray " << servicesArray[i];
        }
        bt_rdev_free_services(servicesArray);
    } else {
        qDebug() << "BBBB updateSensorTag: unable to get service list - errno : " << strerror(errno) << endl;
        return;
    }

}

void ApplicationUI::emitOpenConfig(QString mac)
{
    qDebug() << "BBBB emiting openConfig(" << mac << ")";
    emit openConfig(QVariant(mac));
}

QString ApplicationUI::getBeaconUuid() {
    qDebug() << "BBBB getBeaconUuid :" << _beacon_uuid;
    return _beacon_uuid.toUpper();
}

int ApplicationUI::getBeaconMajor() {
    qDebug() << "BBBB getBeaconMajor:" << _beacon_major;
    return _beacon_major;
}

int ApplicationUI::getBeaconMinor() {
    qDebug() << "BBBB getBeaconMinor:" << _beacon_minor;
    return _beacon_minor;
}

int ApplicationUI::getBeaconPower() {
    qDebug() << "BBBB getBeaconPower:" << _beacon_power;
    return _beacon_power;
}

int ApplicationUI::getBeaconInterval() {
    qDebug() << "BBBB getBeaconInterval:" << _beacon_interval;
    return _beacon_interval;
}

int ApplicationUI::getBeaconPin() {
    qDebug() << "BBBB getBeaconPin:" << _beacon_pin;
    return _beacon_pin;
}

bool ApplicationUI::isBeaconCharacteristicsAvailable() {
    return _beacon_attributes_available;
}
