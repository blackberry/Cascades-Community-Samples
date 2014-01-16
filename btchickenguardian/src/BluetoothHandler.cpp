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

void establishHandles(int service_type, int instance);

static bool bt_initialised = false;
static bool immediate_alert_connected = false;
static bool link_loss_connected = false;
static bool tx_power_connected = false;
static bool rssi_polling_required = false;

// Service UUIDs
static char* IMMEDIATE_ALERT_SERVICE_UUID = "1802";
static char* LINK_LOSS_SERVICE_UUID = "1803";
static char* TX_POWER_SERVICE_UUID = "1804";

// Characteristic UUIDs
static const char* ALERT_LEVEL_UUID = "0x2A06";
static const char* TX_POWER_LEVEL_UUID = "0x2A07";

static const int INDEX_IMMEDIATE_ALERT_SERVICE = 1;
static const int INDEX_LINK_LOSS_SERVICE = 2;
static const int INDEX_TX_POWER_SERVICE = 3;

static const int PROXIMITY_NEAR = 0;
static const int PROXIMITY_MEDIUM = 1;
static const int PROXIMITY_FAR = 2;

// proximity band tracking and change notification - used to smooth the data in the face of RSSI fluctuations either side of a proximity threshold
static int last_proximity_band_notified = -1;         // the proximity band value we last notified to the UI
static int tracking_proximity_band = -1;              // the proximity band we changed to and are now tracking for fluctuations / significance
static int this_proximity_band_count = 0;             // measures how many successive RSSI reads have placed us in "this" proximity band
static int proximity_band_change_notified = 0;        // indicates that we have emitted a signal denoting a change in proximity band

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
        case BT_EVT_LE_NAME_UPDATED:
            return QString("BT_EVT_LE_NAME_UPDATED");
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

void setLinkLossAlertLevel()
{

    if (!link_loss_connected) {
        qDebug() << "XXXX not yet connected to the link loss service so ignoring alert request from user";
        return;
    }

    qDebug() << "XXXX BluetoothHandler::setLinkLossAlertLevel()";

    uint8_t level[] = { 0 };
    // we set the level to HIGH
    level[0] = 2;

    errno= 0;

    DataContainer* dc = DataContainer::getInstance();

    int link_loss_service_instance = dc->getLinkLossServiceInstance();
    uint16_t alert_level_value_handle = dc->getLinkLossAlertLevelValueHandle();

    qDebug() << "XXXX setting link loss service alert level with bt_gatt_write_value_noresp. alert_service_instance=" << link_loss_service_instance;

    if (bt_gatt_write_value_noresp(link_loss_service_instance, alert_level_value_handle, 0, level, sizeof(level)) == EOK) {
        qDebug() << "XXXX BluetoothHandler::setLinkLossAlertLevel() - value written successfully";
    } else {
        qDebug() << "XXXX BluetoothHandler::setLinkLossAlertLevel() - errno=(" << errno<< ") :" << strerror(errno);
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
        _handler->emitSignalSetMessage("Chicken sensor has connected");
        DataContainer *dc = DataContainer::getInstance();
        dc->setDeviceConnected(true);
        _handler->emitSignalLinkEstablished();
        return;
    }

    if (event == BT_EVT_LE_DEVICE_DISCONNECTED) {
        immediate_alert_connected = false;
        link_loss_connected = false;
        tx_power_connected = false;
        rssi_polling_required = false;
        DataContainer *dc = DataContainer::getInstance();
        dc->setDeviceConnected(false);
        dc->setAlertServiceInstance(0);
        dc->setLinkLossServiceInstance(0);
        dc->setTxPowerServiceInstance(0);
        _handler->emitSignalSetMessage("Chicken sensor has disconnected");
        _handler->emitSignalDisableProximityVisualisation();
        return;
    }
}

