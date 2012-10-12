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
#ifndef MainMenu_H
#define MainMenu_H

#include <bb/cascades/QmlDocument>
#include <QtCore/QObject>
#include <bb/cascades/Control>
#include <bb/cascades/Container>
#include <bb/cascades/AbstractPane>
#include <bb/system/InvokeManager.hpp>
#include <bb/system/SystemDialog.hpp>

#include "WriteURI.hpp"
#include "WriteSp.hpp"
#include "WriteText.hpp"
#include "WriteCustom.hpp"
#include "SendVcard.hpp"
#include "ApduDetails.hpp"
#include "About.hpp"
#include "EmulateSp.hpp"
#include "EventLog.hpp"
#include "NfcManager.hpp"
#include "StateManager.hpp"

using namespace bb::cascades;

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
    WriteURI* _writeURI;
    WriteSp* _writeSp;
    WriteText* _writeText;
    WriteCustom* _writeCustom;
    SendVcard* _sendVcard;
    ApduDetails* _apduDetails;
    EventLog* _eventLog;
    About* _about;
    EmulateSp *_emulateSp;
    NfcManager* _nfcManager;
    StateManager *_stateManager;
	bb::system::SystemDialog *_systemDialog;

    QString _appVersion;

    void startEventProcessing();
    void createModules();
    void connectSignals();
    void deleteModules();

    QmlDocument *_qml;
	AbstractPane *_root;
    void findAndConnectControls();

    Application *_app;
    bb::system::InvokeManager * _invokeManager;

signals:
    void detectAppVersionChanged();
    void read_selected();
    void write_uri();
    void write_sp();
    void write_text();
    void write_custom();
    void send_vcard_selected();
    void emulate_tag_selected();
    void iso7816_selected();
    void about_selected();
	void launchEventLog();

public slots:
	void onMainMenuTriggered();
	void onListSelectionChanged(const QVariantList indexPath);
	void backFromEventLog();
	void cleanUpOnExit();
	void receivedInvokeRequest(const bb::system::InvokeRequest& request);

private slots:
	void onDialogAccepted();

};

#endif // ifndef MainMenu_H
