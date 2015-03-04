/*
 * Copyright (c) 2014 BlackBerry Limited.
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

#include "SocketListener.hpp"

SocketListener *SocketListener::_instance;

SocketListener::SocketListener(QObject *parent)
	: QObject(parent)
	, _server(new QLocalServer(this))
	, _socket(NULL)
	, _ops(Operations::getInstance(this))
{
    QFile socketFilePath(_ops->getSocketServerPath());
    if (socketFilePath.exists()) {
        qDebug() << "SSSS Socket server signal file exists from previous instantiation - deleting " << _ops->getSocketServerPath() << endl;
        socketFilePath.remove();
    }
	if (QObject::connect(_server, SIGNAL(newConnection()),
			                this, SLOT(onNewConnection()))) {
		qDebug() << "SSSS Connected socket server signal" << endl;
		listen();
	} else {
		qDebug() << "SSSS Failed to connect socket server signal" << endl;
	}
}

SocketListener::~SocketListener() {
	if (_socket) {
		disconnectSocket();
		_socket->close();
		_socket->deleteLater();
	}
	if (_server) {
		_server->close();
		QObject::disconnect(_server, SIGNAL(newConnection()),
				               this, SLOT(onNewConnection()));
		_server->deleteLater();
	}
	_instance = NULL;
}

SocketListener *SocketListener::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new SocketListener(parent);
    }
    return _instance;
}

void SocketListener::listen()
{

    qDebug() << "SSSS SocketListener::listen() - _ops->getSocketServerPath() " << _ops->getSocketServerPath() << endl;

    qDebug() << "SSSS Start listening" << endl;

	_server->listen(_ops->getSocketServerPath());

    qDebug() << "SSSS QLocalServer info -            serverName() " << _server->serverName() << endl;
    qDebug() << "SSSS QLocalServer info -        fullServerName() " << _server->fullServerName() << endl;
    qDebug() << "SSSS QLocalServer info -           isListening() " << _server->isListening() << endl;
    qDebug() << "SSSS QLocalServer info - maxPendingConnections() " << _server->maxPendingConnections() << endl;
    qDebug() << "SSSS QLocalServer info - hasPendingConnections() " << _server->hasPendingConnections() << endl;
    qDebug() << "SSSS QLocalServer info -           serverError() " << _server->serverError() << endl;
    qDebug() << "SSSS QLocalServer info -           errorString() " << _server->errorString() << endl;

}

void SocketListener::onNewConnection()
{
	QLocalSocket *newSocket = _server->nextPendingConnection();

    qDebug() << "SSSS onNewConnection() - socket state=" << newSocket->state();

	if (newSocket->state() == QLocalSocket::ConnectedState) {
		qDebug() << "SSSS New connection from client";
	}

	if (!socketIsConnected()) {
		_socket = newSocket;
	} else {
		newSocket->close();
	}

	QObject::connect(_socket, SIGNAL(disconnected()),
			            this, SLOT(onDisconnected()));
	QObject::connect(_socket, SIGNAL(readyRead()),
                        this, SLOT(onReadyRead()));

	_ops->setGuiConnected(true);
}

void SocketListener::onDisconnected()
{
	qDebug() << "SSSS Disconnected" << endl;

	disconnectSocket();
	_socket->close();
	_socket->deleteLater();
	_socket = NULL;

	_ops->setGuiConnected(false);
}

void SocketListener::onReadyRead()
{
	QByteArray readData = _socket->readAll();
	qDebug() << "SSSS Read: " << readData.constData() << endl;
}

void SocketListener::disconnectSocket()
{
	qDebug() << "SSSS disconnect Socket" << endl;

	QObject::disconnect(_socket, SIGNAL(disconnected()),
			               this, SLOT(onDisconnected()));
	QObject::disconnect(_socket, SIGNAL(readyRead()),
                           this, SLOT(onReadyRead()));
}

void SocketListener::onMessageForUi(const QVariant &text)
{
	if (socketIsConnected()) {
		qDebug() << "SSSS Message for UI" << text.toString() << endl;
		_socket->write(text.toString().toLatin1().constData());
	} else {
		qDebug() << "SSSS Message for UI - socket not connected" << endl;
	}
}

bool SocketListener::socketIsConnected()
{
	return ((_socket != NULL) && (_socket->state() == QLocalSocket::ConnectedState));
}
