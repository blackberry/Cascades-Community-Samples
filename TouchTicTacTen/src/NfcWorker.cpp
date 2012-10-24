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
#include <QDebug>
#include <QByteArray>
#include <bps/bps.h>
#include <nfc/nfc_bps.h>
#include <cstdlib>
#include <math.h>

#include <bps/navigator.h>
#include <bps/navigator_invoke.h>

#include "NfcWorker.hpp"

#include <sys/time.h>

NfcWorker* NfcWorker::_instance;

/*
 * BPS_EVENT_TIMEOUT(-1)   == no timeout timeout on BPS blocking waits
 * BPS_EVENT_TIMEOUT(3000) ==  3 seconds timeout on BPS blocking waits
 */
NfcWorker::NfcWorker(QObject *parent) :
		QObject(parent), BPS_EVENT_TIMEOUT(500), _failedToInitialize(false), _timeToDie(false), _taskToPerform(NONE_SET), _navigatorExitReceived(false), _emulateNdefMessage(
				0) {
	_state = GameState::getInstance();
	_state->setMyBid(-1);
	_state->setTheirBid(-1);
	_state = GameState::getInstance();
	_taskToPerform = NONE_SET;
	_snep_active = false;
}

NfcWorker::~NfcWorker() {
	_timeToDie = true;
	_instance = 0;

}

NfcWorker* NfcWorker::getInstance() {
	qDebug() << "XXXX NfcWorker::getInstance()";
	if (_instance == 0) {
		_instance = new NfcWorker;
	}
	return _instance;
}

void NfcWorker::startEventLoop() {
	initialize();
	// we always want to start in "bid mode"
	exchangeBid();
	listen();
}

/*
 * This method initialises the interface to BPS
 */
void NfcWorker::initialize() {

	qDebug() << "XXXX NfcWorker::initialize() starts...";

	int rc = BPS_FAILURE;

	rc = bps_initialize();

	if (rc) {
		_failedToInitialize = true;
		qDebug() << "XXXX Error: BPS failed to initialise. rc=" << rc;
	} else {
		qDebug() << "XXXX BPS Initialised";
	}

	rc = nfc_request_events();

	if (rc) {
		_failedToInitialize = true;
		qDebug() << "XXXX Error: Failed to request NFC BPS events. rc=" << rc;
		bps_shutdown();
	} else {
		qDebug() << "XXXX Registered for NFC BPS events OK";
		_interruptMutex.lock();
		_bpsInterruptDomain = bps_register_domain();

		if (_bpsInterruptDomain == -1) {
			qDebug() << "XXXX Failed to register an interrupt domain.";
		}

		_bpsChannel = bps_channel_get_active();
		_interruptMutex.unlock();

		startNavigatorEvents();
		qDebug() << "XXXX Registered for navigator events OK";
	}
	qDebug() << "XXXX NfcWorker::initialize() ends...";
}

/*
 * This method encapsulated the main BPS event loop -- all the details needed to
 * set it in place have been done in the initialize() method
 */
