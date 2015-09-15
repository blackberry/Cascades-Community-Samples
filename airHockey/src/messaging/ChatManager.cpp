/* Copyright (c) 2012, 2013 BlackBerry Limited.
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

#include "ChatManager.hpp"

#include <QtCore>

#include <btapi/btdevice.h>
#include <btapi/btspp.h>

#include <errno.h>
#include <iostream>

/**
 * Callback for events triggered by SPP server API to indicate a new incoming connection.
 */
void BTControllerSPPCallback(long param, int fd)
{
    ChatManager *chatManager = reinterpret_cast<ChatManager*>(param);

    if (chatManager) {
        chatManager->setSPPServer(fd);
    }
}

SPPThread::SPPThread(QObject *parent)
    : QThread(parent)
{
    resetFD();
}

void SPPThread::init(int fd, bool isServer)
{
    m_sppFD = fd;
    m_sppServer = isServer;
}

int SPPThread::getFD() const
{
    return m_sppFD;
}

void SPPThread::resetFD()
{
    m_sppFD = -1;
    m_sppServer = false;
}

bool SPPThread::isServer() const
{
    return m_sppServer;
}

bool SPPThread::active() const
{
    return (m_sppFD >= 0);
}

void SPPThread::run()
{
    int readlen;
    char tempbuff[128];

    while (m_sppFD >= 0) {

        if ((readlen = read(m_sppFD, tempbuff, (128 - 1))) > 0) {
            tempbuff[readlen] = '\0';
            emit incomingMessage(QString::fromLatin1(tempbuff));
        } else {
            if (readlen <= 0) {
                emit connectionClosed();
            }
        }
    }
}

ChatManager::ChatManager(QObject *parent)
    : QObject(parent)
{
    bool ok = connect(&m_sppDataThread, SIGNAL(incomingMessage(QString)), SLOT(incomingMessage(QString)));
    Q_ASSERT(ok);
    ok = connect(&m_sppDataThread, SIGNAL(connectionClosed()), SLOT(closeSPPConnection()));
    Q_ASSERT(ok);
}

ChatManager::~ChatManager()
{
    // Disconect SPP (if connected)
    closeSPPConnection();
}

void ChatManager::setRemoteAddress(const QString &address)
{
    m_remoteAddress = address;
}

void ChatManager::setSPPServer(int fd)
{
    if (fd >= 0) {
		m_sppDataThread.init(fd, true);
		m_sppDataThread.start();
    }
}

void ChatManager::setSPPClient(int fd)
{
    if (fd >= 0) {
        m_sppDataThread.init(fd, false);
        m_sppDataThread.start();
    }
}

bool ChatManager::connectToSPPService(const QString sppUUID)
{
	bool result = false;

	int fd = bt_spp_open(m_remoteAddress.toAscii().data(),
			(char *)sppUUID.toStdString().c_str(), false);

	if (fd >= 0) {
		setSPPClient(fd);
		result = true;
	} else {
		//showDialog("spp_open fail", "errno = " + QString::number(errno));
		qDebug() << "Failed to connect to " + m_remoteAddress +" at UUID " << sppUUID;
	}

	return result;
}

void ChatManager::startSPPServer(const QString sppUUID)
{
    mSppUUID = sppUUID;

    if (bt_spp_open_server((char *) "", (char *) mSppUUID.toStdString().c_str(), false, BTControllerSPPCallback, reinterpret_cast<long>(this)) == 0) {
        m_sppDataThread.init(-1, true);
    } else {
        showDialog("spp_open_server fail", "errno = " + QString::number(errno) );
    }
}

void ChatManager::sendSPPMessage(const QString &msg)
{
    // The msg parameter is the message the user intends to send over SPP.
    if (m_sppDataThread.active()) {
        write(m_sppDataThread.getFD(), msg.toAscii().data(), msg.size());
    } else {
        qDebug() << "ERROR - send with no file descriptor.";
    }
}

void ChatManager::closeSPPConnection()
{
    if (m_sppDataThread.isServer()) {
        m_sppDataThread.resetFD();
        bt_spp_close_server((char *) mSppUUID.toStdString().c_str());
        emit chatEnded();
    } else {
        if (m_sppDataThread.active()) {
            int closeFD = m_sppDataThread.getFD();
            m_sppDataThread.resetFD();
            bt_spp_close(closeFD);
            emit chatEnded();
        }
    }

    std::cout << "SPP Closed" << std::endl;
}

void ChatManager::incomingMessage(const QString &message)
{
    m_chatHistory = message;
    emit chatHistoryChanged(message);

}

void ChatManager::showDialog(const QString &title, const QString &message)
{
    m_waitDialog.setTitle(title);
    m_waitDialog.setBody(message);
    m_waitDialog.show();
}

QString ChatManager::chatHistory() const
{
    return m_chatHistory;
}
