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

#include <QDebug>

using namespace bb::cascades;

WriteURI::WriteURI() :
		_appVersion(QString(Settings::AppVersion)) {
	qDebug() << "XXXX constructing WriteURI...";
	_uri = Settings::URI;
	_qml = QmlDocument::create("asset:///write_uri.qml");
	_qml->setContextProperty("_writeUriMenu", this);
	_root = _qml->createRootObject<Page>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

WriteURI::~WriteURI() {
	qDebug() << "XXXX WriteUri destructor";
}

void WriteURI::createModules() {
	qDebug() << "XXXX creating instances of WriteURI modules...";
	_eventLog = EventLog::getInstance();
	qDebug() << "XXXX ...done";
}

void WriteURI::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void WriteURI::findAndConnectControls() {
	qDebug() << "XXXX finding all WriteURI controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "writeUri");
	QObject::connect(obj, SIGNAL(writeUriRequested()), this,
			SLOT(startWriteProcess()));

	TextArea* txf_uri = _root->findChild<TextArea*>("txf_uri");
	qDebug() << "XXXX found WriteUri text fields";
	qDebug() << "XXXX connecting all TextArea signals and slots...";
	QObject::connect(txf_uri, SIGNAL(textChanged(QString)), this,
			SLOT(onUriChanged(QString)));

	_root = _qml->createRootObject<Page>();

	qDebug() << "XXXX ...done";
}

void WriteURI::startWriteProcess() {
	qDebug() << "XXXX startWriteProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "XXXX Telling NfcManager to start write process";

	qDebug() << "XXXX setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	nfc->writeUri(&_uri);

	qDebug() << "XXXX switching to event log screen";
	_eventLog->show();
}

void WriteURI::backFromEventLog() {

}

void WriteURI::show() {
	qDebug() << "XXXX WriteURI: show()";

	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();

	_root = _qml->createRootObject<Page>();
	navpane->push(_root);

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
	qDebug() << "XXXX WriteURI:setUri(....)";
	if (_uri.compare(uri) == 0)
		return;
	_uri = uri;
	emit detectUriChanged();
}

void WriteURI::onUriChanged(QString uri) {
	qDebug() << "XXXX WriteURI:onUriChanged(....)";
	setUri(uri);
}
