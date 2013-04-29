/* Copyright (c) 2013 Research In Motion Limited.
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
#include "HrDataContainer.hpp"
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

const char *parse_service_uuid(const char *uuid);
const char *parse_characteristic_uuid(const char *uuid);
const char *parse_descriptor_uuid(const char *uuid);

static bool bt_initialised = false;
static BluetoothHandler* _scanner;
static const char* HR_SERVICE_UUID = "180D";
static const char* HEART_RATE_MEASUREMENT = "0x2A37";

typedef struct {
	int uuid;
	const char *name;
} uuidnames_t;

const char *parse_uuid(const char *uuid, const uuidnames_t uuidNames[]) {
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

const char *parse_characteristic_uuid(const char *uuid) {
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

const char *parse_descriptor_uuid(const char *uuid) {
	const uuidnames_t descriptorUUID[] = { { 0x2905, "Characteristic Aggregate Format" }, { 0x2900, "Characteristic Extended Properties" }, { 0x2904, "Characteristic Presentation Format" }, { 0x2901,
			"Characteristic User Description" }, { 0x2902, "Client Characteristic Configuration" }, { 0x2907, "External Report Reference" }, { 0x2908, "Report Reference" }, { 0x2903,
			"Server Characteristic Configuration" }, { 0x2906, "Valid Range" }, { -1, NULL }, };

	return (parse_uuid(uuid, descriptorUUID));
}

const char *parse_service_uuid(const char *uuid) {
	const uuidnames_t serviceUUID[] = { { 0x1800, "Generic Access" }, { 0x1801, "Generic Attribute" }, { 0x1802, "ImmediateAlert" }, { 0x1803, "Link Loss" }, { 0x1804, "Tx Power" }, { 0x1805,
			"Current Time Service" }, { 0x1806, "Reference Time Update Service" }, { 0x1807, "Next DST Change Service" }, { 0x1808, "Glucose" }, { 0x1809, "Health Thermometer" }, { 0x180A,
			"Device Information" }, { 0x180D, "Heart Rate" }, { 0x180E, ":Phone Alert Status Service" }, { 0x180F, "Battery Service" }, { 0x1810, "Blood Pressure" }, { 0x1811,
			"Alert Notification Service" }, { 0x1812, "Human Interface Device" }, { 0x1813, "Scan Parameters" }, { 0x1814, "Running Speed and Cadance" }, { 0x1816, "Cycling Speed and Cadance" }, { -1,
			NULL }, };

	return (parse_uuid(uuid, serviceUUID));
}

QString btEventName(int event) {
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
	case BT_EVT_UNDEFINED_EVENT:
		return QString("BT_EVT_UNDEFINED_EVENT");
	default:
		return QString("UNKNOWN EVENT:%1").arg(event);
	}
}

QString deviceTypeName(int deviceType) {
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

QString boolName(bool value) {
	switch (value) {
	case true:
		return QString("true");
	case false:
		return QString("false");
	}
}

void btEvent(const int event, const char *bt_addr, const char *event_data) {

	Q_UNUSED(bt_addr)

	QString event_name = btEventName(event);
	QString event_data_str = QString(*event_data);
}

void notifications_cb(int instance, uint16_t handle, const uint8_t *val, uint16_t len, void *userData) {

	Q_UNUSED(instance)
	Q_UNUSED(userData)

	qDebug() << "YYYY notifications call back received";
	if (NULL == val)
		return;

	HrDataContainer* hrdc = HrDataContainer::getInstance();

	if (handle == hrdc->getHrValueHandle()) {
		hrdc->addNotification(val,len);
		hrdc->logMostRecentNotification();
	}

}

void gatt_service_connected(const char *bdaddr, const char *service, int instance, int err, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData) {

	Q_UNUSED(superTimeout)
	Q_UNUSED(userData)
	Q_UNUSED(latency)
	Q_UNUSED(connInt)
	Q_UNUSED(err)

	qDebug() << "YYYY gatt_service_connected";

	QString bdaddr_str = QString(bdaddr);
	QString service_str = QString(service);

	qDebug() << "YYYY registering for notifications";
	errno = 0;
	int rc = bt_gatt_reg_notifications(instance, notifications_cb);
	qDebug() << "YYYY rc from bt_gatt_reg_notifications=" << rc;
	if (rc != 0) {
		qDebug() << "YYYY bt_gatt_reg_notifications errno=" << errno;
		qDebug() << "YYYY bt_gatt_reg_notifications errno=" << strerror(errno);
	} else {
		qDebug() << "YYYY bt_gatt_reg_notifications was presumably OK";
	}

	bt_gatt_characteristic_t* heart_rate_measurement;

	heart_rate_measurement = (bt_gatt_characteristic_t*) malloc(sizeof(bt_gatt_characteristic_t));
	if (NULL == heart_rate_measurement) {
		qDebug() << "YYYY GATT characteristics: Not enough memory";
		bt_gatt_disconnect_instance(instance);
		return;
	}

	int num_characteristics = bt_gatt_characteristics_count(instance);

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

		HrDataContainer *hrdc = HrDataContainer::getInstance();

		for (int i = 0; i < characteristicListSize; i++) {
			qDebug() << "YYYY characteristic: uuid,handle,value_handle:" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
			if (strcmp(characteristicList[i].uuid, HEART_RATE_MEASUREMENT) == 0) {
				qDebug() << "YYYY heart rate characteristic available";
				hrdc->setHrHandle(characteristicList[i].handle);
				hrdc->setHrValueHandle(characteristicList[i].value_handle);
				// enable=1 switches the notification on for the specified characteristic
				qDebug() << "YYYY registering for heart_rate_measurement notification. uuid,handle,value_handle=" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
				errno= 0;
				rc = bt_gatt_enable_notify(instance, &characteristicList[i], 1);
				if (rc != 0) {
					qDebug() << "YYYY bt_gatt_enable_notify errno=" << errno;
					qDebug() << "YYYY bt_gatt_enable_notify errno=" << strerror(errno);
				} else {
					qDebug() << "YYYY bt_gatt_enable_notify was presumably OK";
				}
				qDebug() << "YYYY rc from registering for heart_rate_measurement notification=" << rc;

			} else {
				qDebug() << "YYYY other characteristic: uuid,handle,value_handle=" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
			}
		}

		if (characteristicList != NULL) {
			free(characteristicList);
			characteristicList = NULL;
		}

		/* END WORKAROUND */

		qDebug() << "YYYY done registering for heart_rate_measurement notifications";
	}
}

