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

#include "NdefType.hpp"
#include "NfcWorker.hpp"

class NfcManager: public QObject {

Q_OBJECT

private:
	NfcManager();

	static NfcManager* _instance;
	NfcWorker* _workerInstance;
	QThread * _bpsThread;
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

	QFuture<void> *_future;
	QFutureWatcher<void> *_watcher;

public:
	virtual ~NfcManager();
	static NfcManager* getInstance();

	void startEventProcessing();
	void handleTagReadInvocation(QByteArray data);
	void writeUri(QString* uri);
	void writeSp(QString* sp_uri, QString* sp_text);
	void writeText(QString* text);
	void writeCustom(QString* domain, QString* type, QString* payload);
	void sendVcard(QString* first_name, QString* last_name, QString* address,
			QString* email, QString* mobile);
	void iso7816Test(QString* aid, QString* hex_cla, QString* hex_ins, QString* hex_p1p2, QString* hex_lc, QString* hex_command, QString* hex_le);
	void stopNfcWorker();
	void startTagEmulation(QString *uri, QString *text);
	void stopTagEmulation();

signals:
	void start_write_uri(const QVariant &uri);
	void start_write_sp(const QVariant &sp_uri, const QVariant &sp_text);
	void start_write_text(const QVariant &text);
	void start_write_custom(const QVariant &domain, const QVariant &type,
			const QVariant &payload);
	void start_send_vcard(const QVariant &first_name, const QVariant &last_name, const QVariant &address, const QVariant &email, const QVariant &mobile);
	void start_tag_emulation(const QVariant &uri, const QVariant &text);
	void stop_tag_emulation();
    void start_iso7816_test(const QVariant &aid, const QVariant &hex_cla, const QVariant &hex_ins, const QVariant &hex_p1p2, const QVariant &hex_lc, const QVariant &hex_command, const QVariant &hex_le );
	void nfcManagerStopped();

public slots:
	void message(const QVariant &text);
	void clearMessages();
	void workerStopped();

};

#endif // ifndef NFCMANAGER_H
