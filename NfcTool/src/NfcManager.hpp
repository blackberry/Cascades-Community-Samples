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

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>

#include "NdefType.hpp"
#include "NfcWorker.hpp"

class NfcManager : public QObject {
	Q_OBJECT

private:
	NfcManager();

	void startWorker();
	void stopWorker();
	void wakeupWorker();
	void startWorkerListening();

	static NfcManager* _instance;
	NfcWorker* _workerInstance;
	QThread * _bpsThread;
	QList<NdefType *> _ndefReadTypeRegistered;
	QString *_ndefUri;
	QString *_ndefText;
	QString *_ndefSpUri;
	QString *_ndefSpText;
	QString *_ndefDomain;
	QString *_ndefType;
	QString *_ndefPayload;
	QString *_first_name;
	QString *_last_name;
	QString *_address;
	QString *_email;
	QString *_mobile;

    QFuture<void> *future;
    QFutureWatcher<void> *watcher;


public:
	static NfcManager* getInstance();

	void startNdefListener(QList<NdefType *> types);
	void stopNdefListener();
	void writeUri(QString* uri);
	void writeSp(QString* sp_uri, QString* sp_text);
	void writeText(QString* text);
	void writeCustom(QString* domain, QString* type, QString* payload);
	void sendVcard(QString* first_name, QString* last_name, QString* address, QString* email, QString* mobile);
	void stopNdefWriter();

public slots:
	void message(const QVariant &text);
	void workerStopped();
};

#endif // ifndef NFCMANAGER_H
