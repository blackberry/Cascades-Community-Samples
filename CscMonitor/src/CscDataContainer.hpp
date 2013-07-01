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
#ifndef CSCDATACONTAINER_H
#define CSCDATACONTAINER_H
#include <QMap>
#include <QQueue>
#include <QObject>
#include <QVariant>
#include <QString>
#include <btapi/btgatt.h>
#include "CscTypes.hpp"
#include "SessionLogger.hpp"

class CscDataContainer: public QObject {
	Q_OBJECT

private:
	CscDataContainer();
	static CscDataContainer* _instance;
	CscDataContainer* _cscdc;
	unsigned long getSysTimeMs();
	static QString KEY_DEVICE_NAME;
	static QString KEY_DEVICE_ADDR;

	// notification data
	QQueue<csc_notification_t> _csc_notifications;
	csc_notification_t _most_recent_notification;

	// CSC values
	QQueue<csc_values_t> _csc_values;
	csc_values_t _most_recent_csc_values;

	int _min_wheel_rpm;
	int _max_wheel_rpm;
	int _avg_wheel_rpm;
	long _total_wheel_rpm;

	int _min_crank_rpm;
	int _max_crank_rpm;
	int _avg_crank_rpm;
	long _total_crank_rpm;

	bool _log_to_file;
	void sessionLog(QString data);

public:
	static CscDataContainer* getInstance();
	QString _current_device_name;
	QString _current_device_addr;
	uint16_t _csc_handle;
	uint16_t _csc_value_handle;
	QMap<QString, QVariant> _csc_device;
	QList<QMap<QString, QVariant> > _list_of_csc_devices;
	void addCscDevice(char* device_name, char* device_addr, int device_class, int device_type, bool paired, bool encrypted);
	QList<QVariantList> getDeviceIdList();
	QList<QVariantMap> getDeviceList();
	void setCscHandle(uint16_t handle);
	uint16_t getCscHandle();
	void setCscValueHandle(uint16_t handle);
	uint16_t getCscValueHandle();

	// notification data
	csc_notification_t getMostRecentNotification();
	QQueue<csc_notification_t> getNotifications();
	void addNotification(const uint8_t *val, uint16_t len);
	void resetData();
	void logMostRecentNotification();
	QString intToHex(int decimal);

	// CSC values calculated from the notifications data
	csc_values_t getMostRecentCscValues();
	QQueue<csc_values_t> getCscValuess();
	void logMostRecentValues();

	Q_INVOKABLE	int getMinWheelRpm();
	Q_INVOKABLE	int getMaxWheelRpm();
	Q_INVOKABLE	int getAvgWheelRpm();

	Q_INVOKABLE	int getMinCrankRpm();
	Q_INVOKABLE	int getMaxCrankRpm();
	Q_INVOKABLE	int getAvgCrankRpm();

// device data
	void clearDeviceList();
	Q_INVOKABLE	QString getCurrentDeviceName();
	Q_INVOKABLE	QString getCurrentDeviceAddr();
	Q_INVOKABLE	void setCurrentDeviceName(QString device_name);
	Q_INVOKABLE	void setCurrentDeviceAddr(QString device_addr);

	Q_INVOKABLE	void setLogToFile(bool switch_setting);

private:
signals:
	void cscData(const QVariant &wheel_revolutions, const QVariant &crank_revolutions);
};

#endif // ifndef CSCDATACONTAINER_H