void readTxPowerLevel()
{

    DataContainer* dc = DataContainer::getInstance();
    uint8_t *tx_power_level;
    tx_power_level = (uint8_t*) malloc(sizeof(uint8_t));

    // The Transmit Power Level characteristic represents the current transmit power level in dBm, and the level ranges from -100 dBm to +20 dBm to a
    // resolution of 1 dBm.
    //
    // Units of dBm are decibels relative to 1 mW of power, hence, 0 dBm 1 mW.
    // 1/100 mW is -20 dBm and 100 mW is +20 dBm. Powers less than 1 mW are always negative dBm values, and powers greater than
    // 1 mW are always positive.

    errno= 0;
    int result = bt_gatt_read_value(dc->getTxPowerServiceInstance(), dc->getTxPowerLevelValueHandle(), 0, tx_power_level, sizeof(uint8_t), 0);
    if (result < 0) {
        qDebug() << "XXXX bt_gatt_read_value - errno=(" << errno<< ") :" << strerror(errno);
    }

    int power_level_dbm = *tx_power_level;
    if (power_level_dbm > 128) {
        power_level_dbm = 255 - power_level_dbm;
    }

    dc->setTxPowerLevel(power_level_dbm);

    free(tx_power_level);

    qDebug() << "XXXX read TX power level. result=" << result << " : tx_power_level=" << *tx_power_level << " : power_level_dbm=" << power_level_dbm;
}


