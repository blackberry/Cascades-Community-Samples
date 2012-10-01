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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <bps/bps.h>
#include <nfc/nfc_bps.h>

#include "NfcListener.hpp"

using namespace bb::cascades;

NfcListener::NfcListener() :
		_failedToInitialize(false),
		_emulateNdefMessage(0) {
}

NfcListener::~NfcListener() {
	if (_emulateNdefMessage) {
		CHECK(nfc_delete_ndef_message(_emulateNdefMessage, true));
	}
}

void NfcListener::startListening() {
	initialize();
}

void NfcListener::stopListening() {
	terminate();
}

void NfcListener::initialize() {

	qDebug() << "XXXX NfcListener::initialize() starts...";

	int rc = bps_initialize();
	if (rc) {
		_failedToInitialize = true;
		qDebug() << "XXXX Error: BPS failed to initialise. rc=" << rc;
	}

	rc = nfc_request_events();
	if (rc) {
		_failedToInitialize = true;
		qDebug() << "XXXX Error: Failed to request NFC BPS events. rc=" << rc;
		bps_shutdown();
	} else {
		qDebug() << "XXXX Registered for NFC BPS events OK";
		subscribe(nfc_get_domain());
	}
	qDebug() << "XXXX App::initialize() ends...";
}

void NfcListener::terminate() {
	qDebug() << "XXXX NfcListener::terminate entered ...";
	nfc_stop_events();
	unsubscribe(nfc_get_domain());
	bps_shutdown();
}

void NfcListener::event(bps_event_t *event) {
	qDebug() << "XXXX NfcListener::event() entered ...";

	int domain = bps_event_get_domain(event);
	if (nfc_get_domain() == domain) {
		handleNfcEvent(event);
	}
}

void NfcListener::handleNfcEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);

	qDebug() << "XXXX NfcListener::handleEmulateNfcEvent - processing event code: " << code;

	if (NFC_VIRTUAL_TAG_SELECTION_EVENT == code) {
		qDebug() << "XXXX Nfc Virtual Tag Selection Event detected";
		emit tagEmulationSelectEvent();

	} else if (NFC_VIRTUAL_TAG_LEFT_EVENT == code) {
		qDebug() << "XXXX Nfc Virtual Tag Left Event detected";
		emit tagEmulationLeftEvent();

	} else if (NFC_VIRTUAL_TAG_READ_EVENT == code) {
		qDebug() << "XXXX Nfc Virtual Tag Read Event detected";
		emit tagEmulationReadEvent();
	}
	qDebug() << "XXXX Emulate done";
}

void NfcListener::startTagEmulation(const QString &tagData) {

	qDebug() << "XXXX NfcListener::startTagEmulation: " << tagData;

	nfc_ndef_record_t *ndefRecord = makeCustomRecord(QString("my.rim.com"), QString("myrecordtype"), tagData);

	if (_emulateNdefMessage) {
		CHECK(nfc_delete_ndef_message(_emulateNdefMessage, true));
		_emulateNdefMessage = 0;
	}

	CHECK(nfc_create_ndef_message(&_emulateNdefMessage));
	CHECK(nfc_add_ndef_record(_emulateNdefMessage, ndefRecord));
	CHECK(nfc_start_ndef_tag_emulation(_emulateNdefMessage));
}

void NfcListener::updateTagEmulation(const QString &tagData) {

	qDebug() << "XXXX NfcListener::updateTagEmulation: " << tagData;

	nfc_ndef_record_t  *ndefRecord = makeCustomRecord(QString("my.rim.com"), QString("myrecordtype"), tagData);
	nfc_ndef_message_t *ndefMessage;

	CHECK(nfc_create_ndef_message(&ndefMessage));
	CHECK(nfc_add_ndef_record(ndefMessage, ndefRecord));

	CHECK(nfc_start_ndef_tag_emulation(ndefMessage));

	if (_emulateNdefMessage) {
		CHECK(nfc_delete_ndef_message(_emulateNdefMessage, true));
	}

	_emulateNdefMessage = ndefMessage;
}

void NfcListener::stopTagEmulation() {

	qDebug() << "XXXX NfcListener::stopTagEmulation";

	CHECK(nfc_stop_ndef_tag_emulation());

	if (_emulateNdefMessage) {
		CHECK(nfc_delete_ndef_message(_emulateNdefMessage, true));
		_emulateNdefMessage = 0;
	}
}

nfc_ndef_record_t* NfcListener::makeCustomRecord(QString domain, QString type, QString content) {

	qDebug() << QString("XXXX makeCustomRecord DOMAIN: %1 TYPE: %2 CONTENT %3").arg(domain).arg(type).arg(content);

	nfc_ndef_record_t* record = 0;

	int textLen = content.length();
	QString domainPlusType = domain.append(":");
	domainPlusType = domainPlusType.append(type);
	int totalLen = textLen;

	uchar_t payload[totalLen];

	int offset = 0;
	memcpy(&payload[offset], content.toUtf8().constData(), textLen);

	CHECK(nfc_create_ndef_record(NDEF_TNF_EXTERNAL, domainPlusType.toUtf8().constData(), payload, totalLen, 0, &record));

	return record;
}

void NfcListener::receivedInvokeRequest(const bb::system::InvokeRequest& request) {

    QByteArray data = request.data();
    QtMobilitySubset::QNdefMessage ndefMessage = QtMobilitySubset::QNdefMessage::fromByteArray(data);

    handleNdefRequest(ndefMessage);
}

void NfcListener::handleNdefRequest(const QtMobilitySubset::QNdefMessage ndefMessage) {

	QList<QtMobilitySubset::QNdefRecord>::const_iterator ndefRecord;

	for ( ndefRecord = ndefMessage.begin(); ndefRecord != ndefMessage.end(); ndefRecord++) {

	    if (ndefRecord->typeNameFormat() == QtMobilitySubset::QNdefRecord::ExternalRtd) {
			if (QString(ndefRecord->type()).compare("my.rim.com:myrecordtype") == 0 ) {
				emit raceTagDetected(QString(ndefRecord->payload()));
			}
	    }
	}
}

void NfcListener::checkReturnCode(int rc, int line, const char *file, const char *func) {
	if (rc != BPS_SUCCESS) {
		qDebug() << "XXXX Error code " << rc << " in function " << func
				<< " on line " << line << " in " << file;
	}
}
