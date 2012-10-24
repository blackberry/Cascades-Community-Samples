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
#include <nfc/nfc_se_access.h>

#include <bps/navigator.h>
#include <bps/navigator_invoke.h>

#include "NfcWorker.hpp"
#include "Logger.hpp"
#include "Settings.hpp"
#include "StateManager.hpp"
#include "Utilities.hpp"

#include <sys/time.h>

NfcWorker* NfcWorker::_instance;

/*
 * BPS_EVENT_TIMEOUT(-1)   == no timeout timeout on BPS blocking waits
 * BPS_EVENT_TIMEOUT(3000) ==  3 seconds timeout on BPS blocking waits
 */
NfcWorker::NfcWorker(QObject *parent) :
		QObject(parent), BPS_EVENT_TIMEOUT(3000), _failedToInitialize(false), _timeToDie(false), _taskToPerform(NONE_SET), _navigatorExitReceived(false), _emulateNdefMessage(
				0) {

	nfc_set_verbosity(2);
}

NfcWorker::~NfcWorker() {
	qDebug() << "XXXX NfcWorker destructor";
	_timeToDie = true;
	_instance = 0;

	if (_emulateNdefMessage) {
		CHECK(nfc_delete_ndef_message(_emulateNdefMessage, true));
	}
}

NfcWorker* NfcWorker::getInstance() {
	if (_instance == 0) {
		_instance = new NfcWorker;
	}
	return _instance;
}

void NfcWorker::startEventLoop() {
	initialize();
	readTag(); // we're interested in reading tags right from the start
	listen();
}

void NfcWorker::readTag() {
	qDebug() << "XXXX NfcWorker::readTag() starts...";
	prepareToReadNdefTagViaInvoke();
	_eventLog = EventLog::getInstance();
	QObject::connect(this, SIGNAL(read_selected()), _eventLog, SLOT(show()));
	qDebug() << "XXXX NfcWorker::readTag() event log connection made";
	qDebug() << "XXXX NfcWorker::readTag() ends...";
}

void NfcWorker::emulateTag(const QVariant &uri, const QVariant &text) {
	qDebug() << "XXXX NfcWorker::emulateTag() starts...";
	prepareToEmulateTag(uri, text);
	qDebug() << "XXXX NfcWorker::emulateTag() ends...";
}

void NfcWorker::stopEmulatingTag() {
	qDebug() << "XXXX NfcWorker::stopEmulatingTag() starts...";
	prepareToStopEmulation();
	qDebug() << "XXXX NfcWorker::stopEmulatingTag() ends...";
}

void NfcWorker::writeUriTag(const QVariant &uri) {
	qDebug() << "XXXX NfcWorker::writeUriTag(" << uri << ") starts...";
	prepareToWriteNdefUriTag(uri);
	qDebug() << "XXXX NfcWorker::writeUriTag ends...";
}

void NfcWorker::writeSpTag(const QVariant &sp_uri, const QVariant &sp_text) {
	qDebug() << "XXXX NfcWorker::writeSpTag(" << sp_uri << "," << sp_text << ") starts...";
	prepareToWriteNdefSpTag(sp_uri, sp_text);
	qDebug() << "XXXX NfcWorker::writeSpTag ends...";
}

void NfcWorker::writeTextTag(const QVariant &text) {
	qDebug() << "XXXX NfcWorker::writeTextTag(" << text << ") starts...";
	prepareToWriteNdefTextTag(text);
	qDebug() << "XXXX NfcWorker::writeTextTag ends...";
}

void NfcWorker::writeCustomTag(const QVariant &domain, const QVariant &type, const QVariant &payload) {
	qDebug() << "XXXX NfcWorker::writeCustomTag(" << domain << "," << type << "," << payload << ") starts...";
	prepareToWriteNdefCustomTag(domain, type, payload);
	qDebug() << "XXXX NfcWorker::writeCustomTag ends...";
}

