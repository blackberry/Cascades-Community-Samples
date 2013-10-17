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

#ifndef WDCLIENT_HPP_
#define WDCLIENT_HPP_

#include <QObject>
#include <qt4/QtNetwork/QTcpSocket>
#include <qt4/QtNetwork/QHostAddress>
#include <cstdio>

class WDClient: public QObject
{
Q_OBJECT

public:
	WDClient(QObject *parent = 0);
	void connectToAuto(QString autoAddress);

signals:
	void connectionLost();


public slots:
	void connected();
	void conDisconnected();
	void doDisconnect();
	void sendCommand(QString command);
	void socketError(QAbstractSocket::SocketError theError);

private:
	QTcpSocket* mSocket;

};

#endif /* WDCLIENT_HPP_ */
