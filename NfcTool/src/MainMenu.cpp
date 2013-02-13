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
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/ListView>
#include <bb/cascades/Page>
#include <cstdlib>

#include "MainMenu.hpp"
#include "Settings.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"

using namespace bb::cascades;

MainMenu::MainMenu(Application *app) :
		_nfcManager(0), _appVersion(QString(Settings::AppVersion)) {

	bb::cascades::Application::setOrganizationName("RIM");
	bb::cascades::Application::setApplicationName("NfcTool");

	qDebug() << "XXXX NFC Tool V" << Settings::AppVersion;
	qDebug() << "XXXX loading main menu qml document";
	_app = app;
	_qml = QmlDocument::create("asset:///main.qml");
	_qml->setContextProperty("_mainMenu", this);
	_root = _qml->createRootObject<AbstractPane>();
	StateManager* state_mgr = StateManager::getInstance();
	_qml->setContextProperty("_stateManager", state_mgr);

	_invokeManager = new bb::system::InvokeManager();
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = dynamic_cast<NavigationPane*>(_root);
	nav->setNavigationPane(navpane);

	_systemDialog = new bb::system::SystemDialog(this);
	_systemDialog->setTitle("NFC Transaction");
	bb::system::SystemUiButton *confirmButton = _systemDialog->confirmButton();
	confirmButton->setLabel("OK");

	createModules();
	startEventProcessing();
	qDebug() << "XXXX calling connectSignals";
	connectSignals();
	qDebug() << "XXXX done calling connectSignals";
	onMainMenuTriggered();

}

MainMenu::~MainMenu() {
	qDebug() << "XXXX MainMenu destructor";
	deleteModules();
}

void MainMenu::receivedInvokeRequest(const bb::system::InvokeRequest& request) {

	qDebug() << "XXXX invoked by iF: " << request.target() << " MIME=" << request.mimeType() << " ACTION=" << request.action() << " URI=" << request.uri();
	QByteArray data = request.data();
	qDebug() << "XXXX got data";
	if (request.mimeType().compare("application/vnd.rim.nfc.ndef") == 0) {
		qDebug("XXXX launched because an NFC tag has been presented");
		StateManager* state_mgr = StateManager::getInstance();
		if(!state_mgr->isEventLogShowing()) {
			emit launchEventLog();
		}
		_nfcManager = NfcManager::getInstance();
		_nfcManager->handleTagReadInvocation(data);
	} else {
		if (request.mimeType().compare("application/vnd.bb.nfc_transaction") == 0) {
			qDebug("XXXX launched because an NFC card emulation transaction event has been notified by an applet in the secure element");
			QString json = QString(data);
			qDebug() << "XXXX " << json;
			_systemDialog->setBody(json);
			_systemDialog->show();
		}
	}
}

void MainMenu::startEventProcessing() {
	_nfcManager = NfcManager::getInstance();
	_nfcManager->startEventProcessing();
}

void MainMenu::deleteModules() {
	qDebug() << "XXXX deleting instances of all other modules...";
	if (_writeURI) {
		delete _writeURI;
		_writeURI = 0;
	}
	if (_writeSp) {
		delete _writeSp;
		_writeSp = 0;
	}
	if (_writeText) {
		delete _writeText;
		_writeText = 0;
	}
	if (_writeCustom) {
		delete _writeCustom;
		_writeCustom = 0;
	}
	if (_sendVcard) {
		delete _sendVcard;
		_sendVcard = 0;
	}
	if (_apduDetails) {
		delete _apduDetails;
		_apduDetails = 0;
	}
	if (_about) {
		delete _about;
		_about = 0;
	}
	if (_emulateSp) {
		delete _emulateSp;
		_emulateSp = 0;
	}
	if (_eventLog) {
		delete _eventLog;
		_eventLog = 0;
	}
	if (_nfcManager) {
		delete _nfcManager;
		_nfcManager = 0;
	}
	qDebug() << "XXXX ...done";
}

void MainMenu::createModules() {
	qDebug() << "XXXX creating instances of all other modules...";
	_writeURI = new WriteURI();
	_writeSp = new WriteSp();
	_writeText = new WriteText();
	_writeCustom = new WriteCustom();
	_sendVcard = new SendVcard();
	_apduDetails = new ApduDetails();
	_eventLog = EventLog::getInstance();
	_emulateSp = new EmulateSp();
	_about = new About();
	qDebug() << "XXXX ...done";
}

