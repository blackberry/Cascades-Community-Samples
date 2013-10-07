/* Copyright (c) 2013 BlackBerry Limited.
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

#include "BluetoothHandler.hpp"
#include "DataContainer.hpp"
#include "Utilities.hpp"
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <btapi/btdevice.h>
#include <btapi/btgatt.h>
#include <btapi/btle.h>
#include <btapi/btspp.h>
#include <errno.h>

using namespace bb::cascades;

static BluetoothHandler *_handler = 0;

const char *parse_service_uuid(const char *uuid);
const char *parse_characteristic_uuid(const char *uuid);
const char *parse_descriptor_uuid(const char *uuid);

void establishHandles(int instance);

static bool bt_initialised = false;
static bool device_information_connected = false;

// Service UUIDs
static char* DEVICE_INFORMATION_SERVICE_UUID = "180A";

// Characteristic UUIDs
static const char* MANUFACTURER_NAME_UUID = "0x2A29";
static const char* MODEL_NUMBER_UUID = "0x2A24";
static const char* SERIAL_NUMBER_UUID = "0x2A25";
static const char* HARDWARE_REVISION_UUID = "0x2A27";
static const char* FIRMWARE_REVISION_UUID = "0x2A26";
static const char* SOFTWARE_REVISION_UUID = "0x2A28";
static const char* SYSTEM_ID_UUID = "0x2A23";
static const char* CERTIFICATION_DATA_UUID = "0x2A2A";
static const char* PNP_ID_UUID = "0x2A50";

typedef struct
{
    int uuid;
    const char *name;
} uuidnames_t;

const char *parse_uuid(const char *uuid, const uuidnames_t uuidNames[])
{
    uint16_t id = 0;

    if (NULL == uuid)
        return "UNKNOWN";
    if (strncasecmp(uuid, "0x", 2) == 0)
        uuid += 2;
    if (strlen(uuid) != 4)
        return uuid;
    if (sscanf(uuid, "%04hx", &id) != 1)
        return uuid;

    int i;
    for (i = 0; uuidNames[i].uuid != -1; i++) {
        if (uuidNames[i].uuid == id) {
            return uuidNames[i].name;
        }
    }

    return uuid;
}

const char *parse_characteristic_uuid(const char *uuid)
{
    const uuidnames_t characteristicUUID[] = { { 0x2a43, "ALERT_CATEGORY_ID" }, { 0x2A42, "ALERT_CATEGORY_ID_BIT_MASK" }, { 0x2A06, "ALERT_LEVEL" }, { 0x2A44, "ALERT_NOTIFICATION_CONTROL_POINT" }, {
            0x2A3F, "ALERT_STATUS" }, { 0x2A01, "GAP_APPEARANCE" }, { 0x2A19, "BATTERY_LEVEL" }, { 0x2A49, "BLOOD_PRESSURE_FEATURE" }, { 0x2A35, "BLOOD_PRESSURE_MEASUREMENT" }, { 0x2A38,
            "BODY_SENSOR_LOCATION" }, { 0x2A22, "BOOT_KEYBOARD_INPUT_REPORT" }, { 0x2A32, "BOOT_KEYBOARD_OUTPUT_REPORT" }, { 0x2A33, "BOOT_MOUSE_INPUT_REPORT" }, { 0x2A2B, "CURRENT_TIME" }, { 0x2A08,
            "DATE_TIME" }, { 0x2A0A, "DAY_DATE_TIME" }, { 0x2A09, "DAY_OF_WEEK" }, { 0x2A00, "GAP_DEVICE_NAME" }, { 0x2A0D, "DST_OFFSET" }, { 0x2A0C, "EXACT_TIME_256" }, { 0x2A26,
            "FIRMWARE_REVISION_STRING" }, { 0x2A51, "GLUCOSE_FEATURE" }, { 0x2A18, "GLUCOSE_MEASUREMENT" }, { 0x2A34, "GLUCOSE_MEASUREMENT_CONTEXT" }, { 0x2A27, "HARDWARE_REVISION_STRING" }, { 0x2A39,
            "HEART_RATE_CONTROL_POINT" }, { 0x2A37, "HEART_RATE_MEASUREMENT" }, { 0x2A4C, "HID_CONTROL_POINT" }, { 0x2A4A, "HID_INFORMATION" }, { 0x2A2A,
            "IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST" }, { 0x2A36, "INTERMEDIATE_BLOOD_PRESSURE" }, { 0x2A1E, "INTERMEDIATE_TEMPERATURE" }, { 0x2A0F, "LOCAL_TIME_INFORMATION" }, { 0x2A29,
            "MANUFACTURER_NAME_STRING" }, { 0x2A21, "MEASUREMENT_INTERVAL" }, { 0x2A24, "MODEL_NUMBER_STRING" }, { 0x2A46, "NEW_ALERT" }, { 0x2A04, "GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS" },
            { 0x2A02, "GAP_PERIPHERAL_PRIVACY_FLAG" }, { 0x2A50, "PNP_ID" }, { 0x2A4E, "PROTOCOL_MODE" }, { 0x2A03, "GAP_RECONNECTION_ADDRESS" }, { 0x2A52, "RECORD_ACCESS_CONTROL_POINT" }, { 0x2A14,
                    "REFERENCE_TIME_INFORMATION" }, { 0x2A4D, "REPORT" }, { 0x2A4B, "REPORT_MAP" }, { 0x2A40, "RINGER_CONTROL_POINT" }, { 0x2A41, "RINGER_SETTING" },
            { 0x2A4F, "SCAN_INTERVAL_WINDOW" }, { 0x2A31, "SCAN_REFRESH" }, { 0x2A25, "SERIAL_NUMBER_STRING" }, { 0x2A05, "GATT_SERVICE_CHANGED" }, { 0x2A28, "SOFTWARE_REVISION_STRING" }, { 0x2A47,
                    "SUPPORTED_NEW_ALERT_CATEGORY" }, { 0x2A48, "SUPPORTED_UNREAD_ALERT_CATEGORY" }, { 0x2A23, "SYSTEM_ID" }, { 0x2A1C, "TEMPERATURE_MEASUREMENT" }, { 0x2A1D, "TEMPERATURE_TYPE" }, {
                    0x2A12, "TIME_ACCURACY" }, { 0x2A13, "TIME_SOURCE" }, { 0x2A16, "TIME_UPDATE_CONTROL_POINT" }, { 0x2A17, "TIME_UPDATE_STATE" }, { 0x2A11, "TIME_WITH_DST" },
            { 0x2A0E, "TIME_ZONE" }, { 0x2A07, "TX_POWER" }, { 0x2A45, "UNREAD_ALERT_STATUS" }, { -1, NULL }, };

    return (parse_uuid(uuid, characteristicUUID));
}

const char *parse_descriptor_uuid(const char *uuid)
{
    const uuidnames_t descriptorUUID[] = { { 0x2905, "Characteristic Aggregate Format" }, { 0x2900, "Characteristic Extended Properties" }, { 0x2904, "Characteristic Presentation Format" }, { 0x2901,
            "Characteristic User Description" }, { 0x2902, "Client Characteristic Configuration" }, { 0x2907, "External Report Reference" }, { 0x2908, "Report Reference" }, { 0x2903,
            "Server Characteristic Configuration" }, { 0x2906, "Valid Range" }, { -1, NULL }, };

    return (parse_uuid(uuid, descriptorUUID));
}

const char *parse_service_uuid(const char *uuid)
{
    const uuidnames_t serviceUUID[] = { { 0x1800, "Generic Access" }, { 0x1801, "Generic Attribute" }, { 0x1802, "ImmediateAlert" }, { 0x1803, "Link Loss" }, { 0x1804, "Tx Power" }, { 0x1805,
            "Current Time Service" }, { 0x1806, "Reference Time Update Service" }, { 0x1807, "Next DST Change Service" }, { 0x1808, "Glucose" }, { 0x1809, "Health Thermometer" }, { 0x180A,
            "Device Information" }, { 0x180D, "Heart Rate" }, { 0x180E, ":Phone Alert Status Service" }, { 0x180F, "Battery Service" }, { 0x1810, "Blood Pressure" }, { 0x1811,
            "Alert Notification Service" }, { 0x1812, "Human Interface Device" }, { 0x1813, "Scan Parameters" }, { 0x1814, "Running Speed and Cadance" }, { 0x1816, "Cycling Speed and Cadance" }, { -1,
            NULL }, };

    return (parse_uuid(uuid, serviceUUID));
}

QString btEventName(int event)
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
        case BT_EVT_LE_DEVICE_CONNECTED:
            return QString("BT_EVT_LE_DEVICE_CONNECTED");
        case BT_EVT_LE_DEVICE_DISCONNECTED:
            return QString("BT_EVT_LE_DEVICE_DISCONNECTED");
        case BT_EVT_LE_GATT_SERVICES_UPDATED:
            return QString("BT_EVT_LE_GATT_SERVICES_UPDATED");
        case BT_EVT_FAULT:
            return QString("BT_EVT_FAULT");
        case BT_EVT_UNDEFINED_EVENT:
            return QString("BT_EVT_UNDEFINED_EVENT");
        default:
            return QString("UNKNOWN EVENT:%1").arg(event);
    }
}

QString deviceTypeName(int deviceType)
{
    switch (deviceType) {
        case BT_DEVICE_TYPE_REGULAR:
            return QString("REGULAR");
        case BT_DEVICE_TYPE_LE_PUBLIC:
            return QString("LE PUBLIC");
        case BT_DEVICE_TYPE_LE_PRIVATE:
            return QString("LE PRIVATE");
        case BT_DEVICE_TYPE_UNKNOWN:
            return QString("UNKNOWN");
        default:
            return QString("UNEXPECTED TYPE:%1").arg(deviceType);
    }
}

QString boolName(bool value)
{
    switch (value) {
        case true:
            return QString("true");
        case false:
            return QString("false");
    }
}

void btEvent(const int event, const char *bt_addr, const char *event_data)
{

    Q_UNUSED(bt_addr)

    QString event_name = btEventName(event);
    qDebug() << "XXXX bluetooth event:" << event_name;

    if (event_data != NULL) {
        QString event_data_str = QString(*event_data);
        qDebug() << "XXXX BT event_data=" << event_data_str;
    } else {
        qDebug() << "XXXX BT event_data=NULL";
    }

    if (event == BT_EVT_LE_DEVICE_CONNECTED) {
        _handler->emitSignalInfoMessage("Device has connected");
        DataContainer *dc = DataContainer::getInstance();
        dc->setDeviceConnected(true);
        _handler->emitSignalLinkEstablished();
        return;
    }

    if (event == BT_EVT_LE_DEVICE_DISCONNECTED) {
        _handler->emitSignalInfoMessage("Device has disconnected");
        device_information_connected = false;
        DataContainer *dc = DataContainer::getInstance();
        dc->setDeviceConnected(false);
        dc->setDeviceInformationServiceInstance(0);
        _handler->emitSignalLostLink();
        return;
    }
}

void readInfo()
{

    if (!device_information_connected) {
        _handler->emitSignalLogMessage("Not connected to device information service");
        return;
    }

    DataContainer* dc = DataContainer::getInstance();

    if (dc->getManufacturerNameHandle() != 0) {
        qDebug() << "XXXX reading manufacturer name from service instance " << dc->getDeviceInformationServiceInstance();
        int mfn_max_len = 80;
        uint8_t *manufacturer_name_bytes;
        manufacturer_name_bytes = (uint8_t*) calloc(mfn_max_len,sizeof(uint8_t));
        errno= 0;
        int result = bt_gatt_read_value(dc->getDeviceInformationServiceInstance(), dc->getManufacturerNameValueHandle(), 0, manufacturer_name_bytes, mfn_max_len, 0);
        if (result < 0) {
            qDebug() << "XXXX bt_gatt_read_value manufacturer name - errno=(" << errno<< ") :" << strerror(errno);
        } else {
            char *manufacturer_name_string = (char*) manufacturer_name_bytes;
            dc->setManufacturerName(QString(manufacturer_name_string));
            _handler->emitSignalLogMessage("MANUFACTURER NAME:"+dc->getManufacturerName());
        }
        free(manufacturer_name_bytes);
    } else {
        _handler->emitSignalLogMessage("MANUFACTURER NAME:not present");
    }

    if (dc->getModelNumberHandle() != 0) {
        qDebug() << "XXXX reading model number from service instance " << dc->getDeviceInformationServiceInstance();
        int mon_max_len = 80;
        uint8_t *model_number_bytes;
        model_number_bytes = (uint8_t*) calloc(mon_max_len, sizeof(uint8_t));
        errno= 0;
        int result = bt_gatt_read_value(dc->getDeviceInformationServiceInstance(), dc->getModelNumberValueHandle(), 0, model_number_bytes, mon_max_len, 0);
        if (result < 0) {
            qDebug() << "XXXX bt_gatt_read_value model number - errno=(" << errno<< ") :" << strerror(errno);
        } else {
            char *model_number_string = (char*) model_number_bytes;
            dc->setModelNumber(QString(model_number_string));
            _handler->emitSignalLogMessage("MODEL NUMBER:"+dc->getModelNumber());
        }
        free(model_number_bytes);
    } else {
        _handler->emitSignalLogMessage("MODEL NUMBER:not present");
    }

    if (dc->getSerialNumberHandle() != 0) {
        qDebug() << "XXXX reading serial number from service instance " << dc->getDeviceInformationServiceInstance();
        int sen_max_len = 80;
        uint8_t *serial_number_bytes;
        serial_number_bytes = (uint8_t*) calloc(sen_max_len,sizeof(uint8_t));
        errno= 0;
        int result = bt_gatt_read_value(dc->getDeviceInformationServiceInstance(), dc->getSerialNumberValueHandle(), 0, serial_number_bytes, sen_max_len, 0);
        if (result < 0) {
            qDebug() << "XXXX bt_gatt_read_value serial number - errno=(" << errno<< ") :" << strerror(errno);
        } else {
            char *serial_number_string = (char*) serial_number_bytes;
            dc->setSerialNumber(QString(serial_number_string));
            _handler->emitSignalLogMessage("SERIAL NUMBER:"+dc->getSerialNumber());
        }
        free(serial_number_bytes);
    } else {
        _handler->emitSignalLogMessage("SERIAL NUMBER:not present");
    }

    if (dc->getHardwareRevisionHandle() != 0) {
        qDebug() << "XXXX reading hardware revision from service instance " << dc->getDeviceInformationServiceInstance();
        int hwr_max_len = 80;
        uint8_t *hardware_revision_bytes;
        hardware_revision_bytes = (uint8_t*) calloc(hwr_max_len,sizeof(uint8_t));
        errno= 0;
        int result = bt_gatt_read_value(dc->getDeviceInformationServiceInstance(), dc->getHardwareRevisionValueHandle(), 0, hardware_revision_bytes, hwr_max_len, 0);
        if (result < 0) {
            qDebug() << "XXXX bt_gatt_read_value hardware revision - errno=(" << errno<< ") :" << strerror(errno);
        } else {
            char *hardware_revision_string = (char*) hardware_revision_bytes;
            dc->setHardwareRevision(QString(hardware_revision_string));
            _handler->emitSignalLogMessage("HARDWARE REVISION:"+dc->getHardwareRevision());
        }
        free(hardware_revision_bytes);
    } else {
        _handler->emitSignalLogMessage("HARDWARE REVISION:not present");
    }

    if (dc->getFirmwareRevisionHandle() != 0) {
        qDebug() << "XXXX reading firmware revision from service instance " << dc->getDeviceInformationServiceInstance();
        int fwr_max_len = 80;
        uint8_t *firmware_revision_bytes;
        firmware_revision_bytes = (uint8_t*) calloc(fwr_max_len,sizeof(uint8_t));
        errno= 0;
        int result = bt_gatt_read_value(dc->getDeviceInformationServiceInstance(), dc->getFirmwareRevisionValueHandle(), 0, firmware_revision_bytes, fwr_max_len, 0);
        if (result < 0) {
            qDebug() << "XXXX bt_gatt_read_value firmware revision - errno=(" << errno<< ") :" << strerror(errno);
        } else {
            char *firmware_revision_string = (char*) firmware_revision_bytes;
            dc->setFirmwareRevision(QString(firmware_revision_string));
            _handler->emitSignalLogMessage("FIRMWARE REVISION:"+dc->getFirmwareRevision());
        }
        free(firmware_revision_bytes);
    } else {
        _handler->emitSignalLogMessage("FIRMWARE REVISION:not present");
    }

    if (dc->getSoftwareRevisionHandle() != 0) {
        qDebug() << "XXXX reading software revision from service instance " << dc->getDeviceInformationServiceInstance();
        int swr_max_len = 80;
        uint8_t *software_revision_bytes;
        software_revision_bytes = (uint8_t*) calloc(swr_max_len,sizeof(uint8_t));
        errno= 0;
        int result = bt_gatt_read_value(dc->getDeviceInformationServiceInstance(), dc->getSoftwareRevisionValueHandle(), 0, software_revision_bytes, swr_max_len, 0);
        if (result < 0) {
            qDebug() << "XXXX bt_gatt_read_value software revision - errno=(" << errno<< ") :" << strerror(errno);
        } else {
            char *software_revision_string = (char*) software_revision_bytes;
            dc->setSoftwareRevision(QString(software_revision_string));
            _handler->emitSignalLogMessage("SOFTWARE REVISION:"+dc->getSoftwareRevision());
        }
        free(software_revision_bytes);
    } else {
        _handler->emitSignalLogMessage("SOFTWARE REVISION:not present");
    }

    qDebug() << "XXXX system ID next....";

    if (dc->getSystemIdHandle() != 0) {
        qDebug() << "XXXX reading system ID from service instance " << dc->getDeviceInformationServiceInstance();
        int sid_max_len = 8;
        uint8_t *system_id_bytes;
        system_id_bytes = (uint8_t*) calloc(sid_max_len,sizeof(uint8_t));
        errno= 0;
        int result = bt_gatt_read_value(dc->getDeviceInformationServiceInstance(), dc->getSystemIdValueHandle(), 0, system_id_bytes, sid_max_len, 0);
        if (result < 0) {
            qDebug() << "XXXX bt_gatt_read_value system ID - errno=(" << errno<< ") :" << strerror(errno);
        } else {
            uint64_t manufacturer_identifier=((system_id_bytes[4] << 40) + (system_id_bytes[3] << 32) + (system_id_bytes[2] << 16) + (system_id_bytes[1] << 8) + (system_id_bytes[0]));
            dc->setManufacturerIdentifier(manufacturer_identifier);
            _handler->emitSignalLogMessage(QString("MANUFACTURER ID:%1").arg(dc->getManufacturerIdentifier()));
            uint32_t organizationally_unique_identifier=((system_id_bytes[7] << 16) + (system_id_bytes[6] << 8) + (system_id_bytes[5]));
            dc->setOrgUniqueIdentifier(organizationally_unique_identifier);
            _handler->emitSignalLogMessage(QString("ORG UNIQUE ID:%1").arg(dc->getOrgUniqueIdentifier()));
        }
        free(system_id_bytes);
    } else {
        _handler->emitSignalLogMessage("SYSTEM ID:not present");
    }

    qDebug() << "XXXX certification data next....";
    if (dc->getCertificationDataHandle() != 0) {
        qDebug() << "XXXX reading certification data list from service instance " << dc->getDeviceInformationServiceInstance();
        int cdl_max_len = 80;
        uint8_t *certification_data_list_bytes;
        certification_data_list_bytes = (uint8_t*) calloc(cdl_max_len,sizeof(uint8_t));
        errno= 0;
        int result = bt_gatt_read_value(dc->getDeviceInformationServiceInstance(), dc->getCertificationDataHandle(), 0, certification_data_list_bytes, cdl_max_len, 0);
        if (result < 0) {
            qDebug() << "XXXX bt_gatt_read_value certification data - errno=(" << errno<< ") :" << strerror(errno);
        } else {
            QByteArray cdl_ba = QByteArray::fromRawData(reinterpret_cast<const char *>(certification_data_list_bytes), cdl_max_len);
            QString cdl = QString::fromAscii(cdl_ba.toHex());
            dc->setCertificationData(cdl_ba);
            _handler->emitSignalLogMessage("CERTIFICATION DATA:"+cdl);
        }
        free(certification_data_list_bytes);
    } else {
        _handler->emitSignalLogMessage("CERTIFICATION DATA:not present");
    }

    qDebug() << "XXXX PnP ID next....";
    if (dc->getPnpIdHandle() != 0) {
        qDebug() << "XXXX reading PnP ID from service instance " << dc->getDeviceInformationServiceInstance();
        int pnp_max_len = 7;
        uint8_t *pnp_id_bytes;
        pnp_id_bytes = (uint8_t*) calloc(pnp_max_len,sizeof(uint8_t));
        errno= 0;
        int result = bt_gatt_read_value(dc->getDeviceInformationServiceInstance(), dc->getPnpIdHandle(), 0, pnp_id_bytes, pnp_max_len, 0);
        if (result < 0) {
            qDebug() << "XXXX bt_gatt_read_value PnP ID - errno=(" << errno<< ") :" << strerror(errno);
        } else {
            QByteArray pnp_ba = QByteArray::fromRawData(reinterpret_cast<const char *>(pnp_id_bytes), pnp_max_len);
            dc->setPnpId(pnp_ba);
            uint8_t vendor_id_source = pnp_id_bytes[6];
            _handler->emitSignalLogMessage(QString("VENDOR SOURCE ID:%1").arg(vendor_id_source));
            uint16_t vendor_id = (pnp_id_bytes[5] << 8) + (pnp_id_bytes[4]);
            _handler->emitSignalLogMessage(QString("VENDOR ID:%1").arg(vendor_id));
            uint16_t product_id = (pnp_id_bytes[3] << 8) + (pnp_id_bytes[2]);
            _handler->emitSignalLogMessage(QString("PRODUCT ID:%1").arg(product_id));
            uint16_t product_version = (pnp_id_bytes[1] << 8) + (pnp_id_bytes[0]);
            _handler->emitSignalLogMessage(QString("PRODUCT VERSION:%1").arg(product_version));
        }
        free(pnp_id_bytes);
    } else {
        _handler->emitSignalLogMessage("PNP ID:not present");
    }

}

void gatt_service_connected(const char *bdaddr, const char *service, int instance, int err, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{
    Q_UNUSED(superTimeout)
    Q_UNUSED(userData)
    Q_UNUSED(latency)
    Q_UNUSED(connInt)
    Q_UNUSED(err)

    qDebug() << "YYYY gatt_service_connected:" << service;

    if (strcmp(service, DEVICE_INFORMATION_SERVICE_UUID) == 0) {
        // we connected to the device information service so...
        device_information_connected = true;
        bt_remote_device_t *bt_device = bt_rdev_get_device(bdaddr);
        DataContainer *dc = DataContainer::getInstance();
        dc->setDeviceInformationServiceInstance(instance);
        qDebug() << "XXXX stored service instance value:" << instance;
        establishHandles(instance);
        readInfo();
        return;
    }

}

void establishHandles(int instance)
{

    DataContainer *dc = DataContainer::getInstance();

    bt_gatt_characteristic_t* c_manufacturer_name;

    errno= 0;
    int num_characteristics = bt_gatt_characteristics_count(instance);

    if (num_characteristics == -1) {
        qDebug() << "YYYY bt_gatt_characteristics_count failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
        Utilities::alert("GATT connect service request failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")");
        return;
    }

    if (num_characteristics > -1) {
        qDebug() << QString("YYYY # characteristics: %1").arg(num_characteristics);
        bt_gatt_characteristic_t *characteristicList;

        characteristicList = (bt_gatt_characteristic_t*) malloc(num_characteristics * sizeof(bt_gatt_characteristic_t));
        if (NULL == characteristicList) {
            qDebug() << QString("YYYY GATT characteristics: Not enough memory");
            bt_gatt_disconnect_instance(instance);
            return;
        }

        /* BEGIN WORKAROUND - Temporary fix to address race condition */
        int number = 0;
        do {
            number = bt_gatt_characteristics(instance, characteristicList, num_characteristics);
        } while ((number == -1) && (errno== EBUSY));

        int characteristicListSize = number;

        for (int i = 0; i < characteristicListSize; i++) {
            qDebug() << "YYYY characteristic: uuid,handle,value_handle:" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
            if (strcmp(characteristicList[i].uuid, MANUFACTURER_NAME_UUID) == 0) {
                qDebug() << "YYYY found manufacturer name characteristic";
                dc->setManufacturerNameHandle(characteristicList[i].handle);
                dc->setManufacturerNameValueHandle(characteristicList[i].value_handle);
                continue;
            }
            if (strcmp(characteristicList[i].uuid, MODEL_NUMBER_UUID) == 0) {
                qDebug() << "YYYY found model number characteristic";
                dc->setModelNumberHandle(characteristicList[i].handle);
                dc->setModelNumberValueHandle(characteristicList[i].value_handle);
                continue;
            }
            if (strcmp(characteristicList[i].uuid, SERIAL_NUMBER_UUID) == 0) {
                qDebug() << "YYYY found serial number characteristic";
                dc->setSerialNumberHandle(characteristicList[i].handle);
                dc->setSerialNumberValueHandle(characteristicList[i].value_handle);
                continue;
            }
            if (strcmp(characteristicList[i].uuid, HARDWARE_REVISION_UUID) == 0) {
                qDebug() << "YYYY found hardware revision characteristic";
                dc->setHardwareRevisionHandle(characteristicList[i].handle);
                dc->setHardwareRevisionValueHandle(characteristicList[i].value_handle);
                continue;
            }
            if (strcmp(characteristicList[i].uuid, FIRMWARE_REVISION_UUID) == 0) {
                qDebug() << "YYYY found firmware revision characteristic";
                dc->setFirmwareRevisionHandle(characteristicList[i].handle);
                dc->setFirmwareRevisionValueHandle(characteristicList[i].value_handle);
                continue;
            }
            if (strcmp(characteristicList[i].uuid, SOFTWARE_REVISION_UUID) == 0) {
                qDebug() << "YYYY found software revision characteristic";
                dc->setSoftwareRevisionHandle(characteristicList[i].handle);
                dc->setSoftwareRevisionValueHandle(characteristicList[i].value_handle);
                continue;
            }
            if (strcmp(characteristicList[i].uuid, SYSTEM_ID_UUID) == 0) {
                qDebug() << "YYYY found system ID characteristic";
                dc->setSystemIdHandle(characteristicList[i].handle);
                dc->setSystemIdValueHandle(characteristicList[i].value_handle);
                continue;
            }
            if (strcmp(characteristicList[i].uuid, CERTIFICATION_DATA_UUID) == 0) {
                qDebug() << "YYYY found certification data characteristic";
                dc->setCertificationDataHandle(characteristicList[i].handle);
                dc->setCertificationDataValueHandle(characteristicList[i].value_handle);
                continue;
            }
            if (strcmp(characteristicList[i].uuid, PNP_ID_UUID) == 0) {
                qDebug() << "YYYY found PnP ID characteristic";
                dc->setPnpIdHandle(characteristicList[i].handle);
                dc->setPnpIdValueHandle(characteristicList[i].value_handle);
                continue;
            }
            qDebug() << "YYYY other characteristic: uuid,handle,value_handle=" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
        }

        if (characteristicList != NULL) {
            free(characteristicList);
            characteristicList = NULL;
        }

        /* END WORKAROUND */
    }
}

