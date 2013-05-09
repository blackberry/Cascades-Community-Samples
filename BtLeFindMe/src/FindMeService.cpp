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
#include "FindMeService.hpp"

static FindMeService *findMeServiceInstance = 0;

void gattServiceConnected(
		const char *bdaddr, const char *service, int instance,
		int err, uint16_t connInt, uint16_t latency,
		uint16_t superTimeout, void *userData
		)
{
	if (findMeServiceInstance) {
		findMeServiceInstance->emitGattServiceConnected(
				bdaddr, service, instance,
				err, connInt, latency, superTimeout, userData);
    }
}

void gattServiceDisconnected(
		const char *bdaddr, const char *service,
		int instance, int reason, void *userData
		)
{
	if (findMeServiceInstance) {
		findMeServiceInstance->emitGattServiceDisconnected(
				bdaddr, service,
				instance, reason, userData);
    }
}

void gattServiceUpdated(
		const char *bdaddr, int instance,
		uint16_t connInt, uint16_t latency,
		uint16_t superTimeout, void *userData
		)
{
	if (findMeServiceInstance) {
		findMeServiceInstance->emitGattServiceUpdated(
				bdaddr, instance,
				connInt, latency,
				superTimeout, userData);
    }
}

bt_gatt_callbacks_t gatt_callbacks = {
		gattServiceConnected,
		gattServiceDisconnected,
		gattServiceUpdated
};

FindMeService::FindMeService(QObject *parent)
	: QObject(parent)
	, _serviceIsInitialised(false)
	, _remoteDeviceAddress("")
	, IMMEDIATE_ALERT_SERVICE_UUID("1802")
	, IMMEDIATE_ALERT_LEVEL_UUID("0x2A06")
	, _alertLevel(Off)
	, _serviceInstance(0)
	, _alertLevelServiceHandle(0)
	, _alertLevelValueHandle(0)
{
	qDebug() << "XXXX FindMeService::FindMeService(QObject *parent)";
	findMeServiceInstance = this;

	if (!Utilities::getOSVersion().startsWith("10.0")) {
		IMMEDIATE_ALERT_SERVICE_UUID.prepend("0x");
	}

	qRegisterMetaType<uint16_t>("uint16_t");

	QObject::connect(this, SIGNAL(gattServiceConnected(QString, QString, int, int, uint16_t, uint16_t, uint16_t, void *)),
                     this,   SLOT(handleGattServiceConnected(QString, QString, int, int, uint16_t, uint16_t, uint16_t, void *)));

	QObject::connect(this, SIGNAL(gattServiceDisconnected(QString, QString, int, int, void *)),
                     this,   SLOT(handleGattServiceDisconnected(QString, QString, int, int, void *)));

	QObject::connect(this, SIGNAL(gattServiceUpdated(QString, int, uint16_t, uint16_t, uint16_t, void *)),
                     this,   SLOT(handleGattServiceUpdated(QString, int, uint16_t, uint16_t, uint16_t, void *)));
}

FindMeService::~FindMeService()
{
	qDebug() << "XXXX FindMeService::~FindMeService()";
}


void FindMeService::emitGattServiceConnected(
		const QString &bdaddr, const QString &service, int instance,
		int err, uint16_t connInt, uint16_t latency,
		uint16_t superTimeout, void *userData)
{
    emit gattServiceConnected(
   		bdaddr, service, instance,
   		err, connInt, latency,
   		superTimeout, userData);
}

void FindMeService::emitGattServiceDisconnected(
		const QString &bdaddr, const QString &service,
		int instance, int reason, void *userData)
{
    emit gattServiceDisconnected(
    		bdaddr, service,
    		instance, reason, userData);
}

void FindMeService::emitGattServiceUpdated(
		const QString &bdaddr, int instance,
		uint16_t connInt, uint16_t latency,
		uint16_t superTimeout, void *userData)
{
    emit gattServiceUpdated(
    		bdaddr, instance,
    		connInt, latency,
    		superTimeout, userData);
}