void NfcWorker::listen() {
	qDebug() << "XXXX NfcWorker::listen() entered ...";

	int rc, domain;

	if (_failedToInitialize) {
		qDebug() << "XXXX NfcWorker::listen() terminating due to initialisation failure";
		return;
	}

	_timeToDie = false;

	qDebug() << "XXXX NfcWorker::listen() entering event loop";

	TaskToPerform_t lastTaskToPerform;

	while (!_timeToDie) {
		bps_event_t *event;

		lastTaskToPerform = _taskToPerform;

		//-------------------------------------------------------------------------------------------------------
		rc = bps_get_event(&event, BPS_EVENT_TIMEOUT);
		//-------------------------------------------------------------------------------------------------------

		if (!rc) {
			if (event) {
		//-------------------------------------------------------------------------------------------------------
				domain = bps_event_get_domain(event);
		//-------------------------------------------------------------------------------------------------------

				if (nfc_get_domain() == domain) {
					// SNEP events handled here
					handleNfcEvent(event);

				} else if (navigator_get_domain() == domain) {
					// Receiving NDEF messages handled here
					handleNavigatorEvent(event);

				} else if (_bpsInterruptDomain == domain) {
					unsigned int code;
					code = bps_event_get_code(event);

					switch (code) {
					case NfcWorker::TERMINATE:
						_timeToDie = true;
						_taskToPerform = NONE_SET;
						qDebug() << "XXXX Got a custom BPS event to terminate.";
						break;

					case NfcWorker::WAKEUP:
						qDebug() << "XXXX Got a custom BPS event to wake up.";
						break;

					default:
						qDebug() << "XXXX Unrecognised custom BPS event.";
						break;
					}

				} else {
					qDebug() << "XXXX BPS event didn't match a wanted domain";
				}
			} else {

			}
		} else {
			qDebug() << "XXXX NfcWorker::listen() bps_get_event rc=" << rc;
			qDebug() << "XXXX Error getting BPS event.";
		}

		//-------------------------------------------------------------------------------------------------------
		// have we been assigned a new task?
		if (_taskToPerform != lastTaskToPerform) {
			qDebug() << "XXXX task changed:";
			switch (_taskToPerform) {
			case RECEIVE_MESSAGE:
				qDebug() << "XXXX RECEIVE_MESSAGE";
				unregisterSnepClient();
				break;
			case PASS_TURN:
				qDebug() << "XXXX PASS_TURN";
				registerSnepClient();
				break;
			case NONE_SET:
				qDebug() << "XXXX NONE_SET";
				unregisterSnepClient();
				break;
			case BID:
				qDebug() << "XXXX BID";
				registerSnepClient();
				break;
			}
		//-------------------------------------------------------------------------------------------------------
		}

	}

	if (_navigatorExitReceived) {
		qDebug() << "XXXX Exiting listen() loop since got NAVIGATOR_EXIT event";
	}

	qDebug() << "XXXX - exited listen loop - changing state";

	rc = nfc_stop_events();

	rc = navigator_stop_events(0);

	bps_shutdown();

	qDebug() << "XXXX NfcWorker::listen() BPS shutdown. Exiting listen()";
}

/*
 * All NDEF messages dispatched to us by the invocation framework are handled here
 */

void NfcWorker::handleNavigatorEvent(bps_event_t *event) {

	if (NAVIGATOR_EXIT == bps_event_get_code(event)) {
		qDebug() << "XXXX Navigator Exit event requested";
		_timeToDie = true;
		_navigatorExitReceived = true;
		_taskToPerform = NONE_SET;
		return;
	}

	switch (_taskToPerform) {
	case BID:
		// expect NAVIGATOR_INVOKE_TARGET events to deliver NDEF messages to us
		qDebug() << "XXXX Handling a Navigator event in BID state";
		handleNavigatorNdefEvent(event);
		break;
	case RECEIVE_MESSAGE:
		// expect NAVIGATOR_INVOKE_TARGET events to deliver NDEF messages to us
		qDebug() << "XXXX Handling a Navigator event in RECEIVE_MESSAGE state";
		handleNavigatorNdefEvent(event);
		break;
	case PASS_TURN:
		qDebug() << "XXXX ERROR: Handling a Navigator event in PASS_TURN state - this is unexpected";
		break;
	case NONE_SET:
		qDebug() << "XXXX Ignoring a Navigator event";
		break;
	default:
		qDebug() << "XXXX Handling a Navigator event in an undefined state";
		break;
	}
}

/*
 * All detected NFC events are handled here. For this app that means SNEP PUSH events.
 */
void NfcWorker::handleNfcEvent(bps_event_t *event) {

	qDebug() << "XXXX Handling an NFC event in state:" << _taskToPerform;

	switch (_taskToPerform) {
	case BID:
		// expect SNEP NFC events in BID state
		qDebug() << "XXXX Handling an NFC event in BID state";
		handleBidEvent(event);
		break;
	case RECEIVE_MESSAGE:
		// we should receive messages as NAVIGATOR_INVOKE_TARGET events from the invocation framework not as NFC events
		qDebug() << "XXXX Received an NFC event in RECEIVE_MESSAGE state - unexpected";
		break;
	case PASS_TURN:
		// expect SNEP NFC events in PASS_TURN state
		qDebug() << "XXXX Handling an NFC event in PASS_TURN state";
		handleNfcSnepEvent(event);
		break;
	case NONE_SET:
		qDebug() << "XXXX Handling an NFC event in NONE_SET state";
		break;
	default:
		qDebug() << "XXXX Handling an NFC event in an undefined state";
		break;
	}
}

