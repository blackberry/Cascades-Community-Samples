/* Copyright (c) 2013 BlackBerry Limited.
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

#ifndef BLUETOOTHHANDLER_HPP_
#define BLUETOOTHHANDLER_HPP_

#include <QObject>
#include <btapi/btdevice.h>
#include <btapi/btgatt.h>
#include <btapi/btle.h>
#include <btapi/btspp.h>
#include "LocalDeviceInfo.hpp"
#include "RemoteDeviceInfo.hpp"
#include "DeviceListing.hpp"

namespace bb
{
    namespace cascades
    {
        class Application;
    }
}

class BluetoothHandler: public QObject
{
Q_OBJECT

Q_PROPERTY(DeviceListing* deviceListing READ deviceListing CONSTANT)
Q_PROPERTY(LocalDeviceInfo* localDeviceInfo READ localDeviceInfo CONSTANT)
Q_PROPERTY(RemoteDeviceInfo* remoteDeviceInfo READ remoteDeviceInfo CONSTANT)

public:
    BluetoothHandler(QObject *obj);
    virtual ~BluetoothHandler();

    void getInfo();
    void emitSignalLinkEstablished();
    void emitSignalLostLink();
    void emitSignalLogMessage(QString msg);
    void emitSignalInfoMessage(QString msg);

signals:
    void signalLinkEstablished();
    void signalLostLink();
    void signalInfoMessage(QString msg);
    void signalLogMessage(QString msg);

private:
    LocalDeviceInfo *_localDeviceInfo;
    DeviceListing *_deviceListing;
    RemoteDeviceInfo *_remoteDeviceInfo;
    DeviceListing *deviceListing() const;
    LocalDeviceInfo *localDeviceInfo() const;
    RemoteDeviceInfo *remoteDeviceInfo() const;

    void readInfo();

public slots:
    void setRemoteDevice(const QString &address);

};

#endif /* BLUETOOTHHANDLER_HPP_ */