void FindMeService::handleGattServiceConnected(
		const QString &bdaddr, const QString &service, int instance,
		int err, uint16_t connInt, uint16_t latency,
		uint16_t superTimeout, void *userData)
{
	Q_UNUSED(connInt)
	Q_UNUSED(latency)
	Q_UNUSED(superTimeout)
	Q_UNUSED(userData)

	qDebug() << "XXXX FindMeService::handleGattServiceConnected() - " << instance << ", " << bdaddr << ", " << service;

	if (err == EOK) {
		_serviceInstance = instance;
		emit findMeServiceConnected();

		bb::system::SystemToast toast;
	    toast.setBody(tr("Connected to Find Me Service on Device"));
	    toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
	    toast.show();

		int numCharacteristics = bt_gatt_characteristics_count(instance);
		if (numCharacteristics > -1) {
			qDebug() << "XXXX FindMeService::handleGattServiceConnected() - #characteristics=" << numCharacteristics;

			bt_gatt_characteristic_t *characteristicList;

			characteristicList = (bt_gatt_characteristic_t*) malloc(numCharacteristics * sizeof(bt_gatt_characteristic_t));
			if (!characteristicList) {
				qDebug() << "XXXX FindMeService::handleGattServiceConnected() - malloc fail";
				return;
			}

			/* BEGIN WORKAROUND - Temporary fix to address race condition */
			int number = 0;
			do {
				number = bt_gatt_characteristics(instance, characteristicList, numCharacteristics);
			} while ((number == -1) && (errno== EBUSY));

			int characteristicListSize = number;

			qDebug() << "XXXX FindMeService::handleGattServiceConnected() - Characteristics:";
			for (int i = 0; i < characteristicListSize; i++) {
				qDebug() << QString("XXXX characteristic name:%1").arg(parseCharacteristicUuid(characteristicList[i].uuid));
				qDebug() << QString("XXXX characteristic UUID:%1").arg(characteristicList[i].uuid);
				qDebug() << QString("XXXX characteristic handle:%1").arg(characteristicList[i].handle);
				qDebug() << QString("XXXX characteristic value_handle:%1").arg(characteristicList[i].value_handle);
				qDebug() << QString("XXXX properties:%1").arg(characteristicList[i].properties);
				qDebug() << "XXXX FindMeService::handleGattServiceConnected() - characteristicList[i].uuid=" << characteristicList[i].uuid;
				if (!strncasecmp(characteristicList[i].uuid, IMMEDIATE_ALERT_LEVEL_UUID.toAscii().constData(), IMMEDIATE_ALERT_LEVEL_UUID.length())) {
					qDebug() << "XXXX FindMeService::handleGattServiceConnected() - handles set";
					_alertLevelServiceHandle = characteristicList[i].handle;
					_alertLevelValueHandle = characteristicList[i].value_handle;
				} else {
					qDebug() << "XXXX FindMeService::handleGattServiceConnected() - handles not set";
				}
			}

			/* END WORKAROUND */

			if (characteristicList) {
				free(characteristicList);
				characteristicList = NULL;
			}

		} else {
			qDebug() << "XXXX FindMeService::handleGattServiceConnected() Failed to determine number of characteristics";
			qDebug() << "XXXX FindMeService::handleGattServiceConnected() errno=" << errno;
			qDebug() << "XXXX FindMeService::handleGattServiceConnected() errno=" << strerror(errno);
		}
	} else {
		_serviceInstance = 0;
		qDebug() << "XXXX FindMeService::handleGattServiceConnected() - not connected - err=" << strerror(err);
	}
}

void FindMeService::handleGattServiceDisconnected(
		const QString &bdaddr, const QString &service,
		int instance, int reason, void *userData)
{
	Q_UNUSED(userData)

	qDebug() << "XXXX FindMeService::handleGattServiceDisconnected() - " << instance << ", " << bdaddr << ", " << service;

	if (reason == EOK) {
		qDebug() << "XXXX FindMeService::handleGattServiceDisconnected() - a service disconnected";
	} else {
		qDebug() << "XXXX FindMeService::handleGattServiceDisconnected() - error during disconnection - reason=" << strerror(reason);
	}
	if (instance == _serviceInstance) {
		qDebug() << "XXXX FindMeService::handleGattServiceDisconnected() - FindMe Service disconnected";
		_serviceInstance = 0;
		emit findMeServiceDisconnected();
	}
}

