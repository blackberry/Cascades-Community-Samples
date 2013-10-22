/* Copyright (c) 2013 Research In Motion Limited.
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

#ifndef DEVICELISTING_HPP
#define DEVICELISTING_HPP

#include <QObject>

#include <bb/cascades/GroupDataModel>

#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>

#include <btapi/btdevice.h>
#include <btapi/btspp.h>

class DeviceListing : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bb::cascades::DataModel* model READ model CONSTANT)

public:
    DeviceListing(QObject *parent = 0);

public Q_SLOTS:
    void update();
    void discover();

signals:
     void startMonitoring();

private:
    bb::cascades::DataModel* model() const;
    bb::cascades::GroupDataModel* _model;
    bool isAHeartRateMonitor(bt_remote_device_t * remoteDevice);
    QString HR_SERVICE_UUID;
};

#endif
