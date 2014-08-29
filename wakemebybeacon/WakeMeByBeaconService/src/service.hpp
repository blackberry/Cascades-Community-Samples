/*
 * Copyright (c) 2013-2014 BlackBerry Limited.
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

#ifndef SERVICE_H_
#define SERVICE_H_

#include "common.hpp"
#include "BtleAdvertData.hpp"
#include "SocketListener.hpp"

#include <QObject>
#include <errno.h>
#include <bb/cascades/GroupDataModel>
#include <bb/core/ProcessState.hpp>
#include <bb/data/JsonDataAccess>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb;

namespace bb {
    class Application;
    namespace platform {
        class Notification;
    }
    namespace system {
        class InvokeManager;
        class InvokeRequest;
    }
}

class Service: public QObject
{
    Q_OBJECT

public:
    Service();
    virtual ~Service();
    void parseBeaconData(const char *data, int len, int8_t rssi, const char *bdaddr);

    QString btEventName(int event);
    void sendStatus();

    bool _btRadioOn;

signals:
    void messageForGui(const QVariant &);

private slots:
    void onInvoked(const bb::system::InvokeRequest &);
    void onGuiTimeout();
    void onProcessStateChanged(bb::ProcessState::Type processState);
    void onGuiConnected();
    void onGuiDisconnected();
    void onTimeout();

private:
    void triggerNotification();
    void initBluetooth();
    void deinitBluetooth();
    void prepForAdvertisements();
    void expireBeaconData(int seconds);
    const QString constructJsonPayload(QVariantMap parameters);
    void beaconEnteredRange(const QVariantMap &entry);
    void beaconExitedRange(const QVariantMap &entry);

    bb::platform::Notification *_notification;
    bb::system::InvokeManager *_invokeManager;
    bool _btInitialised;
    GroupDataModel *_model;
    QString _socketServerPath;
    SocketListener *_socketListener;
    bool _scanning;
    bool _guiConnected;
    QTimer *_expireTimer;
};

#endif /* SERVICE_H_ */
