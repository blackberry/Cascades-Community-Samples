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


// NOT IN USE - work in progress


#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>
#include <bb/cascades/Button>
#include <bb/cascades/TextArea>

#include "ApduDetails.hpp"
#include "Settings.hpp"
#include "NfcManager.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"

using namespace bb::cascades;

ApduDetails::ApduDetails() {
	_aid = Settings::AID;
	_hex_cla = Settings::HEX_CLA;
	_hex_ins = Settings::HEX_INS;
	_hex_p1p2 = Settings::HEX_P1P2;
	_hex_lc = Settings::HEX_LC;
	_hex_command = Settings::HEX_COMMAND;
	_hex_le = Settings::HEX_LE;

	_qml = QmlDocument::create("asset:///apdu.qml");
	_qml->setContextProperty("_apdu", this);
	_root = _qml->createRootObject<Page>();

	createModules();
	connectNavigationSignals();
	findAndConnectControls();
}

ApduDetails::~ApduDetails() {
	qDebug() << "XXXX ApduDetails destructor";
}

void ApduDetails::createModules() {
	qDebug() << "XXXX creating instances of ApduDetails modules...";
	_eventLog = EventLog::getInstance();
	qDebug() << "XXXX ...done";
}

void ApduDetails::connectNavigationSignals() {
	QObject::connect(_eventLog, SIGNAL(back()), this, SLOT(backFromEventLog()));
}

void ApduDetails::findAndConnectControls() {
	qDebug() << "XXXX finding all ApduDetails controls signals and slots...";

	QObject* obj = _root->findChild<QObject*>((const QString) "apdu_details");
	QObject::connect(obj, SIGNAL(apduRequested()), this, SLOT(startApduDetailsProcess()));

	TextArea* txf_aid = _root->findChild<TextArea*>("txf_aid");
	TextArea* txf_cla = _root->findChild<TextArea*>("txf_cla");
	TextArea* txf_ins = _root->findChild<TextArea*>("txf_ins");
	TextArea* txf_p1p2 = _root->findChild<TextArea*>("txf_p1p2");
	TextArea* txf_lc = _root->findChild<TextArea*>("txf_lc");
	TextArea* txf_command = _root->findChild<TextArea*>("txf_command");
	TextArea* txf_le = _root->findChild<TextArea*>("txf_le");
	qDebug() << "XXXX found ApduDetails text fields";
	qDebug() << "XXXX connecting all TextField signals and slots...";
	QObject::connect(txf_aid, SIGNAL(textChanged(QString)), this, SLOT(onAidChanged(QString)));
	QObject::connect(txf_cla, SIGNAL(textChanged(QString)), this, SLOT(onClaChanged(QString)));
	QObject::connect(txf_ins, SIGNAL(textChanged(QString)), this, SLOT(onInsChanged(QString)));
	QObject::connect(txf_p1p2, SIGNAL(textChanged(QString)), this, SLOT(onP1p2Changed(QString)));
	QObject::connect(txf_lc, SIGNAL(textChanged(QString)), this, SLOT(onLcChanged(QString)));
	QObject::connect(txf_command, SIGNAL(textChanged(QString)), this, SLOT(onCommandChanged(QString)));
	QObject::connect(txf_lc, SIGNAL(textChanged(QString)), this, SLOT(onLcChanged(QString)));
	// refresh _root
	_root = _qml->createRootObject<Page>();

	qDebug() << "XXXX ...done";
}

void ApduDetails::startApduDetailsProcess() {
	qDebug() << "XXXX startApduDetailsProcess()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "XXXX Telling NfcManager to start NDEF push process";
	qDebug() << "XXXX setting inNdefPushState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setNdefPushState(false);

	nfc = NfcManager::getInstance();

	nfc->iso7816Test(&_aid, &_hex_cla, &_hex_ins, &_hex_p1p2, &_hex_lc, &_hex_command, &_hex_le);

	qDebug() << "XXXX switching to event log screen";
	_eventLog->show();
}

void ApduDetails::backFromEventLog() {

}

void ApduDetails::show() {
	qDebug() << "XXXX ApduDetails: show()";

	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	_root = _qml->createRootObject<Page>();
	navpane->push(_root);

	findAndConnectControls();
}

QString ApduDetails::getAid() const {
	return _aid;
}

void ApduDetails::setAid(QString aid) {
	qDebug() << "XXXX ApduDetails:setAid(....)";
	if (_aid.compare(aid) == 0)
		return;

	_aid = aid;
	emit detectAidChanged();
}

QString ApduDetails::getCla() const {
	return _hex_cla;
}

void ApduDetails::setCla(QString cla) {
	qDebug() << "XXXX ApduDetails:setCla(....)";
	if (_hex_cla.compare(cla) == 0)
		return;

	_hex_cla = cla;
	emit detectClaChanged();
}

QString ApduDetails::getIns() const {
	return _hex_ins;
}

void ApduDetails::setIns(QString ins) {
	qDebug() << "XXXX ApduDetails:setIns(....)";
	if (_hex_ins.compare(ins) == 0)
		return;

	_hex_ins = ins;
	emit detectInsChanged();
}

QString ApduDetails::getP1p2() const {
	return _hex_p1p2;
}

void ApduDetails::setP1p2(QString p1p2) {
	qDebug() << "XXXX ApduDetails:setP1p2(....)";
	if (_hex_p1p2.compare(p1p2) == 0)
		return;

	_hex_p1p2 = p1p2;
	emit detectP1p2Changed();
}

QString ApduDetails::getLc() const {
	return _hex_lc;
}

void ApduDetails::setLc(QString lc) {
	qDebug() << "XXXX ApduDetails:setLc(....)";
	if (_hex_lc.compare(lc) == 0)
		return;

	_hex_lc = lc;
	emit detectLcChanged();
}

QString ApduDetails::getCommand() const {
	return _hex_command;
}

void ApduDetails::setCommand(QString command) {
	qDebug() << "XXXX ApduDetails:setCommand(....)";
	if (_hex_command.compare(command) == 0)
		return;

	_hex_command = command;
	emit detectCommandChanged();
}

QString ApduDetails::getLe() const {
	return _hex_le;
}

void ApduDetails::setLe(QString le) {
	qDebug() << "XXXX ApduDetails:setLe(....)";
	if (_hex_le.compare(le) == 0)
		return;

	_hex_le = le;
	emit detectLeChanged();
}

void ApduDetails::onAidChanged(QString aid) {
	qDebug() << "XXXX ApduDetails:onAidChanged(....)";
	setAid(aid);
}

void ApduDetails::onClaChanged(QString cla) {
	qDebug() << "XXXX ApduDetails:onClaChanged(....)";
	setCla(cla);
}

void ApduDetails::onInsChanged(QString ins) {
	qDebug() << "XXXX ApduDetails:onInsChanged(....)";
	setIns(ins);
}

void ApduDetails::onP1p2Changed(QString p1p2) {
	qDebug() << "XXXX ApduDetails:onP1p2Changed(....)";
	setP1p2(p1p2);
}

void ApduDetails::onLcChanged(QString lc) {
	qDebug() << "XXXX ApduDetails:onLcChanged(....)";
	setLc(lc);
}

void ApduDetails::onCommandChanged(QString command) {
	qDebug() << "XXXX ApduDetails:onCommandChanged(....)";
	setCommand(command);
}

void ApduDetails::onLeChanged(QString le) {
	qDebug() << "XXXX ApduDetails:onLeChanged(....)";
	setLe(le);
}
