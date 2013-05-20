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
#include "CscDataContainer.hpp"
#include "Utilities.hpp"
#include <QDebug>
#include <QSettings>
#include <float.h>

CscDataContainer* CscDataContainer::_instance;
QString CscDataContainer::KEY_DEVICE_NAME = "device_name";
QString CscDataContainer::KEY_DEVICE_ADDR = "device_addr";
int MAX_QUEUE_SIZE = 28800; // one notification per second for 8 hours

// FLAGS field bit mask values
static uint8_t WHEEL_REVOLUTION_DATA_PRESENT = 1; // 0
static uint8_t CRANK_REVOLUTION_DATA_PRESENT = 2; // 1

int last_notification_time_seconds;

CscDataContainer::CscDataContainer() :
		_log_to_file(false) {
	QSettings settings;
	_current_device_name = settings.value(KEY_DEVICE_NAME, "").toString();
	_current_device_addr = settings.value(KEY_DEVICE_ADDR, "").toString();
}

CscDataContainer* CscDataContainer::getInstance() {
	if (_instance == 0) {
		_instance = new CscDataContainer;
	}
	return _instance;
}

void CscDataContainer::addCscDevice(char* device_name, char* device_addr, int device_class, int device_type, bool paired, bool encrypted) {

	_csc_device["device_name"] = device_name;
	_csc_device["device_addr"] = device_addr;
	_csc_device["device_class"] = device_class;
	_csc_device["device_type"] = device_type;
	_csc_device["paired"] = paired;
	_csc_device["encrypted"] = encrypted;
	_list_of_csc_devices.append(_csc_device);

}

void CscDataContainer::clearDeviceList() {
	_list_of_csc_devices.clear();
}

void CscDataContainer::resetData() {
	_csc_notifications.clear();
	_csc_values.clear();
	_most_recent_notification = {0, 0, 0, 0, 0, 0, 0};
	_most_recent_csc_values = {0, 0, 0, 0, 0};
}

QList<QVariantList> CscDataContainer::getDeviceIdList() {

	QList<QVariantList> list_of_csc_devices;

	QListIterator<QMap<QString, QVariant> > i(_list_of_csc_devices);

	while (i.hasNext()) {
		QMap<QString, QVariant> device_details = i.next();
		QString device_name = device_details.value("device_name").toString();
		QString device_addr = device_details.value("device_addr").toString();
		qDebug() << QString("YYYY device_name:%1 device_addr:%2").arg(device_name).arg(device_addr);
		QList<QVariant> device_info;
		device_info.append(device_details.value("device_name"));
		device_info.append(device_details.value("device_addr"));
		list_of_csc_devices.append(device_info);
	}
	return list_of_csc_devices;
}

QList<QVariantMap> CscDataContainer::getDeviceList() {

	QList<QVariantMap> list_of_csc_devices;
	QListIterator<QMap<QString, QVariant> > i(_list_of_csc_devices);

	while (i.hasNext()) {
		QMap<QString, QVariant> device_details = i.next();
		QString device_name = device_details.value("device_name").toString();
		QString device_addr = device_details.value("device_addr").toString();
		QVariantMap aDevice;
		aDevice["device_name"] = device_name;
		aDevice["device_addr"] = device_addr;
		list_of_csc_devices.append(aDevice);
	}
	return list_of_csc_devices;
}

QString CscDataContainer::getCurrentDeviceName() {
	return _current_device_name;
}

QString CscDataContainer::getCurrentDeviceAddr() {
	return _current_device_addr;
}

void CscDataContainer::setCurrentDeviceName(QString device_name) {
	_current_device_name = device_name;
	QSettings settings;
	settings.setValue(KEY_DEVICE_NAME, device_name);
}

void CscDataContainer::setCurrentDeviceAddr(QString device_addr) {
	_current_device_addr = device_addr;
	QSettings settings;
	settings.setValue(KEY_DEVICE_ADDR, device_addr);
}

uint16_t CscDataContainer::getCscHandle() {
	return _csc_handle;
}

void CscDataContainer::setCscHandle(uint16_t handle) {
	_csc_handle = handle;
}

uint16_t CscDataContainer::getCscValueHandle() {
	return _csc_value_handle;
}

void CscDataContainer::setCscValueHandle(uint16_t handle) {
	_csc_value_handle = handle;
}

