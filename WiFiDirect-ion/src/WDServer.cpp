/* Copyright (c) 2012 Research In Motion Limited.
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

#include "WDServer.hpp"


WDServer::WDServer(QObject *parent) :
		QObject(parent)
{
	mServer = new QTcpServer(this);
	connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void WDServer::listen()
{
	qDebug() << "Starting to listen...";
	mServer->listen(QHostAddress::Any, 4554);
	qDebug() << "Listening...";
}

void WDServer::newConnection()
{
	qDebug() << "Received connection from operator.";

	mSocket = mServer->nextPendingConnection();

	connect(mSocket, SIGNAL(disconnected()), this, SLOT(conDisconnected()));
	connect(mSocket, SIGNAL(readyRead()), this, SLOT(readCommand()));
	connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

void WDServer::readCommand()
{
	while (mSocket->canReadLine())
	{
		QByteArray commandByte = mSocket->readLine();
		QString command = QString(commandByte);

		qDebug() << "Received command: " << command;

		emit commandReceived(command);
	}
}

void WDServer::conDisconnected()
{
	emit connectionLost();
	qDebug() << "Server:  Socket connection closed.";
	disconnect(mSocket, SIGNAL(disconnected()));
	disconnect(mSocket, SIGNAL(readyRead()));
	disconnect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	mSocket->deleteLater();

}


void WDServer::doDisconnect()
{
	if(mSocket->state() == QTcpSocket::ConnectedState)
	{
		mSocket->disconnectFromHost();
	}

	mServer->close();
}


void WDServer::socketError(QAbstractSocket::SocketError theError)
{
	qDebug() << "Socket Error! " << theError;
}
