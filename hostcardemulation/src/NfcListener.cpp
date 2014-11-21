/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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
#include "BerTLV.h"

#include "NfcListener.hpp"

using namespace bb::cascades;

NfcListener::NfcListener(QObject *obj)
    : QObject(obj)
    , _initialised(false)
    , _invokeManager(new bb::system::InvokeManager(this))
    , _featureSet(-1)
    , _gotGpo(false)
{
    if(!QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
                                   this, SLOT(onInvoked(const bb::system::InvokeRequest&)))) {
        qWarning() << "XXXX HCE: NfcListener::NfcListener() - connect failed - onInvokeRequest" << strerror(errno) << endl;
    }

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    int rc = nfc_get_feature_set(&_featureSet);
    if (rc == NFC_RESULT_SUCCESS) {
        qDebug() << "XXXX HCE: NfcListener::NfcListener() Device supports feature set: " << _featureSet << endl;
    } else {
        qDebug() << "XXXX HCE: NfcListener::NfcListener() Unable to determine device feature set" << endl;
    }
#else
    _featureSet = -2;   // set to invalid value on pre-10.3 build so QML can make choices
#endif
}

NfcListener::~NfcListener()
{
    if (_initialised) {
        terminate();
    }
}

void NfcListener::startListening()
{
	initialise();
}

void NfcListener::stopListening()
{
	terminate();
}

void NfcListener::initialise()
{
	qDebug() << "XXXX HCE: NfcListener::initialise() starts..." << endl;

	if (_initialised) {
	    return;
	}

    emit featureSetSupported(QVariant(_featureSet));

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	if (_featureSet == 0) {
	    qDebug() << "XXXX HCE: NfcListener::initialise() Device does not support NFC" << endl;
	    _initialised = false;
	    return;
    }
#endif

	bool failedToInitialise = false;

	int rc = bps_initialize();
	if (rc != BPS_SUCCESS) {
	    failedToInitialise = true;
		qDebug() << "XXXX HCE: NfcListener::initialise() Error: BPS failed to initialise. rc=" << rc << ", Error: " << strerror(errno) << endl;
	}

	rc = nfc_request_events();
	if (rc != BPS_SUCCESS) {
	    failedToInitialise = true;
		qDebug() << "XXXX HCE: NfcListener::initialise() Error: Failed to request NFC BPS events. rc=" << rc << ", Error: " << strerror(errno) << endl;
		bps_shutdown();
	} else {
		qDebug() << "XXXX HCE: NfcListener::initialise() Registered for NFC BPS events OK" << endl;
		subscribe(nfc_get_domain());
	}

	if (failedToInitialise) {
	    _initialised = false;
	} else {
	    _initialised = true;
	}

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    if (isAidRegistered()) {
        emit message("XXXX HCE: NfcListener::initialise() AID " + _aid + " is currently registered");
        emit aidRegistered(_aid);
    }
    if (isPPSERegistered()) {
        emit message("XXXX HCE: NfcListener::initialise() PPSE " + _ppse + " is currently registered");
        emit ppseRegistered(_ppse);
    }
#endif

	qDebug() << "XXXX HCE: NfcListener::initialise() ends..." << endl;
}

void NfcListener::terminate()
{
	qDebug() << "XXXX HCE: NfcListener::terminate entered ..." << endl;

	nfc_stop_events();
	unsubscribe(nfc_get_domain());
	bps_shutdown();

	_initialised = false;
    qDebug() << "XXXX HCE: NfcListener::terminate completed ..." << endl;
}

void NfcListener::event(bps_event_t *event)
{
	//qDebug() << "XXXX HCE: NfcListener::event() entered ....................." << endl;

	int domain = bps_event_get_domain(event);
	if (nfc_get_domain() == domain) {
		handleNfcEvent(event);
	}
	else qDebug() << "XXXX HCE: NfcListener::event() ignoring a non NFC domain event" << endl;

    //qDebug() << "XXXX HCE: NfcListener::event() exited ....................." << endl;
}
QString NfcListener::getNfcEventName(int event_code) {
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
    return QString("UNKNOWN NFC EVENT");
}