void gatt_service_disconnected(const char *bdaddr, const char *service, int instance, int reason, void *userData) {

	Q_UNUSED(userData)
	Q_UNUSED(instance)
	Q_UNUSED(reason)

	QString bdaddr_str = QString(bdaddr);
	QString service_str = QString(service);
	qDebug() << QString("YYYY gatt_service_disconnected: bdaddr:%1 : %2 - %3").arg(bdaddr_str).arg(service_str).arg(parse_service_uuid(service));
}

void gatt_service_updated(const char *bdaddr, int instance, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData) {

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

BluetoothHandler::BluetoothHandler(QObject *obj)
	: QObject(obj)
	, _localDeviceInfo(new LocalDeviceInfo(this))
	, _deviceListing(new DeviceListing(this))
	, _remoteDeviceInfo(new RemoteDeviceInfo(this))
{
	_scanner = this;

	if (Utilities::getOSVersion().startsWith("10.0")) {
		HR_SERVICE_UUID = "180D";
	} else {
		HR_SERVICE_UUID = "0x180D";
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

BluetoothHandler::~BluetoothHandler() {
	bt_device_deinit();
	bt_gatt_deinit();
}

void BluetoothHandler::receiveHrNotifications() {

	qDebug() << "YYYY setting up to receive GATT notifications";

	if (!bt_initialised) {
		qDebug() << "Bluetooth libraries are not initialised!";
		return;
	}

	HrDataContainer* hrdc = HrDataContainer::getInstance();
	QString device_addr = hrdc->getCurrentDeviceAddr();
	QString device_name = hrdc->getCurrentDeviceName();
	hrdc->clearData();

	bt_gatt_conn_parm_t conParm;
	conParm.minConn = 0x30;
	conParm.maxConn = 0x50;
	conParm.latency = 0;
	conParm.superTimeout = 50;

	errno= 0;

	if (bt_gatt_connect_service(device_addr.toAscii().constData(), HR_SERVICE_UUID, NULL, &conParm, this) < 0) {
		qDebug() << "YYYY GATT connect service request failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
	} else {
		qDebug() << "YYYY requested connection to HR service OK";
	}

}

DeviceListing *BluetoothHandler::deviceListing() const
{
    return _deviceListing;
}

LocalDeviceInfo *BluetoothHandler::localDeviceInfo() const
{
    return _localDeviceInfo;
}

RemoteDeviceInfo *BluetoothHandler::remoteDeviceInfo() const
{
    return _remoteDeviceInfo;
}

void BluetoothHandler::setRemoteDevice(const QString &address)
{
    qDebug() << "YYYY BlueToothLe::setRemoteDevice() " << address;

    _remoteDeviceInfo->update(address);

    HrDataContainer::getInstance()->addHrDevice(
    	_remoteDeviceInfo->property("name").toString().toLatin1().data(),
    	_remoteDeviceInfo->property("address").toString().toLatin1().data(),
    	_remoteDeviceInfo->property("deviceClassInt").toBool(),
    	_remoteDeviceInfo->property("deviceTypeBool").toBool(),
    	_remoteDeviceInfo->property("pairedBool").toBool(),
    	_remoteDeviceInfo->property("encryptedBool").toBool()

    );
    emit foundHrDevice();
}


void BluetoothHandler::stopHrNotifications() {

	errno= 0;

	HrDataContainer* hrdc = HrDataContainer::getInstance();
	QString device_addr = hrdc->getCurrentDeviceAddr();

	if (bt_gatt_disconnect_service(device_addr.toAscii().constData(), HR_SERVICE_UUID) < 0) {
		qDebug() << "YYYY GATT disconnect service request failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
	} else {
		qDebug() << "YYYY disconnected from HR service OK";
	}

}
