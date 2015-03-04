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

#include "Utilities.hpp"

#include <QObject>
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <btapi/btdevice.h>
#include <btapi/btdevice.h>
#include <btapi/btgatt.h>
#include <btapi/btle.h>
#include <btapi/btspp.h>
#include <errno.h>
#include <stdint.h>

using namespace bb::cascades;

namespace bb
{
    namespace cascades
    {
        class LocaleHandler;
    }
}

class QTranslator;

class ApplicationUI : public QObject
{
    Q_OBJECT

public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI();
    void logMessage(QString msg);
    void emitConnectionStateChanged(bool isConnected);

    uint16_t _value_handle;
    uint16_t _handle;

private slots:
    void onSystemLanguageChanged();
    void onConnectToGattService();
    void onDisconnectFromGattService();

signals:
    void message(const QVariant &text);
    void connectionStateChanged(const QVariant &state);
    void enableUi(const QVariant &state);

private:
    QTranslator* _translator;
    bb::cascades::LocaleHandler* _localeHandler;
    bool _bt_initialised;

    char _rns_device_addr[18];
    AbstractPane *_root;
    QmlDocument *_qml;
    QObject *_mainPage;

    void startRandomNumbers();
    void stopRandomNumbers();
    bool findRnsDevice();
    bool hasRandomNumberService(bt_remote_device_t * remoteDevice);
    bool startReceivingRandomNumbers();
    void deinitBluetooth();
    void initBluetooth();
    bool isOneOfFingerPrintServices(const char *serviceUuid);
};

#endif /* ApplicationUI_HPP_ */
