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

#include "NfcWorker.hpp"
#include "Settings.hpp"
#include "StateManager.hpp"

/*
 * Constructor makes sure the failedToInitialize flag is set to false
 *
 * Careful! This is run on the parent thread before moving it to the worker thread!
 *
 * BPS_EVENT_TIMEOUT(-1)   == no timeout timeout on BPS blocking waits
 * BPS_EVENT_TIMEOUT(3000) ==  3 seconds timeout on BPS blocking waits
 */
NfcWorker::NfcWorker(QObject *parent) :
		QObject(parent), BPS_EVENT_TIMEOUT(3000), _failedToInitialize(false), _timeToDie(false), _taskToPerform(NONE_SET) {
}

NfcWorker::~NfcWorker() {
}

void NfcWorker::readTag(QList<NdefType *> types) {
	qDebug() << "NfcWorker::readTag() starts...";
	initialize();
	prepareToReadNdefTag(types);
	listen();
	qDebug() << "NfcWorker::readTag() ends...";
}

void NfcWorker::writeUriTag(QString* uri) {
	qDebug() << "NfcWorker::writeUriTag(" << uri << ") starts...";
	initialize();
	prepareToWriteNdefUriTag(*uri);
	listen();
	qDebug() << "NfcWorker::writeUriTag ends...";
}

void NfcWorker::writeSpTag(QString* sp_uri, QString* sp_text) {
	qDebug() << "NfcWorker::writeSpTag(" << sp_uri << "," << sp_text << ") starts...";
	initialize();
	prepareToWriteNdefSpTag(*sp_uri, *sp_text);
	listen();
	qDebug() << "NfcWorker::writeSpTag ends...";
}

void NfcWorker::writeTextTag(QString* text) {
	qDebug() << "NfcWorker::writeTextTag(" << text << ") starts...";
	initialize();
	prepareToWriteNdefTextTag(*text);
	listen();
	qDebug() << "NfcWorker::writeTextTag ends...";
}

void NfcWorker::writeCustomTag(QString* domain, QString* type, QString* payload) {
	qDebug() << "NfcWorker::writeCustomTag(" << domain << "," << type << "," << payload << ") starts...";
	initialize();
	prepareToWriteNdefCustomTag(*domain, *type, *payload);
	listen();
	qDebug() << "NfcWorker::writeCustomTag ends...";
}

/*
 * This method initialises the interface to BPS
 */
void NfcWorker::initialize() {

	qDebug() << "NfcWorker::initialize() starts...";

	/*
	 * Assume the worst to start
	 */
	int rc = BPS_FAILURE;

	/*
	 * Initialise access to the BPS system - if we can't then we can't proceed any further
	 */
	rc = bps_initialize();
	if (rc) {
		_failedToInitialize = true;
		qDebug() << "XXX Error: BPS failed to initialise. rc=" << rc;
		emit message(QString("BPS failed to initialise. rc=%1").arg(rc));
//		emit message("BPS failed to initialise");
	} else {
		qDebug() << "XXX BPS Initialised";
	}

	/*
	 * Register to receive NFC Events - if we can't then we can't proceed any further
	 */
	rc = nfc_request_events();

	if (rc) {
		_failedToInitialize = true;
		qDebug() << "XXX Error: Failed to request NFC BPS events. rc=" << rc;
		emit message("Failed to request NFC BPS events");
		bps_shutdown();
	} else {
		qDebug() << "XXX Registered for NFC BPS events OK";
		emit message("Registered for NFC BPS events OK");

		/*
		 * At this point we've initialised BPS and registered for NFC events
		 *
		 * Now, the main event processing loop is entered via a different method (listen()) later
		 * and involves waiting on events thus: bps_get_event(&event, BPS_EVENT_TIMEOUT);
		 * This is a blocking call and in order to be able to kick the thread out of this call
		 * we need to set up a special custom BPS message that we can inject into the BPS
		 * message handler. In order to do this we need a BPS domain and our current active
		 * BPS channel.
		 *
		 * This is done using the bps_register_domain() and bps_channel_get_active() methods and
		 * the activity is protected by a lock.
		 *
		 * The results are saved for later use.
		 */
		_interruptMutex.lock();
		_bpsInterruptDomain = bps_register_domain();

		if (_bpsInterruptDomain == -1) {
			qDebug() << "Failed to register an interrupt domain.";
			emit message("Failed to register an interrupt domain");
		}

		_bpsChannel = bps_channel_get_active();
		_interruptMutex.unlock();

	}
	qDebug() << "NfcWorker::initialize() ends...";
}

