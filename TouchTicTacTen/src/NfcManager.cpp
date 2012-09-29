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
#include "NfcWorker.hpp"
#include "App.hpp"
#include <QtCore/qtconcurrentrun.h>

NfcManager* NfcManager::_instance;

NfcManager::NfcManager() :
		_workerInstance(0), _bpsThread(0), _future(0), _watcher(0) {
	qDebug() << "XXXX Constructing NfcManager";
	_future = new QFuture<void>;
	_watcher = new QFutureWatcher<void>;
	_workerInstance = NfcWorker::getInstance();
	App* app = App::getInstance();
	QObject::connect(_workerInstance, SIGNAL(they_made_move()), app, SLOT(ourTurnNow()), Qt::QueuedConnection);
	QObject::connect(_workerInstance, SIGNAL(we_made_move()), app, SLOT(ourTurnDone()), Qt::QueuedConnection);
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
	*_future = QtConcurrent::run(_workerInstance, &NfcWorker::startEventLoop);
	_watcher->setFuture(*_future);
	QObject::connect(_watcher, SIGNAL(finished()), this, SLOT(workerStopped()));
	QObject::connect(_workerInstance, SIGNAL(message(QVariant)), this, SLOT(message(QVariant)), Qt::QueuedConnection);
	qDebug() << "XXXX startEventProcessing - event loop started";
}

void NfcManager::stopNfcWorker() {
	qDebug() << "XXXX About to request NfcWorker to stop";
	if (_workerInstance)
		_workerInstance->interruptBpsWaitLoop(NfcWorker::TERMINATE);
}

void NfcManager::prepareToPassTurn() {
	qDebug() << "XXXX NfcManager::prepareToPassTurn";
	_workerInstance->passTurn();
	qDebug() << "XXXX NfcManager::prepareToPassTurn done";
}

void NfcManager::disablePassingTurn() {
	qDebug() << "XXXX NfcManager::disablePassingTurn";
	_workerInstance->stopPassTurn();
	qDebug() << "XXXX NfcManager::disablePassingTurn done";
}

void NfcManager::workerStopped() {
	qDebug() << "XXXX NfcManager::workerStopped entered";
	emit nfcManagerStopped();
}

void NfcManager::message(const QVariant &text) {
	App::getInstance()->setUiMessage(text.toString());
}
