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
#ifndef NFCMANAGER_H
#define NFCMANAGER_H

#include <bb/cascades/Application>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>

#include "NfcWorker.hpp"

class NfcManager: public QObject {

	Q_OBJECT

private:
	NfcManager();

	static NfcManager* _instance;
	NfcWorker* _workerInstance;
	QThread * _bpsThread;

	QFuture<void> *_future;
	QFutureWatcher<void> *_watcher;

public:
	virtual ~NfcManager();
	static NfcManager* getInstance();
	void startEventProcessing();
	void prepareToPassTurn();
	void disablePassingTurn();
	void stopNfcWorker();

signals:
	void nfcManagerStopped();

public slots:
	void workerStopped();
	void message(const QVariant &text);

};

#endif // ifndef NFCMANAGER_H
