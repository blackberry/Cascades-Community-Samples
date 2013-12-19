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
	/**
	 * Constructor requires open file handle.
	 */
	SPPReadWorker(int fd) {
		_fd = fd;
	}

	/**
	 * Destructor is very simple.
	 */
	virtual ~SPPReadWorker() {
	}

	/**
	 * read is the loop that will get data from the file.
	 */
	Q_SLOT
	void read();

signals:
	/**
	 * Emitted when a message is received. Keep in mind that this could easilly be
	 * a partial line.
	 */
	void messageReceived(QString);

	/**
	 * Emitted when the connection is closed.
	 */
	void closed();

private:
	/**
	 * The file descriptor which will be read from.
	 */
	int _fd;

};

#endif /* SPPCONNECTION_HPP_ */