/*
 * This method encapsulated the main BPS event loop -- all the details needed to
 * set it in place have been done in the initialize() method
 */
void NfcWorker::listen() {
	qDebug() << "NfcWorker::listen() entered ...";

	/*
	 * If we failed to perform initialisation earlier just return here
	 */
	if (_failedToInitialize) {
		qDebug() << "NfcWorker::listen() terminating due to initialisation failure";
		emit message("Terminating due to initialisation failure");
		StateManager* state_mgr = StateManager::getInstance();
		state_mgr->setNoNfcState();
		return;
	}

	_timeToDie = false;

	/*
	 * The main BPS event loop
	 */

	qDebug() << "NfcWorker::listen() entering event loop";

	while (!_timeToDie) {
		/*
		 * Pointer to the event that will be dequeued
		 */
		bps_event_t *event;
		/*
		 * Domain of the event that will be dequeued
		 */
		int rc, domain;

		/*
		 * Blocking wait ( -1 ) for BPS events
		 */
		rc = bps_get_event(&event, BPS_EVENT_TIMEOUT);

		/*
		 * Ignore the event if the return code is bad
		 */
		if (!rc) {
			/*
			 * Ignore the event if its pointer is null
			 */
			if (event) {
				/*
				 * We need to know the domain of the event to determine if it's
				 * a real NFC event or a custom one intended to awake us
				 */
				domain = bps_event_get_domain(event);
				if (nfc_get_domain() == domain) {
					/*
					 * If it's an NFC event then process it
					 */
					handleNfcEvent(event);

				} else if (_bpsInterruptDomain == domain) {
					/*
					 * Custom events to our registered domain indicate a request to perform some local task
					 */
					unsigned int code;
					code = bps_event_get_code(event);

					switch (code) {
					case NfcWorker::TERMINATE:
						_timeToDie = true;
						_taskToPerform = NONE_SET;
						emit message("Got a custom BPS event to terminate");
						qDebug() << "Got a custom BPS event to terminate.";
						break;

					case NfcWorker::WAKEUP:
						emit message("Got a custom BPS event to wake up");
						qDebug() << "Got a custom BPS event to wake up.";
						break;

					default:
						emit message("Unrecognised custom BPS event");
						qDebug() << "Unrecognised custom BPS event.";
						break;
					}

				} else {
					emit message("BPS event didn't match a wanted domain");
					qDebug() << "BPS event didn't match a wanted domain";
				}
			} else {
				qDebug() << "No events from BPS on this wakeup";
			}
		} else {
			qDebug() << "NfcWorker::listen() bps_get_event rc=" << rc;
			emit message("Error getting BPS event");
			qDebug() << "Error getting BPS event.";
		}
	}

	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setNoNfcState();

	CHECK(nfc_unregister_tag_readerwriter());
	CHECK(nfc_stop_events());
	bps_shutdown();

//	emit stopped("Worker has shutdown BPS");
	qDebug() << "NfcWorker::listen() BPS shutdown. Exiting listen()";
}

/*
 * This method is used to inject a custom BPS event into the BPS event handler using
 * the channel and domain that we saved during the initialisation process.
 *
 */
void NfcWorker::interruptBpsWaitLoop(unsigned int code) {
	_interruptMutex.lock();
	/*
	 * This is the BPS event we'll push onto the BPS queue
	 */
	bps_event_t *interruptEvent;
	/*
	 * Create the event and identify it as being a member of the BPS Domain we registered earlier
	 * and use the code type as passed to us.
	 */
	CHECK(bps_event_create(&interruptEvent, _bpsInterruptDomain, code, 0, 0));
	/*
	 * Push the event onto the BPS event queue and target it at our BPS channel which we determined earlier
	 */
	CHECK(bps_channel_push_event(_bpsChannel, interruptEvent));
	qDebug() << "Trying to interrupt...";
	_interruptMutex.unlock();
}

/*
 * Useful utility routine to log BPS errors
 */
void NfcWorker::checkReturnCode(int rc, int line, const char *file, const char *func) {
	if (rc != BPS_SUCCESS) {
		qDebug() << "Error code " << rc << " in function " << func << " on line " << line << " in " << file;
	}
}

