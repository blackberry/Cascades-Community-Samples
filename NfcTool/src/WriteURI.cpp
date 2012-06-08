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

#include "WriteURI.hpp"
#include "NfcManager.hpp"
#include "Settings.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"

#include <qdebug>

using namespace bb::cascades;

WriteURI::WriteURI() :
		_appVersion(QString(Settings::AppVersion)) {
	qDebug() << "constructing WriteURI...";
	_uri = Settings::URI;
	_qml = QmlDocument::create("write_uri.qml");
	_qml->setContextProperty("_writeUriMenu", this);
	_root = _qml->createRootNode<AbstractPane>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

WriteURI::~WriteURI() {
	delete _eventLog;
}

void WriteURI::createModules() {
	qDebug() << "creating instances of WriteURI modules...";
	_eventLog = new EventLog("Bring a tag close.....");
	qDebug() << "...done";
}

void WriteURI::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void WriteURI::findAndConnectControls() {
	qDebug() << "finding all WriteURI controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "writeUri");
	QObject::connect(obj, SIGNAL(writeUriRequested()), this,
			SLOT(startWriteProcess()));

	TextArea* txf_uri = _root->findChild<TextArea*>("txf_uri");
	qDebug() << "found WriteUri text fields";
	qDebug() << "connecting all TextArea signals and slots...";
	QObject::connect(txf_uri, SIGNAL(textChanged(QString)), this,
			SLOT(onUriChanged(QString)));

	// refresh _root
	_root = _qml->createRootNode<AbstractPane>();

	qDebug() << "...done";
}

void WriteURI::startWriteProcess() {
	qDebug() << "startWriteProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "Telling NfcManager to start write process";

	// this transition should really be handled by the NfcManager when integrated
	qDebug() << "setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	nfc->writeUri(&_uri);

	qDebug() << "switching to event log screen";
	_eventLog->show();
}

void WriteURI::stopListening() {
	StateManager* state_mgr = StateManager::getInstance();
	if (state_mgr->inDetectAndWriteState()) {
		qDebug() << "stopListening()";
		NfcManager* nfc = NfcManager::getInstance();
		nfc->stopNdefWriter();
	}
}

void WriteURI::backFromEventLog() {
	stopListening();
}

void WriteURI::show() {
	qDebug() << "WriteURI: show()";

	qDebug() << "finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	// refresh _root
	_root = _qml->createRootNode<AbstractPane>();
	navpane->push(_root);

	//find and reconnect all the controls again, because we had to refresh the_root pointer
	findAndConnectControls();
}

QString WriteURI::appVersion() const {
	return _appVersion;
}

void WriteURI::setAppVersion(QString appVersion) {
	if (_appVersion.compare(appVersion) == 0)
		return;

	_appVersion = appVersion;

	emit detectAppVersionChanged();
}

QString WriteURI::getUri() const {
	return _uri;
}

void WriteURI::setUri(QString uri) {
	qDebug() << "WriteURI:setUri(....)";
	if (_uri.compare(uri) == 0)
		return;
	_uri = uri;
	emit detectUriChanged();
}

void WriteURI::onUriChanged(QString uri) {
	qDebug() << "WriteURI:onUriChanged(....)";
	setUri(uri);
}
