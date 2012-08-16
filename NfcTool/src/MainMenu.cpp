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

MainMenu::MainMenu() :
		_nfcManager(0), _appVersion(QString(Settings::AppVersion)) {

	qDebug() << "XXXX NFC Tool V" << Settings::AppVersion;
	qDebug() << "XXXX loading main menu qml document";
	_qml = QmlDocument::create("main.qml");
	_qml->setContextProperty("_mainMenu", this);
	StateManager* state_mgr = StateManager::getInstance();
	_qml->setContextProperty("_stateManager", state_mgr);
	createModules();
	startEventProcessing();
	connectMainMenuReturnSignals();
	onMainMenuTriggered();

	QString uriEnv(getenv("uri"));

	qDebug() << "XXXX value of environment variable 'uri' = " << uriEnv;
	_launchedByInvoke = false;
	if (uriEnv.compare("invoke://localhost")) {
		_launchedByInvoke = true;
	}
}
MainMenu::~MainMenu() {
	qDebug() << "XXXX MainMenu destructor";
	deleteModules();
}

bool MainMenu::wasLaunchedByInvoke() const {
	return _launchedByInvoke;
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
	_eventLog = EventLog::getInstance();
	_emulateSp = new EmulateSp();
	_about = new About();
	qDebug() << "XXXX ...done";
}

void MainMenu::connectMainMenuReturnSignals() {
	qDebug()
			<< "XXXX connecting 'return to main menu' signals for 'Read' use case...";
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
	qDebug() << "XXXX ...done";
}

void MainMenu::findAndConnectControls() {

	qDebug() << "XXXX finding and cacheing NavigationPane object";
	Navigator* nav = Navigator::getInstance();

	NavigationPane* navpane = dynamic_cast<NavigationPane*>(_root);

	nav->setNavigationPane(navpane);

	qDebug()
			<< "XXXX finding and connecting the ListView to onListSelectionChanged slot";

	ListView *listView = _root->findChild<ListView*>("list");
	QObject::connect(listView, SIGNAL(triggered(const QVariantList)), this,
			SLOT(onListSelectionChanged(const QVariantList)));

	QObject::connect(this, SIGNAL(read_selected()), _eventLog, SLOT(show()));
	QObject::connect(this, SIGNAL(write_uri()), _writeURI, SLOT(show()));
	QObject::connect(this, SIGNAL(write_sp()), _writeSp, SLOT(show()));
	QObject::connect(this, SIGNAL(write_text()), _writeText, SLOT(show()));
	QObject::connect(this, SIGNAL(write_custom()), _writeCustom, SLOT(show()));
	QObject::connect(this, SIGNAL(send_vcard_selected()), _sendVcard, SLOT(show()));
	QObject::connect(this, SIGNAL(emulate_tag_selected()), _emulateSp, SLOT(show()));
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

			} else if (item.compare("item_uri") == 0) {
				qDebug() << "XXXX Write URI was selected!";
				emit write_uri();

			} else if (item.compare("item_sp") == 0) {
				qDebug() << "XXXX Write SP was selected!";
				emit write_sp();

			} else if (item.compare("item_text") == 0) {
				qDebug() << "XXXX Write Text was selected!";
				emit write_text();

			} else if (item.compare("item_custom") == 0) {
				qDebug() << "XXXX Write Custom was selected!";
				emit write_custom();

			} else if (item.compare("item_about") == 0) {
				qDebug() << "XXXX About was selected!";
				emit about_selected();

			} else if (item.compare("item_snep_vcard") == 0) {
				qDebug() << "XXXX Send vCard (SNEP) was selected!";
				emit send_vcard_selected();

			} else if (item.compare("item_emulate_tag") == 0) {
				qDebug() << "XXXX Emulate Tag was selected!";
				emit emulate_tag_selected();
			}
		}
	}
}

void MainMenu::onMainMenuTriggered() {
	qDebug() << "XXXX onMainMenuTriggered()";

	_root = _qml->createRootNode<AbstractPane>();

	qDebug() << "XXXX setting scene to main menu";
	Application::setScene(_root);

	findAndConnectControls();
}

void MainMenu::backFromEventLog() {
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDefaultState();
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
