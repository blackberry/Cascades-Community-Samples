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
#include <bb/cascades/Control>
#include <bb/cascades/Button>
#include <bb/cascades/TextArea>

#include "EmulateSp.hpp"
#include "Settings.hpp"
#include "NfcManager.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"

#include <QDebug>

using namespace bb::cascades;

EmulateSp::EmulateSp() :
		_appVersion(QString(Settings::AppVersion)) {
	qDebug() << "XXXX constructing EmulateSp...";
	_uri = Settings::EMULATE_URI;
	_text = Settings::EMULATE_TEXT;
	_qml = QmlDocument::create("asset:///emulate_sp.qml");
	_qml->setContextProperty("_emulateSp", this);
	_root = _qml->createRootObject<Page>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

EmulateSp::~EmulateSp() {
	qDebug() << "XXXX EmulateSp destructor";
}

void EmulateSp::createModules() {
	qDebug() << "XXXX creating instances of EmulateSp modules...";
	_eventLog = EventLog::getInstance();
	qDebug() << "XXXX ...done";
}

void EmulateSp::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void EmulateSp::findAndConnectControls() {
	qDebug() << "XXXX finding all EmulateSp controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "emulateSp");
	QObject::connect(obj, SIGNAL(emulateSpRequested()), this,
			SLOT(startEmulationProcess()));

	TextArea* txf_uri = _root->findChild<TextArea*>("txf_uri");
	TextArea* txf_text = _root->findChild<TextArea*>("txf_text");
	qDebug() << "XXXX found EmulateSp text fields";
	qDebug() << "XXXX connecting all TextArea signals and slots...";
	QObject::connect(txf_uri, SIGNAL(textChanged(QString)), this,
			SLOT(onUriChanged(QString)));
	QObject::connect(txf_text, SIGNAL(textChanged(QString)), this,
			SLOT(onTextChanged(QString)));

	_root = _qml->createRootObject<Page>();

	qDebug() << "XXXX ...done";
}

void EmulateSp::startEmulationProcess() {
	qDebug() << "XXXX startEmulationProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "XXXX Telling NfcManager to start emulation process";

	qDebug() << "XXXX setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setTagEmulationState(true);

	nfc->startTagEmulation(&_uri, &_text);

	qDebug() << "XXXX switching to event log screen";
	_eventLog->show();
}

void EmulateSp::backFromEventLog() {
	StateManager* state_mgr = StateManager::getInstance();
	NfcManager* nfc = NfcManager::getInstance();
	nfc->stopTagEmulation();
	state_mgr->setDefaultState();
}

void EmulateSp::show() {
	qDebug() << "XXXX EmulateSp: show()";

	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	// refresh _root
	_root = _qml->createRootObject<Page>();
	navpane->push(_root);

	//find and reconnect all the controls again, because we had to refresh the _root pointer
	findAndConnectControls();
}

QString EmulateSp::appVersion() const {
	return _appVersion;
}

void EmulateSp::setAppVersion(QString appVersion) {
	if (_appVersion.compare(appVersion) == 0)
		return;

	_appVersion = appVersion;

	emit detectAppVersionChanged();
}

QString EmulateSp::getUri() const {
	return _uri;
}

void EmulateSp::setUri(QString uri) {
	qDebug() << "XXXX EmulateSp:setUri(....)";
	if (_uri.compare(uri) == 0)
		return;
	_uri = uri;

	emit detectUriChanged();
}

QString EmulateSp::getText() const {
	return _text;
}

void EmulateSp::setText(QString text) {
	qDebug() << "XXXX EmulateSp:setText(....)";
	if (_text.compare(text) == 0)
		return;
	_text = text;

	emit detectTextChanged();
}

void EmulateSp::onUriChanged(QString uri) {
	qDebug() << "XXXX EmulateSp:onUriChanged(....)";
	setUri(uri);
}

void EmulateSp::onTextChanged(QString text) {
	qDebug() << "XXXX EmulateSp:onTextChanged(....)";
	setText(text);
}
