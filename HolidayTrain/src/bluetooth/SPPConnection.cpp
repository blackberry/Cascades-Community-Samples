/*
 * SPPConnection.cpp
 *
 *  Created on: 2013-12-06
 *      Author: robwilliams
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

SPPConnection::SPPConnection(QObject * owner) :
		QObject(owner), _readWorker(new QThread(this)), _fd(-1) {
	++_sppInitCount;
	_readWorker->start();
	if (_sppInitCount == 1) {
		int rc = bt_spp_init();
	}
}

SPPConnection::~SPPConnection() {
	--_sppInitCount;
	if (_sppInitCount == 0) {
		bt_spp_deinit();
	}
}

void SPPConnection::setAddress(QString address) {
	if (address != _address) {
		_address = address;
		emit addressChanged(_address);
	}
}

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
	_fd = bt_spp_open(addr.data(),"0x1101",false);
	if (_fd==-1) {
		qDebug() << "Open failed";
		emit closed();
	} else {
		qDebug() << "Open success" << _fd;

//		struct termios attributes;
//		QString error;
//
//		tcgetattr(_fd, &attributes);
//		if (cfsetispeed(&attributes, B38400) < 0) {
//			error += "Could not set input speed. ";
//		}
//		if (cfsetospeed(&attributes, B38400) < 0) {
//			error += "Could not set output speed. ";
//		}
//		if (tcsetattr(_fd, TCSANOW, &attributes) < 0) {
//			error += "Could not apply speeds settings. ";
//		}
//
//		if (error.size()!=0) {
//			qDebug() << "ERROR:" << error;
//		}

		SPPReadWorker * readWorker = new SPPReadWorker(_fd);
		readWorker->moveToThread(_readWorker);
		QMetaObject::invokeMethod(readWorker,"read",Qt::QueuedConnection);
		emit opened();
	}
}

static char crlf[] = { 10, 13 };

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

void SPPConnection::close() {
	if (_fd!=-1) {
		bt_spp_close(_fd);
		_fd = -1;
		emit closed();
	}
}

void SPPReadWorker::read() {
	qDebug() << "READ <<< INSIDE READ <<<";
	char buffer[20000];
	int readLength = -1;
	do {
		readLength = ::read(_fd,buffer,20000);
		if (readLength==-1) {
			emit closed();
		} else {
			qDebug() << "Just read" << readLength << "bytes from the SPP connection.";
			QByteArray temp(buffer,readLength);
			QString justIn(temp);
			qDebug() << justIn;

			write(_fd,buffer,readLength);
		}
	} while(readLength>=0);
	qDebug() << "Left READ";
}
