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

#include <qdebug>

using namespace bb::cascades;

WriteText::WriteText() :
		_appVersion(QString(Settings::AppVersion)) {
	qDebug() << "constructing WriteURI...";
	_text = Settings::TEXT;
	_qml = QmlDocument::create("write_text.qml");
	_qml->setContextProperty("_writeTextMenu", this);
	_root = _qml->createRootNode<AbstractPane>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

WriteText::~WriteText() {
	delete _eventLog;
}

void WriteText::createModules() {
	qDebug() << "creating instances of WriteText modules...";
	_eventLog = new EventLog("Bring a tag close.....");
	qDebug() << "...done";
}

void WriteText::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void WriteText::findAndConnectControls() {
	qDebug() << "finding all WriteText controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "writeText");
	QObject::connect(obj, SIGNAL(writeTextRequested()), this,
			SLOT(startWriteProcess()));

	TextArea* txf_text = _root->findChild<TextArea*>("txf_text");
	qDebug() << "found WriteText text fields";
	qDebug() << "connecting all TextArea signals and slots...";
	QObject::connect(txf_text, SIGNAL(textChanged(QString)), this,
			SLOT(onTextChanged(QString)));

	// refresh _root
	_root = _qml->createRootNode<AbstractPane>();

	qDebug() << "...done";
}

void WriteText::startWriteProcess() {
	qDebug() << "startWriteProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "Telling NfcManager to start write process";

	// this transition should really be handled by the NfcManager when integrated
	qDebug() << "setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	nfc->writeText(&_text);

	qDebug() << "switching to event log screen";
	_eventLog->show();
}

void WriteText::stopListening() {
	StateManager* state_mgr = StateManager::getInstance();
	if (state_mgr->inDetectAndWriteState()) {
		qDebug() << "stopListening()";
		NfcManager* nfc = NfcManager::getInstance();
		nfc->stopNdefWriter();
	}
}

void WriteText::backFromEventLog() {
	stopListening();
}

void WriteText::show() {
	qDebug() << "WriteText: show()";

	qDebug() << "finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	// refresh _root
	_root = _qml->createRootNode<AbstractPane>();
	navpane->push(_root);

	//find and reconnect all the controls again, because we had to refresh the_root pointer
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
	qDebug() << "WriteText:setText(....)";
	if (_text.compare(text) == 0)
		return;
	_text = text;
	emit detectTextChanged();
}

void WriteText::onTextChanged(QString text) {
	qDebug() << "WriteText:onTextChanged(....)";
	setText(text);
}
