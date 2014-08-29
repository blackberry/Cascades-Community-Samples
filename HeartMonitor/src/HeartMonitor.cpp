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

#include "HeartMonitor.hpp"
#include "BluetoothHandler.hpp"
#include "HrDatabase.hpp"
#include "HrDataContainer.hpp"
#include "Timer.hpp"
#include "Utilities.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <btapi/btdevice.h>
#include <btapi/btgatt.h>
#include <btapi/btle.h>
#include <btapi/btspp.h>
#include <bb/cascades/databinding/groupdatamodel.h>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/ListView>

using namespace bb::cascades;

static AbstractPane *_root = 0;

HeartMonitor::HeartMonitor(bb::cascades::Application *app)
	: QObject(app)
	, _app(app)
	, _handler(new BluetoothHandler(this))
{
	qmlRegisterType<DeviceListing>();
	qmlRegisterType<LocalDeviceInfo>();
    qmlRegisterType<RemoteDeviceInfo>();
	qmlRegisterType<Timer>("CustomTimer", 1, 0, "Timer");

	_sessions = new SessionListing();

	HrDataContainer* hrdc = HrDataContainer::getInstance();
	HrDatabase* hrdb = HrDatabase::getInstance();

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
	qml->setContextProperty("_hrdc", hrdc);
	qml->setContextProperty("_hrdb", hrdb);
	qml->setContextProperty("_hrm", this);
	qml->setContextProperty("_scan", _handler);
	qml->setContextProperty("_sessions", _sessions);

	// create root object for the UI
	_root = qml->createRootObject<AbstractPane>();

	QObject::connect(hrdc, SIGNAL(heartRateData(QVariant)), this, SLOT(logHeartRate(QVariant)), Qt::QueuedConnection);

	// set created root object as a scene
	app->setScene(_root);

	QString os_version = Utilities::getOSVersion();

	qDebug() << "XXXX running on OS version " << os_version;

	if (hrdc->getCurrentDeviceAddr() != NULL) {
		monitorHeartRate(hrdc->getCurrentDeviceAddr(),hrdc->getCurrentDeviceName());
	} else {
	    scanForDevices();
	}
	HrDatabase* db = HrDatabase::getInstance();
	db->initDatabase();

}

HeartMonitor::~HeartMonitor() {
}

void HeartMonitor::monitorHeartRate(QString device_addr, QString device_name) {
	qDebug() << "YYYY monitoring heart rate using device " << device_addr;

	HrDataContainer* hrdc = HrDataContainer::getInstance();
	hrdc->setCurrentDeviceAddr(device_addr);
	hrdc->setCurrentDeviceName(device_name);

	_future = new QFuture<void>;
	_watcher = new QFutureWatcher<void>;
	qDebug() << "YYYY locking notification receiver thread mutex";
	_mutex.lock();
	qDebug() << "YYYY notification receiver thread mutex locked OK";
	*_future = QtConcurrent::run(_handler, &BluetoothHandler::receiveHrNotifications);
	_watcher->setFuture(*_future);
	QObject::connect(_watcher, SIGNAL(finished()), this, SLOT(finishedReceiving()));

}

void HeartMonitor::stopMonitoringHeartRate() {
	qDebug() << "YYYY stop monitoring heart rate requested";
	_handler->stopHrNotifications();
}

void HeartMonitor::finishedReceiving() {
	qDebug() << "YYYY notification receiver thread has finished running";
	_mutex.unlock();
}

void HeartMonitor::logHeartRate(const QVariant &rate) {
	qDebug() << "YYYY logging heart rate now " << rate;
	QMetaObject::invokeMethod(_root, "logHeartRate", Q_ARG(QVariant, rate));
}

void HeartMonitor::hrDevicesFound() {
	qDebug() << "YYYY HR devices were apparently found!";
}

int HeartMonitor::getMinHr() {
	HrDataContainer* hrdc = HrDataContainer::getInstance();
	return hrdc->getMinHr();
}

int HeartMonitor::getMaxHr() {
	HrDataContainer* hrdc = HrDataContainer::getInstance();
	return hrdc->getMaxHr();
}

int HeartMonitor::getAvgHr() {
	HrDataContainer* hrdc = HrDataContainer::getInstance();
	return hrdc->getAvgHr();
}

void HeartMonitor::saveSession(QString session_name) {
	HrDatabase* db = HrDatabase::getInstance();
	bool db_ok = db->saveSession(session_name);
	if (!db_ok) {
		qDebug() << "XXXX ERROR:database saveSession operation failed";
	}
}

void HeartMonitor::showTab(int tab_index)
{
    QMetaObject::invokeMethod(_root, "onShowTab", Q_ARG(QVariant, tab_index));
}

void HeartMonitor::startActivityIndicator() {
    QMetaObject::invokeMethod(_root, "startActivityIndicator");
}

void HeartMonitor::stopActivityIndicator() {
    QMetaObject::invokeMethod(_root, "stopActivityIndicator");
}

void HeartMonitor::scanForDevices() {
    showTab(1);
    startActivityIndicator();
    _handler->discover();
    stopActivityIndicator();
}
