/* Copyright (c) 2013 BlackBerry Limited.
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

#include "Llcp.hpp"
#include "Settings.hpp"
#include "NfcManager.hpp"
#include "NfcWorker.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"

using namespace bb::cascades;

Llcp::Llcp() {
	qDebug() << "XXXX constructing Llcp object";
	_qml = QmlDocument::create("asset:///llcp.qml");
	_qml->setContextProperty("_llcp", this);
	_root = _qml->createRootObject<Page>();
	qDebug() << "XXXX calling Llcp::findAndConnectControls";
}

Llcp::~Llcp() {
	qDebug() << "XXXX Llcp destructor";
}

void Llcp::findAndConnectControls() {
	qDebug() << "XXXX Llcp: findAndConnectControls()";

	_workerInstance = NfcWorker::getInstance();
	QObject::connect(_workerInstance, SIGNAL(logMessageHere(QString)), this, SLOT(logMessageHere(QString)));

	QObject* obj = _root->findChild<QObject*>((const QString) "llcp");

	QObject::connect(obj, SIGNAL(llcpBackButton()), this,	SLOT(onBack()));

	qDebug() << "XXXX ...done";
}

void Llcp::startLlcp() {
	qDebug() << "XXXX startLlcp()";
	NfcManager* nfc = NfcManager::getInstance();

	qDebug() << "XXXX Telling NfcManager to start NDEF push process";
	qDebug() << "XXXX setting inNdefPushState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setLlcpState(true);

	nfc->startLlcp();

}

void Llcp::stopLlcp() {

}

void Llcp::sendOverLlcp(QString text) {
	qDebug() << "XXXX sendOverLlcp(" << text << ")";
	_llcp_send = true;
	_llcp_text = text;
	_workerInstance = NfcWorker::getInstance();
	_workerInstance->setLlcpSendFlag();
	_workerInstance->setLlcpSendText(text);
}

void Llcp::onBack() {
	qDebug() << "XXXX Llcp::onBack()";
	NfcManager* nfc = NfcManager::getInstance();
	nfc->resetWorker();
	Navigator* nav = Navigator::getInstance();
	nav->getNavigationPane()->pop();
}

void Llcp::show() {
	qDebug() << "XXXX Llcp: show()";

	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	_root = _qml->createRootObject<Page>();

	findAndConnectControls();

	navpane->push(_root);

	startLlcp();

}

QString Llcp::getText() const {
	return _llcp_text;
}

void Llcp::setText(QString text) {
	qDebug() << "XXXX Llcp:setText(....)";
	if (_llcp_text.compare(text) == 0)
		return;

	_llcp_text = text;

}

void Llcp::logMessageHere(QString message) {
    qDebug() << "XXXX " << message;
    QMetaObject::invokeMethod(_root, "logMessage", Q_ARG(QVariant, message));
}