void NfcWorker::handleNfcSnepEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);

	qDebug() << "XXXX NfcWorker::handleNfcSnepEvent - processing event code " << getNavigatorEventName(code);
	qDebug() << "XXXX NfcWorker::handleNfcSnepEvent - task= " << _taskToPerform;

	//-------------------------------------------------------------------------------------------------------
	if (NFC_SNEP_CONNECTION_EVENT == code && _taskToPerform == PASS_TURN) {
	//-------------------------------------------------------------------------------------------------------
	// we've completed our move and are handing over to the other player

	//-------------------------------------------------------------------------------------------------------
		sendTurnData(event);
	//-------------------------------------------------------------------------------------------------------
	} else if (NFC_NDEF_PUSH_SUCCEED_EVENT == code && _taskToPerform == PASS_TURN) {
	//-------------------------------------------------------------------------------------------------------
		// indicates we successfully handed over to the other player
		qDebug() << "XXXX NfcWorker::handleNfcSnepEvent - PUSH was successful";
		// so now we're ready to receive a message from them when they complete their move
		_taskToPerform = RECEIVE_MESSAGE;
		_state->setTileState(_state->lastSelection(), 1);
		if (_state->gameState() == 1) {
			emit we_made_move();
			_state->setGameState(2);
		}
	} else if (NFC_NDEF_PUSH_SUCCEED_EVENT == code && _taskToPerform == NONE_SET) {
		qDebug() << "XXXX successful SNEP push operation in NONE_SET state";

	//-------------------------------------------------------------------------------------------------------
	} else if (NFC_NDEF_PUSH_FAILURE_IO_ERROR_EVENT == code) {
	//-------------------------------------------------------------------------------------------------------
		qDebug() << "XXXX NfcWorker::handleNfcSnepEvent - SNEP PUSH FAILED";

		qDebug() << "XXXX NfcWorker::handleNfcSnepEvent - NFC BPS event that we didn't register for: " << getNfcEventName(code);
	}
}

void NfcWorker::sendTurnData(bps_event_t *bps_event) {

	nfc_event_t *nfc_event;
	nfc_target_t* target;
	nfc_ndef_record_t* myNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	// Obtain NFC target object from our BPS NFC event object
	//-------------------------------------------------------------------------------------------------------
	CHECK(nfc_get_nfc_event(bps_event, &nfc_event));
	CHECK(nfc_get_target(nfc_event, &target));
	//-------------------------------------------------------------------------------------------------------

	QByteArray ba;
	ba.resize(9);
	ba[0] = 2; // turn over message ID
	// tile changed
	ba[1] = 0;
	ba[2] = 0;
	ba[3] = 0;
	ba[4] = _state->lastSelection();

	int ls = _state->lastSelection();
	qDebug() << "XXXX sending turn with tile=" << ls;

	// symbol changed
	ba[5] = 0;
	ba[6] = 0;
	ba[7] = 0;
	ba[8] = _state->mySymbol();

	int ms = _state->mySymbol();
	_state->mySymbol();
	qDebug() << "XXXX sending turn with symbol=" << ms;

	myNdefRecord = makeTurnOverRecord(ba.constData());

	qDebug() << "XXXX Create NDEF message";
	//-------------------------------------------------------------------------------------------------------
	CHECK(nfc_create_ndef_message(&myNdefMessage));
	//-------------------------------------------------------------------------------------------------------

	qDebug() << "XXXX Add ndef record";
	CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));

	qDebug() << "XXXX Pushing NDEF message over SNEP";
	//-------------------------------------------------------------------------------------------------------
	CHECK(nfc_push_ndef_message(target, myNdefMessage));
	//-------------------------------------------------------------------------------------------------------

	qDebug() << "XXXX Delete NDEF Message";
	CHECK(nfc_delete_ndef_message(myNdefMessage, true));

	qDebug() << "XXXX Destroy NFC Target";
	CHECK(nfc_destroy_target(target));
}
/*
 * navigator events are generated when we're launched by the Invoke Framework (IF)
 * due to a tag we're interested in (as declared in the  bar-descriptor.xml file)
 * is presented to the device. There are other situations that generate navigator
 * events but in this specific case, we look to acquire the tag data which
 * the IF has and process it.
 *
 */
