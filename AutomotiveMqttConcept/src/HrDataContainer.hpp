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
#ifndef HRDATACONTAINER_H
#define HRDATACONTAINER_H
#include <QMap>
#include <QQueue>
#include <QObject>
#include <QVariant>
#include <QString>
#include <btapi/btgatt.h>
#include "HrTypes.hpp"

class HrDataContainer: public QObject {
	Q_OBJECT

private:
	HrDataContainer();
	static HrDataContainer* _instance;
	HrDataContainer* _hrdc;
	unsigned long getSysTimeMs();
	static QString KEY_DEVICE_NAME;
	static QString KEY_DEVICE_ADDR;
	// notification data
	QQueue<hr_notification_t> _hr_notifications;
	hr_notification_t _most_recent_notification;
	int _min_hr;
	int _max_hr;
	int _avg_hr;
	long _total_hr;

public:
	static HrDataContainer* getInstance();
	QString _current_device_name;
	QString _current_device_addr;
	uint16_t _hr_handle;
	uint16_t _hr_value_handle;
	QMap<QString, QVariant> _hr_device;
	QList<QMap<QString, QVariant> > _list_of_hr_devices;
	void addHrDevice(char* device_name, char* device_addr, int device_class, int device_type, bool paired, bool encrypted);
	QList<QVariantList> getDeviceIdList();
	QList<QVariantMap> getDeviceList();
	void setHrHandle(uint16_t handle);
	uint16_t getHrHandle();
	void setHrValueHandle(uint16_t handle);
	uint16_t getHrValueHandle();

	// notification data
	hr_notification_t getMostRecentNotification();
	QQueue<hr_notification_t> getNotifications();
	void addNotification(const uint8_t *val, uint16_t len);
	void clearData();
	void logMostRecentNotification();
	QString intToHex(int decimal);
	int getMinHr();
	int getMaxHr();
	int getAvgHr();

// device data
	void clearDeviceList();
	Q_INVOKABLE	QString getCurrentDeviceName();
	Q_INVOKABLE	QString getCurrentDeviceAddr();
	Q_INVOKABLE	void setCurrentDeviceName(QString device_name);
	Q_INVOKABLE	void setCurrentDeviceAddr(QString device_addr);

private:
	signals:
	void heartRateData(const QVariant &rate);
};

#endif // ifndef HRDATACONTAINER_H
