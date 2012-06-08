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

NfcManager::NfcManager() : _workerInstance(0), _bpsThread(0) {
	qDebug() << "Constructing NfcManager";
	qDebug() << "Done constructing NfcManager";
}

NfcManager* NfcManager::getInstance() {
	if (_instance == 0 ) {
		_instance = new NfcManager;
	}
	return _instance;
}

void NfcManager::startNdefListener(QList<NdefType *> types) {
	qDebug() << "starting NDEF listener thread...";
	Logger::getInstance()->clearLog();
	_ndefReadTypeRegistered = types;
	future = new QFuture<void>;
	watcher = new QFutureWatcher<void>;
	_workerInstance = new NfcWorker();
	*future = QtConcurrent::run(_workerInstance, &NfcWorker::readTag, types);
	watcher->setFuture(*future);
	QObject::connect(watcher, SIGNAL(finished()), this, SLOT(workerStopped()));
	QObject::connect(_workerInstance, SIGNAL(message(QVariant)), this, SLOT(message(QVariant)), Qt::QueuedConnection);
	qDebug() << "startNdefListener - worker thread started";
}


void NfcManager::writeUri(QString* uri) {
	qDebug() << "NfcManager::writeUri";
	_ndefUri = uri;
	Logger::getInstance()->clearLog();
	future = new QFuture<void>;
	watcher = new QFutureWatcher<void>;
	_workerInstance = new NfcWorker();
	*future = QtConcurrent::run(_workerInstance, &NfcWorker::writeUriTag, uri);
	watcher->setFuture(*future);
	QObject::connect(watcher, SIGNAL(finished()), this, SLOT(workerStopped()));
	QObject::connect(_workerInstance, SIGNAL(message(QVariant)), this, SLOT(message(QVariant)), Qt::QueuedConnection);

	qDebug() << "NfcManager::writeUri done";
}

void NfcManager::writeSp(QString* sp_uri, QString* sp_text) {
	qDebug() << "NfcManager::writeSp";
	_ndefSpUri = sp_uri;
	_ndefSpText = sp_text;
	Logger::getInstance()->clearLog();
	future = new QFuture<void>;
	watcher = new QFutureWatcher<void>;
	_workerInstance = new NfcWorker();
	*future = QtConcurrent::run(_workerInstance, &NfcWorker::writeSpTag, sp_uri, sp_text);
	watcher->setFuture(*future);
	QObject::connect(watcher, SIGNAL(finished()), this, SLOT(workerStopped()));
	QObject::connect(_workerInstance, SIGNAL(message(QVariant)), this, SLOT(message(QVariant)), Qt::QueuedConnection);
	qDebug() << "NfcManager::writeSp done";
}

void NfcManager::writeText(QString* text) {
	qDebug() << "NfcManager::writeText";
	_ndefText = text;
	Logger::getInstance()->clearLog();
	future = new QFuture<void>;
	watcher = new QFutureWatcher<void>;
	_workerInstance = new NfcWorker();
	*future = QtConcurrent::run(_workerInstance, &NfcWorker::writeTextTag, text);
	watcher->setFuture(*future);
	QObject::connect(watcher, SIGNAL(finished()), this, SLOT(workerStopped()));
	QObject::connect(_workerInstance, SIGNAL(message(QVariant)), this, SLOT(message(QVariant)), Qt::QueuedConnection);
	qDebug() << "NfcManager::writeText done";
}

void NfcManager::writeCustom(QString* domain, QString* type, QString* payload) {
	qDebug() << "NfcManager::writeCustom";
	_ndefDomain = domain;
	_ndefType = type;
	_ndefPayload = payload;
	Logger::getInstance()->clearLog();
	future = new QFuture<void>;
	watcher = new QFutureWatcher<void>;
	_workerInstance = new NfcWorker();
	*future = QtConcurrent::run(_workerInstance, &NfcWorker::writeCustomTag, domain, type, payload);
	watcher->setFuture(*future);
	QObject::connect(watcher, SIGNAL(finished()), this, SLOT(workerStopped()));
	QObject::connect(_workerInstance, SIGNAL(message(QVariant)), this, SLOT(message(QVariant)), Qt::QueuedConnection);
	qDebug() << "NfcManager::writeCustom done";
}

void NfcManager::stopNdefListener() {
	qDebug() << "NfcManager::stopNdefListener";
	stopWorker();
}

void NfcManager::stopNdefWriter() {
	qDebug() << "NfcManager::stopNdefWriter";
	stopWorker();
}

void NfcManager::message(const QVariant &text) {
	Logger::getInstance()->log("hh:mm:ss", "INF", text.toString());
}

void NfcManager::stopWorker() {
	qDebug() << "NfcManager::stopWorker entered";
	if (_workerInstance) {
		QMetaObject::invokeMethod(
				_workerInstance,
				"interruptBpsWaitLoop",
				Qt::DirectConnection,
				Q_ARG(unsigned int, NfcWorker::TERMINATE));
	}
}

void NfcManager::workerStopped() {
	qDebug() << "NfcManager::workerStopped entered";

	Logger::getInstance()->log("hh:mm:ss", "INF", "Worker thread has stopped");

	delete _workerInstance;
	_workerInstance = 0;

}