void NfcWorker::sendVcard(const QVariant &first_name, const QVariant &last_name, const QVariant &address, const QVariant &email, const QVariant &mobile) {
	qDebug() << "XXXX NfcWorker::sendVcard(" << first_name << "," << last_name << "," << address << "," << email << "," << mobile << ") starts...";
	prepareToSendVcard(first_name, last_name, address, email, mobile);
	qDebug() << "XXXX NfcWorker::writeCustomTag ends...";
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
		emit message(QString("BPS failed to initialise. rc=%1").arg(rc));
	} else {
		qDebug() << "XXXX BPS Initialised";
	}

	rc = nfc_request_events();

	if (rc) {
		_failedToInitialize = true;
		qDebug() << "XXXX Error: Failed to request NFC BPS events. rc=" << rc;
		emit message("Failed to request NFC BPS events");
		bps_shutdown();
	} else {
		qDebug() << "XXXX Registered for NFC BPS events OK";
		_interruptMutex.lock();
		_bpsInterruptDomain = bps_register_domain();

		if (_bpsInterruptDomain == -1) {
			qDebug() << "XXXX Failed to register an interrupt domain.";
			emit message("Failed to register an interrupt domain");
		}

		_bpsChannel = bps_channel_get_active();
		_interruptMutex.unlock();

		CHECK(navigator_request_events(0));
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
		emit message("Terminating due to initialisation failure");
		StateManager* state_mgr = StateManager::getInstance();
		state_mgr->setDefaultState();
		return;
	}

	_timeToDie = false;

	qDebug() << "XXXX NfcWorker::listen() entering event loop";

	while (!_timeToDie) {
		bps_event_t *event;

		rc = bps_get_event(&event, BPS_EVENT_TIMEOUT);

		if (!rc) {
			if (event) {
				domain = bps_event_get_domain(event);
				if (nfc_get_domain() == domain) {
					handleNfcEvent(event);

				} else if (navigator_get_domain() == domain) {
					handleNavigatorEvent(event);

				} else if (_bpsInterruptDomain == domain) {
					unsigned int code;
					code = bps_event_get_code(event);

					switch (code) {
					case NfcWorker::TERMINATE:
						_timeToDie = true;
						_taskToPerform = NONE_SET;
						emit message("Got a custom BPS event to terminate");
						qDebug() << "XXXX Got a custom BPS event to terminate.";
						break;

					case NfcWorker::WAKEUP:
						emit message("Got a custom BPS event to wake up");
						qDebug() << "XXXX Got a custom BPS event to wake up.";
						break;

					default:
						emit message("Unrecognised custom BPS event");
						qDebug() << "XXXX Unrecognised custom BPS event.";
						break;
					}

				} else {
					emit message("BPS event didn't match a wanted domain");
					qDebug() << "XXXX BPS event didn't match a wanted domain";
				}
			} else {
//				qDebug() << "XXXX No events from BPS on this wakeup";
			}
		} else {
			qDebug() << "XXXX NfcWorker::listen() bps_get_event rc=" << rc;
			emit message("Error getting BPS event");
			qDebug() << "XXXX Error getting BPS event.";
		}
	}

	if (_navigatorExitReceived) {
		qDebug() << "XXXX Exiting listen() loop since got NAVIGATOR_EXIT event";
	}

	qDebug() << "XXXX - exited listen loop - changing state";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDefaultState();

	rc = nfc_stop_events();

	rc = navigator_stop_events(0);

	bps_shutdown();

	qDebug() << "XXXX NfcWorker::listen() BPS shutdown. Exiting listen()";
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

void NfcWorker::prepareToReadNdefTagViaInvoke() {
	if (_failedToInitialize) {
		return;
	}
	qDebug() << "XXXX NfcWorker::prepareToReadNdefTagViaInvoke entered ...";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setReadState(true);
	_taskToPerform = READ_NDEF_TAG;
}

void NfcWorker::prepareToEmulateTag(const QVariant &the_uri, const QVariant &the_text) {
	if (_failedToInitialize) {
		return;
	}

	QString uri = the_uri.toString();
	QString text = the_text.toString();

	qDebug() << "XXXX NfcWorker::prepareToEmulateTag entered ...";

	qDebug() << "XXXX setting inTagEmulationState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setTagEmulationState(true);

	emit clearMessages();
	emit message(QString("Preparing to emulate NDEF tag"));

	_taskToPerform = EMULATE_TAG;

	nfc_ndef_record_t* spNdefRecord;

	_ndefSpUri = uri;
	_ndefSpText = text;

	spNdefRecord = makeSpRecord();

	if (_emulateNdefMessage) {
		CHECK(nfc_delete_ndef_message(_emulateNdefMessage, true));
		_emulateNdefMessage = 0;
	}

	CHECK(nfc_create_ndef_message(&_emulateNdefMessage));
	CHECK(nfc_set_sp_uri(spNdefRecord, _ndefSpUri.toUtf8().constData()));
	CHECK(nfc_add_sp_title(spNdefRecord, Settings::LANG_EN, _ndefSpText.toUtf8().constData()));
	CHECK(nfc_add_ndef_record(_emulateNdefMessage, spNdefRecord));
	CHECK(nfc_start_ndef_tag_emulation(_emulateNdefMessage));

	emit message(QString("Emulating Sp Tag:"));
	emit message(QString("%1").arg(_ndefSpUri));
	emit message(QString("%1").arg(_ndefSpText));
}

void NfcWorker::prepareToStopEmulation() {
	if (_failedToInitialize) {
		return;
	}

	qDebug() << "XXXX NfcWorker::prepareToStopEmulation entered ...";
	qDebug() << "XXXX setting inTagEmulationState=false";

	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setTagEmulationState(false);

	if (state_mgr->inReadState()) {
		_taskToPerform = READ_NDEF_TAG;
	}

	emit message(QString("Preparing to stop Tag emulation"));

	if (_emulateNdefMessage) {
		CHECK(nfc_delete_ndef_message(_emulateNdefMessage, true));
		_emulateNdefMessage = 0;
	}

	CHECK(nfc_stop_ndef_tag_emulation());

	emit message(QString("Tag emulation stopped"));

	qDebug() << "XXXX NfcWorker::prepareToStopEmulation exited ...";
}