void MainMenu::connectSignals() {
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	QObject::connect(navpane, SIGNAL(menuShowing()), this, SLOT(menuShowing()));
	QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(receivedInvokeRequest(const bb::system::InvokeRequest&)));
	QObject::connect(this, SIGNAL(launchEventLog()), _eventLog, SLOT(show()));
}

void MainMenu::findAndConnectControls() {

	qDebug() << "XXXX finding and cacheing NavigationPane object";
	Navigator* nav = Navigator::getInstance();

	qDebug() << "XXXX finding and connecting the ListView to onListSelectionChanged slot";

	ListView *listView = _root->findChild<ListView*>("list");
	QObject::connect(listView, SIGNAL(triggered(const QVariantList)), this, SLOT(onListSelectionChanged(const QVariantList)));

	QObject::connect(this, SIGNAL(read_selected()), _eventLog, SLOT(show()));
	QObject::connect(this, SIGNAL(tag_details_selected()), this, SLOT(tagDetails()));
	QObject::connect(this, SIGNAL(write_uri()), _writeURI, SLOT(show()));
	QObject::connect(this, SIGNAL(write_sp()), _writeSp, SLOT(show()));
	QObject::connect(this, SIGNAL(write_text()), _writeText, SLOT(show()));
	QObject::connect(this, SIGNAL(write_custom()), _writeCustom, SLOT(show()));
	QObject::connect(this, SIGNAL(send_vcard_selected()), _sendVcard, SLOT(show()));
	QObject::connect(this, SIGNAL(emulate_tag_selected()), _emulateSp, SLOT(show()));
	QObject::connect(this, SIGNAL(emulate_echo_selected()), this, SLOT(emulateEcho()));
	QObject::connect(this, SIGNAL(iso7816_selected()), _apduDetails, SLOT(show()));
	QObject::connect(this, SIGNAL(readIso15693_selected()), this, SLOT(readIso15693()));
	QObject::connect(this, SIGNAL(writeIso15693_selected()), this, SLOT(writeIso15693()));
	QObject::connect(this, SIGNAL(readGvb_selected()), this, SLOT(readGvb()));
	QObject::connect(this, SIGNAL(about_selected()), _about, SLOT(show()));

	qDebug() << "XXXX ...done";
}

