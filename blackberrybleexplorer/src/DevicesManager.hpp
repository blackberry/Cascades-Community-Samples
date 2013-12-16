/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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
#ifndef DEVICESMANAGER_H
#define DEVICESMANAGER_H

#include <stdint.h>
#include <QObject>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>
#include "RemoteDeviceInfo.hpp"

#include <btapi/btdevice.h>

class DevicesManager: public QObject
{

Q_OBJECT

public:
    static DevicesManager* getInstance(QObject *parent = 0);
    static DevicesManager* getDevicesManager();
    void findBleDevices();
    void extractAndStoreBleDeviceAttributes(bt_remote_device_t *remoteDevice);
    void selectRemoteDevice(const QString&);

private:
    DevicesManager(QObject *parent = 0);
    virtual ~DevicesManager();
    static DevicesManager *_instance;
    RemoteDeviceInfo *_remoteDeviceInfo;

    int _item_count;

    Q_INVOKABLE

signals:
    void setDeviceCount(QVariant count);
    void finishedScanningForDevices();
    void startedScanningForDevices();
};

#endif // ifndef DEVICESMANAGER_H
