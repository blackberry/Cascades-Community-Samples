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
#include "HrDataContainer.hpp"
#include "Utilities.hpp"
#include <QDebug>
#include <QSettings>

HrDataContainer* HrDataContainer::_instance;
QString HrDataContainer::KEY_DEVICE_NAME = "device_name";
QString HrDataContainer::KEY_DEVICE_ADDR = "device_addr";
int MAX_QUEUE_SIZE = 28800; // one notification per second for 8 hours

// FLAGS field bit mask values
static uint8_t HEART_RATE_VALUE_FORMAT = 1; // 0
static uint8_t SENSOR_CONTACT_DETECTED = 2; // 1
static uint8_t SENSOR_CONTACT_FEATURE = 4; // 2
static uint8_t ENERGY_EXPENDED_FEATURE = 8; // 3
static uint8_t RR_INTERVAL_DATA = 16; // 4

HrDataContainer::HrDataContainer() {
	QSettings settings;
	_current_device_name = settings.value(KEY_DEVICE_NAME, "").toString();
	_current_device_addr = settings.value(KEY_DEVICE_ADDR, "").toString();
}

HrDataContainer* HrDataContainer::getInstance() {
	if (_instance == 0) {
		_instance = new HrDataContainer;
	}
	return _instance;
}

void HrDataContainer::addHrDevice(char* device_name, char* device_addr, int device_class, int device_type, bool paired, bool encrypted) {

	_hr_device["device_name"] = device_name;
	_hr_device["device_addr"] = device_addr;
	_hr_device["device_class"] = device_class;
	_hr_device["device_type"] = device_type;
	_hr_device["paired"] = paired;
	_hr_device["encrypted"] = encrypted;
	_list_of_hr_devices.append(_hr_device);

}

void HrDataContainer::clearDeviceList() {
	_list_of_hr_devices.clear();
}

void HrDataContainer::clearData() {
	_hr_notifications.clear();
	_most_recent_notification = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	_min_hr = 999;
	_max_hr = 0;
	_avg_hr = 0;
	_total_hr = 0;
}

QList<QVariantList> HrDataContainer::getDeviceIdList() {

	QList<QVariantList> list_of_hr_devices;

	QListIterator<QMap<QString, QVariant> > i(_list_of_hr_devices);

	while (i.hasNext()) {
		QMap<QString, QVariant> device_details = i.next();
		QString device_name = device_details.value("device_name").toString();
		QString device_addr = device_details.value("device_addr").toString();
		qDebug() << QString("YYYY device_name:%1 device_addr:%2").arg(device_name).arg(device_addr);
		QList<QVariant> device_info;
		device_info.append(device_details.value("device_name"));
		device_info.append(device_details.value("device_addr"));
		list_of_hr_devices.append(device_info);
	}
	return list_of_hr_devices;
}

QList<QVariantMap> HrDataContainer::getDeviceList() {

	QList<QVariantMap> list_of_hr_devices;
	QListIterator<QMap<QString, QVariant> > i(_list_of_hr_devices);

	while (i.hasNext()) {
		QMap<QString, QVariant> device_details = i.next();
		QString device_name = device_details.value("device_name").toString();
		QString device_addr = device_details.value("device_addr").toString();
		QVariantMap aDevice;
		aDevice["device_name"] = device_name;
		aDevice["device_addr"] = device_addr;
		list_of_hr_devices.append(aDevice);
	}
	return list_of_hr_devices;
}

QString HrDataContainer::getCurrentDeviceName() {
	return _current_device_name;
}

QString HrDataContainer::getCurrentDeviceAddr() {
	return _current_device_addr;
}

void HrDataContainer::setCurrentDeviceName(QString device_name) {
	_current_device_name = device_name;
	QSettings settings;
	settings.setValue(KEY_DEVICE_NAME, device_name);
}

void HrDataContainer::setCurrentDeviceAddr(QString device_addr) {
	_current_device_addr = device_addr;
	QSettings settings;
	settings.setValue(KEY_DEVICE_ADDR, device_addr);
}

uint16_t HrDataContainer::getHrHandle() {
	return _hr_handle;
}

void HrDataContainer::setHrHandle(uint16_t handle) {
	_hr_handle = handle;
}

uint16_t HrDataContainer::getHrValueHandle() {
	return _hr_value_handle;
}

void HrDataContainer::setHrValueHandle(uint16_t handle) {
	_hr_value_handle = handle;
}

