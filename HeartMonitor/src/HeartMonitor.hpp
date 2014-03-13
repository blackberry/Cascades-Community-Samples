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

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class HeartMonitor: public QObject {
Q_OBJECT

public:
	HeartMonitor(bb::cascades::Application *app);
	virtual ~HeartMonitor();
	Q_INVOKABLE	void monitorHeartRate(QString device_addr, QString device_name);
	Q_INVOKABLE	void stopMonitoringHeartRate();
	Q_INVOKABLE	void saveSession(QString session_name);
	Q_INVOKABLE	int getMinHr();
	Q_INVOKABLE	int getMaxHr();
	Q_INVOKABLE	int getAvgHr();
    Q_INVOKABLE void scanForDevices();
	bb::cascades::Application* _app;

private:
	void logServiceNames(char** p);
	BluetoothHandler* _handler;
	SessionListing* _sessions;
	QFuture<void> *_future;
	QFutureWatcher<void> *_watcher;
	QMutex _mutex;
    void showTab(int tab_index);
    void startActivityIndicator();
    void stopActivityIndicator();

public slots:
	void logHeartRate(const QVariant &message);
	void hrDevicesFound();
	void finishedReceiving();
};

#endif /* HeartMonitor_HPP_ */