void NfcWorker::handleNavigatorNdefEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXXX handling NDEF event:" << code;
	qDebug() << "XXXX handling NDEF event:" << getNavigatorEventName(code);

	if (NAVIGATOR_INVOKE_TARGET == code) {
		const navigator_invoke_invocation_t *invocation = navigator_invoke_event_get_invocation(event);
		if (!invocation) {
			qDebug() << "XXXX No invocation for invoke NDEF event";
			return;
		}
		const char *invokeAction = navigator_invoke_invocation_get_action(invocation);
		if (!invokeAction) {
			qDebug() << "XXXX No action for invoke NDEF event";
			return;
		}

		const QString actionToPerform(invokeAction);
		if (actionToPerform == "bb.action.OPEN") {
			qDebug() << "XXXX Navigator	bb.action.OPEN received";
			const char *mimeType = navigator_invoke_invocation_get_type(invocation);
			if (!mimeType) {
				qDebug() << "XXXX No Mime Type with invoke request";
				return;
			}
			const QString mimeTypeRequested(mimeType);
			qDebug() << "XXXX MIME type: " << mimeTypeRequested;
			int invokeDataLength = navigator_invoke_invocation_get_data_length(invocation);
			if (invokeDataLength == -1) {
				qDebug() << "XXXX Invalid invoke data length";
				return;
			}

			//-------------------------------------------------------------------------------------------------------
			// get data from the invocation request
			const void *invokeData = navigator_invoke_invocation_get_data(invocation);
			//-------------------------------------------------------------------------------------------------------

			if (!invokeData) {
				qDebug() << "XXXX Invalid invoke data";
				return;
			}

			//-------------------------------------------------------------------------------------------------------
			// and make an NDEF message from it
			nfc_ndef_message_t *ndefMessage;
			CHECK( nfc_create_ndef_message_from_bytes(reinterpret_cast<const uchar_t *>(invokeData), invokeDataLength, &ndefMessage));
			//-------------------------------------------------------------------------------------------------------

			parseNdefMessage(ndefMessage);

			CHECK(nfc_delete_ndef_message(ndefMessage, true));
		}
	} else if (NAVIGATOR_INVOKE_QUERY_RESULT == code) {

		// just log and ignore query requests for the moment

		const char *id = navigator_event_get_id(event);
		const char *err = navigator_event_get_err(event);

		qDebug() << QString("XXXX Invoke Query Result, Id: '%1' Error: '%2'").arg(id).arg(err);

	} else {
		qDebug() << "XXXX Received Navigator event we don't handle: " << getNavigatorEventName(code);
	}
	qDebug() << "XXXX done handling NDEF event";

}
/*
 * Extract attributes including the payload from the NDEF message
 */
void NfcWorker::parseNdefMessage(nfc_ndef_message_t *ndefMessage) {

	nfc_ndef_record_t *ndefRecord;
	CHECK( nfc_get_ndef_record(ndefMessage, 0, &ndefRecord));

	uchar_t* ndefRecordPayloadData;
	size_t ndefRecordPayloadLength;
	//-------------------------------------------------------------------------------------------------------
	CHECK(nfc_get_ndef_record_payload(ndefRecord, &ndefRecordPayloadData, &ndefRecordPayloadLength));
	//-------------------------------------------------------------------------------------------------------

	tnf_type_t ndefRecordTnf;
	CHECK(nfc_get_ndef_record_tnf(ndefRecord, &ndefRecordTnf));

	char *ndefRecordNdefType;
	CHECK(nfc_get_ndef_record_type(ndefRecord, &ndefRecordNdefType));

	char *ndefRecordIdentifier;
	CHECK(nfc_get_ndef_record_id(ndefRecord, &ndefRecordIdentifier));

	QString ndefRecordPayloadAsHex = QString::fromAscii(reinterpret_cast<const char *>(ndefRecordPayloadData), ndefRecordPayloadLength).toAscii().toHex();

	QByteArray payLoadData = QByteArray::fromRawData(reinterpret_cast<const char *>(ndefRecordPayloadData), ndefRecordPayloadLength);

	qDebug() << "XXXX TNF: " << ndefRecordTnf;
	qDebug() << "XXXX Type: " << ndefRecordNdefType;
	qDebug() << "XXXX Payload: " << ndefRecordPayloadAsHex;

	// we should only be receiving messages with these TNF and type values:
	//			XXXX TNF:  4
	//			XXXX Type:  com.blackberry.nfc.sample:G1

	const char* EXPECTED_TYPE = "com.blackberry.nfc.sample:G1";

	if (NDEF_TNF_EXTERNAL != ndefRecordTnf) {
		qDebug() << "XXXX Unexpected TNF: " << ndefRecordTnf;
		return;
	}

	if (strcmp(ndefRecordNdefType, EXPECTED_TYPE) == 0) {
		qDebug() << "XXXX Received NDEF message of expected type";

		char message_id = ndefRecordPayloadData[0];

		//-------------------------------------------------------------------------------------------------------
		// examine the payload and respond according to the message ID it contains

		// if that was a bid message, check bid status
		if (message_id == 01) {
			int their_bid = (ndefRecordPayloadData[1] << 24) + (ndefRecordPayloadData[2] << 16) + (ndefRecordPayloadData[3] << 8) + ndefRecordPayloadData[4];
			_state->setTheirBid(their_bid);
			qDebug() << "XXXX Received BID message: their_bid:" << their_bid;
			checkBidStatus();
			return;
		}

		if (message_id == 02) {
			qDebug() << "XXXX Received TURN OVER message";
			int tile_changed = (ndefRecordPayloadData[1] << 24) + (ndefRecordPayloadData[2] << 16) + (ndefRecordPayloadData[3] << 8) + ndefRecordPayloadData[4];
			int symbol_changed = (ndefRecordPayloadData[5] << 24) + (ndefRecordPayloadData[6] << 16) + (ndefRecordPayloadData[7] << 8)
					+ ndefRecordPayloadData[8];
			qDebug() << "XXXX tile changed:" << tile_changed;
			qDebug() << "XXXX symbol changed:" << symbol_changed;

			// update board state and UI
			_state->setGameState(1);
			_state->setTileChanged(tile_changed);
			_state->setSymbolChanged(symbol_changed);
			emit they_made_move();
			emit message("It's your turn now");
		}
		//-------------------------------------------------------------------------------------------------------
	}
}