void FindMeService::handleGattServiceUpdated(
		const QString &bdaddr, int instance,
		uint16_t connInt, uint16_t latency,
		uint16_t superTimeout, void *userData)
{
	Q_UNUSED(connInt)
	Q_UNUSED(latency)
	Q_UNUSED(superTimeout)
	Q_UNUSED(userData)

	qDebug() << "XXXX FindMeService::handleGattServiceUpdated()";
	qDebug() << "XXXX FindMeService::handleGattServiceUpdated() - " << instance << ", " << bdaddr;

	emit findMeServiceUpdated();
}

void FindMeService::initialiseService()
{
	qDebug() << "XXXX FindMeService::initialiseService()";
	if (!_serviceIsInitialised) {
		errno= 0;
		_serviceIsInitialised = (bt_gatt_init(&gatt_callbacks) == EOK);
		if (_serviceIsInitialised) {
			qDebug() << "XXXX FindMeService::initialiseService() - initialised OK";
			emit serviceStatusChanged();
		} else {
			qDebug() << "XXXX FindMeService::initialiseService() - errno=(" << errno << ") :" << strerror(errno);
		}
	} else {
		qDebug() << "XXXX FindMeService::initialiseService() - wrong state";
	}
}

void FindMeService::deInitialiseService()
{
	qDebug() << "XXXX FindMeService::deInitialiseService()";
	if (_serviceIsInitialised) {
		bt_gatt_deinit();
		_serviceIsInitialised = false;
		qDebug() << "XXXX FindMeService::deInitialiseService() - de-initialised OK";
		emit serviceStatusChanged();
	} else {
		qDebug() << "XXXX FindMeService::deInitialiseService() - wrong state";
	}
}

void FindMeService::setRemoteDevice(const QString &address)
{
	qDebug() << "XXXX FindMeService::setRemoteDevice()- address=" << address;
	_remoteDeviceAddress = address;
}

void FindMeService::setAlertLevel(const AlertLevel level)
{
	_alertLevel = level;
}

bool FindMeService::serviceIsInitialised() const {
	qDebug() << "XXXX FindMeService::serviceIsInitialised() - " << _serviceIsInitialised;
	return _serviceIsInitialised;
}

bool FindMeService::findMeServiceIsConnected() const {
	qDebug() << "XXXX FindMeService::findMeServiceIsConnected() - " << ((_serviceInstance != 0) ? "true" : "false");
	return (_serviceInstance != 0);
}

void FindMeService::connectToImmediateAlertService() {
	bool ok = false;

	bt_gatt_conn_parm_t conParm;
	conParm.minConn = 0x30;
	conParm.maxConn = 0x50;
	conParm.latency = 0;
	conParm.superTimeout = 50;

	errno = 0;
	if (!_serviceInstance) {
		ok = (bt_gatt_connect_service(_remoteDeviceAddress.toAscii().constData(),
				IMMEDIATE_ALERT_SERVICE_UUID.toAscii().constData(), NULL, &conParm, this) == EOK);
		if (ok) {
			qDebug() << "XXXX FindMeService::connectToImmediateAlertService() - connected OK";
		} else {
			qDebug() << "XXXX FindMeService::connectToImmediateAlertService() - connect failed - errno=(" << errno << ") :" << strerror(errno);
		}
	} else {
		qDebug() << "XXXX FindMeService::connectToImmediateAlertService() - invalid service instance";
	}
}

void FindMeService::disconnectFromImmediateAlertService() {
	bool ok = false;
	errno = 0;
	if (_serviceInstance) {
		ok = (bt_gatt_disconnect_instance(_serviceInstance) == EOK);
		if (ok) {
			qDebug() << "XXXX FindMeService::disconnectFromImmediateAlertService() - disconnected OK";
		} else {
			qDebug() << "XXXX FindMeService::disconnectFromImmediateAlertService() - disconnect failed - errno=(" << errno << ") :" << strerror(errno);
		}
		_serviceInstance = 0;
		emit findMeServiceDisconnected();
	} else {
		qDebug() << "XXXX FindMeService::disconnectFromImmediateAlertService() - invalid service instance";
	}
}

