/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <errno.h>
#include <btapi/btdevice.h>
#include <btapi/btle.h>
#include <QTimer>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <btapi/btdevice.h>
#include <btapi/btle.h>
#include <btapi/btgattsrv.h>
#include "btapi/btgatt.h"
#include <bbndk.h>

namespace bb
{
    namespace cascades
    {
        class LocaleHandler;
    }
}

using namespace bb::cascades;

class QTranslator;

/*!
 * @brief Application UI object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI();
    void logMessage(QString msg);
    void startAdvertising();
    void stopAdvertising();
    void registerService();
    void deregisterService();
    void restartGattServer();
    void startGattServer();
    void stopGattServer();

    void emitClientConnectedSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData);
    void emitClientDisconnectedSignal(int instance, const QString &bdaddr, uint8_t type, void *userData);
    void emitClientWriteRequestSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t handle, uint8_t *val, uint16_t len, void *userData);
    void emitStartRandomNumberNotificationSignal();
    void emitStopRandomNumberNotificationSignal();

    int _instance;
    char *_device_addr;
    uint8_t _device_type;
    bool _notifications_enabled;
    static ApplicationUI *_app;
    bool _connected;
    bool _stopRequested;

private slots:
    void onSystemLanguageChanged();
    void handleClientConnectedSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData);
    void handleClientDisconnectedSignal(int instance, const QString &bdaddr, uint8_t type, void *userData);
    void handleClientWriteRequestSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t handle, uint8_t *val, uint16_t len, void *userData);
    void handleStartRandomNumberNotificationSignal();
    void handleStopRandomNumberNotificationSignal();
    void sendNotification();
    void onStartGattService();
    void onStopGattService();

signals:
    void clientConnectedSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData);
    void clientDisconnectedSignal(int instance, const QString &bdaddr, uint8_t type, void *userData);
    void clientWriteRequestSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t handle, uint8_t *val, uint16_t len, void *userData);
    void startRandomNumberNotificationSignal();
    void stopRandomNumberNotificationSignal();
    void message(const QVariant &text);
    void connectionStateChanged(const QVariant &state);
    void startedStateChanged(const QVariant &state);
    void startGattService();
    void stopGattService();

private:
    QTranslator *_translator;
    bb::cascades::LocaleHandler *_localeHandler;
    QmlDocument *_qml;
    AbstractPane *_root;
    QTimer *_notification_timer;
    int _serviceInstance;
    QObject *_mainPage;
};

#endif /* ApplicationUI_HPP_ */