void NfcWorker::startNavigatorEvents() {
	qDebug() << "XXXX NfcWorker::startNavigatorEvents starts...";
	CHECK(navigator_request_events(0));
	qDebug() << "XXXX NfcWorker::startNavigatorEvents ends...";
}

void NfcWorker::passTurn() {
	qDebug() << "XXXX NfcWorker::passTurn starts...";
	_taskToPerform = PASS_TURN;
	emit message("Place devices back to back to pass turn");
	qDebug() << "XXXX NfcWorker::passTurn ends...";
}

void NfcWorker::stopPassTurn() {
	qDebug() << "XXXX NfcWorker::stopPassTurn starts...";
	unregisterSnepClient();
	qDebug() << "XXXX NfcWorker::stopPassTurn ends...";
}

void NfcWorker::registerSnepClient() {
	qDebug() << "XXXX NfcWorker::registerSnepClient: _snep_active:" << _snep_active;
	if (!_snep_active) {
		//-------------------------------------------------------------------------------------------------------
		CHECK(nfc_register_snep_client());
		//-------------------------------------------------------------------------------------------------------
		_snep_active = true;
	}
	qDebug() << "XXXX NfcWorker::done registerSnepClient: _snep_active:" << _snep_active;
}

void NfcWorker::unregisterSnepClient() {
	if (_snep_active) {
		CHECK(nfc_unregister_snep_client());
		_snep_active = false;
	}
}

void NfcWorker::exchangeBid() {
	qDebug() << "XXXX NfcWorker::exchangeBid starts...";
	prepareToExchangeBid();
	emit message("Place devices back to back to start");
	qDebug() << "XXXX NfcWorker::exchangeBid ends...";
}

void NfcWorker::prepareToExchangeBid() {
	qDebug() << "XXXX NfcWorker::prepareToexchangeBid entered ...";

	if (_failedToInitialize) {
		return;
	}

	_taskToPerform = BID;

	_state->setGameState(0);
	_state->setMyBid(-1);
	_state->setTheirBid(-1);

	registerSnepClient();

	qDebug() << "XXXX NfcWorker::prepareToexchangeBid registered for ndef push ...";

}