void FindMeService::sendAlertRequest() {
	qDebug() << "XXXX FindMeService::sendAlertRequest()";
	uint8_t level[] = {0};

	switch (_alertLevel) {
		case FindMeService::Off:
			level[0] = 0;
			break;
		case FindMeService::Mild:
			level[0] = 1;
			break;
		case FindMeService::High:
			level[0] = 2;
			break;
		default:
			level[0] = 0;
			break;
	}

	qDebug() << "XXXX FindMeService::sendAlertRequest() - _serviceInstance=" << _serviceInstance;
	qDebug() << "XXXX FindMeService::sendAlertRequest() - _findMeServiceHandle=" << _alertLevelServiceHandle;
	qDebug() << "XXXX FindMeService::sendAlertRequest() - _findMeServiceValueHandle=" << _alertLevelValueHandle;
	qDebug() << "XXXX FindMeService::sendAlertRequest() - level=" << level[0];
	qDebug() << "XXXX FindMeService::sendAlertRequest() - sizeof(level)=" << sizeof(level);

	errno = 0;

	if (bt_gatt_write_value_noresp(_serviceInstance, _alertLevelValueHandle, 0, level, sizeof(level)) == EOK) {
		qDebug() << "XXXX FindMeService::sendAlertRequest() - value written successfully";
	} else {
		qDebug() << "XXXX FindMeService::sendAlertRequest() - errno=(" << errno << ") :" << strerror(errno);
	}
}

const char *FindMeService::parseCharacteristicUuid(const char *uuid) {
	const uuidnames_t characteristicUUID[] = {
			{ 0x2a43, "ALERT_CATEGORY_ID" },
			{ 0x2A42, "ALERT_CATEGORY_ID_BIT_MASK" },
			{ 0x2A06, "ALERT_LEVEL" },
			{ 0x2A44, "ALERT_NOTIFICATION_CONTROL_POINT" },
			{ 0x2A3F, "ALERT_STATUS" },
			{ 0x2A01, "GAP_APPEARANCE" },
			{ 0x2A19, "BATTERY_LEVEL" },
			{ 0x2A49, "BLOOD_PRESSURE_FEATURE" },
			{ 0x2A35, "BLOOD_PRESSURE_MEASUREMENT" },
			{ 0x2A38, "BODY_SENSOR_LOCATION" },
			{ 0x2A22, "BOOT_KEYBOARD_INPUT_REPORT" },
			{ 0x2A32, "BOOT_KEYBOARD_OUTPUT_REPORT" },
			{ 0x2A33, "BOOT_MOUSE_INPUT_REPORT" },
			{ 0x2A2B, "CURRENT_TIME" },
			{ 0x2A08, "DATE_TIME" },
			{ 0x2A0A, "DAY_DATE_TIME" },
			{ 0x2A09, "DAY_OF_WEEK" },
			{ 0x2A00, "GAP_DEVICE_NAME" },
			{ 0x2A0D, "DST_OFFSET" },
			{ 0x2A0C, "EXACT_TIME_256" },
			{ 0x2A26, "FIRMWARE_REVISION_STRING" },
			{ 0x2A51, "GLUCOSE_FEATURE" },
			{ 0x2A18, "GLUCOSE_MEASUREMENT" },
			{ 0x2A34, "GLUCOSE_MEASUREMENT_CONTEXT" },
			{ 0x2A27, "HARDWARE_REVISION_STRING" },
			{ 0x2A39, "HEART_RATE_CONTROL_POINT" },
			{ 0x2A37, "HEART_RATE_MEASUREMENT" },
			{ 0x2A4C, "HID_CONTROL_POINT" },
			{ 0x2A4A, "HID_INFORMATION" },
			{ 0x2A2A, "IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST" },
			{ 0x2A36, "INTERMEDIATE_BLOOD_PRESSURE" },
			{ 0x2A1E, "INTERMEDIATE_TEMPERATURE" },
			{ 0x2A0F, "LOCAL_TIME_INFORMATION" },
			{ 0x2A29, "MANUFACTURER_NAME_STRING" },
			{ 0x2A21, "MEASUREMENT_INTERVAL" },
			{ 0x2A24, "MODEL_NUMBER_STRING" },
			{ 0x2A46, "NEW_ALERT" },
			{ 0x2A04, "GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS" },
			{ 0x2A02, "GAP_PERIPHERAL_PRIVACY_FLAG" },
			{ 0x2A50, "PNP_ID" },
			{ 0x2A4E, "PROTOCOL_MODE" },
			{ 0x2A03, "GAP_RECONNECTION_ADDRESS" },
			{ 0x2A52, "RECORD_ACCESS_CONTROL_POINT" },
			{ 0x2A14, "REFERENCE_TIME_INFORMATION" },
			{ 0x2A4D, "REPORT" },
			{ 0x2A4B, "REPORT_MAP" },
			{ 0x2A40, "RINGER_CONTROL_POINT" },
			{ 0x2A41, "RINGER_SETTING" },
			{ 0x2A4F, "SCAN_INTERVAL_WINDOW" },
			{ 0x2A31, "SCAN_REFRESH" },
			{ 0x2A25, "SERIAL_NUMBER_STRING" },
			{ 0x2A05, "GATT_SERVICE_CHANGED" },
			{ 0x2A28, "SOFTWARE_REVISION_STRING" },
			{ 0x2A47, "SUPPORTED_NEW_ALERT_CATEGORY" },
			{ 0x2A48, "SUPPORTED_UNREAD_ALERT_CATEGORY" },
			{ 0x2A23, "SYSTEM_ID" },
			{ 0x2A1C, "TEMPERATURE_MEASUREMENT" },
			{ 0x2A1D, "TEMPERATURE_TYPE" },
			{ 0x2A12, "TIME_ACCURACY" },
			{ 0x2A13, "TIME_SOURCE" },
			{ 0x2A16, "TIME_UPDATE_CONTROL_POINT" },
			{ 0x2A17, "TIME_UPDATE_STATE" },
			{ 0x2A11, "TIME_WITH_DST" },
			{ 0x2A0E, "TIME_ZONE" },
			{ 0x2A07, "TX_POWER" },
			{ 0x2A45, "UNREAD_ALERT_STATUS" },
			{     -1, NULL }
		};

	return (parseUuid(uuid, characteristicUUID));
}

