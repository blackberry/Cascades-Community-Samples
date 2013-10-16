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

#ifndef WDSERVER_HPP_
#define WDSERVER_HPP_

#include <QObject>
#include <qt4/QtNetwork/QTcpServer>
#include <qt4/QtNetwork/QTcpSocket>
#include <cstdio>

class WDServer : public QObject
{
Q_OBJECT

public:
	WDServer(QObject *parent = 0);

	public slots:
	void newConnection();
	void listen();
	void readCommand();
	void conDisconnected();
	void doDisconnect();
	void socketError(QAbstractSocket::SocketError theError);

	signals:
		void connectionLost();
		void commandReceived(QString command);

	private:

	QTcpServer* mServer;
	QTcpSocket* mSocket;

};


#endif /* WDSERVER_HPP_ */