void NfcWorker::prepareToReadNdefTag(QList<NdefType *> types) {
	if (_failedToInitialize) {
		return;
	}

	qDebug() << "XXX NfcWorker::prepareToReadNdefTag entered ...";

	Q_ASSERT_X((types.size() > 0), "prepareToReadNdefTag", "types value invalid");

	qDebug() << "setting inReadState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setReadState(true);

	for (int i = 0; i < types.size(); ++i) {
		qDebug() << "TNF: " << types.at(i)->getTNF() << " type: " << types.at(i)->getType();
		emit message(QString("Registering TNF: %1 Type: %2").arg(types.at(i)->getTNF()).arg(types.at(i)->getType()));
	}

	_ndefReadTypeRegistered = types;
	_taskToPerform = READ_NDEF_TAG;

	for (int i = 0; i < types.size(); ++i) {
		CHECK( nfc_register_ndef_reader(types.at(i)->getTNF(), types.at(i)->getType()));
	}
}

void NfcWorker::prepareToWriteNdefUriTag(QString uri) {
	if (_failedToInitialize) {
		return;
	}

	qDebug() << "NfcWorker::prepareToWriteNdefUriTag entered ...";
	qDebug() << "URI: " << uri;

	qDebug() << "setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	emit message(QString("Preparing to write URI: %1").arg(uri));

	_ndefUri = uri;
	_taskToPerform = WRITE_URI_TAG;

	CHECK(nfc_register_tag_readerwriter(NDEF_TAG));
}

void NfcWorker::prepareToWriteNdefTextTag(QString text) {
	if (_failedToInitialize) {
		return;
	}

	qDebug() << "NfcWorker::prepareToWriteNdefTextTag entered ...";
	qDebug() << "Text: " << text;

	qDebug() << "setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	emit message(QString("Preparing to write Text: '%1'").arg(text));

	_ndefText = text;
	_taskToPerform = WRITE_TEXT_TAG;

	CHECK(nfc_register_tag_readerwriter(NDEF_TAG));
}

void NfcWorker::prepareToWriteNdefSpTag(QString sp_uri, QString sp_text) {
	if (_failedToInitialize) {
		qDebug() << "NfcWorker::prepareToWriteNdefSpTag. Initialisation failed. Exiting.";
		return;
	}

	qDebug() << "NfcWorker::prepareToWriteNdefSpTag entered ...";
	qDebug() << "URI: " << sp_uri << "Text: " << sp_text;

	qDebug() << "setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	emit message(QString("Preparing to write SP URI: '%1' Text: '%2'").arg(sp_uri).arg(sp_text));

	_ndefSpUri = sp_uri;
	_ndefSpText = sp_text;
	_taskToPerform = WRITE_SP_TAG;

	CHECK(nfc_register_tag_readerwriter(NDEF_TAG));

	qDebug() << "NfcWorker::prepareToWriteNdefSpTag done ...";

}

void NfcWorker::prepareToWriteNdefCustomTag(QString domain, QString type, QString payload) {

	if (_failedToInitialize) {
		return;
	}

	qDebug() << "NfcWorker::prepareToWriteNdefCustomTag entered ...";
	qDebug() << "Domain: " << domain << "Type: " << type << "Payload: " << payload;

	qDebug() << "setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	emit message(QString("Preparing to write Custom Tag Domain: '%1' Type: '%2' Payload: '%3'").arg(domain).arg(type).arg(payload));

	_ndefDomain = domain;
	_ndefType = type;
	_ndefPayload = payload;
	_taskToPerform = WRITE_CUSTOM_TAG;

	CHECK(nfc_register_tag_readerwriter(NDEF_TAG));
}

/*
 * All detected NFC events are handled here
 */
void NfcWorker::handleNfcEvent(bps_event_t *event) {
	emit message("Handling an NFC event");
	qDebug() << "XXX Handling an NFC event";

	switch (_taskToPerform) {
	case WRITE_CUSTOM_TAG:
		qDebug() << "XXX Handling an NFC event in WRITE_CUSTOM_TAG state";
		handleNfcWriteCustomTagEvent(event);
		break;

	case WRITE_SP_TAG:
		qDebug() << "XXX Handling an NFC event in WRITE_SP_TAG state";
		handleNfcWriteSpTagEvent(event);
		break;

	case WRITE_TEXT_TAG:
		qDebug() << "XXX Handling an NFC event in WRITE_TEXT_TAG state";
		handleNfcWriteTextTagEvent(event);
		break;

	case WRITE_URI_TAG:
		qDebug() << "XXX Handling an NFC event in WRITE_URI_TAG state";
		handleNfcWriteUriTagEvent(event);
		break;

	case READ_NDEF_TAG:
		qDebug() << "XXX Handling an NFC event in READ_NDEF_TAG state";
		handleNfcReadNdefTagEvent(event);
		break;

	case NONE_SET:
		qDebug() << "XXX Handling an NFC event in NONE_SET state";
		break;

	default:
		qDebug() << "XXX Handling an NFC event in an undefined state";
		break;
	}
}