void NfcWorker::prepareToWriteNdefUriTag(const QVariant &the_uri) {
	if (_failedToInitialize) {
		return;
	}

	QString uri = the_uri.toString();

	qDebug() << "XXXX NfcWorker::prepareToWriteNdefUriTag entered ...";
	qDebug() << "XXXX URI: " << uri;

	qDebug() << "XXXX setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	emit message(QString("Preparing to write URI: %1").arg(uri));

	_ndefUri = uri;
	_taskToPerform = WRITE_URI_TAG;

	qDebug() << "XXXX registering readerwriter NDEF_TAG target";
	CHECK(nfc_register_tag_readerwriter(TAG_TYPE_NDEF));

}

void NfcWorker::prepareToWriteNdefTextTag(const QVariant &the_text) {
	if (_failedToInitialize) {
		return;
	}

	QString text = the_text.toString();

	qDebug() << "XXXX NfcWorker::prepareToWriteNdefTextTag entered ...";
	qDebug() << "XXXX Text: " << text;

	qDebug() << "XXXX setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	emit message(QString("Preparing to write Text: '%1'").arg(text));

	_ndefText = text;
	_taskToPerform = WRITE_TEXT_TAG;

	CHECK(nfc_register_tag_readerwriter(TAG_TYPE_NDEF));
}

void NfcWorker::prepareToWriteNdefSpTag(const QVariant &the_sp_uri, const QVariant &the_sp_text) {
	if (_failedToInitialize) {
		qDebug() << "XXXX NfcWorker::prepareToWriteNdefSpTag. Initialisation failed. Exiting.";
		return;
	}

	QString sp_uri = the_sp_uri.toString();
	QString sp_text = the_sp_text.toString();

	qDebug() << "XXXX NfcWorker::prepareToWriteNdefSpTag entered ...";
	qDebug() << "XXXX URI: " << sp_uri << "Text: " << sp_text;

	qDebug() << "XXXX setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	emit message(QString("Preparing to write SP URI: '%1' Text: '%2'").arg(sp_uri).arg(sp_text));

	_ndefSpUri = sp_uri;
	_ndefSpText = sp_text;
	_taskToPerform = WRITE_SP_TAG;

	CHECK(nfc_register_tag_readerwriter(TAG_TYPE_NDEF));

	qDebug() << "XXXX NfcWorker::prepareToWriteNdefSpTag done ...";

}

void NfcWorker::prepareToWriteNdefCustomTag(const QVariant &the_domain, const QVariant &the_type, const QVariant &the_payload) {

	if (_failedToInitialize) {
		return;
	}

	QString domain = the_domain.toString();
	QString type = the_type.toString();
	QString payload = the_payload.toString();

	qDebug() << "XXXX NfcWorker::prepareToWriteNdefCustomTag entered ...";
	qDebug() << "XXXX Domain: " << domain << "Type: " << type << "Payload: " << payload;

	qDebug() << "XXXX setting inDetectAndWriteState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setDetectAndWriteState(true);

	emit message(QString("Preparing to write Custom Tag Domain: '%1' Type: '%2' Payload: '%3'").arg(domain).arg(type).arg(payload));

	_ndefDomain = domain;
	_ndefType = type;
	_ndefPayload = payload;
	_taskToPerform = WRITE_CUSTOM_TAG;

	CHECK(nfc_register_tag_readerwriter(TAG_TYPE_NDEF));
}

void NfcWorker::prepareToSendVcard(const QVariant &the_first_name, const QVariant &the_last_name, const QVariant &the_address, const QVariant &the_email,
		const QVariant &the_mobile) {

	if (_failedToInitialize) {
		return;
	}

	QString first_name = the_first_name.toString();
	QString last_name = the_last_name.toString();
	QString address = the_address.toString();
	QString email = the_email.toString();
	QString mobile = the_mobile.toString();

	qDebug() << "XXXX NfcWorker::prepareToSendVcard entered ...";
	qDebug() << "XXXX First Name: " << first_name << ", Last Name: " << last_name << ", Address: " << address << ", Email: " << email << ", Mobile: " << mobile;

	qDebug() << "XXXX setting inNdefPushState=true";
	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setNdefPushState(true);

	emit message(QString("Preparing to send VCARD using SNEP"));
	emit message(QString("First Name: '%1'").arg(first_name));
	emit message(QString("Last Name: '%1'").arg(last_name));
	emit message(QString("Address: '%1'").arg(address));
	emit message(QString("Email: '%1'").arg(email));
	emit message(QString("Mobile: '%1'").arg(mobile));

	_first_name = first_name;
	_last_name = last_name;
	_address = address;
	_email = email;
	_mobile = mobile;
	_taskToPerform = SEND_VCARD;

	CHECK(nfc_register_snep_client());
	qDebug() << "XXXX NfcWorker::prepareToSendVcard registered SNEP client ...";
}

