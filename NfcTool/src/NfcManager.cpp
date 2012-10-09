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
#include "NfcManager.hpp"
#include "NdefType.hpp"
#include "Logger.hpp"
#include "NfcWorker.hpp"

NfcManager* NfcManager::_instance;

NfcManager::NfcManager() :
		_workerInstance(0), _bpsThread(0), _future(0), _watcher(0) {
	qDebug() << "XXXX Constructing NfcManager";
	_workerInstance = NfcWorker::getInstance();
	qDebug() << "XXXX Done constructing NfcManager";
}

NfcManager::~NfcManager() {
	qDebug() << "XXXX NcfManager destructor";

	_instance = 0;

	stopNfcWorker();

	if (_future) {
		qDebug() << "XXXX NcfManager deleting QtConcurrent";
		delete _future;
		_future = 0;
	}

	if (_watcher) {
		qDebug() << "XXXX NcfManager deleting Watcher";
		delete _watcher;
		_watcher = 0;
	}

	if (_workerInstance) {
		qDebug() << "XXXX NcfManager deleting Worker";
		delete _workerInstance;
		_workerInstance = 0;
	}
}

NfcManager* NfcManager::getInstance() {
	if (_instance == 0) {
		_instance = new NfcManager;
	}
	return _instance;
}

void NfcManager::startEventProcessing() {
	qDebug() << "XXXX starting event processing...";
	_future = new QFuture<void>;
	_watcher = new QFutureWatcher<void>;
	*_future = QtConcurrent::run(_workerInstance, &NfcWorker::startEventLoop);
	_watcher->setFuture(*_future);
	QObject::connect(_watcher, SIGNAL(finished()), this, SLOT(workerStopped()));
	QObject::connect(_workerInstance, SIGNAL(message(QVariant)), this, SLOT(message(QVariant)), Qt::QueuedConnection);
	QObject::connect(_workerInstance, SIGNAL(clearMessages()), this, SLOT(clearMessages()), Qt::QueuedConnection);
	qDebug() << "XXXX startEventProcessing - event loop started";
}

void NfcManager::stopNfcWorker() {
	qDebug() << "XXXX About to request NfcWorker to stop";
	if (_workerInstance)
		_workerInstance->interruptBpsWaitLoop(NfcWorker::TERMINATE);
}


void NfcManager::handleTagReadInvocation(QByteArray data) {
	qDebug() << "XXXX NfcManager::handleTagReadInvocation";
	_workerInstance->handleTagReadInvocation(data);
}

void NfcManager::startTagEmulation(QString *uri, QString *text) {
	qDebug() << "XXXX NfcManager::startTagEmulation";
	_ndefSpUri = uri;
	_ndefSpText = text;
	Logger::getInstance()->clearLog();
	emit message("Touch reader");
	_workerInstance = NfcWorker::getInstance();
	QObject::connect(this, SIGNAL(start_tag_emulation(QVariant,QVariant)), _workerInstance, SLOT(emulateTag(QVariant,QVariant)), Qt::QueuedConnection);
	emit start_tag_emulation(QVariant::fromValue(*uri), QVariant::fromValue(*text));
	qDebug() << "XXXX NfcManager::startTagEmulation done";
}

void NfcManager::stopTagEmulation() {
	qDebug() << "XXXX NfcManager::stopTagEmulation";
	Logger::getInstance()->clearLog();
	_workerInstance = NfcWorker::getInstance();
	QObject::connect(this, SIGNAL(stop_tag_emulation()), _workerInstance, SLOT(stopEmulatingTag()), Qt::QueuedConnection);
	emit stop_tag_emulation();
	qDebug() << "XXXX NfcManager::stopTagEmulation done";
}

void NfcManager::writeUri(QString* uri) {
	qDebug() << "XXXX NfcManager::writeUri";
	_ndefUri = uri;
	Logger::getInstance()->clearLog();
	_workerInstance = NfcWorker::getInstance();
	QObject::connect(this, SIGNAL(start_write_uri(QVariant)), _workerInstance, SLOT(writeUriTag(QVariant)), Qt::QueuedConnection);
	emit start_write_uri(QVariant::fromValue(*uri));
	qDebug() << "XXXX NfcManager::writeUri done";
}

