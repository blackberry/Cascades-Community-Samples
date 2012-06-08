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
#ifndef NFCWORKER_HPP_
#define NFCWORKER_HPP_

#define CHECK(rc) NfcWorker::checkReturnCode((rc), __LINE__, __FILE__, __PRETTY_FUNCTION__)

#include <QObject>
#include <QMutex>
#include <QVariantMap>
#include <bps/event.h>

#include <nfc.h>
#include "NdefType.hpp"

class NfcWorker : public QObject {
	Q_OBJECT

public:
	NfcWorker(QObject *parent = 0);
	virtual ~NfcWorker();

	enum CustomEventType {
		TERMINATE,
		WAKEUP
	};

	enum TaskToPerform_t {
		NONE_SET,
		READ_NDEF_TAG,
		WRITE_URI_TAG,
		WRITE_TEXT_TAG,
		WRITE_SP_TAG,
		WRITE_CUSTOM_TAG
	};

signals:
	void message(const QVariant &message);
	void stopped(const QVariant &message);
	void started(const QVariant &message);

public slots:
	void initialize();
	void listen();
	void interruptBpsWaitLoop(unsigned int code);
	void prepareToReadNdefTag(QList<NdefType *> types);
	void prepareToWriteNdefUriTag(QString uri);
	void prepareToWriteNdefTextTag(QString text);
	void prepareToWriteNdefSpTag(QString sp_uri, QString sp_text);
	void prepareToWriteNdefCustomTag(QString domain, QString type, QString payload);

public:
	void readTag(QList<NdefType *> types);
	void writeUriTag(QString* uri);
	void writeSpTag(QString* sp_uri, QString* sp_text);
	void writeTextTag(QString* text);
	void writeCustomTag(QString* domain, QString* type, QString* payload);

private:
	void handleNfcEvent(bps_event_t *event);
	void handleNfcWriteCustomTagEvent(bps_event_t *event);
	void handleNfcWriteSpTagEvent(bps_event_t *event);
	void handleNfcWriteTextTagEvent(bps_event_t *event);
	void handleNfcWriteUriTagEvent(bps_event_t *event);
	void handleNfcReadNdefTagEvent(bps_event_t *event);
	void parseNdefMessage(nfc_target_t *target);
	nfc_ndef_record_t* makeUriRecord(uchar_t prefix, QString uri);
	nfc_ndef_record_t* makeTextRecord(QString language, QString text);
	nfc_ndef_record_t* makeSpRecord();
	nfc_ndef_record_t* makeCustomRecord(QString domain, QString type, QString text);
	static void checkReturnCode(int rc, int line, const char *file, const char *func);

	const int BPS_EVENT_TIMEOUT;
	int _bpsInterruptDomain;
	int _bpsChannel;
	bool _failedToInitialize;
	bool _timeToDie;
	QMutex _interruptMutex;
	TaskToPerform_t _taskToPerform;

	QList<NdefType *> _ndefReadTypeRegistered;
	QString _ndefUri;
	QString _ndefText;
	QString _ndefSpUri;
	QString _ndefSpText;
	QString _ndefDomain;
	QString _ndefType;
	QString _ndefPayload;
};

#endif /* NFCWORKER_HPP_ */