void NfcWorker::doIso7816Test(const QVariant &aid, const QVariant &hex_cla, const QVariant &hex_ins, const QVariant &hexp1p2, const QVariant &hex_lc,
		const QVariant &hex_command, const QVariant &hex_le) {
	emit message(QString("ISO7816-4 test starts"));

	QString _aid = aid.toString();
	QString _hex_cla = hex_cla.toString();
	QString _hex_ins = hex_ins.toString();
	QString _hex_p1p2 = hexp1p2.toString();
	QString _hex_lc = hex_lc.toString();
	QString _hex_command = hex_command.toString();
	QString _hex_le = hex_le.toString();

	QString apdu = _hex_cla;
	apdu.append(_hex_ins);
	apdu.append(_hex_p1p2);
	apdu.append(_hex_lc);
	apdu.append(_hex_command);
	apdu.append(_hex_le);

	emit message(QString("Le: '%1'").arg(_hex_le));
	emit message(QString("COMMAND: '%1'").arg(_hex_command));
	emit message(QString("Lc: '%1'").arg(_hex_lc));
	emit message(QString("P1 P2: '%1'").arg(_hex_p1p2));
	emit message(QString("INS: '%1'").arg(_hex_ins));
	emit message(QString("CLA: '%1'").arg(_hex_cla));
	emit message(QString("AID: '%1'").arg(_aid));
	emit message(QString("APDU: '%1'").arg(apdu));
	emit message(QString("ISO7816-4 request APDU:"));

	// variables for handles
	uint32_t hSESession;
	uint32_t seChannel;
	uint32_t uiccSeReaderID;

	// variables for retrieving the Readers, holders of possible secure elements
	nfc_result_t rc;
	uint32_t numberOfReaders = 0;
	uint32_t *phReaders = NULL;
	static int DEF_LEN = 10;
	char readerName[DEF_LEN];
	uint32_t len = 10;

	//variables for opening and exchanging data
	fcp_type_t fcpResponseType;
	int32_t openResponseLen;
	uint32_t exchangeResponseLen;
	uint32_t nReceiveAPDUBufferSize;
	uint8_t* result;

	QByteArray hex_encoded = QByteArray(aid.toByteArray());

	int aid_size = _aid.length() / 2;
	uint8_t the_aid[aid_size];

	int apdu_size = apdu.length() / 2;
	uint8_t the_apdu[apdu_size];

	Utilities::hexToIntArray(_aid,the_aid);
	Utilities::hexToIntArray(apdu,the_apdu);

	// loop variable
	uint32_t i;

	// Call nfc_se_service_get_num_readers() with the handle acquired in previous step to get the number of available Readers in the system
	CHECK(nfc_se_service_get_num_readers(&numberOfReaders));
	emit message(QString("number of readers=%1").arg(numberOfReaders));

	// Allocate space for the readers
	phReaders = (uint32_t*) malloc(sizeof(uint32_t) * numberOfReaders);
	emit message(QString("space allocated for readers"));

	// get the handles for the readers
	CHECK(nfc_se_service_get_readers(phReaders, &numberOfReaders));
	emit message(QString("got handles for readers"));

	// Iterate through the readers to find the SIM reader.
	int sim_readers_found=0;
	for (i = 0; i < numberOfReaders; i++) {
		len = 10;
		CHECK(nfc_se_reader_get_name(phReaders[i], readerName, &len));
		if ((len == 3) && (strcmp(readerName, "SIM") == 0)) {
			uiccSeReaderID = phReaders[i];
			emit message(QString("got handle to UICC SE reader"));
			sim_readers_found++;
			break;
		}
	}

	if (sim_readers_found == 0) {
		emit message(QString("No SIM based reader found"));
		return;
	}

	// Deallocate the array for holding the readers.
	free(phReaders);

	// Open a session with the SIM Reader
	// Note: You may hold onto this session for the lifetime
	// of you application.
	rc = nfc_se_reader_open_session( uiccSeReaderID,&hSESession );
	if (rc != NFC_RESULT_SUCCESS) {
		qDebug() << QString("XXXX ERROR opening session:%1").arg(rc);
		emit message(QString("ERROR opening session:%1").arg(rc));
		if (rc == NFC_RESULT_SE_NOT_PRESENT) {
			emit message(QString("......or SIM does not include secure element"));
			emit message(QString("No SIM/UICC present"));
		}
		return;
	}

	emit message(QString("opened session with UICC reader"));

	// Open a channel to AID
	fcpResponseType = OPEN_NO_FCP_INFO;
	openResponseLen = 0;

	rc = nfc_se_session_open_logical_channel(hSESession, the_aid, aid_size, fcpResponseType, &seChannel, &openResponseLen);
	if (rc != NFC_RESULT_SUCCESS) {
		qDebug() << QString("XXXX ERROR opening logical channel:%1").arg(rc);
		emit message(QString("ERROR opening logical channel:%1").arg(rc));
		return;
	}

	emit message(QString("opened logical channel"));

	// send APDU command
	emit message(QString("transmit:%1").arg(apdu));

	rc = nfc_se_channel_transmit_apdu( seChannel, the_apdu, apdu_size, &exchangeResponseLen );
	if (rc != NFC_RESULT_SUCCESS) {
		qDebug() << QString("XXXX ERROR transmitting APDU:%1").arg(rc);
		emit message(QString("ERROR transmitting APDU:%1").arg(rc));
	}

	// uint8_t is an 8 bit unsigned type
	result = (uint8_t*) malloc(sizeof(uint8_t) * exchangeResponseLen);
	//get the response of the open call

	nReceiveAPDUBufferSize = exchangeResponseLen;

	if (exchangeResponseLen >= 2) {
		CHECK(nfc_se_channel_get_transmit_data(seChannel, &result[0], &nReceiveAPDUBufferSize));
		emit message(QString("got response APDU. Len=%1").arg(nReceiveAPDUBufferSize));
		QByteArray responseData = QByteArray::fromRawData(reinterpret_cast<const char *>(result), nReceiveAPDUBufferSize);
		QString responseAsHex = QString::fromAscii(responseData.toHex());
		emit message(QString("APDU response: %1").arg(responseAsHex));
	}

	free(result);

	// Close the channel
	if (NFC_RESULT_SUCCESS != nfc_se_channel_close_channel(seChannel)) {
		emit message(QString("nfc_se_channel_close_channel failed"));
	}
	emit message(QString("ISO7816-4 test ends"));

}