void gatt_service_connected(const char *bdaddr, const char *service, int instance, int err, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{

    Q_UNUSED(superTimeout)
    Q_UNUSED(userData)
    Q_UNUSED(latency)
    Q_UNUSED(connInt)
    Q_UNUSED(err)

    qDebug() << "YYYY gatt_service_connected:" << service;

    if (strcmp(service, IMMEDIATE_ALERT_SERVICE_UUID) == 0) {
        // we connected to the immediate alert service or the link lost service so...
        immediate_alert_connected = true;
        // create a remote BT device structure since we need this for RSSI monitoring later
        bt_remote_device_t *bt_device = bt_rdev_get_device(bdaddr);
        DataContainer *dc = DataContainer::getInstance();
        dc->setCurrentDevice(bt_device);
        establishHandles(INDEX_IMMEDIATE_ALERT_SERVICE, instance);
        _handler->emitSignalEnableProximityVisualisation();
        return;
    }

    if (strcmp(service, LINK_LOSS_SERVICE_UUID) == 0) {
        // we connected to the link loss service or the link lost service so...
        link_loss_connected = true;
        establishHandles(INDEX_LINK_LOSS_SERVICE, instance);
        setLinkLossAlertLevel();
        return;
    }

    if (strcmp(service, TX_POWER_SERVICE_UUID) == 0) {
        // we connected to the tx power service or the link lost service so...
        tx_power_connected = true;
        establishHandles(INDEX_TX_POWER_SERVICE, instance);
        readTxPowerLevel();
        _handler->startRssiPolling();
        return;
    }

}

void establishHandles(int service_type, int instance)
{
    if (service_type < 1 || service_type > 3) {
        qDebug() << "XXXX invalid service type indicator passed to establishAlertLevelHandle function:" << service_type;
        return;
    }

    const char* characteristic_uuids[] = { ALERT_LEVEL_UUID, ALERT_LEVEL_UUID, TX_POWER_LEVEL_UUID };

    DataContainer *dc = DataContainer::getInstance();

    bt_gatt_characteristic_t* alert_level;

// find the handle of the alert level characteristic
    int num_characteristics = bt_gatt_characteristics_count(instance);
    qDebug() << "YYYY # characteristics=" << num_characteristics;

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
            if (strcmp(characteristicList[i].uuid, characteristic_uuids[service_type - 1]) == 0) {
                qDebug() << "YYYY found alert level characteristic";
                switch (service_type) {
                    case INDEX_IMMEDIATE_ALERT_SERVICE:
                        qDebug() << "XXXX CONNECTED TO IMMEDIATE ALERT SERVICE - saving handles";
                        dc->setAlertServiceInstance(instance);
                        dc->setImmediateAlertLevelHandle(characteristicList[i].handle);
                        dc->setImmediateAlertLevelValueHandle(characteristicList[i].value_handle);
                        break;
                    case INDEX_LINK_LOSS_SERVICE:
                        qDebug() << "XXXX CONNECTED TO LINK LOSS SERVICE - saving handles";
                        dc->setLinkLossAlertLevelHandle(characteristicList[i].handle);
                        dc->setLinkLossAlertLevelValueHandle(characteristicList[i].value_handle);
                        dc->setLinkLossServiceInstance(instance);
                        break;
                    case INDEX_TX_POWER_SERVICE:
                        qDebug() << "XXXX CONNECTED TO TX POWER SERVICE - saving handles";
                        dc->setTxPowerLevelHandle(characteristicList[i].handle);
                        dc->setTxPowerLevelValueHandle(characteristicList[i].value_handle);
                        dc->setTxPowerServiceInstance(instance);
                        break;
                }
            } else {
                qDebug() << "YYYY other characteristic: uuid,handle,value_handle=" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
            }
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

    if (instance == dc->getLinkLossServiceInstance()) {
        qDebug() << "XXXX reacting to link loss service disconnection";
        link_loss_connected = false;
        dc->setLinkLossServiceInstance(0);
        _handler->emitSignalLostLink();
    }

    if (instance == dc->getTxPowerServiceInstance()) {
        qDebug() << "XXXX reacting to tx power service disconnection";
        tx_power_connected = false;
        dc->setTxPowerServiceInstance(0);
    }

    if (instance == dc->getAlertServiceInstance()) {
        qDebug() << "XXXX reacting to immediate alert service disconnection";
        immediate_alert_connected = false;
        dc->setAlertServiceInstance(0);
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
        IMMEDIATE_ALERT_SERVICE_UUID = "0x1802";
        LINK_LOSS_SERVICE_UUID = "0x1803";
        TX_POWER_SERVICE_UUID = "0x1804";
    }

    if (!bt_initialised) {

        // Initialize the Bluetooth device and allocate the required resources for the library
        bt_device_init(btEvent);
        // make sure the Bluetooth radio is switched on
        if (!bt_ldev_get_power()) {
            bt_ldev_set_power(true);
        }

        _deviceListing->update();
        _localDeviceInfo->update();

        bt_gatt_init(&gatt_callbacks);
        bt_initialised = true;
    }

}

BluetoothHandler::~BluetoothHandler()
{
    qDebug() << "XXXX destroying BluetoothHandler";
    rssi_polling_required = false;
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

void BluetoothHandler::emitSignalEnableProximityVisualisation()
{
    emit signalEnableProximityVisualisation();
}

void BluetoothHandler::emitSignalDisableProximityVisualisation()
{
    emit signalDisableProximityVisualisation();
}

void BluetoothHandler::emitSignalSetMessage(QString msg)
{
    emit signalSetMessage(msg);
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

    DataContainer::getInstance()->addProximityDevice(_remoteDeviceInfo->property("name").toString().toLatin1().data(), _remoteDeviceInfo->property("address").toString().toLatin1().data(),
            _remoteDeviceInfo->property("deviceClassInt").toBool(), _remoteDeviceInfo->property("deviceTypeBool").toBool(), _remoteDeviceInfo->property("pairedBool").toBool(),
            _remoteDeviceInfo->property("encryptedBool").toBool()

            );

}

void BluetoothHandler::setAlertLevel(const AlertLevel level)
{
    _alertLevel = level;
}

void BluetoothHandler::sendAlertRequest()
{

    if (!immediate_alert_connected) {
        qDebug() << "XXXX not yet connected to the immediate alert service so ignoring alert request from user";
        return;
    }

    qDebug() << "XXXX BluetoothHandler::sendAlertRequest()";
    uint8_t level[] = { 0 };

    switch (_alertLevel) {
        case BluetoothHandler::Off:
            level[0] = 0;
            break;
        case BluetoothHandler::Mild:
            level[0] = 1;
            break;
        case BluetoothHandler::High:
            level[0] = 2;
            break;
        default:
            level[0] = 0;
            break;
    }

    errno= 0;

    DataContainer* dc = DataContainer::getInstance();

    int alert_service_instance = dc->getAlertServiceInstance();
    uint16_t alert_level_value_handle = dc->getImmediateAlertLevelValueHandle();

    qDebug() << "XXXX setting immediate alert service alert level with bt_gatt_write_value_noresp. alert_service_instance=" << alert_service_instance;

    if (bt_gatt_write_value_noresp(alert_service_instance, alert_level_value_handle, 0, level, sizeof(level)) == EOK) {
        qDebug() << "XXXX BluetoothHandler::sendAlertRequest() - value written successfully";
    } else {
        qDebug() << "XXXX BluetoothHandler::sendAlertRequest() - errno=(" << errno<< ") :" << strerror(errno);
    }
}

void BluetoothHandler::sendAlertOff()
{
    qDebug() << "XXXX BluetoothHandler::sendAlertOff()";
    setAlertLevel(BluetoothHandler::Off);
    sendAlertRequest();
}

void BluetoothHandler::sendAlertMild()
{
    qDebug() << "XXXX BluetoothHandler::sendAlertMild()";
    setAlertLevel(BluetoothHandler::Mild);
    sendAlertRequest();
}

void BluetoothHandler::sendAlertHigh()
{
    qDebug() << "XXXX BluetoothHandler::sendAlertHigh()";
    setAlertLevel(BluetoothHandler::High);
    sendAlertRequest();
}


void BluetoothHandler::startProximityMonitoring()
{

    qDebug() << "YYYY setting up proximity monitoring";

// connect to the immediate alert service
// connect to the link loss service
// connect to the TX power service
// switch on link loss alerts by writing the Alert Level characteristic
// read the TX Power Level periodically

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

// connect to the immediate alert service

    int function_result = 0;
    int retry_count = 0;

    /* BEGIN WORKAROUND - Temporary fix to address race condition */
    do {
        function_result = bt_gatt_connect_service(device_addr.toAscii().constData(), IMMEDIATE_ALERT_SERVICE_UUID, NULL, &conParm, this);
        retry_count++;
        delay(50);
    } while ((retry_count < 50) && (function_result == -1) && (errno== EBUSY));

    if (function_result < 0) {
        qDebug() << "YYYY GATT connect service request IMMEDIATE_ALERT_SERVICE_UUID failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
        Utilities::alert("GATT connect service request IMMEDIATE_ALERT_SERVICE_UUID failed: " + QString::number(errno));
    } else {
        qDebug() << "YYYY requested connection to IMMEDIATE_ALERT_SERVICE_UUID OK";
    }

// connect to the link loss service
    function_result = 0;
    retry_count = 0;

    /* BEGIN WORKAROUND - Temporary fix to address race condition */
    do {
        function_result = bt_gatt_connect_service(device_addr.toAscii().constData(), LINK_LOSS_SERVICE_UUID, NULL, &conParm, this);
        retry_count++;
        delay(50);
    } while ((retry_count < 50) && (function_result == -1) && (errno== EBUSY));

    if (function_result < 0) {
        qDebug() << "YYYY GATT connect service request LINK_LOSS_SERVICE_UUID failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
        Utilities::alert("GATT connect service request LINK_LOSS_SERVICE_UUID failed: " + QString::number(errno));
    } else {
        qDebug() << "YYYY requested connection to LINK_LOSS_SERVICE_UUID OK";
    }

// connect to the TX power service

    function_result = 0;
    retry_count = 0;

    /* BEGIN WORKAROUND - Temporary fix to address race condition */
    do {
        function_result = bt_gatt_connect_service(device_addr.toAscii().constData(), TX_POWER_SERVICE_UUID, NULL, &conParm, this);
        retry_count++;
        delay(50);
    } while ((retry_count < 50) && (function_result == -1) && (errno== EBUSY));

    if (function_result < 0) {
        Utilities::alert("GATT connect service request TX_POWER_SERVICE_UUID failed: " + QString::number(errno));
    } else {
        qDebug() << "YYYY requested connection to TX_POWER_SERVICE_UUID OK";
    }

}

void BluetoothHandler::startRssiPolling()
{
    qDebug() << "XXXX startRssiPolling()";

    // we'll poll the RSSI in a background thread

    DataContainer* dc = DataContainer::getInstance();

    rssi_polling_required = true;

    _future = new QFuture<void>;
    _watcher = new QFutureWatcher<void>;
    *_future = QtConcurrent::run(_handler, &BluetoothHandler::pollRssi);
    _watcher->setFuture(*_future);
    QObject::connect(_watcher, SIGNAL(finished()), this, SLOT(finishedRssiPolling()));
}

void BluetoothHandler::pollRssi()
{
    qDebug() << "XXXX pollRssi() - starting";

    DataContainer* dc = DataContainer::getInstance();

    int *rssi;
    rssi = (int*) malloc(sizeof(int));

    while (rssi_polling_required) {
        delay(1000);

        errno= 0;

        int result = bt_rdev_get_current_rssi(dc->getCurrentDevice(), rssi);
        if (result < 0) {
            qDebug() << "XXXX bt_rdev_get_current_rssi - errno=(" << errno<< ") :" << strerror(errno);
        }

        qDebug() << "XXXX read RSSI level. result=" << result << " : RSSI=" << *rssi;
        dc->setRssi(*rssi);

        int proximity_indicator = calculateProximityIndicator(rssi);

        emit signalRssi(QVariant(proximity_indicator), QVariant(*rssi));

        qDebug() << "XXXX checking: proximity_indicator=" << proximity_indicator << " tracking_proximity_band=" << tracking_proximity_band << " last_proximity_band_notified=" << last_proximity_band_notified;

        if (proximity_indicator == tracking_proximity_band) {
            qDebug() << "XXXX tracked proximity band not changed, count=" << this_proximity_band_count;
            this_proximity_band_count++;
        } else {
            this_proximity_band_count = 1;
            proximity_band_change_notified = 0;
            tracking_proximity_band = proximity_indicator;
            qDebug() << "XXXX tracking proximity band " << proximity_indicator;
        }

        qDebug() << "XXXX checking: proximity_band_change_notified=" << proximity_band_change_notified << " this_proximity_band_count=" << this_proximity_band_count << " last_proximity_band_notified=" << last_proximity_band_notified;
        if ((proximity_band_change_notified == 0 && this_proximity_band_count > (10 - dc->getRssiFluctuationSensitivity())) || (last_proximity_band_notified == -1)) {
            qDebug() << "XXXX signalling proximity band change: proximity_band_change_notified=" << proximity_band_change_notified << " this_proximity_band_count=" << this_proximity_band_count << " last_proximity_band_notified=" << last_proximity_band_notified;
            dc->setProximityIndicator(proximity_indicator);
            emit signalProximityChange(proximity_indicator);
            last_proximity_band_notified = proximity_indicator;
            proximity_band_change_notified = 1;
        }
    }

    free(rssi);

    qDebug() << "XXXX pollRssi() - exiting";

}

int BluetoothHandler::calculateProximityIndicator(int *rssi) {

    // Should really be comparing with the reported TX Power Level but this characteristic cannot be read on the Philips AEA1000 for some reason

    DataContainer* dc = DataContainer::getInstance();

    if (*rssi > dc->getMinRssiNear()) {
        return PROXIMITY_NEAR;
    }

    if (*rssi > dc->getMinRssiMedium()) {
        return PROXIMITY_MEDIUM;
    }

    return PROXIMITY_FAR;

}

void BluetoothHandler::finishedRssiPolling()
{
    qDebug() << "XXXX finishedRssiPolling()";
}

void BluetoothHandler::quit()
{
    qDebug() << "XXXX BluetoothHandler::quit()";
    rssi_polling_required = false;
}
