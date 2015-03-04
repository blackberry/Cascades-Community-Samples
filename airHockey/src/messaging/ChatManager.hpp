/* Copyright (c) 2012, 2013  BlackBerry Limited.
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

#ifndef CHATMANAGER_HPP
#define CHATMANAGER_HPP

#include <bb/system/SystemDialog>

#include <QObject>
#include <QThread>

/**
 * @short A helper class to read data from a bluetooth serial device.
 *
 * The purpose of this class is to continually wait for incoming data via blocking read() and post it to the main thread,
 * as well as watch for disconnection signaled by a negative return value.
 */
//! [0]
class SPPThread : public QThread
{
    Q_OBJECT

public:
    // Creates a new SPPThread object
    SPPThread(QObject *parent = 0);

    // Initializes the device descriptor to listen on and sets whether the thread is in server or client mode
    void init(int fd, bool isServer);

    // Returns the device descriptor
    int getFD() const;

    // Resets the device descriptor
    void resetFD();

    // Returns whether the thread is in server mode
    bool isServer() const;

    // Returns whether the thread is active (a valid device descriptor has been set)
    bool active() const;

    // Reimplemented from QThread, the content is executed in the worker thread
    void run();

Q_SIGNALS:
    // This signal is emitted whenever new data have been received through the bluetooth device
    void incomingMessage(const QString&);

    // This signal is emitted whenever the remote device closed the connection
    void connectionClosed();

private:
    // A flag to store server/client mode information
    bool m_sppServer;

    // The bluetooth device descriptor
    int m_sppFD;
};
//! [0]

/**
 * @short A class that encapsulates the business logic for SPP chat functionality.
 *
 * The ChatManager encapsulates all the logic for the SPP chat. It provides methods
 * to start a local SPP service (server mode) or connect to a remote SPP service (client mode).
 * It monitors the life-cycle of the session and provides methods to send messages to the
 * remote peer and receive them.
 */
//! [1]
class ChatManager : public QObject
{
    Q_OBJECT

    // The messages that have been sent between local and remote peer
    Q_PROPERTY(QString chatHistory READ chatHistory NOTIFY chatHistoryChanged)

public:
    // Creates a new ChatManager object
    ChatManager(QObject *parent = 0);

    // Destroys the ChatManager object
    ~ChatManager();

    // Sets the bluetooth address of the remote peer
    void setRemoteAddress(const QString &address);

    // Sets the bluetooth devices descriptor (in server mode)
    void setSPPServer(int fd);

    // Sets the bluetooth devices descriptor (in client mode)
    void setSPPClient(int fd);

    // Establishes a connection to a remote SPP service (client mode)
    Q_INVOKABLE bool connectToSPPService(const QString sppUUID);

public Q_SLOTS:

    // Starts a local SPP service (server mode)
    void startSPPServer(const QString sppUUID);

    // Sends a message to the remote peer
    void sendSPPMessage(const QString &msg);

    // Closes the SPP session (in client and server mode)
    void closeSPPConnection();

Q_SIGNALS:
    // This signal is emitted whenever the remote peer has closed the connection
    void chatEnded();

    // The change notification signal of the property
    void chatHistoryChanged(QString chatHistory);

private Q_SLOTS:
    // This slot is invoked whenever the SPPThread reports a new incoming message
    void incomingMessage(const QString &msg);

private:
    // A helper method to show a system dialog to the user
    void showDialog(const QString &title, const QString &message);

    // The accessor method of the property
    QString chatHistory() const;

    // The SPPThread that listens for incoming messages from the remote peer
    SPPThread m_sppDataThread;

    // The bluetooth address of the remote peer
    QString m_remoteAddress;

    // The system dialog to show error messages to the user
    bb::system::SystemDialog m_waitDialog;

    // The property value
    QString m_chatHistory;

    QString mSppUUID;
};
//! [1]

#endif