void NfcWorker::handleBidEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXXX NfcWorker::handleBidEvent - processing event code " << getNfcEventName(code);

	nfc_event_t *nfcEvent;
	nfc_target_t* target;
	nfc_ndef_record_t* myNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_SNEP_CONNECTION_EVENT == code) {

		unsigned char c1;
		unsigned char c2;
		unsigned char c3;
		unsigned char c4;

		// generate random numbers for bid

		srand((unsigned) getSysTimeMs());
		c1 = rand() % 256;
		c2 = rand() % 256;
		c3 = rand() % 256;
		c4 = rand() % 128;

//// make sure we're player 1!
//		c1 = 255;
//		c2 = 255;
//		c3 = 255;
//		c4 = 127;

		int bid = (c4 << 24) + (c3 << 16) + (c2 << 8) + c1;
		_state->setMyBid(bid);
		qDebug() << "XXXX c4: " << c4;
		qDebug() << "XXXX c3: " << c3;
		qDebug() << "XXXX c2: " << c2;
		qDebug() << "XXXX c1: " << c1;
		qDebug() << "XXXX bid: " << bid;

		CHECK(nfc_get_nfc_event(event, &nfcEvent));
		CHECK(nfc_get_target(nfcEvent, &target));

		QByteArray ba;
		ba.resize(4);
		ba[0] = c4;
		ba[1] = c3;
		ba[2] = c2;
		ba[3] = c1;

		myNdefRecord = makeBidRecord(ba.constData());

		qDebug() << "XXXX Create NDEF message";
		CHECK(nfc_create_ndef_message(&myNdefMessage));

		qDebug() << "XXXX Add ndef media record";
		CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));

		qDebug() << "XXXX Write NDEF message";
		CHECK(nfc_push_ndef_message(target, myNdefMessage));

		qDebug() << "XXXX Delete NDEF Message";
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));

		qDebug() << "XXXX Destroy NFC Target";
		CHECK(nfc_destroy_target(target));

		checkBidStatus();

	} else if (NFC_NDEF_PUSH_SUCCEED_EVENT == code) {
		qDebug() << "XXXX NfcWorker::handleBidEvent - PUSH was successful";
		_taskToPerform = RECEIVE_MESSAGE;
	} else if (NFC_NDEF_PUSH_FAILURE_IO_ERROR_EVENT == code) {
		qDebug() << "XXXX NfcWorker::handleBidEvent - PUSH FAILED";
	} else {
		qDebug() << "XXXX NfcWorker::handleBidEvent - NFC BPS event that we didn't register for: " << getNfcEventName(code);
	}

	qDebug() << "XXXX handleBidEvent done";
}

nfc_ndef_record_t* NfcWorker::makeBidRecord(const char* bid) {
	qDebug() << QString("makeBidRecord BID");
	nfc_ndef_record_t* record = 0;

	QString domain_plus_type = "com.blackberry.nfc.sample:G1";

	// message type 1 byte
	// bid value 4 bytes

	uchar_t payload[5];

	payload[0] = 1;
	int offset = 1;
	memcpy(&payload[offset], bid, 4);

	CHECK( nfc_create_ndef_record(NDEF_TNF_EXTERNAL, domain_plus_type.toUtf8().constData(), payload, 5, 0, &record));
	return record;
}

nfc_ndef_record_t* NfcWorker::makeTurnOverRecord(const char* turn_data) {
	qDebug() << QString("makeTurnOverRecord");
	nfc_ndef_record_t* record = 0;

	QString domain_plus_type = "com.blackberry.nfc.sample:G1";

	// message type 1 byte
	// tile value 4 bytes
	// symbol value 4 bytes

	uchar_t payload[9];

	int offset = 0;
	memcpy(&payload[offset], turn_data, 9);

	CHECK( nfc_create_ndef_record(NDEF_TNF_EXTERNAL, domain_plus_type.toUtf8().constData(), payload, 9, 0, &record));
	return record;
}

unsigned long NfcWorker::getSysTimeMs() {
	timeval theTime;
	gettimeofday(&theTime, 0);
	return (theTime.tv_sec * 1000) + (theTime.tv_usec / 1000);
}