// notification data
void HrDataContainer::addNotification(const uint8_t *val, uint16_t len) {
	QString hex = "";

	for (int i = 0; i < len; i++) {
		hex.append(intToHex(val[i]));
		hex.append(":");
	}

	qDebug() << "XXXX " << hex;

	// parse value, paying attention to the bit settings in the FLAGS field

	bool hr_data_format_8 = false;
	bool sensor_contact_detected = false;
	bool sensor_contact_feature_supported = false;
	bool energy_expended_feature_supported = false;
	bool rr_interval_data_present = false;
	uint8_t hr_measurement = 0;
	uint16_t energy_expended = 0;
	uint8_t num_rr_intervals = 0;
	uint16_t* rr_intervals;

	int index = 0;

	uint8_t flags = val[index++];
	qDebug() << "XXXX FLAGS=" << flags;

	hr_data_format_8 = ((flags & HEART_RATE_VALUE_FORMAT) != HEART_RATE_VALUE_FORMAT); // 0 means 8 bit, 1 means 16 bit
	sensor_contact_detected = ((flags & SENSOR_CONTACT_DETECTED) == SENSOR_CONTACT_DETECTED);
	sensor_contact_feature_supported = ((flags & SENSOR_CONTACT_FEATURE) == SENSOR_CONTACT_FEATURE);
	energy_expended_feature_supported = ((flags * ENERGY_EXPENDED_FEATURE) == ENERGY_EXPENDED_FEATURE);
	rr_interval_data_present = ((flags & RR_INTERVAL_DATA) == RR_INTERVAL_DATA);

	if (!hr_data_format_8) {
		qDebug() << "XXXX 16 bit heart rate measurement data encountered - we only support 8 bit values. Please unstrap your HR monitor from that frightened gerbil immediately!";
		return;
	}

	hr_measurement = val[index++];

	if (hr_measurement < _min_hr) {
		_min_hr = hr_measurement;
	}

	if (hr_measurement > _max_hr) {
		_max_hr = hr_measurement;
	}

	if (energy_expended_feature_supported) {
		energy_expended = val[index];
		index = index + 2;
	}

	if (rr_interval_data_present) {
		num_rr_intervals = (len - index) / 2; // rr interval data is uint16_t
		qDebug() << "XXXX copying with num_rr_intervals=" << num_rr_intervals;
		int num_bytes = num_rr_intervals * sizeof(uint16_t);
		qDebug() << "XXXX allocating num bytes=" << num_bytes;
		rr_intervals = (uint16_t*) malloc(num_rr_intervals * sizeof(uint16_t*));
		qDebug() << "XXXX copying num bytes=" << num_bytes << " to address " << &rr_intervals;
		memcpy(rr_intervals, (&val) + index, num_bytes);
		qDebug() << "XXXX memcpy done";
		QString data = "";
		uint16_t* rr_ptr = rr_intervals;
	}

	// store in the notifications vector

	_most_recent_notification = {Utilities::getSysTimeSeconds(), hr_data_format_8, sensor_contact_detected, sensor_contact_feature_supported, energy_expended_feature_supported, rr_interval_data_present, hr_measurement,energy_expended,num_rr_intervals,rr_intervals};

	if (_hr_notifications.size() == MAX_QUEUE_SIZE) {
		qDebug() << "XXXX queue full - dequeuing oldest notification data";
		hr_notification_t deleted_hr = _hr_notifications.dequeue();
		_total_hr = _total_hr = deleted_hr.hr_measurement;
	}

	_hr_notifications.enqueue(_most_recent_notification);
	_total_hr = _total_hr + hr_measurement;
	qDebug() << "XXXX notifications queue contains " << _hr_notifications.size() << " items";

	_avg_hr = _total_hr / _hr_notifications.size();

	QVariant hr_data(hr_measurement);
	emit heartRateData(hr_data);
}

void HrDataContainer::logMostRecentNotification() {
	qDebug() << "XXXX ---------------------------------------------------------------------";
	qDebug() << "XXXX timestamp=" << _most_recent_notification.timestamp;
	qDebug() << "XXXX hr_data_format_8=" << _most_recent_notification.hr_data_format_8;
	qDebug() << "XXXX sensor_contact_detected=" << _most_recent_notification.sensor_contact_detected;
	qDebug() << "XXXX sensor_contact_feature_supported=" << _most_recent_notification.sensor_contact_feature_supported;
	qDebug() << "XXXX energy_expended_feature_supported=" << _most_recent_notification.energy_expended_feature_supported;
	qDebug() << "XXXX rr_interval_data_present=" << _most_recent_notification.rr_interval_data_present;
	qDebug() << "XXXX hr_measurement=" << _most_recent_notification.hr_measurement;
	qDebug() << "XXXX energy_expended=" << _most_recent_notification.energy_expended;
	qDebug() << "XXXX num_rr_intervals=" << _most_recent_notification.num_rr_intervals;
	if (_most_recent_notification.num_rr_intervals > 0) {
		for (int i = 0; i < _most_recent_notification.num_rr_intervals; i++) {
			qDebug() << "XXXX rr_interval[" << i << "]=" << _most_recent_notification.rr_intervals[i];
		}
	}

}

QString HrDataContainer::intToHex(int decimal) {
	QString hexadecimal;
	hexadecimal.setNum(decimal, 16);
	return hexadecimal;
}

hr_notification_t HrDataContainer::getMostRecentNotification() {
	return _most_recent_notification;
}

QQueue<hr_notification_t> HrDataContainer::getNotifications() {
	return _hr_notifications;
}

int HrDataContainer::getMinHr() {
	return _min_hr;
}

int HrDataContainer::getMaxHr() {
	return _max_hr;
}

int HrDataContainer::getAvgHr() {
	return _avg_hr;
}
