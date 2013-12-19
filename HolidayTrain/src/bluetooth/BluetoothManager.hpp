/*
 * BluetoothManager.hpp
 *
 *  Created on: 2013-12-05
 *      Author: robwilliams
 */

#ifndef BLUETOOTHMANAGER_HPP_
#define BLUETOOTHMANAGER_HPP_

#include <bb/cascades/ArrayDataModel>

#include <QObject>
#include <QThread>

#include <btapi/btdevice.h>

#include "SPPConnection.hpp"

class BluetoothManager: public QObject {
Q_OBJECT
public:
	BluetoothManager(QObject * owner = NULL);
	virtual ~BluetoothManager();

	Q_PROPERTY (bb::cascades::DataModel * devices READ devices NOTIFY devicesChanged)

	Q_INVOKABLE
	bb::cascades::DataModel * devices() {
		return _devices;
	}

	Q_SLOT
	void refreshDevices();

	Q_SLOT
	void startInquiry();

	Q_PROPERTY (bool inquiryInProgress READ inquiryInProgress NOTIFY inquiryInProgressChanged)

	Q_PROPERTY (SPPConnection * connection READ connection NOTIFY connectionChanged)

	Q_INVOKABLE
	SPPConnection * connection() {
		return _connection;
	}

	Q_INVOKABLE
	bool inquiryInProgress();

	Q_INVOKABLE
	void connectDevice(QString address);

	Q_INVOKABLE
	void pair(QString address);

	Q_SLOT
	void callback(QVariantList);

signals:
	void devicesChanged(bb::cascades::DataModel *);
	void inquiryInProgressChanged(bool);
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