// notification data
void CscDataContainer::addNotification(const uint8_t *val, uint16_t len) {
	QString hex = "";

	for (int i = 0; i < len; i++) {
		hex.append(intToHex(val[i]));
		hex.append(":");
	}

	qDebug() << "XXXXX notification payload:" << hex;
	sessionLog(hex);

	uint now_seconds = Utilities::getSysTimeSeconds();
	bool last_notification_is_too_old = false;

	if ((now_seconds - last_notification_time_seconds) > 64) {
		qDebug("XXXX More than 64 seconds since last notification... unsafe to derive RPM values so discarding and establishing new base line");
		last_notification_time_seconds = now_seconds;
		// we'll do the calculations to establish a new baseline but not report the RPM results
		last_notification_is_too_old = true;
	}

	// parse value, paying attention to the bit settings in the FLAGS field

	bool wheel_revolution_data_present = false; // bit 0 of Flags field
	bool crank_revolution_data_present = false; // bit 1 of Flags field

	/**
	 * From the CSC specification:
	 *
	 The Cumulative Wheel Revolutions value, which represents the number of times a wheel rotates, is used in combination with the Last Wheel Event Time
	 and the wheel circumference stored on the Client to determine:
	 1) the speed of the bicycle and
	 2) the distance traveled.
	 In addition, if there is link loss, the Cumulative Wheel Revolutions value can be used to calculate the average speed of the bicycle during the link loss.
	 This value is expected to be set to 0 (or another desired value in case of e.g. a sensor upgrade) at initial installation on a bicycle as described in Section 3.4.2.1.
	 The Cumulative Wheel Revolutions value may decrement for some implementations (e.g. If the bicycle is rolled in reverse), but shall not decrease below 0.
	 The ‘wheel event time’ is a free-running-count of 1/1024 second units and it represents the time when the wheel revolution was detected by the wheel rotation sensor.
	 Since several wheel events can occur between transmissions, only the Last Wheel Event Time value is transmitted. This value is used in combination with the Cumulative
	 Wheel Revolutions value to enable the Client to calculate speed and distance.

	 The Last Wheel Event Time value rolls over every 64 seconds. This equates to a value of 65536 (* 1/1024 = 64 seconds).

	 Interpretation:
	 Wheel event time is a time stamp so by subtracting adjacent time stamp and cumulative wheel revolution values, you can determine the number of revolutions in a given time period and
	 hence, if the wheel circumference is known, the distance travelled and therefore speed during that period.

	 **/
	uint32_t cumulative_wheel_revolutions = 0;
	uint16_t last_wheel_event_time = 0;
	uint16_t cumulative_crank_revolutions = 0;
	uint16_t last_crank_event_time = 0;

	uint16_t wheel_rpm = 0;
	uint16_t crank_rpm = 0;

	int index = 0;

	uint8_t flags = val[index];
	qDebug() << "XXXX FLAGS=" << flags;

	wheel_revolution_data_present = ((flags & WHEEL_REVOLUTION_DATA_PRESENT) == WHEEL_REVOLUTION_DATA_PRESENT);
	crank_revolution_data_present = ((flags & CRANK_REVOLUTION_DATA_PRESENT) == CRANK_REVOLUTION_DATA_PRESENT);

	qDebug() << "XXXX wheel_revolution_data_present=" << wheel_revolution_data_present << ",crank_revolution_data_present" << crank_revolution_data_present;

	// Fields are in the order of LSO to MSO. Where LSO = Least Significant Octet and MSO = Most Significant Octet.

	index++;

	if (wheel_revolution_data_present) {
		// Cumulative Wheel Revolutions, 4 bytes.  Field exists if the key of bit 0 of the Flags field is set to 1.
		cumulative_wheel_revolutions = (val[index + 3] << 24) | (val[index + 2] << 16) | (val[index + 1] << 8) | val[index];
		index = index + 4;
		// Last Wheel Event Time, 2 bytes.  Field exists if the key of bit 0 of the Flags field is set to 1.
		last_wheel_event_time = (val[index + 1] << 8) | val[index];
		qDebug() << "XXXXX cumulative wheel revolutions=" << cumulative_wheel_revolutions << " , last wheel event time=" << last_wheel_event_time;
		index = index + 2;

		// now calculate the wheel RPM value assuming we have more than one data point available
		if (_csc_notifications.size() != 0 && !last_notification_is_too_old) {
			uint16_t event_time_delta = 0;
			uint16_t wheel_revs_delta = 0;
			qDebug() << "XXXXX last_wheel_event_time=" << last_wheel_event_time << ", _most_recent_notification.last_wheel_event_time=" << _most_recent_notification.last_wheel_event_time;
			qDebug() << "XXXXX cumulative_wheel_revolutions=" << cumulative_wheel_revolutions << ", _most_recent_notification.cumulative_wheel_revolutions="
					<< _most_recent_notification.cumulative_wheel_revolutions;
			if (last_wheel_event_time >= _most_recent_notification.last_wheel_event_time) {
				event_time_delta = last_wheel_event_time - _most_recent_notification.last_wheel_event_time;
			} else {
				event_time_delta = 65536 - _most_recent_notification.last_wheel_event_time + last_wheel_event_time;
			}
			wheel_revs_delta = cumulative_wheel_revolutions - _most_recent_notification.cumulative_wheel_revolutions;
			float event_time_s = event_time_delta / 1024.0;
			uint16_t event_time_delta_ms = event_time_s * 1000;
			qDebug() << "XXXXX wheel event time delta=" << event_time_delta << ",wheel event time delta (ms)=" << event_time_delta_ms << ", wheel revs delta=" << wheel_revs_delta;
			qDebug() << "XXXXX wheel event time (s)=" << event_time_s;
			if ((event_time_delta_ms * wheel_revs_delta) > 0) {
				float rpm_real = (60000.0 / event_time_delta_ms) * wheel_revs_delta;
				wheel_rpm = rpm_real;
			} else {
				wheel_rpm = _most_recent_csc_values.wheel_revolutions_per_minute;
			}
			qDebug() << "XXXXX wheel event time delta (ms)=" << event_time_delta_ms << ", wheel RPM=" << wheel_rpm;
			qDebug() << "XXXXX ----------------------------------------------------------------------";
		}
	}

	if (crank_revolution_data_present) {
		// Cumulative Crank Revolutions, 2 bytes.  Field exists if the key of bit 1 of the Flags field is set to 1.
		qDebug() << "XXXX crank data: " << Utilities::intToHex(val[index]) << "," << Utilities::intToHex(val[index + 1]);
		cumulative_crank_revolutions = (val[index + 1] << 8) | val[index];
		qDebug() << "XXXX cumulative_crank_revolutions=" << cumulative_crank_revolutions;
		index = index + 2;
		// Last Crank Event Time, 2 bytes.  Field exists if the key of bit 1 of the Flags field is set to 1.
		last_crank_event_time = (val[index + 1] << 8) | val[index];
		qDebug() << "XXXX cumulative crank revolutions=" << cumulative_crank_revolutions << " , last crank event time=" << last_crank_event_time;
		index = index + 2;

		// now calculate the crank RPM value assuming we have more than one data point available
		if (_csc_notifications.size() != 0 && !last_notification_is_too_old) {
			uint16_t event_time_delta = 0;
			uint16_t crank_revs_delta = 0;
			if (last_crank_event_time >= _most_recent_notification.last_crank_event_time) {
				event_time_delta = last_crank_event_time - _most_recent_notification.last_crank_event_time;
			} else {
				event_time_delta = 65536 - _most_recent_notification.last_crank_event_time + last_crank_event_time;
			}
			float event_time_s = event_time_delta / 1024.0;
			uint16_t event_time_delta_ms = event_time_s * 1000;
			crank_revs_delta = cumulative_crank_revolutions - _most_recent_notification.cumulative_crank_revolutions;
			qDebug() << "XXXX crank event time delta=" << event_time_delta << ", crank revs delta=" << crank_revs_delta;
			if ((event_time_delta_ms * crank_revs_delta) > 0) {
				float rpm_real = (60000.0 / event_time_delta_ms) * crank_revs_delta;
				crank_rpm = rpm_real;
			} else {
				crank_rpm = _most_recent_csc_values.crank_revolutions_per_minute;
			}
			qDebug() << "XXXX crank event time delta (ms)=" << event_time_delta_ms << ", crank RPM=" << crank_rpm;
		}
	}

	if (!last_notification_is_too_old) {
		if (((wheel_rpm < _min_wheel_rpm) && (wheel_rpm > 0)) || _min_wheel_rpm == 0) {
			_min_wheel_rpm = wheel_rpm;
		}
		if (wheel_rpm > _max_wheel_rpm) {
			_max_wheel_rpm = wheel_rpm;
		}
		_total_wheel_rpm = _total_wheel_rpm + wheel_rpm;

		if (((crank_rpm < _min_crank_rpm) && (crank_rpm > 0)) || _min_crank_rpm == 0) {
			_min_crank_rpm = crank_rpm;
		}
		if (crank_rpm > _max_crank_rpm) {
			_max_crank_rpm = crank_rpm;
		}
		_total_crank_rpm = _total_crank_rpm + crank_rpm;
	}

	// store in the notifications vector
	_most_recent_notification = {Utilities::getSysTimeSeconds(),
		wheel_revolution_data_present, crank_revolution_data_present,
		cumulative_wheel_revolutions, last_wheel_event_time,
		cumulative_crank_revolutions, last_crank_event_time};

	if (_csc_notifications.size() == MAX_QUEUE_SIZE) {
		qDebug() << "XXXX queue full - dequeuing oldest notification data";
		csc_notification_t deleted_csc = _csc_notifications.dequeue();
	}

	_csc_notifications.enqueue(_most_recent_notification);
	qDebug() << "XXXX notifications queue contains " << _csc_notifications.size() << " items";

	if (!last_notification_is_too_old) {
		_avg_wheel_rpm = _total_wheel_rpm / _csc_notifications.size();
		_avg_crank_rpm = _total_crank_rpm / _csc_notifications.size();
		QVariant csc_wheel_rpm(wheel_rpm);
		QVariant csc_crank_rpm(crank_rpm);
		emit cscData(wheel_rpm, crank_rpm);
	}
}

