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
#ifndef BLUETOOTHMANAGER_HPP_
#define BLUETOOTHMANAGER_HPP_

#include <bb/cascades/ArrayDataModel>

#include <QObject>
#include <QThread>

#include <btapi/btdevice.h>

#include "SPPConnection.hpp"

/**
 * The BluetoothManager allows us to see inrange, paired and connected devices.
 */
class BluetoothManager: public QObject {
Q_OBJECT
public:
	BluetoothManager(QObject * owner = NULL);
	virtual ~BluetoothManager();

	/**
	 * This DataModel contains the known devices.
	 */
	Q_PROPERTY (bb::cascades::DataModel * devices READ devices NOTIFY devicesChanged)

	/**
	 * This DataModel contains the known devices.
	 */
	Q_INVOKABLE
	bb::cascades::DataModel * devices() {
		return _devices;
	}

	/**
	 * Refresh the list of devices (for handled state changes)
	 */
	Q_SLOT
	void refreshDevices();

	/**
	 * Ask the Bluetooth systems to check for devices. This is slow (but asynchronous)
	 */
	Q_SLOT
	void startInquiry();

	/**
	 * The inquiryInProgress property lets us know if an inquiry is already running.
	 */
	Q_PROPERTY (bool inquiryInProgress READ inquiryInProgress NOTIFY inquiryInProgressChanged)

	Q_INVOKABLE
	bool inquiryInProgress();

	/**
	 * The SPP connection records the current connection to a peer.
	 *
	 * If you were going to expand this class for multiple connections - this property
	 *
	 */
	Q_PROPERTY (SPPConnection * connection READ connection NOTIFY connectionChanged)

	Q_INVOKABLE
	SPPConnection * connection() {
		return _connection;
	}

	/**
	 * Attempt to connect to the given address.
	 */
	Q_INVOKABLE
	void connectDevice(QString address);

	/**
	 * Attempt to pair with the given address.
	 */
	Q_INVOKABLE
	void pair(QString address);

	/**
	 * Used to handle callback from C.
	 */
	Q_SLOT
	void callback(QVariantList);

signals:
	/**
	 * The datamodel has changed. (Not the content - but the actual model)
	 */
	void devicesChanged(bb::cascades::DataModel *);

	/**
	 * An inquiry has started or finished.
	 */
	void inquiryInProgressChanged(bool);

	/**
	 * A connection is open or cloesd. (NULL is used for no connection)
	 */
	void connectionChanged(SPPConnection *);

protected:
	Q_SLOT
	void inquiryFinished();

	Q_SLOT
	void connectionOpened();

	Q_SLOT
	void connectionClosed();

	Q_SLOT
	void connectionMessage(QString);

private:
	static void bluetoothCallback(const int event, const char * bt_addr,
			const char * event_data);
	static int bluetoothInitCount;

	bool _inquiryInProgress;

	int _iterationNumber;

	bb::cascades::ArrayDataModel * _devices;
	bt_device_cb _callbackInfo;

	QThread * _workerThread;

	SPPConnection * _connection;
};

/**
 * Bluetooth pairing takes time - so is handled in a different thread by this worker class.
 */
class BluetoothPairWorker: public QObject {
Q_OBJECT
public:
	BluetoothPairWorker() {
	}
	virtual ~BluetoothPairWorker() {
	}

	Q_SLOT
	void pair(QString address);

signals:
	void finished();
};

/**
 * Bluetooth inquiry takes time - so is handled in a different thread by this worker class.
 */
class BluetoothInquiryWorker: public QObject {
Q_OBJECT
public:
	BluetoothInquiryWorker() {
	}
	virtual ~BluetoothInquiryWorker() {
	}

	Q_SLOT
	void inquire();

signals:
	void finished();
};

#endif /* BLUETOOTHMANAGER_HPP_ */
