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
	_qml = QmlDocument::create("vcard.qml");
	_qml->setContextProperty("_sendVcard", this);
	_root = _qml->createRootNode<AbstractPane>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

SendVcard::~SendVcard() {
	delete _eventLog;
}

void SendVcard::createModules() {
	qDebug() << "creating instances of SendVcard modules...";
	_eventLog = new EventLog("Bring a tag close.....");
	qDebug() << "...done";
}

void SendVcard::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void SendVcard::findAndConnectControls() {
	qDebug() << "finding all SendVcard controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "sendVcard");
	QObject::connect(obj, SIGNAL(sendVcardRequested()), this,
			SLOT(startSendVcardProcess()));

	TextArea* txf_first_name = _root->findChild<TextArea*>("txf_first_name");
	TextArea* txf_last_name = _root->findChild<TextArea*>("txf_last_name");
	TextArea* txf_address = _root->findChild<TextArea*>("txf_address");
	TextArea* txf_email = _root->findChild<TextArea*>("txf_email");
	TextArea* txf_mobile = _root->findChild<TextArea*>("txf_mobile");
	qDebug() << "found SendVcard text fields";
	qDebug() << "connecting all TextField signals and slots...";
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
	_root = _qml->createRootNode<AbstractPane>();

	qDebug() << "...done";
}

void SendVcard::startSendVcardProcess() {
	qDebug() << "startSendVcardProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "Telling NfcManager to start NDEF push process";
	qDebug() << "setting inNdefPushState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setNdefPushState(true);

	nfc->sendVcard(&_first_name, &_last_name, &_address, &_email, &_mobile);

	qDebug() << "switching to event log screen";
	_eventLog->show();
}

void SendVcard::backFromEventLog() {

}

void SendVcard::show() {
	qDebug() << "SendVcard: show()";

	qDebug() << "finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	// refresh _root
	_root = _qml->createRootNode<AbstractPane>();
	navpane->push(_root);

	//find and reconnect all the controls again, because we had to refresh the root pointer
	findAndConnectControls();
}

QString SendVcard::getFirstName() const {
	return _first_name;
}

void SendVcard::setFirstName(QString first_name) {
	qDebug() << "SendVcard:setFirstName(....)";
	if (_first_name.compare(first_name) == 0)
		return;

	_first_name = first_name;

	emit detectFirstNameChanged();
}

QString SendVcard::getLastName() const {
	return _last_name;
}

void SendVcard::setLastName(QString last_name) {
	qDebug() << "SendVcard:setLastName(....)";
	if (_last_name.compare(last_name) == 0)
		return;

	_last_name = last_name;

	emit detectLastNameChanged();
}


QString SendVcard::getAddress() const {
	return _address;
}

void SendVcard::setAddress(QString address) {
	qDebug() << "SendVcard:setAddress(....)";
	if (_address.compare(address) == 0)
		return;

	_address = address;

	emit detectAddressChanged();
}

QString SendVcard::getEmail() const {
	return _email;
}

void SendVcard::setEmail(QString email) {
	qDebug() << "SendVcard:setEmail(....)";
	if (_email.compare(email) == 0)
		return;

	_email = email;

	emit detectEmailChanged();
}

QString SendVcard::getMobile() const {
	return _mobile;
}

void SendVcard::setMobile(QString mobile) {
	qDebug() << "SendVcard:setMobile(....)";
	if (_mobile.compare(mobile) == 0)
		return;

	_mobile = mobile;

	emit detectMobileChanged();
}


void SendVcard::onFirstNameChanged(QString first_name) {
	qDebug() << "SendVcard:onFirstNameChanged(....)";
	setFirstName(first_name);
}

void SendVcard::onLastNameChanged(QString last_name) {
	qDebug() << "SendVcard:onLastNameChanged(....)";
	setLastName(last_name);
}

void SendVcard::onAddressChanged(QString address) {
	qDebug() << "SendVcard:onAddressChanged(....)";
	setAddress(address);
}

void SendVcard::onEmailChanged(QString email) {
	qDebug() << "SendVcard:onEmailChanged(....)";
	setEmail(email);
}

void SendVcard::onMobileChanged(QString mobile) {
	qDebug() << "SendVcard:onMobileChanged(....)";
	setMobile(mobile);
}
