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

#include <QtCore/QObject>
#include <QMutex>
#include <QVariantMap>
#include <bb/cascades/QmlDocument>
#include <bps/event.h>
#include <nfc/nfc.h>
#include "NdefType.hpp"
#include "EventLog.hpp"

class NfcWorker: public QObject {

	Q_OBJECT

public:
	virtual ~NfcWorker();

	enum CustomEventType {
		TERMINATE, WAKEUP
	};

	enum TaskToPerform_t {
		NONE_SET,
		READ_NDEF_TAG,
		WRITE_URI_TAG,
		WRITE_TEXT_TAG,
		WRITE_SP_TAG,
		WRITE_CUSTOM_TAG,
		SEND_VCARD,
		EMULATE_TAG
	};

signals:
	void message(const QVariant &message);
	void clearMessages();
	void stopped(const QVariant &message);
	void started(const QVariant &message);
	void read_selected();

public slots:
	void startEventLoop();
	void initialize();
	void listen();
	void interruptBpsWaitLoop(unsigned int code);
	void prepareToReadNdefTagViaInvoke();
	void prepareToWriteNdefUriTag(const QVariant &uri);
	void prepareToWriteNdefTextTag(const QVariant &text);
	void prepareToWriteNdefSpTag(const QVariant &sp_uri,
			const QVariant &sp_text);
	void prepareToWriteNdefCustomTag(const QVariant &domain,
			const QVariant &type, const QVariant &payload);
	void prepareToSendVcard(const QVariant &first_name,
			const QVariant &last_name, const QVariant &address,
			const QVariant &email, const QVariant &mobile);
	void prepareToEmulateTag(const QVariant &the_uri, const QVariant &the_text);
	void prepareToStopEmulation();
	void writeUriTag(const QVariant &uri);
	void writeSpTag(const QVariant &sp_uri, const QVariant &sp_text);
	void writeTextTag(const QVariant &text);
	void writeCustomTag(const QVariant &domain, const QVariant &type,
			const QVariant &payload);
	void sendVcard(const QVariant &first_name, const QVariant &last_name,
			const QVariant &address, const QVariant &email,
			const QVariant &mobile);
	void emulateTag(const QVariant &uri, const QVariant &text);
	void doIso7816Test(const QVariant &aid, const QVariant &hex_cla, const QVariant &hex_ins, const QVariant &hexp1p2, const QVariant &hex_lc,
			const QVariant &hex_command, const QVariant &hex_le);
	void stopEmulatingTag();

public:
	static NfcWorker* getInstance();
	void readTag();
	void handleTagReadInvocation(const QByteArray data);

private:
	NfcWorker(QObject *parent = 0);
	void handleNavigatorEvent(bps_event_t *event);
	void handleNfcEvent(bps_event_t *event);
	void handleNfcReadNdefTagEvent(bps_event_t *event);
	void handleNfcWriteCustomTagEvent(bps_event_t *event);
	void handleNfcWriteSpTagEvent(bps_event_t *event);
	void handleNfcWriteTextTagEvent(bps_event_t *event);
	void handleNfcWriteUriTagEvent(bps_event_t *event);
	void handleSendVcardEvent(bps_event_t *event);
	void handleEmulateNfcEvent(bps_event_t *event);
	void parseNdefMessage(nfc_ndef_message_t *ndefMessage);
	nfc_ndef_record_t* makeUriRecord(uchar_t prefix, QString uri);
	nfc_ndef_record_t* makeTextRecord(QString language, QString text);
	nfc_ndef_record_t* makeSpRecord();
	nfc_ndef_record_t* makeCustomRecord(QString domain, QString type,
			QString text);
	nfc_ndef_record_t* makeVcardMessage(QString name, QString address,
			QString email, QString mobile);
	nfc_ndef_record_t* makeMediaRecord(QString type, QString text);
	static void checkReturnCode(int rc, int line, const char *file,
			const char *func);
	unsigned long getSysTimeMs();

	static NfcWorker* _instance;
	const int BPS_EVENT_TIMEOUT;
	int _bpsInterruptDomain;
	int _bpsChannel;
	bool _failedToInitialize;
	bool _timeToDie;
	QMutex _interruptMutex;
	TaskToPerform_t _taskToPerform;
	bool _navigatorExitReceived;
	nfc_ndef_message_t *_emulateNdefMessage;

	QString _ndefUri;
	QString _ndefText;
	QString _ndefSpUri;
	QString _ndefSpText;
	QString _ndefDomain;
	QString _ndefType;
	QString _ndefPayload;
	QString _first_name;
	QString _last_name;
	QString _address;
	QString _email;
	QString _mobile;
	QString _aid;
	QString _hex_cla;
	QString _hex_ins;
	QString _hexp1p2;
	QString _hex_lc;
	QString _hex_command;
	QString _hex_le;
	EventLog* _eventLog;
};

#endif /* NFCWORKER_HPP_ */