/*
 * This method processed an NFC Event when we're intending to Write a Custom Tag
 *
 * NDEF_TNF_EXTERNAL
 */

void NfcWorker::handleNfcWriteCustomTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXX NfcWorker::handleNfcWriteCustomTagEvent - processing event code " << code;

	nfc_target_t* target;
	nfc_ndef_record_t* myNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug() << "XXX NfcWorker::handleNfcWriteCustomTagEvent - Target Read Write Event";
		CHECK(nfc_get_target(event, &target));
		qDebug() << "XXX NfcWorker::handleWriteCustomTagEvent - Preparing to write Custom: DOMAIN=" << _ndefDomain << ", TYPE=" << _ndefType;
		myNdefRecord = makeCustomRecord(_ndefDomain, _ndefType, _ndefPayload);
		CHECK(nfc_create_ndef_message(&myNdefMessage));
		CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));
		CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));
		emit message(QString("Custom Tag Written"));
	} else {
		qDebug() << "XXX NfcWorker::handleNfcWriteCustomTagEvent - NFC BPS event that we didn't register for< " << code;
	}

	qDebug() << "Write Custom Tag written";
	emit message(QString("Custom tag written OK"));

}

/*
 * This method processed an NFC Event when we're intending to Write an Sp Tag
 */
void NfcWorker::handleNfcWriteSpTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXX NfcWorker::handleNfcWriteSpTagEvent - processing event code " << code;

	nfc_target_t* target;
	nfc_ndef_record_t* spNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug() << "XXX NfcWorker::handleNfcWriteSpTagEvent - Target Read Write Event";
		CHECK(nfc_get_target(event, &target));

		qDebug() << "XXX NfcWorker::handleWriteSpTagEvent - Preparing to write Sp: URI=" << _ndefSpUri << ", Text=" << _ndefSpText;
		spNdefRecord = makeSpRecord();
		CHECK(nfc_create_ndef_message(&myNdefMessage));
		CHECK(nfc_set_sp_uri(spNdefRecord, _ndefSpUri.toUtf8().constData()));
		CHECK( nfc_add_sp_title(spNdefRecord, Settings::LANG_EN, _ndefSpText.toUtf8().constData(), false));
		CHECK(nfc_add_ndef_record(myNdefMessage, spNdefRecord));
		CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));
		emit message(QString("Tag Type Sp Written: %1 %2").arg(_ndefSpUri).arg(_ndefSpText));
	} else {
		qDebug() << "XXX NfcWorker::handleNfcWriteSpTagEvent - NFC BPS event that we didn't register for: " << code;
	}

	qDebug() << "Write Sp Tag written";
	emit message(QString("Smart Poster tag written OK"));

}

/*
 * This method processed an NFC Event when we're intending to Write Text Tag
 */
void NfcWorker::handleNfcWriteTextTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXX NfcWorker::handleNfcWriteTextTagEvent - processing event code " << code;

	nfc_target_t* target;
	nfc_ndef_record_t* myNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug() << "XXX NfcWorker::handleNfcWriteTextTagEvent - Target Read Write Event";

		CHECK(nfc_get_target(event, &target));

		qDebug() << "XXX NfcWorker::handleWriteUriTagEvent - Preparing to write Text: " << _ndefText;
		myNdefRecord = makeTextRecord(Settings::LANG_EN, _ndefText);

		CHECK(nfc_create_ndef_message(&myNdefMessage));
		CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));
		CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));
		emit message(QString("Tag Type Written Text: %1").arg(_ndefText));
	} else {
		qDebug() << "XXX NfcWorker::handleNfcWriteTextTagEvent - NFC BPS event that we didn't register for: " << code;
	}
}

/*
 * This method processed an NFC Event when we're intending to Write a Uri Tag
 */