/*
 * All detected NFC events are handled here other than NDEF Read Tag events
 */
void NfcWorker::handleNfcEvent(bps_event_t *event) {
	//emit message("Handling an NFC event");
	qDebug() << "XXXX Handling an NFC event";

	switch (_taskToPerform) {
	case WRITE_CUSTOM_TAG:
		qDebug() << "XXXX Handling an NFC event in WRITE_CUSTOM_TAG state";
		handleNfcWriteCustomTagEvent(event);
		break;

	case WRITE_SP_TAG:
		qDebug() << "XXXX Handling an NFC event in WRITE_SP_TAG state";
		handleNfcWriteSpTagEvent(event);
		break;

	case WRITE_TEXT_TAG:
		qDebug() << "XXXX Handling an NFC event in WRITE_TEXT_TAG state";
		handleNfcWriteTextTagEvent(event);
		break;

	case WRITE_URI_TAG:
		qDebug() << "XXXX Handling an NFC event in WRITE_URI_TAG state";
		handleNfcWriteUriTagEvent(event);
		break;

	case READ_NDEF_TAG:
		qDebug() << "XXXX Handling an NFC event in READ_NDEF_TAG state";
		handleNfcReadNdefTagEvent(event);
		break;

	case SEND_VCARD:
		qDebug() << "XXXX Handling an NFC event in SEND_VCARD state";
		handleSendVcardEvent(event);
		break;

	case EMULATE_TAG:
		qDebug() << "XXXX Handling an NFC event in EMULATE_TAG state";
		handleEmulateNfcEvent(event);
		break;

	case NONE_SET:
		qDebug() << "XXXX Handling an NFC event in NONE_SET state";
		break;

	default:
		qDebug() << "XXXX Handling an NFC event in an undefined state";
		break;
	}
}

/*
 * All detected Navigator events are handled here ( NDEF Tags Read Events included )
 */
void NfcWorker::handleNavigatorEvent(bps_event_t *event) {

	if (NAVIGATOR_EXIT == bps_event_get_code(event)) {
		qDebug() << "XXXX Navigator Exit event requested";
		_timeToDie = true;
		_navigatorExitReceived = true;
		_taskToPerform = NONE_SET;
		return;
	}

}

void NfcWorker::handleTagReadInvocation(const QByteArray data) {

	qDebug() << "XXXX handleTagReadInvocation";
	nfc_ndef_message_t *ndefMessage;

	CHECK( nfc_create_ndef_message_from_bytes(reinterpret_cast<const uchar_t *>(data.data()), data.length(), &ndefMessage));

	// cause event log to be displayed now

	StateManager* state_mgr = StateManager::getInstance();
	emit clearMessages();
	if (!state_mgr->isEventLogShowing()) {
		qDebug() << "XXXXX NfcWorker emiting read_selected";
		emit read_selected();
	}

	parseNdefMessage(ndefMessage);

	CHECK(nfc_delete_ndef_message(ndefMessage, true));
}


/*
 * This method processed an NFC Event when we're intending to read an NDEF Tag
 */
