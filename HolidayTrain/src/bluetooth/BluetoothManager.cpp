/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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
#include "BluetoothManager.hpp"
#include <QDebug>
#include <QList>
#include <QMutex>
#include <QMutexLocker>

/**
 * This mutex protects the list of managers.
 */
static QMutex managersMutex;

/**
 * This list of managers tracks all instantiated BluetoothManager instances.
 */
static QList<BluetoothManager*> managers;

using namespace bb::cascades;

/**
 * Track how many times we've called init so we can tidy up nicely.
 */
int BluetoothManager::bluetoothInitCount = 0;

/**
 * This static method is called directly from C. It then iterates all instantiated
 * BluetoothManagers and sends them the events.
 */
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

/**
 * Construct a BluetoothManager.
 */
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

/**
 * Delete a BluetoothManager.
 */
BluetoothManager::~BluetoothManager() {
	QMutexLocker locker(&managersMutex);
	--bluetoothInitCount;
	if (bluetoothInitCount == 0) {
		bt_device_deinit();
	}
	managers.removeAll(this);
}

/**
 * Refresh the list of devices.
 *
 * Why's this so long??? Code in here (should) prevent the screen from flickering by
 * making sure that objects are replaced in the list - not appended / removed.
 */
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

/**
 * Check to see if an inquiry is already in progress.
 */
bool BluetoothManager::inquiryInProgress() {
	return _inquiryInProgress;
}

/**
 * Connect to the given device.
 */
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

/**
 * Track that a connection was opened.
 */
void BluetoothManager::connectionOpened() {
	qDebug() << "Connection opened";
}

/**
 * Track that a connection was closed.
 */
void BluetoothManager::connectionClosed() {
	qDebug() << "Connection closed";
	if (_connection != NULL) {
		_connection->deleteLater();
		_connection = NULL;
		emit connectionChanged(NULL);
	}
}

/**
 * Receive a message from the connection.
 */
void BluetoothManager::connectionMessage(QString message) {
	qDebug() << "Message" << message;
}

/**
 * Start an inquery - using a worker in a different thread.
 */
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

/**
 * Record that an inquiry has finished.
 */
void BluetoothManager::inquiryFinished() {
	_inquiryInProgress = false;
	emit inquiryInProgressChanged(false);
	refreshDevices();
}

/**
 * Hear about the callback that happened in the library. Lazily ignoring
 * the parameters and just updated the device list.
 */
void BluetoothManager::callback(QVariantList) {
	refreshDevices();
}

/**
 * Start a pairing in another thread with a worker.
 */
void BluetoothManager::pair(QString address) {
	BluetoothPairWorker * bpw = new BluetoothPairWorker();
	bpw->moveToThread(_workerThread);

	connect(bpw, SIGNAL(finished()), this, SLOT(refreshDevices()));
	connect(bpw, SIGNAL(finished()), bpw, SLOT(deleteLater()));

	QMetaObject::invokeMethod(bpw, "pair", Qt::QueuedConnection,
			Q_ARG(QString,address));
}

/**
 * Pair from the worker. bt_rdev_get_device() can take a while.
 */
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

/**
 * Inquire from the worker. bt_disc_start_inqury() can take a while.
 */
void BluetoothInquiryWorker::inquire() {
	qDebug() << "Inquiry start";
	bt_disc_start_inquiry(BT_INQUIRY_GIAC);
	qDebug() << "Inquiry end";
	emit finished();
}
