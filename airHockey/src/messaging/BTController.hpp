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

#ifndef BTCONTROLLER_HPP
#define BTCONTROLLER_HPP

#include "ChatManager.hpp"

#include <QObject>

/**
 * @short The central class that encapsulates the access to the bluetooth functionality.
 *
 * The BTController encapsulates the access to the bluetooth functionality and provides properties
 * to make the functionality accessible inside QML.
 */
//! [0]
class BTController : public QObject
{
    Q_OBJECT

    // Properties to access various bluetooth functionality
    Q_PROPERTY(ChatManager* chatManager READ chatManager CONSTANT)
    Q_PROPERTY(ChatManager* chatManager2 READ chatManager2 CONSTANT)

    Q_PROPERTY(bool bluetoothActive READ bluetoothActive NOTIFY bluetoothActiveChanged)
    Q_PROPERTY(bool discoverableActive READ discoverableActive NOTIFY discoverableActiveChanged)

public:
    /**
     * Creates a new BTController object.
     *
     * @param parent The parent object.
     */
    BTController(QObject* parent = 0);

    // Destroys the BTController object
    virtual ~BTController();

    // A helper method used for integration with bluetooth low-level C API
    void emitBTDeviceSignal(int event, const QString &btAddr, const QString &eventData);

public Q_SLOTS:
    // This slot is invoked to switch the local bluetooth functionality on/off
    void toggleBluetoothActive();

    // This slot is invoked to change whether the bluetooth device is discoverable or not
    void toggleDiscoverableActive();

    // This slot is invoked to set the address of the remote device the user has selected in the UI
    void setRemoteDevice(const QString &address);

Q_SIGNALS:
    // A helper signal used for integration with bluetooth low-level C API
    void BTDeviceSignal(int event, const QString &btAddr, const QString &eventData);

    // The change notification signals of the properties
    void bluetoothActiveChanged();
    void discoverableActiveChanged();

private Q_SLOTS:
    // A helper slot used for integration with bluetooth low-level C API
    void handleBTDeviceEvent(int event, const QString &btAddr, const QString &eventData);

private:

    // The accessor methods for the properties
    bool bluetoothActive() const;
    bool discoverableActive() const;

    ChatManager* chatManager() const;
    ChatManager* chatManager2() const;

    // The property values
    ChatManager* m_chatManager;
    ChatManager* m_chatManager2;
};
//! [0]

#endif