void NfcWorker::handleNfcReadNdefTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	nfc_event_t *nfcEvent;

	qDebug() << "XXXX NfcWorker::handleNfcReadNdefTagEvent - processing event code " << code;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug() << "XXXX NfcWorker::handleNfcReadNdefTagEvent - NDEF read event";

		CHECK(nfc_get_nfc_event(event, &nfcEvent));
		qDebug() << "XXXX NfcWorker::handleNfcReadNdefTagEvent - got NFC event object";

		nfc_target_t* target;
		CHECK(nfc_get_target(nfcEvent, &target));
		qDebug() << "XXXX NfcWorker::handleNfcReadNdefTagEvent - got target";

		unsigned int ndefMsgCount = 0;
		CHECK(nfc_get_ndef_message_count(target, &ndefMsgCount));
		qDebug() << "XXXX NfcWorker::handleNfcReadNdefTagEvent - target message count=" << ndefMsgCount;

		for (unsigned int ndefMsgIndex = 0; ndefMsgIndex < ndefMsgCount; ++ndefMsgIndex) {

			// Now processing an an NDEF message

			qDebug() << "XXXX Processing NDEF Message index: " << ndefMsgIndex;

			nfc_ndef_message_t *ndefMessage;
			CHECK(nfc_get_ndef_message(target, ndefMsgIndex, &ndefMessage));

			parseNdefMessage(ndefMessage);
			CHECK(nfc_destroy_target(target));
		}
	} else {
		qDebug() << "XXXX NfcWorker::handleNfcReadNdefTagEvent - NFC BPS event that we didn't register for: " << code;
	}
}

/*
 * This method processed an NFC Event when we're intending to Write a Custom Tag
 *
 * NDEF_TNF_EXTERNAL
 */

void NfcWorker::handleNfcWriteCustomTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXXX NfcWorker::handleNfcWriteCustomTagEvent - processing event code " << code;

	nfc_event_t *nfcEvent;
	nfc_target_t* target;
	nfc_ndef_record_t* myNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug() << "XXXX NfcWorker::handleNfcWriteCustomTagEvent - Target Read Write Event";
		CHECK(nfc_get_nfc_event(event, &nfcEvent));
		CHECK(nfc_get_target(nfcEvent, &target));
		qDebug() << "XXXX NfcWorker::handleWriteCustomTagEvent - Preparing to write Custom: DOMAIN=" << _ndefDomain << ", TYPE=" << _ndefType;
		myNdefRecord = makeCustomRecord(_ndefDomain, _ndefType, _ndefPayload);
		CHECK(nfc_create_ndef_message(&myNdefMessage));
		CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));
		CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));
		CHECK(nfc_destroy_target(target));
		emit message(QString("Custom Tag Written"));
	} else {
		qDebug() << "XXXX NfcWorker::handleNfcWriteCustomTagEvent - NFC BPS event that we didn't register for< " << code;
	}

	qDebug() << "XXXX Write Custom Tag written";
	emit message(QString("Custom tag written OK"));

}

/*
 * This method processed an NFC Event when we're intending to Write an Sp Tag
 */
void NfcWorker::handleNfcWriteSpTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXXX NfcWorker::handleNfcWriteSpTagEvent - processing event code " << code;

	nfc_event_t *nfcEvent;
	nfc_target_t* target;
	nfc_ndef_record_t* spNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug() << "XXXX NfcWorker::handleNfcWriteSpTagEvent - Target Read Write Event";
		CHECK(nfc_get_nfc_event(event, &nfcEvent));
		CHECK(nfc_get_target(nfcEvent, &target));

		qDebug() << "XXXX NfcWorker::handleWriteSpTagEvent - Preparing to write Sp: URI=" << _ndefSpUri << ", Text=" << _ndefSpText;
		spNdefRecord = makeSpRecord();
		CHECK(nfc_create_ndef_message(&myNdefMessage));
		CHECK(nfc_set_sp_uri(spNdefRecord, _ndefSpUri.toUtf8().constData()));
		CHECK( nfc_add_sp_title(spNdefRecord, Settings::LANG_EN, _ndefSpText.toUtf8().constData()));
		CHECK(nfc_add_ndef_record(myNdefMessage, spNdefRecord));
		CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));
		CHECK(nfc_destroy_target(target));
		emit message(QString("Tag Type Sp Written: %1 %2").arg(_ndefSpUri).arg(_ndefSpText));
	} else {
		qDebug() << "XXXX NfcWorker::handleNfcWriteSpTagEvent - NFC BPS event that we didn't register for: " << code;
	}

	qDebug() << "XXXX Write Sp Tag written";
	emit message(QString("Smart Poster tag written OK"));

}

/*
 * This method processed an NFC Event when we're intending to Write Text Tag
 */
