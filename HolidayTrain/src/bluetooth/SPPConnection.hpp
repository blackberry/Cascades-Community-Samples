/*
 * SPPConnection.hpp
 *
 *  Created on: 2013-12-06
 *      Author: robwilliams
 */

#ifndef SPPCONNECTION_HPP_
#define SPPCONNECTION_HPP_

#include <QObject>
#include <QMutex>
#include <QMutexLocker>

/**
 * SPPConnection manages the Bluetooth SPP connection to a peer.
 */
class SPPConnection: public QObject {
Q_OBJECT
public:
	SPPConnection(QObject * owner = NULL);
	virtual ~SPPConnection();

	Q_PROPERTY (QString address READ address WRITE setAddress NOTIFY addressChanged)

	Q_SLOT
	void setAddress(QString);

	Q_INVOKABLE
	QString address() {
		return _address;
	}

	/**
	 * Open a connection to the configured remote address.
	 */
	Q_SLOT
	void open();

	/**
	 * Send a message to the peer.
	 */
	Q_SLOT
	void sendMessage(QString message);

	/**
	 * Close the connection with the peer.
	 */
	Q_SLOT
	void close();

signals:
	/**
	 * Signals that the connection has been opened.
	 */
	void opened();

	/**
	 * Signals that the connection has been closed.
	 */
	void closed();

	/**
	 * Signals that a message has be received.
	 */
	void message(QString);

	/**
	 * Signals that the address to connect with has been changed.
	 */
	void addressChanged(QString);

private:
	/**
	 * Address of the peer.
	 */
	QString _address;

	/**
	 * File that SPP connection is happening under.
	 */
	int _fd;

	/**
	 * Lock to protected _fd
	 */
	QMutex _fdLock;

	/**
	 * Count so we correctly init/deinit the library.
	 */
	static int _sppInitCount;

	/**
	 * Thread for performing read operations.
	 */
	QThread * _readWorker;

};

/**
 * The SPPReadWorker will handle the implementation of reading in another thread.
 */
class SPPReadWorker: public QObject {
Q_OBJECT
public:
	SPPReadWorker(int fd) {
		_fd = fd;
	}

	virtual ~SPPReadWorker() {

	}

	Q_SLOT
	void read();

signals:
	void messageReceived(QString);
	void closed();

private:
	int _fd;

};

#endif /* SPPCONNECTION_HPP_ */