void NfcWorker::handleNfcWriteUriTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXX NfcWorker::handleWriteUriTagEvent - processing event code " << code;

	nfc_target_t* target;
	nfc_ndef_record_t* myNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug() << "XXX NfcWorker::handleWriteUriTagEvent - Target Read Write Event";

		CHECK(nfc_get_target(event, &target));

		qDebug() << "XXX NfcWorker::handleWriteUriTagEvent - Preparing to write URI: " << _ndefUri;
		myNdefRecord = makeUriRecord(Settings::NfcRtdUriPrefixNone, _ndefUri);

		CHECK(nfc_create_ndef_message(&myNdefMessage));
		CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));
		CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));
		emit message(QString("Tag Type Written URI: %1").arg(_ndefUri));
	} else {
		qDebug() << "XXX NfcWorker::handleWriteUriTagEvent - NFC BPS event that we didn't register for: " << code;
	}
}

/*
 * This method processed an NFC Event when we're intending to read an NDEF Tag
 */
void NfcWorker::handleNfcReadNdefTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXX NfcWorker::handleNfcReadNdefTagEvent - processing event code " << code;

	if (NFC_NDEF_READ_EVENT == code) {
		qDebug() << "XXX NfcWorker::handleNfcReadNdefTagEvent - NDEF read event";
		nfc_target_t* target;
		CHECK(nfc_get_target(event, &target));
		parseNdefMessage(target);
		CHECK(nfc_destroy_target(target));
	} else {
		qDebug() << "XXX NfcWorker::handleNfcReadNdefTagEvent - NFC BPS event that we didn't register for: " << code;
	}
}