QString NfcWorker::getNfcEventName(int event_code) {
	switch (event_code) {
	case NFC_SNEP_CONNECTION_EVENT:
		return QString("NFC_SNEP_CONNECTION_EVENT");
	case NFC_TAG_READWRITE_EVENT:
		return QString("NFC_TAG_READWRITE_EVENT");
	case NFC_HANDOVER_DETECTED_EVENT:
		return QString("NFC_HANDOVER_DETECTED_EVENT");
	case NFC_HANDOVER_COMPLETE_EVENT:
		return QString("NFC_HANDOVER_COMPLETE_EVENT");
	case NFC_LLCP_CONNECTION_EVENT:
		return QString("NFC_LLCP_CONNECTION_EVENT");
	case NFC_OFF_EVENT:
		return QString("NFC_OFF_EVENT");
	case NFC_ON_EVENT:
		return QString("NFC_ON_EVENT");
	case NFC_VIRTUAL_TAG_SELECTION_EVENT:
		return QString("NFC_VIRTUAL_TAG_SELECTION_EVENT");
	case NFC_VIRTUAL_TAG_LEFT_EVENT:
		return QString("NFC_VIRTUAL_TAG_LEFT_EVENT");
	case NFC_VIRTUAL_TAG_READ_EVENT:
		return QString("NFC_VIRTUAL_TAG_READ_EVENT");
	case NFC_VIRTUAL_TAG_WRITE_EVENT:
		return QString("NFC_VIRTUAL_TAG_WRITE_EVENT");
	case NFC_NDEF_PUSH_SUCCEED_EVENT:
		return QString("NFC_NDEF_PUSH_SUCCEED_EVENT");
	case NFC_NDEF_PUSH_FAILURE_MSG_OVER_SIZE_EVENT:
		return QString("NFC_NDEF_PUSH_FAILURE_MSG_OVER_SIZE_EVENT");
	case NFC_NDEF_PUSH_FAILURE_REJECTED_EVENT:
		return QString("NFC_NDEF_PUSH_FAILURE_REJECTED_EVENT");
	case NFC_NDEF_PUSH_FAILURE_IO_ERROR_EVENT:
		return QString("NFC_NDEF_PUSH_FAILURE_IO_ERROR_EVENT");
	case NFC_HANDOVER_FAILURE_NO_TRANSPORT_EVENT:
		return QString("NFC_HANDOVER_FAILURE_NO_TRANSPORT_EVENT");
	case NFC_ISO14443_4_COMMAND_EVENT:
		return QString("NFC_ISO14443_4_COMMAND_EVENT");
	case NFC_ISO14443_4_EVENT_CODE_EVENT:
		return QString("NFC_ISO14443_4_EVENT_CODE_EVENT");
	case NFC_LLCP_READ_COMPLETE_EVENT:
		return QString("NFC_LLCP_READ_COMPLETE_EVENT");
	case NFC_LLCP_WRITE_COMPLETE_EVENT:
		return QString("NFC_LLCP_WRITE_COMPLETE_EVENT");
	case NFC_CE_ENABLE_BACKLIGHT_OFF_EVENT:
		return QString("NFC_CE_ENABLE_BACKLIGHT_OFF_EVENT");
	case NFC_CE_ENABLE_POWERED_OFF_EVENT:
		return QString("NFC_CE_ENABLE_POWERED_OFF_EVENT");
	case NFC_CE_DISABLE_BACKLIGHT_OFF_EVENT:
		return QString("NFC_CE_DISABLE_BACKLIGHT_OFF_EVENT");
	case NFC_CE_DISABLE_POWERED_OFF_EVENT:
		return QString("NFC_CE_DISABLE_POWERED_OFF_EVENT");
	default:
		return QString("UNKNOWN NFC EVENT");
	};

}

