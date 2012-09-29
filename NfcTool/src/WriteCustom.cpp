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

#include "WriteCustom.hpp"
#include "Settings.hpp"
#include "NfcManager.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"

using namespace bb::cascades;

WriteCustom::WriteCustom() :
		_appVersion(QString(Settings::AppVersion)) {
	_domain = Settings::DOMAIN;
	_type = Settings::TYPE;
	_content = Settings::CONTENT;
	_qml = QmlDocument::create("asset:///write_custom.qml");
	_qml->setContextProperty("_writeCustMenu", this);
	_root = _qml->createRootObject<Page>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

WriteCustom::~WriteCustom() {
	qDebug() << "XXXX WriteCustom destructor";
}

void WriteCustom::createModules() {
	qDebug() << "XXXX creating instances of WriteCustom modules...";
	_eventLog = EventLog::getInstance();
	qDebug() << "XXXX ...done";
}

void WriteCustom::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void WriteCustom::findAndConnectControls() {
	qDebug() << "XXXX finding all WriteCustom controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "writeCustom");
	QObject::connect(obj, SIGNAL(writeCustomRequested()), this,
			SLOT(startWriteProcess()));

	TextArea* txf_domain = _root->findChild<TextArea*>("txf_domain");
	TextArea* txf_type = _root->findChild<TextArea*>("txf_type");
	TextArea* txf_content = _root->findChild<TextArea*>("txf_content");
	qDebug() << "XXXX found WriteCustom text fields";
	qDebug() << "XXXX connecting all TextField signals and slots...";
	QObject::connect(txf_domain, SIGNAL(textChanged(QString)), this,
			SLOT(onDomainChanged(QString)));
	QObject::connect(txf_type, SIGNAL(textChanged(QString)), this,
			SLOT(onTypeChanged(QString)));
	QObject::connect(txf_content, SIGNAL(textChanged(QString)), this,
			SLOT(onContentChanged(QString)));

	_root = _qml->createRootObject<Page>();

	qDebug() << "XXXX ...done";
}

void WriteCustom::startWriteProcess() {
	qDebug() << "XXXX startWriteProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "XXXX Telling NfcManager to start write process";

	// this transition should really be handled by the NfcManager when integrated
	qDebug() << "XXXX setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	nfc->writeCustom(&_domain, &_type, &_content);

	qDebug() << "XXXX switching to event log screen";
	_eventLog->show();
}

void WriteCustom::backFromEventLog() {

}

void WriteCustom::show() {
	qDebug() << "XXXX WriteCustom: show()";

	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();

	_root = _qml->createRootObject<Page>();
	navpane->push(_root);

	findAndConnectControls();
}

QString WriteCustom::getAppVersion() const {
	return _appVersion;
}

void WriteCustom::setAppVersion(QString appVersion) {
	if (_appVersion.compare(appVersion) == 0)
		return;

	_appVersion = appVersion;

	emit detectAppVersionChanged();
}

QString WriteCustom::getDomain() const {
	return _domain;
}

void WriteCustom::setDomain(QString domain) {
	qDebug() << "XXXX WriteCustom:setDomain(....)";
	if (_domain.compare(domain) == 0)
		return;

	_domain = domain;

	emit detectDomainChanged();
}

QString WriteCustom::getType() const {
	return _type;
}

void WriteCustom::setType(QString type) {
	qDebug() << "XXXX WriteCustom:setType(....)";
	if (_type.compare(type) == 0)
		return;

	_type = type;

	emit detectTypeChanged();
}

QString WriteCustom::getContent() const {
	return _content;
}

void WriteCustom::setContent(QString content) {
	qDebug() << "XXXX WriteCustom:setContent(....)";
	if (_content.compare(content) == 0)
		return;

	_content = content;

	emit detectContentChanged();
}

void WriteCustom::onDomainChanged(QString domain) {
	qDebug() << "XXXX WriteCustom:onDomainChanged(....)";
	setDomain(domain);

}

void WriteCustom::onTypeChanged(QString type) {
	qDebug() << "XXXX WriteCustom:onTypeChanged(....)";
	setType(type);
}

void WriteCustom::onContentChanged(QString content) {
	qDebug() << "XXXX WriteCustom:onContentChanged(....)";
	setContent(content);
}
