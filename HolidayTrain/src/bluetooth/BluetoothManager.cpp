/*
 * BluetoothManager.cpp
 *
 *  Created on: 2013-12-05
 *      Author: robwilliams
 */

#include "BluetoothManager.hpp"
#include <QDebug>
#include <QList>
#include <QMutex>
#include <QMutexLocker>

static QMutex managersMutex;
static QList<BluetoothManager*> managers;

using namespace bb::cascades;

int BluetoothManager::bluetoothInitCount = 0;

void BluetoothManager::bluetoothCallback(const int a, const char * b,
		const char * c) {
	QMutexLocker locker(&managersMutex);

	qDebug() << "btcb" << a << b << c;
	QVariantList args;
	args << a;
	args << b;
	args << c;
	for (int i = 0; i < managers.size(); ++i) {
		QMetaObject::invokeMethod(managers[i], "callback", Qt::QueuedConnection,
				Q_ARG(QVariantList,args));
	}
}

BluetoothManager::BluetoothManager(QObject * owner) :
		QObject(owner), _inquiryInProgress(false), _devices(
				new ArrayDataModel(this)), _workerThread(new QThread(this)), _connection(
				NULL) {
	QMutexLocker locker(&managersMutex);
	managers << this;
	++bluetoothInitCount;
	_workerThread->start();
	if (bluetoothInitCount == 1) {
		int rc = bt_device_init(bluetoothCallback);
		if (rc != 0) {
			qDebug() << "Error" << rc;
		} else {
			qDebug() << "BT init";
		}
	}
	refreshDevices();
}

BluetoothManager::~BluetoothManager() {
	QMutexLocker locker(&managersMutex);
	--bluetoothInitCount;
	if (bluetoothInitCount == 0) {
		bt_device_deinit();
	}
	managers.removeAll(this);
}

void BluetoothManager::refreshDevices() {
	int count(0);
	++_iterationNumber;

	qDebug() << "Start retrieve";
	bt_remote_device_t ** devices = bt_disc_retrieve_devices(BT_DISCOVERY_ALL,
			&count);
	if (devices != NULL) {
		qDebug() << "There are" << count << "devices";

		QString connected("");
		if (_connection != NULL) {
			connected = _connection->address();
		}

		for (int i = 0; i < count; ++i) {
			QVariantMap device;
			device["type"] = "device";
			char buffer[200];
			int rc = bt_rdev_get_address(devices[i], buffer);
			if (rc == 0) {
				device["address"] = buffer;
				device["connected"] = device["address"] == connected;
				rc = bt_rdev_get_friendly_name(devices[i], buffer,
						sizeof(buffer));
				if (rc == 0) {
					device["name"] = buffer;

					bool paired;

					rc = bt_rdev_is_paired(devices[i], &paired);
					if (rc == 0) {
						device["paired"] = paired;
					}
				}
			}
			if (rc == 0) {
				device["iterationNumber"] = _iterationNumber;
				qDebug() << "Device" << device;

				bool replaced(false);

				for (int j = 0; j < _devices->size(); ++j) {
					QVariantMap current(_devices->value(j).toMap());
					if (current["address"] == device["address"]) {
						qDebug() << "REplaced";
						replaced = true;
						_devices->replace(j, device);
						break;
					}
				}

				if (!replaced) {
					qDebug() << "Added";
					_devices->append(device);
				}
			}
		}

		for (int i = 0; i < _devices->size(); ++i) {
			QVariantMap current(_devices->value(i).toMap());
			if (current["iterationNumber"].toInt() != _iterationNumber) {
				qDebug() << "Removed";
				_devices->removeAt(i);
				--i;
			}
		}

		bt_rdev_free_array(devices);
	} else {
		qDebug() << "Problem getting devices";
	}
	qDebug() << "End retrieve";
}

bool BluetoothManager::inquiryInProgress() {
	return _inquiryInProgress;
}

void BluetoothManager::connectDevice(QString address) {
	qDebug() << "ConnectDevice" << address;
	if (_connection == NULL) {
		_connection = new SPPConnection(this);

		connect(_connection, SIGNAL(opened()), this, SLOT(connectionOpened()));
		connect(_connection, SIGNAL(closed()), this, SLOT(connectionClosed()));
		connect(_connection, SIGNAL(message(QString)), this,
				SLOT(connectionMessage(QString)));

		_connection->setAddress(address);
		_connection->open();

		emit connectionChanged(_connection);
	} else {
		qDebug() << "_connection != NULL";
	}
}

void BluetoothManager::connectionOpened() {

	qDebug() << "Connection opened";
}

void BluetoothManager::connectionClosed() {
	qDebug() << "Connection closed";
	if (_connection != NULL) {
		_connection->deleteLater();
		_connection = NULL;
		emit connectionChanged(NULL);
	}
}

void BluetoothManager::connectionMessage(QString message) {
	qDebug() << "Message" << message;
}

void BluetoothManager::startInquiry() {
	if (_inquiryInProgress) {
		return;
	}

	_inquiryInProgress = true;
	BluetoothInquiryWorker * biw = new BluetoothInquiryWorker();
	biw->moveToThread(_workerThread);

	connect(biw, SIGNAL(finished()), this, SLOT(inquiryFinished()));
	connect(biw, SIGNAL(finished()), biw, SLOT(deleteLater()));

	QMetaObject::invokeMethod(biw, "inquire", Qt::QueuedConnection);

	emit inquiryInProgressChanged(true);
}

void BluetoothManager::inquiryFinished() {
	_inquiryInProgress = false;
	emit inquiryInProgressChanged(false);
	refreshDevices();
}

void BluetoothManager::callback(QVariantList) {
	refreshDevices();
}

void BluetoothManager::pair(QString address) {
	BluetoothPairWorker * bpw = new BluetoothPairWorker();
	bpw->moveToThread(_workerThread);

	connect(bpw, SIGNAL(finished()), this, SLOT(refreshDevices()));
	connect(bpw, SIGNAL(finished()), bpw, SLOT(deleteLater()));

	QMetaObject::invokeMethod(bpw, "pair", Qt::QueuedConnection,
			Q_ARG(QString,address));
}

void BluetoothPairWorker::pair(QString address) {
	QByteArray addr(address.toAscii());
	qDebug() << "Get device";
	bt_remote_device_t * device(bt_rdev_get_device(addr.constData()));
	qDebug() << "Pair start";
	bt_rdev_pair(device);
	qDebug() << "Pair end";
	bt_rdev_free(device);
	emit finished();
}

void BluetoothInquiryWorker::inquire() {
	qDebug() << "Inquiry start";
	bt_disc_start_inquiry(BT_INQUIRY_GIAC);
	qDebug() << "Inquiry end";
	emit finished();
}