QString NfcWorker::getNavigatorEventName(int event_code) {

	switch (event_code) {
	case NAVIGATOR_INVOKE:
		return QString("NAVIGATOR_INVOKE");
	case NAVIGATOR_EXIT:
		return QString("NAVIGATOR_EXIT");
	case NAVIGATOR_WINDOW_STATE:
		return QString("NAVIGATOR_WINDOW_STATE");
	case NAVIGATOR_SWIPE_DOWN:
		return QString("NAVIGATOR_SWIPE_DOWN");
	case NAVIGATOR_SWIPE_START:
		return QString("NAVIGATOR_SWIPE_START");
	case NAVIGATOR_LOW_MEMORY:
		return QString("NAVIGATOR_LOW_MEMORY");
	case NAVIGATOR_ORIENTATION_CHECK:
		return QString("NAVIGATOR_ORIENTATION_CHECK");
	case NAVIGATOR_ORIENTATION:
		return QString("NAVIGATOR_ORIENTATION");
	case NAVIGATOR_BACK:
		return QString("NAVIGATOR_BACK");
	case NAVIGATOR_WINDOW_ACTIVE:
		return QString("NAVIGATOR_WINDOW_ACTIVE");
	case NAVIGATOR_WINDOW_INACTIVE:
		return QString("NAVIGATOR_WINDOW_INACTIVE");
	case NAVIGATOR_ORIENTATION_DONE:
		return QString("NAVIGATOR_ORIENTATION_DONE");
	case NAVIGATOR_ORIENTATION_RESULT:
		return QString("NAVIGATOR_ORIENTATION_RESULT");
	case NAVIGATOR_WINDOW_LOCK:
		return QString("NAVIGATOR_WINDOW_LOCK");
	case NAVIGATOR_WINDOW_UNLOCK:
		return QString("NAVIGATOR_WINDOW_UNLOCK");
	case NAVIGATOR_INVOKE_TARGET:
		return QString("NAVIGATOR_INVOKE_TARGET");
	case NAVIGATOR_INVOKE_QUERY_RESULT:
		return QString("NAVIGATOR_INVOKE_QUERY_RESULT");
	case NAVIGATOR_INVOKE_VIEWER:
		return QString("NAVIGATOR_INVOKE_VIEWER");
	case NAVIGATOR_INVOKE_TARGET_RESULT:
		return QString("NAVIGATOR_INVOKE_TARGET_RESULT");
	case NAVIGATOR_INVOKE_VIEWER_RESULT:
		return QString("NAVIGATOR_INVOKE_VIEWER_RESULT");
	case NAVIGATOR_INVOKE_VIEWER_RELAY:
		return QString("NAVIGATOR_INVOKE_VIEWER_RELAY");
	case NAVIGATOR_INVOKE_VIEWER_STOPPED:
		return QString("NAVIGATOR_INVOKE_VIEWER_STOPPED");
	case NAVIGATOR_KEYBOARD_STATE:
		return QString("NAVIGATOR_KEYBOARD_STATE");
	case NAVIGATOR_KEYBOARD_POSITION:
		return QString("NAVIGATOR_KEYBOARD_POSITION");
	case NAVIGATOR_INVOKE_VIEWER_RELAY_RESULT:
		return QString("NAVIGATOR_INVOKE_VIEWER_RELAY_RESULT");
	case NAVIGATOR_DEVICE_LOCK_STATE:
		return QString("NAVIGATOR_DEVICE_LOCK_STATE");
	case NAVIGATOR_OTHER:
		return QString("NAVIGATOR_OTHER");
	default:
		return QString("UNKNOWN NAVIGATOR EVENT");
	};

}

void NfcWorker::checkBidStatus() {
	qDebug() << "XXXX checking bid status";
	if (_state->myBid() != -1 && _state->theirBid() != -1) {
		// won the bid so I am player 1, it's my turn and I'm playing with noughts (will add ability to choose noughts or crosses later)
		if (_state->myBid() > _state->theirBid()) {
			qDebug() << "XXXX PLAYER 1";
			_state->setGameState(1);
			_state->setMySymbol(1);
			_state->setPlayerNumber(1);
			_taskToPerform = NONE_SET;
			emit message("You are player 1");
			return;
		}
		// lost the bid so I am player 2, it's not my turn and I'm playing with crosses (will add ability to choose noughts or crosses later)
		if (_state->myBid() < _state->theirBid()) {
			qDebug() << "XXXX PLAYER 2";
			_state->setGameState(2);
			_state->setMySymbol(2);
			_state->setPlayerNumber(2);
			_taskToPerform = RECEIVE_MESSAGE;
			emit message("You are player 2");
			return;
		}
		emit message("You were equally lucky! Please try again!");
		qDebug() << "XXXX equal bids received!";
	}
}

/*
 * This method is used to inject a custom BPS event into the BPS event handler using
 * the channel and domain that we saved during the initialisation process.
 */
void NfcWorker::interruptBpsWaitLoop(unsigned int code) {
	int rc = 0;
	if (!_timeToDie) {
		qDebug() << "XXXX Trying to interrupt...";

		_interruptMutex.lock();
		bps_event_t *interruptEvent;

		rc = 0;
		rc = bps_event_create(&interruptEvent, _bpsInterruptDomain, code, 0, 0);

		if (rc) {
			qDebug() << "XXXX Unable to create a BPS custom event";
		} else {
			qDebug() << "XXXX Created a BPS custom event";
		}

		rc = 0;
		rc = bps_channel_push_event(_bpsChannel, interruptEvent);

		if (rc) {
			qDebug() << "XXXX Unable to push custom event onto BPS channel";
		} else {
			qDebug() << "XXXX Pushed custom event onto BPS channel";
		}

		_interruptMutex.unlock();
	} else {
		qDebug() << "XXXX No need to interrupt NfcWorker since already closing down";
	}
}

void NfcWorker::checkReturnCode(int rc, int line, const char *file, const char *func) {
	if (rc != BPS_SUCCESS) {
		qDebug() << "XXXX Error code " << rc << " in function " << func << " on line " << line << " in " << file;
	}
}