void NfcWorker::handleNfcWriteTextTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXXX NfcWorker::handleNfcWriteTextTagEvent - processing event code " << code;

	nfc_event_t *nfcEvent;
	nfc_target_t* target;
	nfc_ndef_record_t* myNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug() << "XXXX NfcWorker::handleNfcWriteTextTagEvent - Target Read Write Event";

		CHECK(nfc_get_nfc_event(event, &nfcEvent));
		CHECK(nfc_get_target(nfcEvent, &target));

		qDebug() << "XXXX NfcWorker::handleWriteUriTagEvent - Preparing to write Text: " << _ndefText;
		myNdefRecord = makeTextRecord(Settings::LANG_EN, _ndefText);

		CHECK(nfc_create_ndef_message(&myNdefMessage));
		CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));
		CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));
		CHECK(nfc_destroy_target(target));
		emit message(QString("Tag Type Written Text: %1").arg(_ndefText));
	} else {
		qDebug() << "XXXX NfcWorker::handleNfcWriteTextTagEvent - NFC BPS event that we didn't register for: " << code;
	}
}

/*
 * This method processed an NFC Event when we're intending to Write a Uri Tag
 */
void NfcWorker::handleNfcWriteUriTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXXX NfcWorker::handleWriteUriTagEvent - processing event code " << code;

	nfc_event_t *nfcEvent;
	nfc_target_t* target;
	nfc_ndef_record_t* myNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug() << "XXXX NfcWorker::handleWriteUriTagEvent - Target Read Write Event";

		CHECK(nfc_get_nfc_event(event, &nfcEvent));
		CHECK(nfc_get_target(nfcEvent, &target));

		qDebug() << "XXXX NfcWorker::handleWriteUriTagEvent - Preparing to write URI: " << _ndefUri;
		myNdefRecord = makeUriRecord(Settings::NfcRtdUriPrefixNone, _ndefUri);

		CHECK(nfc_create_ndef_message(&myNdefMessage));
		CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));
		CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));
		CHECK(nfc_destroy_target(target));
		emit message(QString("Tag Type Written URI: %1").arg(_ndefUri));
	} else {
		qDebug() << "XXXX NfcWorker::handleWriteUriTagEvent - NFC BPS event that we didn't register for: " << code;
	}
}

void NfcWorker::handleSendVcardEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug() << "XXXX NfcWorker::handleSendVcardEvent - processing event code " << code;

	nfc_event_t *nfcEvent;
	nfc_target_t* target;
	nfc_ndef_record_t* myNdefRecord;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_SNEP_CONNECTION_EVENT == code) {

		QString mimeType = QString("text/x-vCard");
		QString vCard = QString("BEGIN:VCARD\n").append("VERSION:3.0\n").append("N:").append(_last_name).append(";").append(_first_name).append("\n").append(
				"FN:").append(_first_name).append(" ").append(_last_name).append("\n").append("ADR;TYPE=WORK:").append(_address).append("\n").append(
				"TEL;TYPE=CELL:").append(_mobile).append("\n").append("EMAIL;TYPE=INTERNET:").append(_email).append("\n").append("END:VCARD");

		CHECK(nfc_get_nfc_event(event, &nfcEvent));
		CHECK(nfc_get_target(nfcEvent, &target));

		qDebug() << "XXXX NfcWorker::handleSendVcardEvent - Preparing to send vCard: FIRST NAME=" << _first_name << ", LAST NAME=" << _last_name << ", ADDRESS="
				<< _address << ", EMAIL=" << _email << ", MOBILE=" << _mobile;
		qDebug() << "XXXX make a media record";
		myNdefRecord = makeMediaRecord(mimeType, vCard);

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

		emit message(QString("vCard sent OK"));
	} else {
		qDebug() << "XXXX NfcWorker::handleSendVcardEvent - NFC BPS event that we didn't register for: " << code;
	}

	qDebug() << "XXXX SendVcard done";
}

void NfcWorker::handleEmulateNfcEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);

	qDebug() << "XXXX NfcWorker::handleEmulateNfcEvent - processing event code: " << code;

	if (NFC_VIRTUAL_TAG_SELECTION_EVENT == code) {
		qDebug() << "XXXX Nfc Virtual Tag Selection Event detected";
		emit message("NFC Virtual Tag Selection Event detected");

	} else if (NFC_VIRTUAL_TAG_LEFT_EVENT == code) {
		qDebug() << "XXXX Nfc Virtual Tag Left Event detected";
		emit message("NFC Virtual Tag Left Event detected");

	} else if (NFC_VIRTUAL_TAG_READ_EVENT == code) {
		qDebug() << "XXXX Nfc Virtual Tag Read Event detected";
		emit message("NFC Virtual Tag Read Event detected");
	}

	qDebug() << "XXXX Emulate done";
}

