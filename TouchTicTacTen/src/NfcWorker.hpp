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
#include <nfc/nfc_bps.h>
#include "GameState.hpp"

class NfcWorker: public QObject {

Q_OBJECT

public:
	virtual ~NfcWorker();

	enum CustomEventType {
		TERMINATE, WAKEUP
	};

	enum TaskToPerform_t {
		NONE_SET, BID, RECEIVE_MESSAGE, PASS_TURN
	};

signals:
	void stopped(const QVariant &message);
	void started(const QVariant &message);
	void message(const QVariant &message);
	void they_made_move();
	void we_made_move();

public slots:
	void startEventLoop();
	void initialize();
	void listen();
	void interruptBpsWaitLoop(unsigned int code);

public:
	static NfcWorker* getInstance();
	void prepareToExchangeBid();
	void exchangeBid();
	void passTurn();
	void stopPassTurn();

private:
	NfcWorker(QObject *parent = 0);
	GameState* _state;
	void handleNfcEvent(bps_event_t *event);
	void handleNfcSnepEvent(bps_event_t *event);
	void handleNavigatorEvent(bps_event_t *event);
	void startNavigatorEvents();
	void registerSnepClient();
	void unregisterSnepClient();
	void handleNavigatorNdefEvent(bps_event_t *event);
	void parseNdefMessage(nfc_ndef_message_t *ndefMessage);
	void handleBidEvent(bps_event_t *event);
	void checkBidStatus();
	nfc_ndef_record_t* makeBidRecord(const char* bid);
	nfc_ndef_record_t* makeTurnOverRecord(const char* turn_data);
	static void checkReturnCode(int rc, int line, const char *file, const char *func);
	unsigned long getSysTimeMs();
	QString getNavigatorEventName(int event_code);
	QString getNfcEventName(int event_code);
	void sendTurnData(bps_event_t *event);

	static NfcWorker* _instance;
	const int BPS_EVENT_TIMEOUT;
	int _bpsInterruptDomain;
	int _bpsChannel;
	bool _failedToInitialize;
	bool _timeToDie;
	bool _snep_active;
	QMutex _interruptMutex;
	TaskToPerform_t _taskToPerform;
	bool _navigatorExitReceived;
	nfc_ndef_message_t *_emulateNdefMessage;

	QString _ndefDomain;
	QString _ndefType;
	QString _ndefPayload;
};

#endif /* NFCWORKER_HPP_ */
