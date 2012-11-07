/* Copyright (c) 2012 Research In Motion Limited.
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
#ifndef MAINMENU_H
#define MAINMENU_H

#include <QObject>

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/NavigationPane>

#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>
#include <bb/system/ApplicationStartupMode.hpp>
#include <QtNfcSubset/qndefmessage.h>

#include <bps/bps.h>
#include <nfc/nfc.h>
#include <nfc/nfc_ndef.h>

#include "Settings.hpp"
#include "NdefFactory.hpp"

using namespace bb::cascades;

/*!
 * @brief Application GUI object
 */
class MainMenu : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY detectAppVersionChanged)

public:
    MainMenu(Application *app);
    virtual ~MainMenu();

    QString appVersion() const;
	void setAppVersion(QString appVersion);

private:
    void handleNdefRequest(const QtMobilitySubset::QNdefMessage message);
    NdefFactory* _ndefFactory;
    QString _appVersion;
    void createModules();
    void deleteModules();
    QmlDocument *_qml;
	AbstractPane *_root;
    void connectSignals();
    bb::system::InvokeManager * _invokeManager;
    Application *_app;

signals:
	void detectAppVersionChanged();
	void message(const QVariant &text);
	void launchReader(const QVariant &text);

public slots:
	void receivedInvokeRequest(const bb::system::InvokeRequest& request);
	void cleanUpOnExit();
};

#endif // ifndef MAINMENU_H
