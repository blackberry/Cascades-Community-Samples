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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include "common.hpp"

#include <QObject>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/InvokeManager>
#include <bb/data/JsonDataAccess>

#include <errno.h>

using namespace bb::cascades;
using namespace bb::data;

namespace bb {
    namespace cascades {
        class LocaleHandler;
    }
    namespace system {
        class InvokeManager;
    }
}

class QTranslator;

class ApplicationUI: public QObject
{
    Q_OBJECT

public:
    ApplicationUI();
    virtual ~ApplicationUI();

    Q_INVOKABLE void resendNotification();

signals:
    void message(const QVariant &text);
    void scanStateChanged(const QVariant &state);
    void startedStateChanged(const QVariant &state);
	void iBeaconEnteredRange(const QVariant &timeStamp, const QVariant &macAddress, const QVariant &uuid,
			const QVariant &major, const QVariant &minor, const QVariant &rssi, const QVariant &loss);
	void iBeaconExitedRange(const QVariant &timeStamp, const QVariant &macAddress, const QVariant &uuid,
			const QVariant &major, const QVariant &minor, const QVariant &rssi, const QVariant &loss);
	void altBeaconEnteredRange(const QVariant &timeStamp, const QVariant &macAddress, const QVariant &companyCode, const QVariant &companyName,
	        const QVariant &beaconId, const QVariant &reserved, const QVariant &rssi, const QVariant &loss);
    void altBeaconExitedRange(const QVariant &timeStamp, const QVariant &macAddress, const QVariant &companyCode, const QVariant &companyName,
            const QVariant &beaconId, const QVariant &reserved, const QVariant &rssi, const QVariant &loss);

private slots:
    void onSystemLanguageChanged();
    void onAddScanRequest(const QString &address);
    void onEnableScanInvokeRequest();
    void onDisableScanInvokeRequest();
    void onRemoveScanRequest(const QString &address);
    void onStartHeadlessService();
    void onStopHeadlessService();

    void onConnected();
    void onDisconnected();
    void onError(QLocalSocket::LocalSocketError socketError);
    void onStateChanged(QLocalSocket::LocalSocketState socketState);
    void onReadyRead();
    void onTimeout();

    void onInvoked(const bb::system::InvokeRequest &request);

private:
    void connectSocketSignals();
    void disconnectSocketSignals();
    void connectToHeadless();
    void sendToHeadless(const QVariant &text);
    void triggerRetryConnection();
    void handleBeaconEvent(const QVariantMap &payload);
    bool monitoringUuid(const QString & uuid, int major, int minor);
    bool monitoringAltBeacon(int companyCode, const QString & beaconId, int reserved);

    QTranslator *_translator;
    bb::cascades::LocaleHandler *_localeHandler;
    bb::system::InvokeManager *_invokeManager;
    QmlDocument *_qml;
    AbstractPane *_root;
    QObject *_mainPage;
    QLocalSocket *_socket;
    bool _connectedToServer;
    int _socketRetryInterval;
    QString _socketServerPath;
    bool _scanning;
};

#endif /* ApplicationUI_HPP_ */
