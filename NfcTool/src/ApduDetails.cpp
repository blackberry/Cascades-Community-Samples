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
#include <bb/platform/NotificationDialog>

#include "ApduDetails.hpp"
#include "Settings.hpp"
#include "NfcManager.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"
#include "Utilities.hpp"
#include <bb/system/SystemDialog.hpp>
#include <bb/system/SystemUiButton.hpp>

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

	_systemDialog = new bb::system::SystemDialog(this);
	_systemDialog->setTitle("Validation Error");
	bb::system::SystemUiButton *confirmButton = _systemDialog->confirmButton();
	confirmButton->setLabel("OK");
	connect(_systemDialog, SIGNAL(accepted()), this, SLOT(onDialogAccepted()));

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
	QObject::connect(txf_le, SIGNAL(textChanged(QString)), this, SLOT(onLeChanged(QString)));
	// refresh _root
	_root = _qml->createRootObject<Page>();

	qDebug() << "XXXX ...done";
}

void ApduDetails::startApduDetailsProcess() {
	qDebug() << "XXXX startApduDetailsProcess()";

	// validate and remove spaces

	_aid.remove(QChar(' '), Qt::CaseInsensitive);
	_hex_cla.remove(QChar(' '), Qt::CaseInsensitive);
	_hex_ins.remove(QChar(' '), Qt::CaseInsensitive);
	_hex_p1p2.remove(QChar(' '), Qt::CaseInsensitive);
	_hex_lc.remove(QChar(' '), Qt::CaseInsensitive);
	_hex_command.remove(QChar(' '), Qt::CaseInsensitive);
	_hex_le.remove(QChar(' '), Qt::CaseInsensitive);

	_aid = _aid.toUpper();
	_hex_cla = _hex_cla.toUpper();
	_hex_ins = _hex_ins.toUpper();
	_hex_p1p2 = _hex_p1p2.toUpper();
	_hex_lc = _hex_lc.toUpper();
	_hex_command = _hex_command.toUpper();
	_hex_le = _hex_le.toUpper();

	qDebug() << "XXXX startApduDetailsProcess() is validating user input";
	if (!Utilities::isValidHex(_aid)) {
		qDebug() << "XXXX AID does not contain a valid hex string";
		_systemDialog->setBody("AID does not contain a valid hex string");
		_systemDialog->show();
		return;
	}

	if (!Utilities::isValidHex(_hex_cla)) {
		qDebug() << "XXXX CLA does not contain a valid hex string";
		_systemDialog->setBody("CLA does not contain a valid hex string");
		_systemDialog->show();
		return;
	}

	if (!Utilities::isValidHex(_hex_ins)) {
		qDebug() << "XXXX INS does not contain a valid hex string";
		_systemDialog->setBody("INS does not contain a valid hex string");
		_systemDialog->show();
		return;
	}

	if (!Utilities::isValidHex(_hex_p1p2)) {
		qDebug() << "XXXX P1 P2 does not contain a valid hex string";
		_systemDialog->setBody("P1 P2 does not contain a valid hex string");
		_systemDialog->show();
		return;
	}

	if (!Utilities::isValidHex(_hex_lc)) {
		qDebug() << "XXXX LC does not contain a valid hex string";
		_systemDialog->setBody("LC does not contain a valid hex string");
		_systemDialog->show();
		return;
	}

	if (!Utilities::isValidHex(_hex_command)) {
		qDebug() << "XXXX COMMAND does not contain a valid hex string";
		_systemDialog->setBody("COMMAND does not contain a valid hex string");
		_systemDialog->show();
		return;
	}

	if (!Utilities::isValidHex(_hex_le)) {
		qDebug() << "XXXX LE does not contain a valid hex string";
		_systemDialog->setBody("LE does not contain a valid hex string");
		_systemDialog->show();
		return;
	}

	NfcManager* nfc = NfcManager::getInstance();

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

void ApduDetails::onDialogAccepted() {
	qDebug() << "XXXX ApduDetails:onDialogAccepted()";
}
