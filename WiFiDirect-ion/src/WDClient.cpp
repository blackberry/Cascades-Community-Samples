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

#include "WDClient.hpp"

WDClient::WDClient(QObject *parent) :
		QObject(parent)
{

	mSocket = new QTcpSocket(this);
	connect(mSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

void WDClient::connectToAuto(QString autoAddress)
{
	qDebug() << "Connecting to: " << autoAddress;
	QHostAddress hostAddr = QHostAddress(autoAddress);
	mSocket->connectToHost(hostAddr, 4554);
}

void WDClient::sendCommand(QString command)
{
	if(mSocket->state() == QTcpSocket::ConnectedState)
	{
		mSocket->write(command.toLatin1());
		mSocket->flush();
	}
}

void WDClient::connected()
{
	qDebug() << "Connected to automatron.";

	connect(mSocket, SIGNAL(disconnected()), this, SLOT(conDisconnected()));
}

void WDClient::conDisconnected()
{
	emit connectionLost();
	qDebug() << "Client:  Socket connection closed.";
	disconnect(mSocket, SIGNAL(disconnected()));
	disconnect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	mSocket->deleteLater();
}

void WDClient::socketError(QAbstractSocket::SocketError theError)
{
	qDebug() << "Socket Error! " << theError;
	conDisconnected();
}

void WDClient::doDisconnect()
{
	if(mSocket->state() == QTcpSocket::ConnectedState)
	{
		mSocket->disconnectFromHost();
	}
}
