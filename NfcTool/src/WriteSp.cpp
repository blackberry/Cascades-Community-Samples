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

#include "WriteSp.hpp"
#include "Settings.hpp"
#include "NfcManager.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"

#include <QDebug>

using namespace bb::cascades;

WriteSp::WriteSp() :
		_appVersion(QString(Settings::AppVersion)) {
	qDebug() << "XXXX constructing WriteSp...";
	_uri = Settings::SP_URI;
	_text = Settings::SP_TEXT;
	_qml = QmlDocument::create("asset:///write_sp.qml");
	_qml->setContextProperty("_writeSpMenu", this);
	_root = _qml->createRootObject<Page>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

WriteSp::~WriteSp() {
	qDebug() << "XXXX WriteSp destructor";
}

void WriteSp::createModules() {
	qDebug() << "XXXX creating instances of WriteSp modules...";
	_eventLog = EventLog::getInstance();
	qDebug() << "XXXX ...done";
}

void WriteSp::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void WriteSp::findAndConnectControls() {
	qDebug() << "XXXX finding all WriteSp controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "writeSp");
	QObject::connect(obj, SIGNAL(writeSpRequested()), this,
			SLOT(startWriteProcess()));

	TextArea* txf_uri = _root->findChild<TextArea*>("txf_uri");
	TextArea* txf_text = _root->findChild<TextArea*>("txf_text");
	qDebug() << "XXXX found WriteSp text fields";
	qDebug() << "XXXX connecting all TextArea signals and slots...";
	QObject::connect(txf_uri, SIGNAL(textChanged(QString)), this,
			SLOT(onUriChanged(QString)));
	QObject::connect(txf_text, SIGNAL(textChanged(QString)), this,
			SLOT(onTextChanged(QString)));

	_root = _qml->createRootObject<Page>();

	qDebug() << "XXXX ...done";
}

void WriteSp::startWriteProcess() {
	qDebug() << "XXXX startWriteProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "XXXX Telling NfcManager to start write process";

	qDebug() << "XXXX setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	nfc->writeSp(&_uri, &_text);

	qDebug() << "XXXX switching to event log screen";
	_eventLog->show();
}

void WriteSp::backFromEventLog() {

}

void WriteSp::show() {
	qDebug() << "XXXX WriteSp: show()";

	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	// refresh _root
	_root = _qml->createRootObject<Page>();
	navpane->push(_root);

	//find and reconnect all the controls again, because we had to refresh the _root pointer
	findAndConnectControls();
}

QString WriteSp::appVersion() const {
	return _appVersion;
}

void WriteSp::setAppVersion(QString appVersion) {
	if (_appVersion.compare(appVersion) == 0)
		return;

	_appVersion = appVersion;

	emit detectAppVersionChanged();
}

QString WriteSp::getUri() const {
	return _uri;
}

void WriteSp::setUri(QString uri) {
	qDebug() << "XXXX WriteSp:setUri(....)";
	if (_uri.compare(uri) == 0)
		return;
	_uri = uri;

	emit detectUriChanged();
}

QString WriteSp::getText() const {
	return _text;
}

void WriteSp::setText(QString text) {
	qDebug() << "XXXX WriteSp:setText(....)";
	if (_text.compare(text) == 0)
		return;
	_text = text;

	emit detectTextChanged();
}

void WriteSp::onUriChanged(QString uri) {
	qDebug() << "XXXX WriteSp:onUriChanged(....)";
	setUri(uri);
}

void WriteSp::onTextChanged(QString text) {
	qDebug() << "XXXX WriteSp:onTextChanged(....)";
	setText(text);
}
