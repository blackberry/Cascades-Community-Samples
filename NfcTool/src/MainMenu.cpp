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

#include "MainMenu.hpp"
#include "Settings.hpp"
#include "StateManager.hpp"
#include "NfcManager.hpp"
#include "Navigator.hpp"

using namespace bb::cascades;

MainMenu::MainMenu() :
		_appVersion(QString(Settings::AppVersion)) {

	qDebug() << "NFC Tool V" << Settings::AppVersion;

	qDebug() << "loading main menu qml document";
	_qml = QmlDocument::create("main.qml");
	_qml->setContextProperty("_mainMenu", this);

	StateManager* state_mgr = StateManager::getInstance();
	_qml->setContextProperty("_stateManager", state_mgr);

	createModules();
	connectMainMenuReturnSignals();

	onMainMenuTriggered();
}

MainMenu::~MainMenu() {
	delete _writeURI;
	delete _writeSp;
	delete _writeText;
	delete _writeCustom;
	delete _sendVcard;
	delete _eventLog;
	delete _about;
}

void MainMenu::createModules() {
	qDebug() << "creating instances of all other modules...";
	_writeURI = new WriteURI();
	_writeSp = new WriteSp();
	_writeText = new WriteText();
	_writeCustom = new WriteCustom();
	_sendVcard = new SendVcard();
	_eventLog = new EventLog("");
	_about = new About();
	qDebug() << "...done";
}

//only need to call this once, the signals should never get lost when switching scenes
void MainMenu::connectMainMenuReturnSignals() {
	qDebug()
			<< "connecting 'return to main menu' signals for 'Read' use case...";
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
	qDebug() << "...done";
}

void MainMenu::findAndConnectControls() {

	qDebug() << "finding and cacheing NavigationPane object";
	Navigator* nav = Navigator::getInstance();

	NavigationPane* navpane = dynamic_cast<NavigationPane*>(_root);

	nav->setNavigationPane(navpane);

	qDebug()
			<< "finding and connecting the ListView to onListSelectionChanged slot";

	ListView *listView = _root->findChild<ListView*>("list");
	QObject::connect(listView,
			SIGNAL(selectionChanged(const QVariantList, bool)), this,
			SLOT(onListSelectionChanged(const QVariantList, bool)));

	QObject::connect(this, SIGNAL(read_selected()), _eventLog, SLOT(show()));
	QObject::connect(this, SIGNAL(write_uri()), _writeURI, SLOT(show()));
	QObject::connect(this, SIGNAL(write_sp()), _writeSp, SLOT(show()));
	QObject::connect(this, SIGNAL(write_text()), _writeText, SLOT(show()));
	QObject::connect(this, SIGNAL(write_custom()), _writeCustom, SLOT(show()));
	QObject::connect(this, SIGNAL(send_vcard_selected()), _sendVcard, SLOT(show()));
	QObject::connect(this, SIGNAL(about_selected()), _about, SLOT(show()));

	qDebug() << "...done";
}

void MainMenu::onListSelectionChanged(const QVariantList indexPath,
		bool selected) {

	// Get the corresponding model data element from the list (sender).
	if (selected) {

		// We use the sender to get the list view for accessing the data model and then the actual data.
		if (sender()) {
			ListView* menuList = dynamic_cast<ListView*>(sender());
			DataModel* menuModel = menuList->dataModel();

			// Update the content view context property so that it corresponds to
			// the selected item and navigate to the page.
			QVariantMap map = menuModel->data(indexPath).toMap();
			if (map["itemName"].canConvert(QVariant::String)) {
				QString item = map["itemName"].toString();

				qDebug() << "selected item name=" << item;

				if (item.compare("item_read") == 0) {
					qDebug() << "Read Tag was selected!";
					startListening();
					_eventLog->setMessage("Bring a tag close");
					emit read_selected();

				} else if (item.compare("item_uri") == 0) {
					qDebug() << "Write URI was selected!";
					emit write_uri();

				} else if (item.compare("item_sp") == 0) {
					qDebug() << "Write SP was selected!";
					emit write_sp();

				} else if (item.compare("item_text") == 0) {
					qDebug() << "Write Text was selected!";
					emit write_text();

				} else if (item.compare("item_custom") == 0) {
					qDebug() << "Write Custom was selected!";
					emit write_custom();
				} else if (item.compare("item_about") == 0) {
					qDebug() << "About was selected!";
					emit about_selected();
				} else if (item.compare("item_snep_vcard") == 0) {
					qDebug() << "Send vCard (SNEP) was selected!";
					emit send_vcard_selected();
				}
			}
		}
	}
}

void MainMenu::onMainMenuTriggered() {
	qDebug() << "onMainMenuTriggered()";

	//refresh the root pointer, because setScene to any other root destroys this pointer
	_root = _qml->createRootNode<AbstractPane>();

	qDebug() << "setting scene to main menu";
	Application::setScene(_root);

	//find and reconnect all the controls again, because we had to refresh the root pointer
	findAndConnectControls();

}

void MainMenu::backFromEventLog() {
	stopListening();
}

void MainMenu::stopListening() {
	StateManager* state_mgr = StateManager::getInstance();
	if (state_mgr->inReadState()) {
		qDebug() << "stopListening()";
		NfcManager* nfc = NfcManager::getInstance();
		nfc->stopNdefListener();
	}
}

void MainMenu::startListening() {
	qDebug() << "startListening()";
	NfcManager* nfc = NfcManager::getInstance();

	NdefType* type_sp = new NdefType(NDEF_TNF_WELL_KNOWN,
			Settings::NfcRtdSmartPoster);
	NdefType* type_t = new NdefType(NDEF_TNF_WELL_KNOWN, Settings::NfcRtdText);
	NdefType* type_u = new NdefType(NDEF_TNF_WELL_KNOWN, Settings::NfcRtdUri);
	QList<NdefType *> types;

	Q_ASSERT_X((types.size() == 0), "startListening", "types value not 0");

	types.append(type_sp);
	types.append(type_t);
	types.append(type_u);

	Q_ASSERT_X((types.size() > 0), "startListening", "types value not > 0");

	qDebug() << "Telling NfcManager to start listening";
	nfc->startNdefListener(types);

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

