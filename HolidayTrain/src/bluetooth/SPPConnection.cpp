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
#include "SPPConnection.hpp"

#include <btapi/btspp.h>

#include <stdio.h>
#include <QThread>
#include <QDebug>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

int SPPConnection::_sppInitCount(0);

/**
 * Create a connection and init the library if not already done.
 */
SPPConnection::SPPConnection(QObject * owner) :
		QObject(owner), _fd(-1), _readWorker(new QThread(this)) {
	++_sppInitCount;
	_readWorker->start();
	if (_sppInitCount == 1) {
		bt_spp_init();
	}
}

/**
 * Destroy a connection, deiniting the library if we were the last.
 */
SPPConnection::~SPPConnection() {
	--_sppInitCount;
	if (_sppInitCount == 0) {
		bt_spp_deinit();
	}
}

/**
 * Set the address of the peer.
 */
void SPPConnection::setAddress(QString address) {
	if (address != _address) {
		_address = address;
		emit addressChanged(_address);
	}
}

/**
 * Open a connection to the peer.
 */
void SPPConnection::open() {
	qDebug() << "SPPConnection::open";
	if (_address.length() == 0) {
		qDebug() << "No address";
		return;
	}
	if (_fd != -1) {
		qDebug() << "Already open";
		return;
	}
	QByteArray addr(_address.toAscii());
	QByteArray service("0x1101");
	_fd = bt_spp_open(addr.data(), service.data(), false);
	if (_fd == -1) {
		qDebug() << "Open failed";
		emit closed();
	} else {
		qDebug() << "Open success" << _fd;

		SPPReadWorker * readWorker = new SPPReadWorker(_fd);
		readWorker->moveToThread(_readWorker);

		connect(readWorker, SIGNAL(closed()), this, SIGNAL(closed()));
		connect(readWorker, SIGNAL(messageReceived(QString)), this,
				SIGNAL(message(QString)));
		QMetaObject::invokeMethod(readWorker, "read", Qt::QueuedConnection);
		emit opened();
	}
}

/**
 * Constant for sending carriage return/line feed.
 */
static char crlf[] = { 10, 13 };

/**
 * Send a message to the peer. The function inserts crlf between messages.
 */
void SPPConnection::sendMessage(QString message) {
	qDebug() << "Sending " << message;
	QMutexLocker lock(&_fdLock);
	if (_fd != -1) {
		QByteArray buffer(message.toAscii());
		qDebug() << "Des << size" << _fd << buffer.size();
		write(_fd, buffer.constData(), buffer.size());
		write(_fd, crlf, 2);
	}
}

/**
 * Close the connection.
 */
void SPPConnection::close() {
	if (_fd != -1) {
		bt_spp_close(_fd);
		_fd = -1;
		emit closed();
	}
}

/**
 * Read from the file. This must be called inside a thread - it blocks.
 */
void SPPReadWorker::read() {
	qDebug() << "READ <<< INSIDE READ <<<";
	char buffer[20000];
	int readLength = -1;
	do {
		readLength = ::read(_fd, buffer, 20000);
		if (readLength == -1) {
			qDebug() << "Closed";
			emit closed();
		} else {
			QByteArray temp(buffer, readLength);
			QString justIn(temp);
			emit messageReceived(justIn);
		}
	} while (readLength >= 0);
	qDebug() << "Left READ";
}