void CscDataContainer::logMostRecentNotification() {
	qDebug() << "XXXX ---------------------------------------------------------------------";
	qDebug() << "XXXX timestamp=" << _most_recent_notification.timestamp;
	qDebug() << "XXXX wheel_revolution_data_present=" << _most_recent_notification.wheel_revolution_data_present;
	qDebug() << "XXXX crank_revolution_data_present=" << _most_recent_notification.crank_revolution_data_present;
	qDebug() << "XXXX cumulative_wheel_revolutions=" << _most_recent_notification.cumulative_wheel_revolutions;
	qDebug() << "XXXX last_wheel_event_time=" << _most_recent_notification.last_wheel_event_time;
	qDebug() << "XXXX cumulative_crank_revolutions=" << _most_recent_notification.cumulative_crank_revolutions;
	qDebug() << "XXXX last_crank_event_time=" << _most_recent_notification.last_crank_event_time;
}

QString CscDataContainer::intToHex(int decimal) {
	QString hexadecimal;
	hexadecimal.setNum(decimal, 16);
	return hexadecimal;
}

csc_notification_t CscDataContainer::getMostRecentNotification() {
	return _most_recent_notification;
}

QQueue<csc_notification_t> CscDataContainer::getNotifications() {
	return _csc_notifications;
}

int CscDataContainer::getMinWheelRpm() {
	return _min_wheel_rpm;
}

int CscDataContainer::getMaxWheelRpm() {
	return _max_wheel_rpm;
}

int CscDataContainer::getAvgWheelRpm() {
	return _avg_wheel_rpm;
}

int CscDataContainer::getMinCrankRpm() {
	return _min_crank_rpm;
}

int CscDataContainer::getMaxCrankRpm() {
	return _max_crank_rpm;
}

int CscDataContainer::getAvgCrankRpm() {
	return _avg_crank_rpm;
}

void CscDataContainer::setLogToFile(bool switch_setting) {
	SessionLogger *session_logger = SessionLogger::getInstance();
	if (_log_to_file & !switch_setting) {
		sessionLog("CSC SessionLog -- switching off");
		session_logger->closeSession();
		_log_to_file = switch_setting;
	} else if (!_log_to_file && switch_setting) {
		session_logger->newSession();
		_log_to_file = switch_setting;
		sessionLog("CSC SessionLog -- switched on");
	}
}

void CscDataContainer::sessionLog(QString data) {
	if (_log_to_file) {
		SessionLogger::getInstance()->writeSessionLog(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ").append(data));
	}
}
