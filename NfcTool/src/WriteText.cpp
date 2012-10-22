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

#include "WriteText.hpp"
#include "NfcManager.hpp"
#include "StateManager.hpp"
#include "Settings.hpp"
#include "Navigator.hpp"

#include <QDebug>

using namespace bb::cascades;

WriteText::WriteText() :
		_appVersion(QString(Settings::AppVersion)) {
	qDebug() << "XXXX constructing WriteURI...";
	_text = Settings::TEXT;
	_qml = QmlDocument::create("asset:///write_text.qml");
	_qml->setContextProperty("_writeTextMenu", this);
	_root = _qml->createRootObject<Page>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

WriteText::~WriteText() {
	qDebug() << "XXXX WriteText destructor";
}

void WriteText::createModules() {
	qDebug() << "XXXX creating instances of WriteText modules...";
	_eventLog = EventLog::getInstance();
	qDebug() << "XXXX ...done";
}

void WriteText::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void WriteText::findAndConnectControls() {
	qDebug() << "XXXX finding all WriteText controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "writeText");
	QObject::connect(obj, SIGNAL(writeTextRequested()), this,
			SLOT(startWriteProcess()));

	TextArea* txf_text = _root->findChild<TextArea*>("txf_text");
	qDebug() << "XXXX found WriteText text fields";
	qDebug() << "XXXX connecting all TextArea signals and slots...";
	QObject::connect(txf_text, SIGNAL(textChanged(QString)), this,
			SLOT(onTextChanged(QString)));

	_root = _qml->createRootObject<Page>();

	qDebug() << "XXXX ...done";
}

void WriteText::startWriteProcess() {
	qDebug() << "XXXX startWriteProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "XXXX Telling NfcManager to start write process";

	qDebug() << "XXXX setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	nfc->writeText(&_text);

	qDebug() << "XXXX switching to event log screen";
	_eventLog->show();
}

void WriteText::backFromEventLog() {

}

void WriteText::show() {
	qDebug() << "XXXX WriteText: show()";

	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();

	_root = _qml->createRootObject<Page>();
	navpane->push(_root);

	findAndConnectControls();
}

QString WriteText::appVersion() const {
	return _appVersion;
}

void WriteText::setAppVersion(QString appVersion) {
	if (_appVersion.compare(appVersion) == 0)
		return;

	_appVersion = appVersion;

	emit detectAppVersionChanged();
}

QString WriteText::getText() const {
	return _text;
}

void WriteText::setText(QString text) {
	qDebug() << "XXXX WriteText:setText(....)";
	if (_text.compare(text) == 0)
		return;
	_text = text;
	emit detectTextChanged();
}

void WriteText::onTextChanged(QString text) {
	qDebug() << "XXXX WriteText:onTextChanged(....)";
	setText(text);
}