void NfcWorker::parseNdefMessage(nfc_target_t *nfcTarget) {

	// Figure out the number of NDEF messages

	int ndefMsgCount = 0;
	CHECK(nfc_get_ndef_message_count(nfcTarget, &ndefMsgCount));

	for (int ndefMsgIndex = 0; ndefMsgIndex < ndefMsgCount; ++ndefMsgIndex) {

		// Now processing an an NDEF message

		qDebug() << "Processing NDEF Message index: " << ndefMsgIndex;

		nfc_ndef_message_t *ndefMessage;
		CHECK(nfc_get_ndef_message(nfcTarget, ndefMsgIndex, &ndefMessage));

		// See how many NDEF records are in this NDEF Message

		int ndefRecordCount = 0;
		CHECK(nfc_get_ndef_record_count(ndefMessage, &ndefRecordCount));

		for (int ndefRecordIndex = 0; ndefRecordIndex < ndefRecordCount; ++ndefRecordIndex) {

			// Now processing an NDEF Record in an NDEF Message

			qDebug() << "Processing NDEF Record index: " << ndefRecordIndex;

			// Now parse this NDEF Record

			nfc_ndef_record_t *ndefRecord;
			CHECK( nfc_get_ndef_record(ndefMessage, ndefRecordIndex, &ndefRecord));

			// Figure out the record's payload and its length

			uchar_t* ndefRecordPayloadData;
			int ndefRecordPayloadLength;
			CHECK( nfc_get_ndef_record_payload(ndefRecord, &ndefRecordPayloadData, &ndefRecordPayloadLength));
			/*

			 NOTE: Commented out since libnfcapi.so does not yet (sprint 1212) contain this function

			 // Figure out what TNF this record has

			 tnf_type_t* ndefRecordTnf;
			 CHECK(nfc_get_ndef_record_tnf(ndefRecord, ndefRecordTnf));
			 */
			// Figure out the Type ( e.g. "Sp", MIME-Type etc. etc of this record
			char *ndefRecordNdefType;
			CHECK(nfc_get_ndef_record_type(ndefRecord, &ndefRecordNdefType));

			// Figure out the identifier for this NDEF record

			char *ndefRecordIdentifier;
			CHECK(nfc_get_ndef_record_id(ndefRecord, &ndefRecordIdentifier));

			/*
			 * messy trick to interpret char* as an ascii ( yes ascii ) string and convert to QString
			 * then use .toAscii() and .toHex() methods of QString to convert it to a hex representation
			 * of the original char*
			 *
			 * Output may typically be something like:
			 *
			 * 91010f55036e6577732e6262632e636f2e756b5101065402656e424243
			 *
			 * This is the payload of a single Smart Poster NDEF Message with one record containing
			 * an NDEF message which itself contains two NDEF records thus:
			 *
			 * 91 ( NDEF Record : MB=1, ME=0, CF=0, SR=1, IL=0, TNF=1 )
			 *   01 ( Type Length : 1 )
			 *     0f ( Payload Length : 15 - no Id Length since IL=0 )
			 *       55 ( Type : "U" )
			 *         03 ( Absolute URI )
			 *           6e6577732e6262632e636f2e756b ( URI Payload "news.bbc.co.uk" )
			 *
			 * 51 ( NDEF Record : MB=0, ME=1, CF=0, SR=1, TNF=1 )
			 *   01 ( Type Length : 1 )
			 *     06 ( Payload Length : 6 - no Id length since IL=0 )
			 *       54 ( Type : "T" )
			 *         02 ( UTF-8 + ISO Language code length : 2 )
			 *           656e ( "en" )
			 *               424243 ( "BBC" )
			 *
			 * The challenge is now to parse this :-)
			 */
//			QString ndefRecordPayloadAsAscii =
//					QString::fromAscii(reinterpret_cast<const char *>(ndefRecordPayloadData),ndefRecordPayloadLength);
			QString ndefRecordPayloadAsHex = QString::fromAscii(reinterpret_cast<const char *>(ndefRecordPayloadData), ndefRecordPayloadLength).toAscii().toHex();

			QByteArray payLoadData = QByteArray::fromRawData(reinterpret_cast<const char *>(ndefRecordPayloadData), ndefRecordPayloadLength);

			// These message go to the event log

//			emit message(QString("Payload (chars): %1").arg(ndefRecordPayloadAsAscii));

			emit message(QString("Payload (Hex): %1").arg(ndefRecordPayloadAsHex));
			/*
			 NOTE: Commented out since libnfcapi.so does not yet (sprint 1212) contain function this depends on

			 emit message(QString("TNF: %1").arg(*ndefRecordTnf));
			 */
			emit message(QString("Type: %1").arg(ndefRecordNdefType));
			emit message(QString("Id: %1").arg(ndefRecordIdentifier));

			if (strcmp(ndefRecordNdefType, Settings::NfcRtdSmartPoster) == 0) {
				qDebug() << "Smart Poster";

				emit message(QString("Detected a Smart Poster Tag"));

				char *utf_title;
				char *found_lang;
				char *uri;
				bool is_utf16 = false;

				qDebug() << "Calling nfc_get_sp_title";
				CHECK( nfc_get_sp_title(ndefRecord, Settings::LANG_EN, &utf_title, &found_lang, &is_utf16, true));
				qDebug() << "Done calling nfc_get_sp_title";

				qDebug() << "Calling nfc_get_sp_uri";
				CHECK(nfc_get_sp_uri(ndefRecord, &uri));
				qDebug() << "Done calling nfc_get_sp_uri";

				// emit

				emit message(QString("Language: %1").arg(found_lang));
				emit message(QString("Title: %1").arg(utf_title));
				emit message(QString("URI: %1").arg(uri));

				// debug

				qDebug() << QString("Message(%1)/Record(%2) Language: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(found_lang);

				qDebug() << QString("Message(%1)/Record(%2) Title: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(utf_title);

				qDebug() << QString("Message(%1)/Record(%2) URI: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(uri);

				// free memory
				free(utf_title);
				free(uri);
			} else if (strcmp(ndefRecordNdefType, Settings::NfcRtdUri) == 0) {

				qDebug() << "URI Tag";
				emit message(QString("Detected a URI Tag"));

				QString uri;
				uchar_t uriType;
				int uriLen;

				uriType = ndefRecordPayloadData[0];
				uriLen = payLoadData.length() - 1;

				uri = QString::fromUtf8(payLoadData.right(uriLen).constData(), uriLen);

				if (uriType == Settings::NfcRtdUriPrefixHttpWww) {
					uri.prepend(QString("http://www."));

				} else if (uriType == Settings::NfcRtdUriPrefixHttpsWww) {
					uri.prepend(QString("https://www."));

				} else if (uriType == Settings::NfcRtdUriPrefixHttp) {
					uri.prepend(QString("http://"));

				} else if (uriType == Settings::NfcRtdUriPrefixHttps) {
					uri.prepend(QString("https://"));

				} else if (uriType != Settings::NfcRtdUriPrefixNone) {
					emit message(QString("URI Prefix %1 not implemented").arg(uriType));
				}

				emit message(QString("URI: %1").arg(uri));

			} else if (strcmp(ndefRecordNdefType, Settings::NfcRtdText) == 0) {

				qDebug() << "Text Tag";
				emit message(QString("Detected a Text Tag"));

				QString text;
				QString language;
				int languageLen;
				int textLen;

				languageLen = ndefRecordPayloadData[0];
				textLen = payLoadData.length() - (languageLen + 1);

				language = QString::fromUtf8(payLoadData.mid(1, languageLen).constData(), languageLen);
				text = QString::fromUtf8(payLoadData.right(textLen).constData(), textLen);

				emit message(QString("Language: %1").arg(language));
				emit message(QString("Text: %1").arg(text));
			}

			// more detailed information to debug

			qDebug() << QString("Message(%1)/Record(%2) Payload (Hex): %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(ndefRecordPayloadAsHex);
			/*
			 NOTE: Commented out since libnfcapi.so does not yet (sprint 1212) contain function this depends on

			 qDebug() << QString("Message(%1)/Record(%2) TNF: %3")
			 .arg(ndefMsgIndex)
			 .arg(ndefRecordIndex)
			 .arg(*ndefRecordTnf);
			 */
			qDebug() << QString("Message(%1)/Record(%2) Type: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(ndefRecordNdefType);

			qDebug() << QString("Message(%1)/Record(%2) Id: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(ndefRecordIdentifier);

		} /* rinse and repeat with next NDEF Record if any */
	} /* rinse and repeat with next NDEF Message is any */
}

/*
 * Build an NDEF Uri Record
 *
 */
nfc_ndef_record_t* NfcWorker::makeUriRecord(uchar_t prefix, QString uri) {
	qDebug() << QString("makeUriRecord URI: %1").arg(uri);
	nfc_ndef_record_t* record = 0;
	int len = uri.length();
	uchar_t payload[len + 1];
	payload[0] = prefix;
	memcpy(&payload[1], uri.toUtf8().constData(), len);
	CHECK( nfc_create_ndef_record(NDEF_TNF_WELL_KNOWN, Settings::NfcRtdUri, payload, len + 1, 0, &record));
	return record;
}

/*
 * Build an NDEF Text Record
 *
 */
nfc_ndef_record_t* NfcWorker::makeTextRecord(QString language, QString text) {
	qDebug() << QString("makeTextRecord TEXT: %1").arg(text);
	nfc_ndef_record_t* record = 0;

	int textLen = text.length();
	int languageLen = language.length();
	int totalLen = textLen + languageLen + 1;

	uchar_t payload[totalLen];

	int offset = 0;
	// set status byte. Since text is UTF-8 and RFU must be 0, bits 7 and 6 are 0 and therefore the entire status byte value is the language code length
	payload[offset] = languageLen;

	offset += 1;
	memcpy(&payload[offset], language.toUtf8().constData(), languageLen);

	offset += languageLen;
	memcpy(&payload[offset], text.toUtf8().constData(), textLen);

	CHECK( nfc_create_ndef_record(NDEF_TNF_WELL_KNOWN, Settings::NfcRtdText, payload, totalLen, 0, &record));
	return record;
}

/*
 * Build an NDEF Sp Record to which we will add a Uri record and one or more Text/Title records later
 *
 */
nfc_ndef_record_t* NfcWorker::makeSpRecord() {
	qDebug() << "NfcWorker::makeSpRecord";
	nfc_ndef_record_t* record = 0;
	uchar_t payload[0];
	CHECK( nfc_create_ndef_record(NDEF_TNF_WELL_KNOWN, Settings::NfcRtdSmartPoster, payload, 0, 0, &record));
	return record;
}

/*
 * Build an NDEF Text Record
 *
 */
nfc_ndef_record_t* NfcWorker::makeCustomRecord(QString domain, QString type, QString text) {
	qDebug() << QString("makeCustomRecord DOMAIN: %1 TYPE: %2 TEXT %3").arg(domain).arg(type).arg(text);
	nfc_ndef_record_t* record = 0;

	int textLen = text.length();
	QString domain_plus_type = domain.append(":");
	domain_plus_type = domain_plus_type.append(type);
	int totalLen = textLen;

	uchar_t payload[totalLen];

	int offset = 0;
	memcpy(&payload[offset], text.toUtf8().constData(), textLen);

	CHECK( nfc_create_ndef_record(NDEF_TNF_EXTERNAL, domain_plus_type.toUtf8().constData(), payload, totalLen, 0, &record));
	return record;
}