void NfcListener::handleNfcEvent(bps_event_t *event)
{
	uint16_t code = bps_event_get_code(event);
    nfc_event_t *nfcEvent;
    nfc_target_t* target;

	qDebug() << "XXXX HCE: NfcListener::handleNfcEvent - *** Emulation starting, processing BPS event code: " << getNfcEventName(code) << endl;

    if (NFC_ISO14443_4_EVENT_CODE_EVENT == code) {
        nfc_get_nfc_event(event, &nfcEvent);
        processIso144434CodeEvent(nfcEvent);

    } else if (NFC_ISO14443_4_COMMAND_EVENT == code) {
        nfc_get_nfc_event(event, &nfcEvent);
        nfc_get_target(nfcEvent, &target);
        processIso144434CommandEvent(target);
    }

	qDebug() << "XXXX HCE: NfcListener::handleNfcEvent - *** Emulation done" << endl;
}
void NfcListener::processIso144434CodeEvent(nfc_event_t *nfcEvent)
{
    unsigned int event_value;
    nfc_get_notification_value(nfcEvent, &event_value);
    if (ISO14443_4EVENT_SELECTED==event_value) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CodeEvent - NFC_ISO14443_4_EVENT_CODE_EVENT: SELECTED BY CARD READER";
        emit message("Selected by reader");
    }
    else if (ISO14443_4EVENT_DEACTIVATED == event_value) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CodeEvent - NFC_ISO14443_4_EVENT_CODE_EVENT: DEACTIVATED BY CARD READER";
        emit message("Deactivated by reader");
    }
}
void NfcListener::processIso144434CommandEvent(nfc_target_t *target)
{
    unsigned char COMMAND_HELLO_POS[]               = { 0xa0, 0x37, 0x00, 0x00, 0x01, 0x99, 0x00 };
    unsigned char RESPONSE_HELLO_POS[]              = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'P', 'o', 'S', '!', 0x90, 0x00 };

    unsigned char SELECT_F00012345210[]             = { 0x00, 0xa4, 0x04, 0x00, 0x06, 0xF0, 0x00, 0x12, 0x34, 0x52, 0x10, 0x00};

    unsigned char SELECT_ISD[]                      = { 0x00, 0xa4, 0x04, 0x00, 0x00 };
    unsigned char SELECT_FILE_MASTER_FILE[]         = { 0x00, 0xa4, 0x00, 0x00, 0x00 };
    unsigned char SELECT_FILE_MASTER_FILE_BY_ID[]   = { 0x00, 0xa4, 0x00, 0x00, 0x02, 0x3f, 0x00, 0x00 };
    unsigned char SELECT_FILE_EF_ATR_1[]            = { 0x00, 0xa4, 0x02, 0x00, 0x02, 0x2f, 0x01, 0x00};
    unsigned char SELECT_FILE_EF_ATR_2[]            = { 0x00, 0xa4, 0x00, 0x00, 0x04, 0x3f, 0x00, 0x2f, 0x01, 0x00 };
    unsigned char SELECT_FILE_EF_DIR_1[]            = { 0x00, 0xa4, 0x01, 0x00, 0x02, 0x3f, 0x00, 0x00 };
    unsigned char SELECT_FILE_EF_DIR_2[]            = { 0x00, 0xa4, 0x02, 0x00, 0x02, 0x2f, 0x00, 0x00 };
    unsigned char SELECT_FILE_EF_DIR_3[]            = { 0x00, 0xa4, 0x00, 0x00, 0x04, 0x3f, 0x00, 0x2f, 0x00, 0x00 };
    unsigned char SELECT_XXXX[]                     = { 0x00, 0xa4, 0x04 };

    unsigned char SELECT_MCARD_CREDIT[]             = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0x00 };
    unsigned char SELECT_MCARD_MAESTRO[]            = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x30, 0x60, 0x00 };
    unsigned char SELECT_MCARD_CIRRUS[]             = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x60, 0x00, 0x00 };

    unsigned char SELECT_VISA_CREDIT[]              = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, 0x00 };
    unsigned char SELECT_VISA_ELECTRON[]            = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x03, 0x20, 0x10, 0x00 };
    unsigned char SELECT_VISA_PAY[]                 = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x03, 0x20, 0x20, 0x00 };
    unsigned char SELECT_VISA[]                     = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x03, 0x30, 0x10, 0x00 };
    unsigned char SELECT_VISA_PLUS[]                = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x10, 0x00 };
    unsigned char SELECT_AMEX_CREDIT[]              = { 0x00, 0xa4, 0x04, 0x00, 0x06, 0xa0, 0x00, 0x00, 0x00, 0x25, 0x01, 0x00};
    unsigned char SELECT_DISCOVER[]                 = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x03, 0x24, 0x10, 0x10, 0x00 };
    unsigned char SELECT_JCB[]                      = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x65, 0x10, 0x10, 0x00 };
    unsigned char SELECT_MICROSOFT[]                = { 0x00, 0xa4, 0x04, 0x00, 0x0b, 0xa0, 0x00, 0x00, 0x03, 0x97, 0x43, 0x49, 0x44, 0x5f, 0x01, 0x00 };
    unsigned char SELECT_MSFT[]                     = { 0x00, 0xa4, 0x04, 0x00, 0x09, 0xa0, 0x00, 0x00, 0x03, 0x97, 0x42, 0x54, 0x46, 0x59};
    unsigned char SELECT_NIST[]                     = { 0x00, 0xa4, 0x04, 0x00, 0x09, 0xa0, 0x00, 0x00, 0x03, 0x08, 0x00, 0x00, 0x10, 0x00};
    unsigned char SELECT_EMV_PSE[]                  = { 0x00, 0xa4, 0x04, 0x00, 0x0e, 0x31, 0x50, 0x41, 0x59, 0x2e, 0x53, 0x59, 0x53, 0x2e, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00 };
    unsigned char SELECT_EMV_PPSE[]                 = { 0x00, 0xa4, 0x04, 0x00, 0x0e, 0x32, 0x50, 0x41, 0x59, 0x2e, 0x53, 0x59, 0x53, 0x2e, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00};
    unsigned char READ_RECORD_1[]                   = { 0x00, 0xb2, 0x01, 0x0c, 0x00 };
    unsigned char READ_RECORD_2[]                   = { 0x00, 0xb2, 0x02, 0x0c, 0x00 };
    unsigned char SELECT_APP_BY_AID[]               = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0x00 };
    unsigned char GET_PROCESSING_OPTIONS_VISA[]     = { 0x80, 0xa8, 0x00, 0x00, 0x04, 0x83, 0x02, 0x80, 0x00, 0x00};
    unsigned char GET_PROCESSING_OPTIONS_MC[]       = { 0x80, 0xa8, 0x00, 0x00, 0x02, 0x83, 0x00, 0x00};
    //unsigned char GET_PROCESSING_OPTIONS_MC1[]      = { 0x80, 0x2A, 0x8E, 0x80, 0x0F, 0x00, 0x00, 0x07, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x40, 0x08, 0x40, 0x00};
    unsigned char READ_RECORD_SFI_1_1[]             = { 0x00, 0xb2, 0x01, 0x0c, 0x00 };
    unsigned char READ_RECORD_SFI_2_1[]             = { 0x00, 0xb2, 0x01, 0x14, 0x00 };
    unsigned char READ_RECORD_SFI_2_2[]             = { 0x00, 0xb2, 0x02, 0x14, 0x00 };
    unsigned char READ_RECORD_SFI_2_3[]             = { 0x00, 0xb2, 0x03, 0x14, 0x00 };
    unsigned char READ_RECORD_SFI_3_1[]             = { 0x00, 0xb2, 0x01, 0x1c, 0x00 };
    unsigned char READ_RECORD_SFI_3_2[]             = { 0x00, 0xb2, 0x02, 0x1c, 0x00 };
    unsigned char READ_RECORD_SFI_3_3[]             = { 0x00, 0xb2, 0x03, 0x1c, 0x00 };
    unsigned char GET_DATA_XXXX[]                   = { 0x80, 0xca, 0x9f };
    unsigned char GET_DATA_PIN_TC[]                 = { 0x80, 0xca, 0x9f, 0x17, 0x00 };
    unsigned char GET_CHALLENGE[]                   = { 0x00, 0x84, 0x00, 0x00, 0x00 };

    unsigned char RESPONSE_OK[]                     = { 0x90, 0x00 };
    unsigned char RESPONSE_67_00[]                  = { 0x67, 0x00 };
    unsigned char RESPONSE_69_85[]                  = { 0x69, 0x85 };
    unsigned char RESPONSE_6A_82[]                  = { 0x6a, 0x82 };
    unsigned char RESPONSE_6A_83[]                  = { 0x6a, 0x83 };
    unsigned char RESPONSE_6A_86[]                  = { 0x6a, 0x86 };
    unsigned char RESPONSE_6A_88[]                  = { 0x6a, 0x88 };
    unsigned char RESPONSE_6D_00[]                  = { 0x6d, 0x00 };
    unsigned char RESPONSE_VISA_FOUND[]             = {
                                                        0x6F,  // File Control Information (FCI) Template
                                                           0x1E,  // length = 30 (0x1E)
                                                               0x84,  // Dedicated File (DF) Name
                                                               0x07,  // DF length = 7

                                                              // A0000000031010  (Visa debit or credit AID)
                                                              0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10,

                                                               0xA5,  // File Control Information (FCI) Proprietary Template
                                                               0x13,  // length = 19 (0x13)
                                                                   0x50,  // Application Label
                                                                   0x0B,  // length
                                                                  'V', 'I', 'S', 'A', ' ', 'C', 'R', 'E', 'D', 'I', 'T',
                                                                   0x9F,  0x38,  // Processing Options Data Object List (PDOL)
                                                                   0x03,  // length
                                                                   0x9F,  0x66,  0x02, // PDOL value (Does this request terminal type?)
                                                           0x90,  // SW1
                                                           0x00   // SW2
    };
    unsigned char RESPONSE_MC_FOUND[]               = { 0x6F, 0x1F,
                                                        0x84, // Dedicated File (DF) Name
                                                        0x07, // DF length = 7
                                                        // A0000000031010  (Mastercard debit or credit AID)
                                                        0xA0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10,
                                                        0xA5, // File Control Information (FCI) Proprietary Template
                                                        0x14, // length = 20 (0x14)
                                                        0x50, // Application Label
                                                        0x0A, // length
                                                        'M','a','s','t','e','r','C','a','r','d',
                                                        0xBF, 0x0C, // File Control Information (FCI) Issuer Discretionary Data
                                                        0x05, // length
                                                        0x9F, 0x4D, //Log Entry
                                                        0x02, // length
                                                        0x0B, // SFI of the Transaction Log file
                                                        0x0A, // Transaction Log file number of records
                                                        0x90, 0x00 };
    unsigned char RESPONSE_EMV_PSE_FOUND[]          = { 0x6f, 0x2d, 0x84, 0x0e, 0x31, 0x50, 0x41, 0x59, 0x2e, 0x53, 0x59, 0x53, 0x2e, 0x44, 0x44, 0x46,
                                                        0x30, 0x31, 0xa5, 0x1b, 0x88, 0x01, 0x01, 0x5f, 0x2d, 0x02, 0x65, 0x6e, 0x9f, 0x11, 0x01, 0x01,
                                                        0xbf, 0x0c, 0x0c, 0xc5, 0x0a, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x02, 0xff, 0xff, 0x02,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_READ_REC_1[]             = { 0x70, 0x29, 0x61, 0x27, 0x4f, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0x50, 0x0b, 0x4d,
                                                        0x61, 0x73, 0x74, 0x65, 0x72, 0x43, 0x61, 0x72, 0x64, 0x20, 0x9f, 0x12, 0x0b, 0x4d, 0x61, 0x73,
                                                        0x74, 0x65, 0x72, 0x43, 0x61, 0x72, 0x64, 0x20, 0x87, 0x01, 0x01,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_SELECT_APP_BY_AID[]      = { 0x6f, 0x41, 0x84, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0xa5, 0x36, 0x50, 0x0b, 0x4d,
                                                        0x61, 0x73, 0x74, 0x65, 0x72, 0x43, 0x61, 0x72, 0x64, 0x20, 0x87, 0x01, 0x01, 0x5f, 0x2d, 0x02,
                                                        0x65, 0x6e, 0x9f, 0x11, 0x01, 0x01, 0x9f, 0x12, 0x0b, 0x4d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x43,
                                                        0x61, 0x72, 0x64, 0x20, 0xbf, 0x0c, 0x0c, 0xc5, 0x0a, 0x01, 0x01, 0x7f, 0x51, 0x47, 0x00, 0x03,
                                                        0x0f, 0xff, 0x03,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_GET_PROCESSING_OPTIONS[] = { 0x80, 0x0e, 0x5c, 0x00, 0x08, 0x01, 0x01, 0x00, 0x10, 0x01, 0x03, 0x01, 0x18, 0x01, 0x03, 0x00,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_GET_PROCESSING_OPTIONS_VISA[] = { 0x80, 0x0e, 0x5c, 0x00, 0x08, 0x01, 0x01, 0x00, 0x10, 0x01, 0x03, 0x01, 0x18, 0x01, 0x03, 0x00,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_GET_PROCESSING_OPTIONS_MC[] = { 0x77, 0x16, 0x82, 0x02, 0x5B, 0x80, 0x94, 0x10, 0x08, 0x01, 0x01, 0x00, 0x10, 0x01, 0x01, 0x01, 0x18, 0x01, 0x02, 0x00, 0x20, 0x01, 0x02, 0x00,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_GET_PROCESSING_OPTIONS_MC1[] = { 0x77, 0x0F, 0x9F, 0x61, 0x02, 0x3A, 0xE8, 0x9F, 0x60, 0x02, 0xEB, 0x1A, 0x9F, 0x36, 0x02, 0x02, 0xB2,
                                                        0x90, 0x00 };

    unsigned char RESPONSE_READ_RECORD_SFI_1_1[]    = { 0x70, 0x4d,
                                                              0x5f, 0x20, 0x1a,
                                                                    'F',   'L',  'I',  'N',  'T',  'S',  'T',  'O',  'N',  'E',  '/',
                                                                    'W',   'I',  'L',  'M',  'A',  '.',  'M',  'R',  'S',  ' ',  ' ',
                                                                    ' ',   ' ',  ' ',  ' ',
                                                        0x57, 0x13,
                                                                    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                                                                    0xd9, 0x91, 0x22, 0x01, 0x00, 0x00, 0x00, 0x00, 0x53, 0x00, 0x1f,
                                                        0x9f, 0x1f, 0x18,
                                                                    '0', '0', '0', '0', '0', '0', '0', '0', '0',
                                                                    '0', '0', '0', '0', '0', '0', '0', '0', '0',
                                                                    '0', '0', '0', '0', '0', '0',
                                                        0x90, 0x00 };

    unsigned char RESPONSE_READ_RECORD_SFI_1_1_MC[]    = {0x70, 0x81,
                                                          0x87,
                                                          0x9F, 0x6C, 0x02, 0x00, 0x01,
                                                          0x9F, 0x62, 0x06, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00,
                                                          0x9F, 0x63, 0x06, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE0, 0x56, 0x34, 0x42, 0x35, 0x34, 0x31, 0x33, 0x33, 0x33, 0x39, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x35, 0x31, 0x33, 0x5E, 0x20, 0x2F, 0x5E, 0x31, 0x35, 0x31, 0x32, 0x32, 0x30, 0x31, 0x33, 0x33, 0x30, 0x30, 0x30, 0x33, 0x33, 0x33, 0x30, 0x30, 0x30, 0x32, 0x32, 0x32, 0x32, 0x32, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x30,
                                                          0x9F, 0x64, 0x01, 0x03,
                                                          0x9F, 0x65, 0x02, 0x00, 0x0E,
                                                          0x9F, 0x66, 0x02, 0x0E, 0x70,
                                                          0x9F, 0x6B, 0x13, 0x54, 0x13, 0x33, 0x90, 0x00, 0x00, 0x15, 0x13, 0xD1, 0x51, 0x22, 0x01, 0x90, 0x00, 0x99, 0x00, 0x00, 0x00, 0x0F,
                                                          0x9F, 0x67, 0x01, 0x03,
                                                          0x9F, 0x69, 0x0F,
                                                          0x9F, 0x6A, 0x04,
                                                          0x9F, 0x7E, 0x01,
                                                          0x9F, 0x02, 0x06, 0x5F, 0x2A, 0x02,
                                                          0x9F, 0x1A, 0x02,
                                                          0x90, 0x00

    };
    unsigned char RESPONSE_READ_RECORD_SFI_2_1[]    = { 0x70, 0x4e,
                                                              0x5f, 0x25,
                                                                    0x03, 0x98, 0x01, 0x01,
                                                              0x5f, 0x24,
                                                                    0x03, 0x99, 0x12, 0x31,
                                                              0x5a,
                                                                    0x08,
                                                                          0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                                                              0x5f, 0x34,
                                                                    0x01, 0x03,
                                                              0x9f, 0x07,
                                                                    0x02, 0xff, 0x00,
                                                              0x8e,
                                                                    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41,
                                                                    0x03, 0x1e, 0x03, 0x02, 0x03, 0x1f, 0x03,
                                                              0x9f, 0x0d,
                                                                    0x05, 0xf0, 0x00, 0x7c, 0x20, 0x00,
                                                              0x9f, 0x0e,
                                                                    0x05, 0x00, 0x50, 0x80, 0x00, 0x00,
                                                              0x9f, 0x0f,
                                                                    0x05, 0xf0, 0x20, 0x7c, 0xf8, 0x00,
                                                              0x5f, 0x28,
                                                                    0x02, 0x08, 0x26,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_READ_RECORD_SFI_2_2[]    = { 0x70, 0x30, 0x8c, 0x15, 0x9f, 0x02, 0x06, 0x9f, 0x03, 0x06, 0x9f, 0x1a, 0x02, 0x95, 0x05, 0x5f,
                                                        0x2a, 0x02, 0x9a, 0x03, 0x9c, 0x01, 0x9f, 0x37, 0x04, 0x8d, 0x17, 0x8a, 0x02, 0x9f, 0x02, 0x06,
                                                        0x9f, 0x03, 0x06, 0x9f, 0x1a, 0x02, 0x95, 0x05, 0x5f, 0x2a, 0x02, 0x9a, 0x03, 0x9c, 0x01, 0x9f,
                                                        0x37, 0x04,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_READ_RECORD_SFI_2_3[]    = { 0x70, 0x17, 0x9f, 0x08, 0x02, 0x00, 0x02,
                                                              0x9f, 0x42, 0x02,
                                                                    0x09, 0x51,
                                                              0x9f, 0x44, 0x01, 0x02,
                                                              0x5f, 0x30, 0x02, 0x02, 0x01, 0x9f, 0x4a, 0x01, 0x82,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_READ_RECORD_SFI_3_1[]    = { 0x70, 0x81, 0x93, 0x90, 0x81, 0x90, 0x03, 0x51, 0x29, 0x80, 0xdc, 0x0b, 0xe7, 0xbe, 0x57, 0x2a,
                                                        0xda, 0xfe, 0xdf, 0x50, 0xd0, 0x6d, 0x11, 0x32, 0x84, 0x81, 0x77, 0xc6, 0xa2, 0x72, 0x68, 0x96,
                                                        0x99, 0xcb, 0x0d, 0x78, 0x6d, 0x6b, 0x5e, 0xce, 0x24, 0xf1, 0x73, 0xc2, 0x20, 0x5b, 0x2c, 0x74,
                                                        0x3c, 0xa6, 0x93, 0x24, 0x4d, 0xb4, 0xfe, 0x7e, 0x21, 0x2a, 0x17, 0x9c, 0x7d, 0x3d, 0x40, 0xec,
                                                        0xce, 0x0e, 0x91, 0x89, 0xf7, 0xe8, 0x60, 0x0d, 0x8a, 0x34, 0x07, 0xff, 0x3d, 0xcd, 0xa1, 0xf3,
                                                        0x22, 0x6a, 0x4f, 0x79, 0x52, 0x68, 0x9d, 0xb0, 0xb7, 0xf3, 0x04, 0xba, 0x9d, 0x35, 0x4c, 0xce,
                                                        0xb0, 0x16, 0x6a, 0x87, 0x35, 0x28, 0x0c, 0xd7, 0xf6, 0xa3, 0x5f, 0xb3, 0x6c, 0xb4, 0xd1, 0x7f,
                                                        0x4e, 0x2b, 0xc9, 0xb7, 0xf3, 0xb0, 0x48, 0xa3, 0xba, 0x81, 0x0d, 0x95, 0x6a, 0xf1, 0xc6, 0x17,
                                                        0x97, 0x73, 0x97, 0x78, 0xc0, 0xf0, 0x62, 0x54, 0x69, 0xaf, 0x16, 0x67, 0x3e, 0x73, 0x3c, 0x1d,
                                                        0x50, 0xae, 0xdb, 0x88, 0x47, 0x0c,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_READ_RECORD_SFI_3_2[]    = { 0x70, 0x1d, 0x8f, 0x01, 0x04, 0x9f, 0x32, 0x01, 0x03, 0x92, 0x14, 0x9e, 0x17, 0x56, 0xc3, 0xc5,
                                                        0x31, 0x65, 0xe9, 0x38, 0x65, 0x9f, 0xbe, 0x29, 0xe8, 0xca, 0xd5, 0x42, 0x11, 0xb7, 0xe5,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_READ_RECORD_SFI_3_3[]    = { 0x70, 0x81, 0x83, 0x93, 0x81, 0x80, 0x1a, 0x7f, 0x45, 0x62, 0xfa, 0x79, 0x7c, 0xe9, 0x47, 0xf8,
                                                        0x38, 0x29, 0xa2, 0x31, 0xa2, 0x5d, 0x0c, 0xe1, 0xae, 0x1b, 0x3c, 0x39, 0xae, 0xb4, 0x6f, 0x70,
                                                        0xdf, 0x0c, 0xc6, 0x09, 0x75, 0xd4, 0x18, 0xad, 0x24, 0x35, 0x74, 0xd6, 0x0d, 0xa5, 0x65, 0x73,
                                                        0xd4, 0xcd, 0xae, 0x60, 0xb1, 0x9e, 0x9f, 0x13, 0xa1, 0x30, 0xf6, 0x9c, 0x99, 0x26, 0x38, 0x94,
                                                        0x5b, 0xbc, 0x4a, 0xb7, 0x7f, 0xc6, 0x76, 0x39, 0x68, 0x9b, 0x73, 0x2c, 0x00, 0xe7, 0x13, 0x38,
                                                        0xb3, 0x51, 0xc5, 0x11, 0xa9, 0xb5, 0xa9, 0x97, 0x13, 0xfc, 0x15, 0x19, 0x12, 0xd4, 0xc7, 0x1b,
                                                        0x71, 0xe5, 0xe2, 0x86, 0x1d, 0x5d, 0x1b, 0x7c, 0x30, 0x61, 0x8d, 0x97, 0x7f, 0xf7, 0x5f, 0xf2,
                                                        0x91, 0xb7, 0x58, 0x01, 0x69, 0x18, 0xaa, 0x3b, 0x68, 0xe3, 0xe9, 0x28, 0xd6, 0xdd, 0x77, 0x8e,
                                                        0xf1, 0xcf, 0x69, 0xbe, 0x54, 0x0c,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_GET_DATA_PIN_TC[]        = { 0x9f, 0x17, 0x01, 0x03,
                                                        0x90, 0x00 };

    unsigned char cmd[NFC_ISO14443_4_COMMAND_BUFFER_LENGTH];
    size_t cLen = 0;
    nfc_result_t result;
    if (!target) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent has null target!" << endl;
        emit message("processIso144434CommandEvent has null target!");
        return;
    }

    memset(cmd, 0, NFC_ISO14443_4_COMMAND_BUFFER_LENGTH);
    result = nfc_get_iso14443_4_emulation_command(target, cmd, NFC_ISO14443_4_COMMAND_BUFFER_LENGTH, &cLen);

    if (result != NFC_RESULT_SUCCESS) {
        qDebug() << QString("XXXX HCE: NfcListener::processIso144434CommandEvent: nfc_get_iso14443_4_emulation_command failed: %d").arg(result) << endl;
        emit message(QString("nfc_get_iso14443_4_emulation_command failed: %d").arg(result));
        return;
    }

    if (COMPARE(cmd, COMMAND_HELLO_POS))  {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:COMMAND_HELLO_POS " << endl;
        emit message("COMMAND_HELLO_POS");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_HELLO_POS);
    }
    else if (COMPARE(cmd, SELECT_F00012345210))  {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_F00012345210 " << endl;
        emit message("SELECT_F00012345210");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_OK);
    }
    else if (COMPARE(cmd, SELECT_ISD)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_ISD " << endl;
        emit message("SELECT_ISD");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_67_00); _gotGpo = false;
    }
    else if (COMPARE(cmd, SELECT_FILE_MASTER_FILE)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_FILE_MASTER_FILE " << endl;
        emit message("SELECT_FILE_MASTER_FILE");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_OK);
    }
    else if (COMPARE(cmd, SELECT_FILE_MASTER_FILE_BY_ID)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_FILE_MASTER_FILE_BY_ID " << endl;
        emit message("SELECT_FILE_MASTER_FILE_BY_ID");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_69_85);
    }
    else if (COMPARE(cmd, SELECT_FILE_EF_ATR_1)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_FILE_EF_ATR_1 " << endl;
        emit message("SELECT_FILE_EF_ATR_1");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_6A_86);
    }
    else if (COMPARE(cmd, SELECT_FILE_EF_ATR_2)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_FILE_EF_ATR_2 " << endl;
        emit message("SELECT_FILE_EF_ATR_2");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_67_00);
    }
    else if (COMPARE(cmd, SELECT_FILE_EF_DIR_1)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_FILE_EF_DIR_1 " << endl;
        emit message("SELECT_FILE_EF_DIR_1");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_6A_86);
    }
    else if (COMPARE(cmd, SELECT_FILE_EF_DIR_2)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_FILE_EF_DIR_2 " << endl;
        emit message("SELECT_FILE_EF_DIR_2");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_6A_86);
    }
    else if (COMPARE(cmd, SELECT_FILE_EF_DIR_3)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_FILE_EF_DIR_3 " << endl;
        emit message("SELECT_FILE_EF_DIR_3");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_67_00);
    }
    else if (COMPARE(cmd, SELECT_MCARD_CREDIT))  {
        if (_aid == AID_MASTERCARD_CREDIT_DEBIT) {
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_MASTERCARD_CREDIT " << endl;
            emit message("SELECT_MASTERCARD_CREDIT");
            RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_MC_FOUND);
        }
        else {
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_MASTERCARD_CREDIT UNSUPPORTED " << endl;
            emit message("SELECT_MASTERCARD_CREDIT UNSUPPORTED");
            RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
        }
    }
    else if (COMPARE(cmd, SELECT_VISA_CREDIT))
    {
        if (_aid == AID_VISA_CREDIT_DEBIT) {
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_VISA_CREDIT " << endl;
            emit message("SELECT_VISA_CREDIT");
            RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_VISA_FOUND);
        }
        else {
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_VISA_CREDIT UNSUPPORTED" << endl;
            emit message("SELECT_VISA_CREDIT UNSUPPORTED");
            RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
        }
    }
    else if (COMPARE(cmd, SELECT_EMV_PSE))          {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_EMV_PSE " << endl;
        emit message("SELECT_EMV_PSE");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_EMV_PSE_FOUND);
    }
    else if (COMPARE(cmd, SELECT_EMV_PPSE))          {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_EMV_PPSE " << endl;
        emit message("SELECT_EMV_PPSE");
        QByteArray apduOut;
        QStringList paymentAIDs = getActivePaymentAIDs();
        QString emv_ppse_aid = getAIDFomSelect(cmd, cLen);
        // Generate FCI response for PPSE command
        appendFCI(apduOut, emv_ppse_aid, paymentAIDs);
        // append 9000: command executed ok
        appendSw1Sw2(apduOut, 0x9000);
        RESPOND1(cmd, cLen, EMIT_APDU_PRINTS, apduOut);
    }
    else if (COMPARE(cmd, READ_RECORD_1) && !_gotGpo) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_REC_1 " << endl;
        emit message("READ_REC_1");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_READ_REC_1);
    }
    else if (COMPARE(cmd, READ_RECORD_2))             {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_RECORD_2 " << endl;
        emit message("READ_REC_2");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_6A_83);
    }
    else if (COMPARE(cmd, SELECT_APP_BY_AID))         {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_APP_BY_AID " << endl;
        emit message("SELECT_APP_BY_AID");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_SELECT_APP_BY_AID);
    }
    else if (COMPARE(cmd, GET_PROCESSING_OPTIONS_VISA)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:GET_PROCESSING_OPTIONS VISA " << endl;
        emit message("GET_PROCESSING_OPTIONS VISA");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_GET_PROCESSING_OPTIONS_VISA); _gotGpo = true;
    }
    else if (COMPARE(cmd, GET_PROCESSING_OPTIONS_MC)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:GET_PROCESSING_OPTIONS MASTERCARD" << endl;
        emit message("GET_PROCESSING_OPTIONS MASTERCARD");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_GET_PROCESSING_OPTIONS_MC); _gotGpo = true;
    }
    else if (isGpoCommand(cmd)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:GET_PROCESSING_OPTIONS " << endl;
        emit message("GET_PROCESSING_OPTIONS");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_GET_PROCESSING_OPTIONS); _gotGpo = true;
    }
    else if (isMCGpoCommand(cmd)) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:GET_PROCESSING_OPTIONS MASTERCARD1" << endl;
        emit message("GET_PROCESSING_OPTIONS MASTERCARD 1");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_GET_PROCESSING_OPTIONS_MC1); _gotGpo = true;
    }
    else if (COMPARE(cmd, READ_RECORD_SFI_1_1) && _gotGpo) {
        if (_aid == AID_MASTERCARD_CREDIT_DEBIT)  {
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_RECORD_SFI_1_1 MASTERCARD" << endl;
            emit message("READ_RECORD_SFI_1_1 MASTERCARD");
            RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_READ_RECORD_SFI_1_1_MC);
        }
        else {
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_RECORD_SFI_1_1 " << endl;
            emit message("READ_RECORD_SFI_1_1");
            RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_READ_RECORD_SFI_1_1);
        }
    }
    else if (COMPARE(cmd, READ_RECORD_SFI_2_1) && _gotGpo) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_RECORD_SFI_2_1 " << endl;
        emit message("READ_RECORD_SFI_2_1");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_READ_RECORD_SFI_2_1);
    }
    else if (COMPARE(cmd, READ_RECORD_SFI_2_2) && _gotGpo) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_RECORD_SFI_2_2 " << endl;
        emit message("READ_RECORD_SFI_2_2");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_READ_RECORD_SFI_2_2);
    }
    else if (COMPARE(cmd, READ_RECORD_SFI_2_3) && _gotGpo) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_RECORD_SFI_2_3 " << endl;
        emit message("READ_RECORD_SFI_2_3");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_READ_RECORD_SFI_2_3);
    }
    else if (COMPARE(cmd, READ_RECORD_SFI_3_1) && _gotGpo) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_RECORD_SFI_3_1 " << endl;
        emit message("READ_RECORD_SFI_3_1");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_READ_RECORD_SFI_3_1);
    }
    else if (COMPARE(cmd, READ_RECORD_SFI_3_2) && _gotGpo) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_RECORD_SFI_3_2 " << endl;
        emit message("READ_RECORD_SFI_3_2");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_READ_RECORD_SFI_3_2);
    }
    else if (COMPARE(cmd, READ_RECORD_SFI_3_3) && _gotGpo) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:READ_RECORD_SFI_3_3 " << endl;
        emit message("READ_RECORD_SFI_3_3");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_READ_RECORD_SFI_3_3);
    }
    else if (COMPARE(cmd, GET_DATA_PIN_TC)     && _gotGpo) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:GET_DATA_PIN_TC " << endl;
        emit message("GET_DATA_PIN_TC");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_GET_DATA_PIN_TC);
    }
    else if (COMPARE(cmd, GET_CHALLENGE)       && _gotGpo) {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:GET_CHALLENGE " << endl;
        emit message("GET_CHALLENGE");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS,  RESPONSE_6D_00);
    }
    else if (COMPARE(cmd, SELECT_MICROSOFT))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_MICROSOFT UNSUPPORTED " << endl;
        emit message("SELECT_MICROSOFT UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_MSFT))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_MICROSOFT UNSUPPORTED " << endl;
        emit message("SELECT_MICROSOFT UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_NIST))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_NIST UNSUPPORTED " << endl;
        emit message("SELECT_NIST UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_MCARD_MAESTRO))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_MCARD_MAESTRO UNSUPPORTED " << endl;
        emit message("SELECT_MCARD_MAESTRO UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_MCARD_CIRRUS))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_MCARD_CIRRUS UNSUPPORTED " << endl;
        emit message("SELECT_MCARD_CIRRUS UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_VISA_ELECTRON))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_VISA_ELECTRON UNSUPPORTED " << endl;
        emit message("SELECT_VISA_ELECTRON UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_VISA_PAY))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_VISA_PAY UNSUPPORTED " << endl;
        emit message("SELECT_VISA_PAY UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_VISA_PLUS))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_VISA_PLUS UNSUPPORTED " << endl;
        emit message("SELECT_VISA_PLUS UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_VISA))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_VISA UNSUPPORTED " << endl;
        emit message("SELECT_VISA UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_AMEX_CREDIT))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_AMEX_CREDIT UNSUPPORTED " << endl;
        emit message("SELECT_AMEX_CREDIT UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_DISCOVER))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_DISCOVER UNSUPPORTED " << endl;
        emit message("SELECT_DISCOVER UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_JCB))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_JCB UNSUPPORTED " << endl;
        emit message("SELECT_JCB UNSUPPORTED");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    }
    else if (COMPARE(cmd, SELECT_XXXX))                    {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:SELECT_UNKNOWN_AID " << endl;
        emit message("SELECT UNKNOWN AID");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_82);
    } // catch-all
    else if (COMPARE(cmd, GET_DATA_XXXX))                  {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:GET_DATA_UNKNOWN " << endl;
        emit message("GET DATA UNKNOWN");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6A_88);
    } // catch-all
    else                                                   {
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:COMMAND_UNKNOWN " << endl;
        emit message("COMMAND UNKNOWN");
        RESPOND(cmd, cLen, EMIT_APDU_PRINTS, RESPONSE_6D_00);
    } // reject all others
}

void NfcListener::onStartEmulating()
{

	qDebug() << "XXXX HCE:NfcListener::onStartEmulating: " << endl;

    char validId[] = { 0x4E, 0x01, 0x02, 0x03 };
    char dataStr[] = "Testing_data";

    nfc_iso14443_4_card_t cardTypeA;

    cardTypeA.type = ISO14443_4TYPE_A;
    cardTypeA.info.A.identifier = validId;
    cardTypeA.info.A.identifier_size = sizeof(validId)/sizeof(validId[0]);
    cardTypeA.info.A.applicationData = dataStr;
    cardTypeA.info.A.applicationData_size = sizeof(dataStr)/sizeof(dataStr[0]);

    int rc = nfc_start_iso14443_4_emulation(&cardTypeA);
    if (rc == NFC_RESULT_SUCCESS) {
        qDebug() << "XXXX HCE:Emulating applet success " << endl;
        emit message("Emulating applet");
        emit emulationStarted();
    } else {
        qDebug() << QString("Error starting emulation: rc=%1, %2").arg(rc).arg(strerror(errno)) << endl;
        emit message(QString("Error starting emulation: rc=%1, %2").arg(rc).arg(strerror(errno)));
    }
}

void NfcListener::onStopEmulating()
{

	qDebug() << "XXXX HCE: NfcListener::onStopEmulating" << endl;

    nfc_stop_iso14443_4_emulation();
    emit emulationStopped();
}

void NfcListener::onRegisterAid()
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX HCE: NfcListener::onRegisterAid: " << endl;

    bool isRegistered = false;
    int rc = 0;
    QByteArray aidBytes;


       if (_featureSet == 2) {

       if (_aid != "*") // wildcard?
       {
           // sanity check on AID: it must have even length
           if (_aid.size() % 2 != 0) {
               qDebug() << "XXXX HCE: NfcListener::onRegisterAid: nfc_hce_register_aid() : Rejected AID with odd length" << endl;
               return;
           }
           aidBytes = QByteArray::fromHex(_aid.toAscii());  // convert the hexstring to bytes
       }
       const uint8_t* aidPtr = reinterpret_cast<const uint8_t*>(aidBytes.constData());

        rc = nfc_hce_is_aid_registered(aidPtr, aidBytes.size(), &isRegistered);

        if ((rc != NFC_RESULT_SUCCESS) || !isRegistered) {

            rc = nfc_hce_register_aid(aidPtr, aidBytes.size(), APP_CATEGORY_PAYMENT, HCE_INVOKE_AID_SELECTED);

            if (rc == NFC_RESULT_SUCCESS) {

                qDebug() << "XXXX HCE: NfcListener::onRegisterAid: nfc_hce_register_aid() :SUCCESS" << endl;
                emit aidRegistered(_aid);

            } else if (rc == NFC_RESULT_OPERATION_REJECTED) {

                qDebug() << "XXXX HCE: NfcListener::onRegisterAid: AID already registered" << endl;
                emit aidRegistered(_aid);
                emit message("AID: " + _aid + " was already registered");

            } else if (rc == NFC_RESULT_UNSUPPORTED_API) {

                qDebug() << "XXXX HCE:NfcListener::onRegisterAid: Unsupported API" << endl;
                emit message("AID: " + _aid + " unsupported operation");

            } else {
                qDebug() << "XXXX HCE: NfcListener::onRegisterAid: registration failed rc=" << rc << endl;
                emit message("AID: " + _aid + " registration failed");
            }
        } else {

            qDebug() << "XXXX HCE: NfcListener::onRegisterAid: nfc_hce_is_aid_registered() : REGISTERED" << endl;
            emit aidRegistered(_aid);
            emit message("AID: " + _aid + " was already registered");
        }

    } else {
        qDebug() << "XXXX HCE: NfcListener::onRegisterAid: Feature set doesn't support HCE AID registration"<< endl;
        emit message("HCE Feature set doesn't support HCE AID registration");
    }
#endif

    return;
}

void NfcListener::onUnregisterAid()
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX HCE: NfcListener::onUnregisterAid: " << endl;


    bool isRegistered = false;
    int rc = 0;
    QByteArray aidBytes;
    if (_featureSet == 2) {

        if (_aid != "*") // wildcard?
        {
            // sanity check on AID: it must have even length
            if (_aid.size() % 2 != 0) {
                qDebug() << "XXXX HCE: NfcListener::onUnRegisterAid: nfc_hce_register_aid() : Rejected AID with odd length" << endl;
                return;
            }
            aidBytes = QByteArray::fromHex(_aid.toAscii());  // convert the hexstring to bytes
        }
        const uint8_t* aidPtr = reinterpret_cast<const uint8_t*>(aidBytes.constData());

        rc = nfc_hce_is_aid_registered(aidPtr, aidBytes.size(), &isRegistered);
        if ((rc == NFC_RESULT_SUCCESS) && isRegistered) {

            qDebug() << "XXXX HCE: NfcListener::onUnregisterAid: nfc_hce_is_aid_registered() : REGISTERED" << endl;

            rc = nfc_hce_unregister_aid(aidPtr, aidBytes.size());

            if (rc == NFC_RESULT_SUCCESS) {

                qDebug() << "XXXX HCE: NfcListener::onUnregisterAid: nfc_hce_unregister_aid() : SUCCESS" << endl;
                emit aidUnregistered(_aid);

            } else if (rc == NFC_RESULT_OPERATION_REJECTED) {

                qDebug() << "XXXX HCE: NfcListener::onUnregisterAid: AID hand't been registered" << endl;
                emit aidUnregistered(_aid);
                emit message("AID: " + _aid + " was not registered");

            } else if (rc == NFC_RESULT_UNSUPPORTED_API) {

                qDebug() << "XXXX HCE: NfcListener::onUnregisterAid: Unsupported API" << endl;
                emit message("AID: " + _aid + " unsupported operation");

            } else {
                qDebug() << "XXXX HCE: NfcListener::onUnregisterAid: unregistration failed" << endl;
                emit message("AID: " + _aid + " unregistration failure");
            }
        } else {
            qDebug() << "XXXX HCE: NfcListener::onUnregisterAid: AID wasn't registered" << endl;
            emit aidUnregistered(_aid);
            emit message("AID: " + _aid + " was not registered");
        }
    } else {
        qDebug() << "XXXX HCE: NfcListener::onUnregisterAid: Feature set doesn't support HCE AID registration"<< endl;
        emit message("HCE Feature set doesn't support HCE AID registration");
    }
#endif

    return;
}

void NfcListener::onRegisterPPSE()
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX HCE: NfcListener::onRegisterPPSE: " << endl;

    bool isRegistered = false;
    int rc = 0;
    QByteArray aidBytes;


       if (_featureSet == 2) {

       if (_ppse != "*") // wildcard?
       {
           // sanity check on AID: it must have even length
           if (_ppse.size() % 2 != 0) {
               qDebug() << "XXXX HCE: NfcListener::onRegisterPPSE: nfc_hce_register_aid() : Rejected PPSE AID with odd length" << endl;
               return;
           }
           aidBytes = QByteArray::fromHex(_ppse.toAscii());  // convert the hexstring to bytes
       }
       const uint8_t* aidPtr = reinterpret_cast<const uint8_t*>(aidBytes.constData());

        rc = nfc_hce_is_aid_registered(aidPtr, aidBytes.size(), &isRegistered);

        if ((rc != NFC_RESULT_SUCCESS) || !isRegistered) {

            rc = nfc_hce_register_aid(aidPtr, aidBytes.size(), APP_CATEGORY_PAYMENT, HCE_INVOKE_AID_SELECTED);

            if (rc == NFC_RESULT_SUCCESS) {

                qDebug() << "XXXX HCE: NfcListener::onRegisterPPSE: nfc_hce_register_aid() :SUCCESS" << endl;
                emit ppseRegistered(_ppse);

            } else if (rc == NFC_RESULT_OPERATION_REJECTED) {

                qDebug() << "XXXX HCE: NfcListener::onRegisterPPSE: PPSE AID already registered" << endl;
                emit ppseRegistered(_ppse);
                emit message("PPSE AID: " + _ppse + " was already registered");

            } else if (rc == NFC_RESULT_UNSUPPORTED_API) {

                qDebug() << "XXXX HCE:NfcListener::onRegisterPPSE: Unsupported API" << endl;
                emit message("PPSE AID: " + _ppse + " unsupported operation");

            } else {
                qDebug() << "XXXX HCE: NfcListener::onRegisterPPSE: registration failed rc=" << rc << endl;
                emit message("PPSE AID: " + _ppse + " registration failed");
            }
        } else {

            qDebug() << "XXXX HCE: NfcListener::onRegisterPPSE: nfc_hce_is_aid_registered() : REGISTERED" << endl;
            emit ppseRegistered(_ppse);
            emit message("PPSE AID: " + _ppse + " was already registered");
        }

    } else {
        qDebug() << "XXXX HCE: NfcListener::onRegisterPPSE: Feature set doesn't support HCE AID registration"<< endl;
        emit message("HCE Feature set doesn't support HCE AID registration");
    }
#endif

    return;
}

void NfcListener::onUnregisterPPSE()
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX HCE: NfcListener::onUnregisterPPSE: " << endl;


    bool isRegistered = false;
    int rc = 0;
    QByteArray aidBytes;
    if (_featureSet == 2) {

        if (_ppse != "*") // wildcard?
        {
            // sanity check on AID: it must have even length
            if (_ppse.size() % 2 != 0) {
                qDebug() << "XXXX HCE: NfcListener::onUnregisterPPSE: nfc_hce_register_aid() : Rejected PPSE AID with odd length" << endl;
                return;
            }
            aidBytes = QByteArray::fromHex(_ppse.toAscii());  // convert the hexstring to bytes
        }
        const uint8_t* aidPtr = reinterpret_cast<const uint8_t*>(aidBytes.constData());

        rc = nfc_hce_is_aid_registered(aidPtr, aidBytes.size(), &isRegistered);
        if ((rc == NFC_RESULT_SUCCESS) && isRegistered) {

            qDebug() << "XXXX HCE: NfcListener::onUnregisterPPSE: nfc_hce_is_aid_registered() : REGISTERED" << endl;

            rc = nfc_hce_unregister_aid(aidPtr, aidBytes.size());

            if (rc == NFC_RESULT_SUCCESS) {

                qDebug() << "XXXX HCE: NfcListener::onUnregisterPPSE: nfc_hce_unregister_aid() : SUCCESS" << endl;
                emit ppseUnregistered(_ppse);

            } else if (rc == NFC_RESULT_OPERATION_REJECTED) {

                qDebug() << "XXXX HCE: NfcListener::onUnregisterPPSE: AID hadn't been registered" << endl;
                emit ppseUnregistered(_ppse);
                emit message("AID: " + _ppse + " was not registered");

            } else if (rc == NFC_RESULT_UNSUPPORTED_API) {

                qDebug() << "XXXX HCE: NfcListener::onUnregisterPPSE: Unsupported API" << endl;
                emit message("AID: " + _ppse + " unsupported operation");

            } else {
                qDebug() << "XXXX HCE: NfcListener::onUnregisterPPSE: unregistration failed" << endl;
                emit message("AID: " + _ppse + " unregistration failure");
            }
        } else {
            qDebug() << "XXXX HCE: NfcListener::onUnregisterPPSE: AID wasn't registered" << endl;
            emit ppseUnregistered(_ppse);
            emit message("AID: " + _ppse + " was not registered");
        }
    } else {
        qDebug() << "XXXX HCE: NfcListener::onUnregisterPPSE: Feature set doesn't support HCE AID registration"<< endl;
        emit message("HCE Feature set doesn't support HCE AID registration");
    }
#endif

    return;
}


bool NfcListener::isAidRegistered()
{
    bool isRegistered = false;

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX HCE: NfcListener::isAidRegistered: " << endl;

    int rc = 0;
    QByteArray aidBytes;

    if (_aid != "*") // wildcard?
    {
        // sanity check on AID: it must have even length
        if (_aid.size() % 2 != 0) {
            qDebug() << "XXXX NfcListener::isAidRegistered: Rejected AID with odd length" << endl;
            return isRegistered;
        }
        aidBytes = QByteArray::fromHex(_aid.toAscii());  // convert the hexstring to bytes
    }

    const uint8_t* aidPtr = reinterpret_cast<const uint8_t*>(aidBytes.constData());

    if (_featureSet == 2) {
        rc = nfc_hce_is_aid_registered(aidPtr, aidBytes.size(), &isRegistered);
        if ((rc == NFC_RESULT_SUCCESS) && isRegistered) {
            qDebug() << "XXXX HCE: NfcListener::isAidRegistered: nfc_hce_is_aid_registered() : REGISTERED" << endl;
            isRegistered = true;
        } else {
            qDebug() << "XXXX HCE: NfcListener::isAidRegistered: nfc_hce_is_aid_registered() : NOT REGISTERED " << strerror(errno)  << endl;
        }
    } else {
        qDebug() << "XXXX HCE: NfcListener::isAidRegistered: Feature set doesn't support HCE AID registration"<< endl;
    }
#endif

    return isRegistered;
}

bool NfcListener::isPPSERegistered()
{
    bool isRegistered = false;

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX HCE: NfcListener::isPPSERegistered() " << endl;

    int rc = 0;
    QByteArray aidBytes;

    if (_ppse != "*") // wildcard?
    {
        // sanity check on AID: it must have even length
        if (_ppse.size() % 2 != 0) {
            qDebug() << "XXXX NfcListener::isPPSERegistered: Rejected PPSE AID with odd length" << endl;
            return isRegistered;
        }
        aidBytes = QByteArray::fromHex(_ppse.toAscii());  // convert the hexstring to bytes
    }

    const uint8_t* aidPtr = reinterpret_cast<const uint8_t*>(aidBytes.constData());

    if (_featureSet == 2) {
        rc = nfc_hce_is_aid_registered(aidPtr, aidBytes.size(), &isRegistered);
        if ((rc == NFC_RESULT_SUCCESS) && isRegistered) {
            qDebug() << "XXXX HCE: NfcListener::isPPSERegistered: nfc_hce_is_aid_registered() : REGISTERED" << endl;
            isRegistered = true;
        } else {
            qDebug() << "XXXX HCE: NfcListener::isPPSERegistered: nfc_hce_is_aid_registered() : NOT REGISTERED " << strerror(errno)  << endl;
        }
    } else {
        qDebug() << "XXXX HCE: NfcListener::isPPSERegistered: Feature set doesn't support HCE AID registration"<< endl;
    }
#endif

    return isRegistered;
}
bool NfcListener::initialised()
{
    return !_initialised;
}

void NfcListener::onInvoked(const bb::system::InvokeRequest &request)
{
    qDebug() << "XXXX HCE: NfcListener::onInvoked() Received invoke action=" << request.action() << endl;
    qDebug() << "XXXX HCE: NfcListener::onInvoked() Received invoke target=" << request.target() << endl;
    qDebug() << "XXXX HCE: NfcListener::onInvoked() Received invoke mine-type=" << request.mimeType() << endl;

#if BBNDK_VERSION_AT_LEAST(10,3,0)

    bool launchedByHce = (request.action().compare("bb.action.NOTIFY", Qt::CaseInsensitive) == 0) &&
                         (request.target().compare(HCE_INVOKE_AID_SELECTED, Qt::CaseInsensitive) == 0) &&
                         (request.mimeType().compare("application/vnd.bb.nfc_hce", Qt::CaseInsensitive) == 0);

    if (launchedByHce) {
        qDebug() << "XXXX HCE: NfcListener::onInvoked() AID has been selected and launched by HCE" << endl;
        emit message("AID has been selected by reader and app launched by HCE");
    } else {
        qWarning() << "XXXX HCE: NfcListener::onInvoked() received invocation request we don't handle:" << request.action() << endl;
    }
#else
    qWarning() << "XXXX HCE: NfcListener::onInvoked() received invocation request we don't handle:" << request.action() << endl;
#endif
}

void NfcListener::setAid(const QString& aid)
{
    _aid = aid;
}
void NfcListener::setPPSE(const QString& aid)
{
    _ppse = aid;
}

/*
 *  The data in the request can vary, but it won't affect our response. This method
 *  checks the initial 4 bytes of an APDU to see if it's a GPO command.
 */
bool NfcListener::isGpoCommand(unsigned char* apdu) {
    unsigned char GPO_COMMAND[]     = { 0x80, 0xa8, 0x00, 0x00, 0x04, 0x83};
    return (apdu[0] == GPO_COMMAND[0] &&
            apdu[1] == GPO_COMMAND[1] &&
            apdu[2] == GPO_COMMAND[2] &&
            apdu[3] == GPO_COMMAND[3] && // Skip index 4 which is the length
            apdu[5] == GPO_COMMAND[5]
    );
}
/*
 *  The data in the request can vary, but it won't affect our response. This method
 *  checks the initial 4 bytes of an APDU to see if it's a Mastercard GPO command.
 */
bool NfcListener::isMCGpoCommand(unsigned char* apdu) {
    unsigned char GPO_COMMAND[]     = { 0x80, 0x2A, 0x8E, 0x80, 0x0F};
    return (apdu[0] == GPO_COMMAND[0] &&
            apdu[1] == GPO_COMMAND[1] &&
            apdu[2] == GPO_COMMAND[2] &&
            apdu[3] == GPO_COMMAND[3] &&
            apdu[4] == GPO_COMMAND[4]
    );
}
QString NfcListener::hexToString(const QByteArray& array)
{
    return QString( array.toHex() ).toUpper();
}

QString NfcListener::hexToString(const uchar_t* command, size_t commandLength)
{
    return hexToString( QByteArray(reinterpret_cast<const char *>(command), static_cast<int>(commandLength)) );
}
QString NfcListener::getAIDFomSelect(const uchar_t* command, size_t commandLength )
{
    //select instruction is supposed to be 0xA4
    if( commandLength < 5 || command[1] != 0xA4 )
    {
        return "";
    }

    int lc = command[4];
    if (( (lc + 5) != commandLength )   // case 3
    && ((lc + 5 + 1) != commandLength ))// case 4
    {
        return ""; // case 1&2 do not contain data
    }

    QString commandString = hexToString(command, commandLength);

    //if LE exists then let's chop it off
    if((lc + 5 + 1) == commandLength )
    {
        commandString.chop(2);
    }

    return commandString.remove(0, 10);

}
QStringList NfcListener::getActivePaymentAIDs() {
    QStringList activePaymentAIDs;
    activePaymentAIDs << _aid;
    return activePaymentAIDs;
}
void NfcListener::appendSw1Sw2(QByteArray& buffer, uint16_t sw1sw2) {
    buffer.append(static_cast<char>(sw1sw2 >> 8));
    buffer.append(static_cast<char>(sw1sw2 & 0xFF));
}
void NfcListener::appendFCI(QByteArray& apduOut, const QString& ppseAid, const QStringList& paymentAIDs) {

    if (paymentAIDs.size() == 0) {
        return;
    }

    BerCompositeTLV fciTemplate6F("6F", "FCI template");
    {
        BerSimpleTLV* dfNameTLV = new BerSimpleTLV("84", "DF name", 1, QByteArray::fromHex(ppseAid.toAscii()));
        fciTemplate6F.addChild(dfNameTLV);

        BerCompositeTLV* fciProprietaryTemplateA5 = new BerCompositeTLV("A5", "FCI propriatory");
        {
            BerCompositeTLV* fciIssuerTemplateBF0C = new BerCompositeTLV("BF0C", "FCI issuer discretionary data");
            {
                for(int i = 0; i < paymentAIDs.size(); i++)
                {
                    QString paymentAID = paymentAIDs.at(i);

                    BerCompositeTLV* applicationTemplate61 = new BerCompositeTLV("61", "App template");
                    {
                        BerSimpleTLV* aidPaymentTLV = new BerSimpleTLV("4F", "AID", 1, QByteArray::fromHex(paymentAID.toAscii()));
                        applicationTemplate61->addChild(aidPaymentTLV);

                        const char prioIndicator = static_cast<const char>(i);
                        QByteArray appPrioIndicatorOctet(&prioIndicator, sizeof(prioIndicator));
                        BerSimpleTLV* appPriorityTLV = new BerSimpleTLV("87", "App Prio Indicator", 1, appPrioIndicatorOctet);
                        applicationTemplate61->addChild(appPriorityTLV);
                    }

                    // add application template to FCI issuer template
                    fciIssuerTemplateBF0C->addChild(applicationTemplate61);
                }
            }
            fciProprietaryTemplateA5->addChild(fciIssuerTemplateBF0C);
        }
        fciTemplate6F.addChild(fciProprietaryTemplateA5);
    }

    fciTemplate6F.appendBytes(apduOut);
    //fciTemplate6F.trace();
}
