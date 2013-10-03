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

#ifndef HeartMonitor_HPP_
#define HeartMonitor_HPP_

#include <QObject>
#include <QString>
#include <QSettings>
#include <QFuture>
#include <QMutex>
#include <QFutureWatcher>
#include "BluetoothHandler.hpp"
#include "SessionListing.hpp"
#include <MqttClient.h>

using mqtt_client::MqttClient;
using mqtt_client::MqttError;

class HeartMonitor: public QObject {
Q_OBJECT

public:
	HeartMonitor(bb::cascades::Application *app);
	virtual ~HeartMonitor();
	Q_PROPERTY(
	        QString mqtt_host
	        READ getMqttHost
	        WRITE setMqttHost
	        NOTIFY signalMqttHostChanged)
    Q_PROPERTY(
            QString mqtt_port
            READ getMqttPort
            WRITE setMqttPort
            NOTIFY signalMqttPortChanged)
	Q_INVOKABLE	void stopMonitoringHeartRate();
	Q_INVOKABLE void setConnectionArgs(QString hostName,QString portNumber);

	bb::cascades::Application* _app;

    QString getMqttHost();
    void setMqttHost(QString host);
    QString getMqttPort();
    void setMqttPort(QString port);

    signals:
        void signalMqttHostChanged();
        void signalMqttPortChanged();


private:
	void logServiceNames(char** p);
	BluetoothHandler* _handler;
	SessionListing* _sessions;
	QFuture<void> *_future;
	QFutureWatcher<void> *_watcher;
	QMutex _mutex;
	mqtt_client::MqttClient *_mqttClient;
    DeviceListing *_deviceListing;

    static QString KEY_MQTT_HOST;
    static QString KEY_MQTT_PORT;

    QString _mqtt_host;
    QString _mqtt_port;

public slots:
	void logHeartRate(const QVariant &message);
	void hrDevicesFound();
	void finishedReceiving();
    void monitorHeartRate();
};

#endif /* HeartMonitor_HPP_ */