const char *FindMeService::parseDescriptorUuid(const char *uuid) {
	const uuidnames_t descriptorUUID[] = {
			{ 0x2905, "Characteristic Aggregate Format" },
			{ 0x2900, "Characteristic Extended Properties" },
			{ 0x2904, "Characteristic Presentation Format" },
			{ 0x2901, "Characteristic User Description" },
			{ 0x2902, "Client Characteristic Configuration" },
			{ 0x2907, "External Report Reference" },
			{ 0x2908, "Report Reference" },
			{ 0x2903, "Server Characteristic Configuration" },
			{ 0x2906, "Valid Range" },
			{ -1, NULL }
		};

	return (parseUuid(uuid, descriptorUUID));
}

const char *FindMeService::parseServiceUuid(const char *uuid) {
	const uuidnames_t serviceUUID[] = {
			{ 0x1800, "Generic Access" },
			{ 0x1801, "Generic Attribute" },
			{ 0x1802, "ImmediateAlert" },
			{ 0x1803, "Link Loss" },
			{ 0x1804, "Tx Power" },
			{ 0x1805, "Current Time Service" },
			{ 0x1806, "Reference Time Update Service" },
			{ 0x1807, "Next DST Change Service" },
			{ 0x1808, "Glucose" },
			{ 0x1809, "Health Thermometer" },
			{ 0x180A, "Device Information" },
			{ 0x180D, "Heart Rate" },
			{ 0x180E, ":Phone Alert Status Service" },
			{ 0x180F, "Battery Service" },
			{ 0x1810, "Blood Pressure" },
			{ 0x1811, "Alert Notification Service" },
			{ 0x1812, "Human Interface Device" },
			{ 0x1813, "Scan Parameters" },
			{ 0x1814, "Running Speed and Cadance" },
			{ 0x1816, "Cycling Speed and Cadance" },
			{     -1, NULL }
		};

	return (parseUuid(uuid, serviceUUID));
}

const char *FindMeService::parseUuid(const char *uuid, const uuidnames_t uuidNames[]) {
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
