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

class SPPConnection: public QObject {
Q_OBJECT
public:
	SPPConnection(QObject * owner=NULL);
	virtual ~SPPConnection();

	Q_PROPERTY (QString address READ address WRITE setAddress NOTIFY addressChanged)
	;

	Q_SLOT
	void setAddress(QString);Q_INVOKABLE

	Q_INVOKABLE
	QString address() {
		return _address;
	}

	Q_SLOT
	void open();

	Q_SLOT
	void sendMessage(QString message);

	Q_SLOT
	void close();

signals:
	void opened();
	void closed();

	void message(QString);

	void addressChanged(QString);

private:
	QString _address;
	int _fd;
	QMutex _fdLock;
	static int _sppInitCount;
	QThread * _readWorker;

};

class SPPReadWorker: public QObject {
Q_OBJECT
public:
	SPPReadWorker(int fd) {
		_fd = fd;
	}

	virtual ~SPPReadWorker() {

	}

	Q_SLOT void read();

signals:
	void messageReceived(QString);
	void closed();

private:
	int _fd;

};

#endif /* SPPCONNECTION_HPP_ */