void NfcManager::writeSp(QString* sp_uri, QString* sp_text) {
	qDebug() << "XXXX NfcManager::writeSp";
	_ndefSpUri = sp_uri;
	_ndefSpText = sp_text;
	Logger::getInstance()->clearLog();
	_workerInstance = NfcWorker::getInstance();
	QObject::connect(this, SIGNAL(start_write_sp(QVariant,QVariant)), _workerInstance, SLOT(writeSpTag(QVariant,QVariant)), Qt::QueuedConnection);
	emit start_write_sp(QVariant::fromValue(*sp_uri), QVariant::fromValue(*sp_text));
	qDebug() << "XXXX NfcManager::writeSp done";
}

void NfcManager::writeText(QString* text) {
	qDebug() << "XXXX NfcManager::writeText";
	_ndefText = text;
	Logger::getInstance()->clearLog();
	_workerInstance = NfcWorker::getInstance();
	QObject::connect(this, SIGNAL(start_write_text(QVariant)), _workerInstance, SLOT(writeTextTag(QVariant)), Qt::QueuedConnection);
	emit start_write_text(QVariant::fromValue(*text));
	qDebug() << "XXXX NfcManager::writeText done";
}

void NfcManager::writeCustom(QString* domain, QString* type, QString* payload) {
	qDebug() << "XXXX NfcManager::writeCustom";
	_ndefDomain = domain;
	_ndefType = type;
	_ndefPayload = payload;
	Logger::getInstance()->clearLog();
	_workerInstance = NfcWorker::getInstance();
	QObject::connect(this, SIGNAL(start_write_custom(QVariant,QVariant,QVariant)), _workerInstance, SLOT(writeCustomTag(QVariant,QVariant,QVariant)),
			Qt::QueuedConnection);
	emit start_write_custom(QVariant::fromValue(*domain), QVariant::fromValue(*type), QVariant::fromValue(*payload));
	qDebug() << "XXXX NfcManager::writeCustom done";
}

void NfcManager::sendVcard(QString* first_name, QString* last_name, QString* address, QString* email, QString* mobile) {
	qDebug() << "XXXX NfcManager::sendVcard";
	_first_name = first_name;
	_last_name = last_name;
	_address = address;
	_email = email;
	_mobile = mobile;
	Logger::getInstance()->clearLog();
	_workerInstance = NfcWorker::getInstance();
	QObject::connect(this, SIGNAL(start_send_vcard(QVariant,QVariant,QVariant,QVariant,QVariant)), _workerInstance,
			SLOT(sendVcard(QVariant,QVariant,QVariant,QVariant,QVariant)), Qt::QueuedConnection);
	emit start_send_vcard(QVariant::fromValue(*first_name), QVariant::fromValue(*last_name), QVariant::fromValue(*address), QVariant::fromValue(*email),
			QVariant::fromValue(*mobile));
	qDebug() << "XXXX NfcManager::sendVcard done";
}

void NfcManager::iso7816Test(QString* aid, QString* hex_cla, QString* hex_ins, QString* hex_p1p2, QString* hex_lc, QString* hex_command, QString* hex_le) {
	Logger::getInstance()->clearLog();
	_workerInstance = NfcWorker::getInstance();
	QObject::connect(this, SIGNAL(start_iso7816_test(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)), _workerInstance,
			SLOT(doIso7816Test(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)), Qt::QueuedConnection);
	emit start_iso7816_test(QVariant::fromValue(*aid), QVariant::fromValue(*hex_cla), QVariant::fromValue(*hex_ins), QVariant::fromValue(*hex_p1p2),
			QVariant::fromValue(*hex_lc), QVariant::fromValue(*hex_command), QVariant::fromValue(*hex_le));
}

void NfcManager::message(const QVariant &text) {
	Logger::getInstance()->log("hh:mm:ss", "INF", text.toString());
}

void NfcManager::clearMessages() {
	Logger::getInstance()->clearLog();
}

void NfcManager::workerStopped() {
	qDebug() << "XXXX NfcManager::workerStopped entered";

	Logger::getInstance()->log("hh:mm:ss", "INF", "Worker thread has stopped");
	emit nfcManagerStopped();
}