void gatt_service_disconnected(const char *bdaddr, const char *service, int instance, int reason, void *userData)
{

    Q_UNUSED(userData)
    Q_UNUSED(instance)
    Q_UNUSED(reason)

    QString bdaddr_str = QString(bdaddr);
    QString service_str = QString(service);
    qDebug() << QString("XXXX gatt_service_disconnected: bdaddr:%1 : %2 - %3").arg(bdaddr_str).arg(service_str).arg(parse_service_uuid(service));

    DataContainer *dc = DataContainer::getInstance();

    if (instance == dc->getDeviceInformationServiceInstance()) {
        qDebug() << "XXXX reacting to device information service disconnection";
        device_information_connected = false;
        dc->setDeviceInformationServiceInstance(0);
    }

}

void gatt_service_updated(const char *bdaddr, int instance, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{

    Q_UNUSED(superTimeout)
    Q_UNUSED(userData)
    Q_UNUSED(latency)
    Q_UNUSED(connInt)
    Q_UNUSED(instance)

    QString bdaddr_str = QString(bdaddr);
    qDebug() << QString("YYYY gatt_service_updated: bdaddr:%1").arg(bdaddr_str);
}

// positional parameters indicating the implementations of each of the function types in bt_gatt_callbacks_t
bt_gatt_callbacks_t gatt_callbacks = { gatt_service_connected, gatt_service_disconnected, gatt_service_updated };

BluetoothHandler::BluetoothHandler(QObject * obj) :
        QObject(obj), _localDeviceInfo(new LocalDeviceInfo(this)), _deviceListing(new DeviceListing(this)), _remoteDeviceInfo(new RemoteDeviceInfo(this))
{
    _handler = this;

    if (!Utilities::getOSVersion().startsWith("10.0")) {
        DEVICE_INFORMATION_SERVICE_UUID = "0x180A";
    }

    if (!bt_initialised) {

        // Initialize the Bluetooth device and allocate the required resources for the library
        bt_device_init(btEvent);
        // make sure the Bluetooth radio is switched on
        if (!bt_ldev_get_power()) {
            bt_ldev_set_power(true);
        }
        bt_gatt_init(&gatt_callbacks);
        bt_initialised = true;
    }

}

BluetoothHandler::~BluetoothHandler()
{
    qDebug() << "XXXX destroying BluetoothHandler";
    bt_device_deinit();
    bt_gatt_deinit();
}

void BluetoothHandler::emitSignalLinkEstablished()
{
    emit signalLinkEstablished();
}

void BluetoothHandler::emitSignalLostLink()
{
    emit signalLostLink();
}

void BluetoothHandler::emitSignalInfoMessage(QString msg)
{
    emit signalInfoMessage(msg);
}

void BluetoothHandler::emitSignalLogMessage(QString msg)
{
    emit signalLogMessage(msg);
}

DeviceListing * BluetoothHandler::deviceListing() const
{
    return _deviceListing;
}

LocalDeviceInfo * BluetoothHandler::localDeviceInfo() const
{
    return _localDeviceInfo;
}

RemoteDeviceInfo * BluetoothHandler::remoteDeviceInfo() const
{
    return _remoteDeviceInfo;
}

void BluetoothHandler::setRemoteDevice(const QString &address)
{
    qDebug() << "YYYY BlueToothLe::setRemoteDevice() " << address;

    _remoteDeviceInfo->update(address);

    DataContainer::getInstance()->addDevice(_remoteDeviceInfo->property("name").toString().toLatin1().data(), _remoteDeviceInfo->property("address").toString().toLatin1().data(),
            _remoteDeviceInfo->property("deviceClassInt").toBool(), _remoteDeviceInfo->property("deviceTypeBool").toBool(), _remoteDeviceInfo->property("pairedBool").toBool(),
            _remoteDeviceInfo->property("encryptedBool").toBool()

            );

}

void BluetoothHandler::getInfo()
{

    qDebug() << "YYYY getInfo()";

// connect to the device information service
// read the various characteristic values

    if (!bt_initialised) {
        qDebug() << "Bluetooth libraries are not initialised!";
        return;
    }

    bt_gatt_conn_parm_t conParm;
    conParm.minConn = 0x30;
    conParm.maxConn = 0x50;
    conParm.latency = 0;
    conParm.superTimeout = 50;

    errno= 0;

    DataContainer* dc = DataContainer::getInstance();
    QString device_addr = dc->getCurrentDeviceAddr();

// connect to the device information service

    int function_result = 0;
    int retry_count = 0;

    _handler->emitSignalInfoMessage("Connecting to Device Information Service");

    /* BEGIN WORKAROUND - Temporary fix to address race condition */
    do {
        function_result = bt_gatt_connect_service(device_addr.toAscii().constData(), DEVICE_INFORMATION_SERVICE_UUID, NULL, &conParm, this);
        retry_count++;
        delay(50);
    } while ((retry_count < 50) && (function_result == -1) && (errno== EBUSY));

    if (function_result < 0) {
        qDebug() << "YYYY GATT connect service request DEVICE_INFORMATION_SERVICE_UUID failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
        Utilities::alert("GATT connect service request DEVICE_INFORMATION_SERVICE_UUID failed: " + QString::number(errno));
    } else {
        qDebug() << "YYYY requested connection to DEVICE_INFORMATION_SERVICE_UUID OK";
    }

}