nfc_ndef_record_t* NfcWorker::makeMediaRecord(QString type, QString text) {
	qDebug() << "XXXX In make media record";

	nfc_ndef_record_t* record = 0;
	int textLen = text.length();
	int totalLen = textLen;
	uchar_t payload[totalLen];
	int offset = 0;
	memcpy(&payload[offset], text.toUtf8().constData(), textLen);

	qDebug() << "XXXX Creating NDEF Record";
	CHECK( nfc_create_ndef_record(NDEF_TNF_MEDIA, type.toUtf8().constData(), payload, totalLen, 0, &record));

	qDebug() << "XXXX Leaving make media record";
	return record;
}

void NfcWorker::parseNdefMessage(nfc_ndef_message_t *ndefMessage) {

	int ndefMsgCount = 0;

	ndefMsgCount = 1;

	for (int ndefMsgIndex = 0; ndefMsgIndex < ndefMsgCount; ++ndefMsgIndex) {

		qDebug() << "XXXX Processing NDEF Message index: " << ndefMsgIndex;

		unsigned int ndefRecordCount = 0;
		CHECK(nfc_get_ndef_record_count(ndefMessage, &ndefRecordCount));

		for (unsigned int ndefRecordIndex = 0; ndefRecordIndex < ndefRecordCount; ++ndefRecordIndex) {

			qDebug() << "XXXX Processing NDEF Record index: " << ndefRecordIndex;
			nfc_ndef_record_t *ndefRecord;
			CHECK( nfc_get_ndef_record(ndefMessage, ndefRecordIndex, &ndefRecord));

			uchar_t* ndefRecordPayloadData;
			size_t ndefRecordPayloadLength;
			CHECK(nfc_get_ndef_record_payload(ndefRecord, &ndefRecordPayloadData, &ndefRecordPayloadLength));

			tnf_type_t ndefRecordTnf;
			CHECK(nfc_get_ndef_record_tnf(ndefRecord, &ndefRecordTnf));

			char *ndefRecordNdefType;
			CHECK(nfc_get_ndef_record_type(ndefRecord, &ndefRecordNdefType));

			char *ndefRecordIdentifier;
			CHECK(nfc_get_ndef_record_id(ndefRecord, &ndefRecordIdentifier));

			QString ndefRecordPayloadAsHex =
					QString::fromAscii(reinterpret_cast<const char *>(ndefRecordPayloadData), ndefRecordPayloadLength).toAscii().toHex();

			QByteArray payLoadData = QByteArray::fromRawData(reinterpret_cast<const char *>(ndefRecordPayloadData), ndefRecordPayloadLength);

			emit message(QString("TNF: %1").arg(ndefRecordTnf));
			emit message(QString("Type: %1").arg(ndefRecordNdefType));
			emit message(QString("Id: %1").arg(ndefRecordIdentifier));

			if (strcmp(ndefRecordNdefType, Settings::NfcRtdSmartPoster) == 0) {
				qDebug() << "XXXX Smart Poster";

				emit message(QString("Smart Poster Tag"));

				char *utf_title;
				char *found_lang;
				char *uri;
				nfc_ndef_rtd_encoding_t rtd_encoding;
				rtd_encoding = UTF_8;

				qDebug() << "XXXX Calling nfc_get_sp_title";
				CHECK( nfc_get_sp_title(ndefRecord, Settings::LANG_EN, &utf_title, &found_lang, &rtd_encoding, true));

				qDebug() << "XXXX Done calling nfc_get_sp_title";

				qDebug() << "XXXX Calling nfc_get_sp_uri";
				CHECK(nfc_get_sp_uri(ndefRecord, &uri));
				qDebug() << "XXXX Done calling nfc_get_sp_uri";

				emit message(QString("Language: %1").arg(found_lang));
				emit message(QString("Title: %1").arg(utf_title));
				emit message(QString("URI: %1").arg(uri));

				qDebug() << QString("Message(%1)/Record(%2) Language: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(found_lang);

				qDebug() << QString("Message(%1)/Record(%2) Title: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(utf_title);

				qDebug() << QString("Message(%1)/Record(%2) URI: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(uri);

				free(utf_title);
				free(uri);
			} else if (strcmp(ndefRecordNdefType, Settings::NfcRtdUri) == 0) {

				qDebug() << "XXXX URI Tag";
				emit message(QString("URI Tag"));

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

				qDebug() << "XXXX Text Tag";
				emit message(QString("Text Tag"));

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

			qDebug() << QString("Message(%1)/Record(%2) Payload (Hex): %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(ndefRecordPayloadAsHex);
			qDebug() << QString("Message(%1)/Record(%2) Type: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(ndefRecordNdefType);

			qDebug() << QString("Message(%1)/Record(%2) Id: %3").arg(ndefMsgIndex).arg(ndefRecordIndex).arg(ndefRecordIdentifier);

		}
	}
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
	qDebug() << "XXXX NfcWorker::makeSpRecord";
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

unsigned long NfcWorker::getSysTimeMs() {
	timeval theTime;
	gettimeofday(&theTime, 0);
	return (theTime.tv_sec * 1000) + (theTime.tv_usec / 1000);
}