void MainMenu::onListSelectionChanged(const QVariantList indexPath) {

	if (sender()) {
		ListView* menuList = dynamic_cast<ListView*>(sender());
		DataModel* menuModel = menuList->dataModel();

		QVariantMap map = menuModel->data(indexPath).toMap();
		if (map["itemName"].canConvert(QVariant::String)) {
			QString item = map["itemName"].toString();

			qDebug() << "XXXX selected item name=" << item;

			if (item.compare("item_read") == 0) {
				qDebug() << "XXXX Read Tag was selected!";
				StateManager* state_mgr = StateManager::getInstance();
				state_mgr->setEventLogShowing(true);
				_eventLog->setMessage("Bring a tag close");
				emit read_selected();

			} else if (item.compare("item_tag_details") == 0) {
				qDebug() << "XXXX Tag Details was selected!";
				emit tag_details_selected();

			} else if (item.compare("item_uri") == 0) {
				qDebug() << "XXXX Write URI was selected!";
				QObject::connect(_eventLog, SIGNAL(back()), _writeURI, SLOT(backFromEventLog()));
				emit write_uri();

			} else if (item.compare("item_sp") == 0) {
				qDebug() << "XXXX Write SP was selected!";
				QObject::connect(_eventLog, SIGNAL(back()), _writeSp, SLOT(backFromEventLog()));
				emit write_sp();

			} else if (item.compare("item_text") == 0) {
				qDebug() << "XXXX Write Text was selected!";
				QObject::connect(_eventLog, SIGNAL(back()), _writeText, SLOT(backFromEventLog()));
				emit write_text();

			} else if (item.compare("item_custom") == 0) {
				qDebug() << "XXXX Write Custom was selected!";
				QObject::connect(_eventLog, SIGNAL(back()), _writeCustom, SLOT(backFromEventLog()));
				emit write_custom();

			} else if (item.compare("item_about") == 0) {
				qDebug() << "XXXX About was selected!";
				emit about_selected();

			} else if (item.compare("item_snep_vcard") == 0) {
				qDebug() << "XXXX Send vCard (SNEP) was selected!";
				QObject::connect(_eventLog, SIGNAL(back()), _sendVcard, SLOT(backFromEventLog()));
				emit send_vcard_selected();

			} else if (item.compare("item_emulate_tag") == 0) {
				qDebug() << "XXXX Emulate Tag was selected!";
				QObject::connect(_eventLog, SIGNAL(back()), _emulateSp, SLOT(backFromEventLog()));
				emit emulate_tag_selected();
			} else if (item.compare("item_emulate_echo") == 0) {
				qDebug() << "XXXX Emulate Echo was selected!";
				StateManager* state_mgr = StateManager::getInstance();
				state_mgr->setEventLogShowing(true);
				_eventLog->setMessage("Place BlackBerry on reader");
				emit emulate_echo_selected();
			} else if (item.compare("item_iso7816") == 0) {
				qDebug() << "XXXX ISO7816 APDU was selected!";
				StateManager* state_mgr = StateManager::getInstance();
				state_mgr->setEventLogShowing(true);
				QObject::connect(_eventLog, SIGNAL(back()), _apduDetails, SLOT(backFromEventLog()));
				emit iso7816_selected();
			} else if (item.compare("item_read_iso15693") == 0) {
				qDebug() << "XXXX Read ISO15693 was selected!";
				StateManager* state_mgr = StateManager::getInstance();
				state_mgr->setEventLogShowing(false);
				emit readIso15693_selected();
			} else if (item.compare("item_write_iso15693") == 0) {
				qDebug() << "XXXX Write ISO15693 was selected!";
				StateManager* state_mgr = StateManager::getInstance();
				state_mgr->setEventLogShowing(false);
				emit writeIso15693_selected();
			} else if (item.compare("item_read_gvb") == 0) {
				qDebug() << "XXXX Read GVB was selected!";
				StateManager* state_mgr = StateManager::getInstance();
				state_mgr->setEventLogShowing(false);
				emit readGvb_selected();
			}
		}
	}
}

void MainMenu::onMainMenuTriggered() {
	qDebug() << "XXXX onMainMenuTriggered()";

	qDebug() << "XXXX setting scene to main menu";
	_app->setScene(_root);

	findAndConnectControls();
}

void MainMenu::menuShowing() {
	qDebug() << "XXXX menuShowing()";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDefaultState();
	_nfcManager = NfcManager::getInstance();
	_nfcManager->resetWorker();
}

QString MainMenu::appVersion() const {
	return _appVersion;
}

void MainMenu::setAppVersion(QString appVersion) {
	if (_appVersion.compare(appVersion) == 0)
		return;

	_appVersion = appVersion;

	emit detectAppVersionChanged();
}

void MainMenu::cleanUpOnExit() {
	qDebug() << "XXXX Clean up on application exit";
}

void MainMenu::readIso15693() {
	qDebug() << "XXXX MainMenu:readIso15693() start";
	_nfcManager->readIso15693();
	qDebug() << "XXXX MainMenu:readIso15693() end";
}

void MainMenu::writeIso15693() {
	qDebug() << "XXXX MainMenu:writeIso15693() start";
	QString data = QString("Hello");
	_nfcManager->writeIso15693(&data);
	qDebug() << "XXXX MainMenu:writeIso15693() end";
}

void MainMenu::emulateEcho() {
	qDebug() << "XXXX MainMenu:emulateEcho() start";
	_eventLog->show();
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setEventLogShowing(true);
	_nfcManager->startEchoEmulation();
	qDebug() << "XXXX MainMenu:emulateEcho() end";
}

void MainMenu::tagDetails() {
	qDebug() << "XXXX MainMenu:tagDetails() start";
	_eventLog->show();
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setEventLogShowing(true);
	_nfcManager->tagDetails();
	qDebug() << "XXXX MainMenu:tagDetails() end";
}

void MainMenu::readGvb() {
	qDebug() << "XXXX MainMenu:readGbv() start";
	_nfcManager->readGbv();
	qDebug() << "XXXX MainMenu:readGbv() end";
}
