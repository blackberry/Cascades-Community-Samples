// Default empty project template
#include "CscMonitor.hpp"
#include "BluetoothHandler.hpp"
#include "CscDataContainer.hpp"
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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

static AbstractPane *_root = 0;

CscMonitor::CscMonitor(bb::cascades::Application *app) :
		QObject(app), _app(app), _handler(new BluetoothHandler(this)) {


	qDebug() << "XXXX ====================================================================================================";

	qmlRegisterType<DeviceListing>();
	qmlRegisterType<LocalDeviceInfo>();
	qmlRegisterType<RemoteDeviceInfo>();
	qmlRegisterType<Timer>("CustomTimer", 1, 0, "Timer");

	CscDataContainer* cscdc = CscDataContainer::getInstance();

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
	qml->setContextProperty("_cscdc", cscdc);
	qml->setContextProperty("_cscm", this);
	qml->setContextProperty("_scan", _handler);

	// create root object for the UI
	_root = qml->createRootObject<AbstractPane>();

	QObject::connect(cscdc, SIGNAL(cscData(QVariant,QVariant)), this, SLOT(logCscData(QVariant,QVariant)), Qt::QueuedConnection);

	// set created root object as a scene
	app->setScene(_root);

	QString os_version = Utilities::getOSVersion();

	qDebug() << "XXXX running on OS version " << os_version;

	if (cscdc->getCurrentDeviceAddr() != NULL) {
		monitor(cscdc->getCurrentDeviceAddr(), cscdc->getCurrentDeviceName());
    } else {
        scanForDevices();
    }
}

void CscMonitor::monitor(QString device_addr, QString device_name) {
	qDebug() << "YYYY monitoring CSC data using device " << device_addr;

	CscDataContainer* cscdc = CscDataContainer::getInstance();
	cscdc->setCurrentDeviceAddr(device_addr);
	cscdc->setCurrentDeviceName(device_name);

	_future = new QFuture<void>;
	_watcher = new QFutureWatcher<void>;
	qDebug() << "YYYY locking notification receiver thread mutex";
	_mutex.lock();
	qDebug() << "YYYY notification receiver thread mutex locked OK";
	*_future = QtConcurrent::run(_handler, &BluetoothHandler::receiveCscNotifications);
	_watcher->setFuture(*_future);
	QObject::connect(_watcher, SIGNAL(finished()), this, SLOT(finishedReceiving()));

}

void CscMonitor::stopMonitoring() {
	qDebug() << "YYYY stop monitoring requested";
	_handler->stopCscNotifications();
}

void CscMonitor::finishedReceiving() {
	qDebug() << "YYYY notification receiver thread has finished running";
	_mutex.unlock();
}

void CscMonitor::logCscData(const QVariant &wheel,const QVariant &crank) {
	QString wheel_string = wheel.toString();
	QString crank_string = crank.toString();
	qDebug() << "YYYY logging CSC data now: wheel=" << wheel_string << " , crank=" << crank_string;
	QMetaObject::invokeMethod(_root, "logCscData", Q_ARG(QVariant, wheel), Q_ARG(QVariant, crank));
}

void CscMonitor::cscDevicesFound() {
	qDebug() << "YYYY CSC devices were found!";
}

void CscMonitor::showTab(int tab_index)
{
    QMetaObject::invokeMethod(_root, "onShowTab", Q_ARG(QVariant, tab_index));
}

void CscMonitor::startActivityIndicator() {
    QMetaObject::invokeMethod(_root, "startActivityIndicator");
}

void CscMonitor::stopActivityIndicator() {
    QMetaObject::invokeMethod(_root, "stopActivityIndicator");
}

void CscMonitor::scanForDevices() {
    showTab(1);
    startActivityIndicator();
    _handler->discover();
    stopActivityIndicator();
}
