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

#include "SendVcard.hpp"
#include "Settings.hpp"
#include "NfcManager.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"

using namespace bb::cascades;

SendVcard::SendVcard() {
	_first_name = Settings::FIRST_NAME;
	_last_name = Settings::LAST_NAME;
	_address = Settings::ADDRESS;
	_email = Settings::EMAIL;
	_mobile = Settings::MOBILE;
	_qml = QmlDocument::create("asset:///vcard.qml");
	_qml->setContextProperty("_sendVcard", this);
	_root = _qml->createRootObject<Page>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

SendVcard::~SendVcard() {
	qDebug() << "XXXX SendVcard destructor";
}

void SendVcard::createModules() {
	qDebug() << "XXXX creating instances of SendVcard modules...";
	_eventLog = EventLog::getInstance();
	qDebug() << "XXXX ...done";
}

void SendVcard::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void SendVcard::findAndConnectControls() {
	qDebug() << "XXXX finding all SendVcard controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "sendVcard");
	QObject::connect(obj, SIGNAL(sendVcardRequested()), this,
			SLOT(startSendVcardProcess()));

	TextArea* txf_first_name = _root->findChild<TextArea*>("txf_first_name");
	TextArea* txf_last_name = _root->findChild<TextArea*>("txf_last_name");
	TextArea* txf_address = _root->findChild<TextArea*>("txf_address");
	TextArea* txf_email = _root->findChild<TextArea*>("txf_email");
	TextArea* txf_mobile = _root->findChild<TextArea*>("txf_mobile");
	qDebug() << "XXXX found SendVcard text fields";
	qDebug() << "XXXX connecting all TextField signals and slots...";
	QObject::connect(txf_first_name, SIGNAL(textChanged(QString)), this,
			SLOT(onFirstNameChanged(QString)));
	QObject::connect(txf_last_name, SIGNAL(textChanged(QString)), this,
			SLOT(onLastNameChanged(QString)));
	QObject::connect(txf_address, SIGNAL(textChanged(QString)), this,
			SLOT(onAddressChanged(QString)));
	QObject::connect(txf_email, SIGNAL(textChanged(QString)), this,
			SLOT(onEmailChanged(QString)));
	QObject::connect(txf_mobile, SIGNAL(textChanged(QString)), this,
			SLOT(onMobileChanged(QString)));

	// refresh _root
	_root = _qml->createRootObject<Page>();

	qDebug() << "XXXX ...done";
}

void SendVcard::startSendVcardProcess() {
	qDebug() << "XXXX startSendVcardProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "XXXX Telling NfcManager to start NDEF push process";
	qDebug() << "XXXX setting inNdefPushState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setNdefPushState(true);

	nfc->sendVcard(&_first_name, &_last_name, &_address, &_email, &_mobile);

	qDebug() << "XXXX switching to event log screen";
	_eventLog->show();
}

void SendVcard::backFromEventLog() {

}

void SendVcard::show() {
	qDebug() << "XXXX SendVcard: show()";

	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	_root = _qml->createRootObject<Page>();
	navpane->push(_root);

	findAndConnectControls();
}

QString SendVcard::getFirstName() const {
	return _first_name;
}

void SendVcard::setFirstName(QString first_name) {
	qDebug() << "XXXX SendVcard:setFirstName(....)";
	if (_first_name.compare(first_name) == 0)
		return;

	_first_name = first_name;

	emit detectFirstNameChanged();
}

QString SendVcard::getLastName() const {
	return _last_name;
}

void SendVcard::setLastName(QString last_name) {
	qDebug() << "XXXX SendVcard:setLastName(....)";
	if (_last_name.compare(last_name) == 0)
		return;

	_last_name = last_name;

	emit detectLastNameChanged();
}

QString SendVcard::getAddress() const {
	return _address;
}

void SendVcard::setAddress(QString address) {
	qDebug() << "XXXX SendVcard:setAddress(....)";
	if (_address.compare(address) == 0)
		return;

	_address = address;

	emit detectAddressChanged();
}

QString SendVcard::getEmail() const {
	return _email;
}

void SendVcard::setEmail(QString email) {
	qDebug() << "XXXX SendVcard:setEmail(....)";
	if (_email.compare(email) == 0)
		return;

	_email = email;

	emit detectEmailChanged();
}

QString SendVcard::getMobile() const {
	return _mobile;
}

void SendVcard::setMobile(QString mobile) {
	qDebug() << "XXXX SendVcard:setMobile(....)";
	if (_mobile.compare(mobile) == 0)
		return;

	_mobile = mobile;

	emit detectMobileChanged();
}

void SendVcard::onFirstNameChanged(QString first_name) {
	qDebug() << "XXXX SendVcard:onFirstNameChanged(....)";
	setFirstName(first_name);
}

void SendVcard::onLastNameChanged(QString last_name) {
	qDebug() << "XXXX SendVcard:onLastNameChanged(....)";
	setLastName(last_name);
}

void SendVcard::onAddressChanged(QString address) {
	qDebug() << "XXXX SendVcard:onAddressChanged(....)";
	setAddress(address);
}

void SendVcard::onEmailChanged(QString email) {
	qDebug() << "XXXX SendVcard:onEmailChanged(....)";
	setEmail(email);
}

void SendVcard::onMobileChanged(QString mobile) {
	qDebug() << "XXXX SendVcard:onMobileChanged(....)";
	setMobile(mobile);
}
