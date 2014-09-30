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
        qWarning() << "XXXX NfcListener::ApplicationUI() - connect failed - onInvoked" << strerror(errno) << endl;
    }

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    int rc = nfc_get_feature_set(&_featureSet);
    if (rc == NFC_RESULT_SUCCESS) {
        qDebug() << "XXXX Device supports feature set: " << _featureSet << endl;
    } else {
        qDebug() << "XXXX Unable to determine device feature set" << endl;
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
	qDebug() << "XXXX NfcListener::initialise() starts..." << endl;

	if (_initialised) {
	    return;
	}

    emit featureSetSupported(QVariant(_featureSet));

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	if (_featureSet == 0) {
	    qDebug() << "XXXX Device does not support NFC" << endl;
	    _initialised = false;
	    return;
    }
#endif

	bool failedToInitialise = false;

	int rc = bps_initialize();
	if (rc != BPS_SUCCESS) {
	    failedToInitialise = true;
		qDebug() << "XXXX Error: BPS failed to initialise. rc=" << rc << ", Error: " << strerror(errno) << endl;
	}

	rc = nfc_request_events();
	if (rc != BPS_SUCCESS) {
	    failedToInitialise = true;
		qDebug() << "XXXX Error: Failed to request NFC BPS events. rc=" << rc << ", Error: " << strerror(errno) << endl;
		bps_shutdown();
	} else {
		qDebug() << "XXXX Registered for NFC BPS events OK" << endl;
		subscribe(nfc_get_domain());
	}

	if (failedToInitialise) {
	    _initialised = false;
	} else {
	    _initialised = true;
	}

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    if (isAidRegistered()) {
        emit message("AID: " + _aid + " is currently registered");
        emit aidRegistered(_aid);
    }
#endif

	qDebug() << "XXXX NfcListener::initialise() ends..." << endl;
}

void NfcListener::terminate()
{
	qDebug() << "XXXX NfcListener::terminate entered ..." << endl;

	nfc_stop_events();
	unsubscribe(nfc_get_domain());
	bps_shutdown();

	_initialised = false;
}

void NfcListener::event(bps_event_t *event)
{
	qDebug() << "XXXX NfcListener::event() entered ..." << endl;

	int domain = bps_event_get_domain(event);
	if (nfc_get_domain() == domain) {
		handleNfcEvent(event);
	}
}

void NfcListener::handleNfcEvent(bps_event_t *event)
{
	uint16_t code = bps_event_get_code(event);
    nfc_event_t *nfcEvent;
    nfc_target_t* target;

	qDebug() << "XXXX NfcListener::handleNfcEvent - processing event code: " << code << endl;

    if (NFC_ISO14443_4_EVENT_CODE_EVENT == code) {
        qDebug() << "XXXX HCE: selected by reader" << endl;
        emit message("Selected by reader");
        emit message("*** Certain APDUs not displayed ***");
        emit message("*** See qDebug() for details    ***");

    } else if (NFC_ISO14443_4_COMMAND_EVENT == code) {
        qDebug() << "XXXX HCE: command received" << endl;
        nfc_get_nfc_event(event, &nfcEvent);
        nfc_get_target(nfcEvent, &target);
        processIso144434CommandEvent(target);
    }

	qDebug() << "XXXX Emulate done" << endl;
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
    unsigned char SELECT_XXXX[]                     = { 0x00, 0xa4, 0x04, 0x00 };
    unsigned char SELECT_MCARD_CREDIT[]             = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0x00 };
    unsigned char SELECT_PPSE[]                     = { 0x00, 0xa4, 0x04, 0x00, 0x0e, 0x31, 0x50, 0x41, 0x59, 0x2e, 0x53, 0x59, 0x53,
                                                        0x2e, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00 };
    unsigned char READ_RECORD_1[]                   = { 0x00, 0xb2, 0x01, 0x0c, 0x00 };
    unsigned char READ_RECORD_2[]                   = { 0x00, 0xb2, 0x02, 0x0c, 0x00 };
    unsigned char SELECT_APP_BY_AID[]               = { 0x00, 0xa4, 0x04, 0x00, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10, 0x00 };
    unsigned char GET_PROCESSING_OPTIONS[]          = { 0x80, 0xa8, 0x00, 0x00, 0x02, 0x83, 0x00, 0x00 };
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
    unsigned char RESPONSE_MC_FOUND[]               = { 0x6f, 0x41,
                                                              0x84, 0x07, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x10, 0x10,
                                                              0xa5, 0x36,
                                                              0x50, 0x0b,
                                                                     'M',  'y',  'T',  'e',  's',  't', 'C',  'a',  'r',  'd',  ' ',
                                                              0x87, 0x01, 0x01,
                                                              0x5f, 0x2d, 0x02,
                                                                     'e', 'o',
                                                              0x9f, 0x11, 0x01, 0x01,
                                                              0x9f, 0x12, 0x0b,
                                                                           'M',  'y',  'T',  'e',  's',  't', 'C',  'a',  'r',  'd',  ' ',
                                                              0xbf, 0x0c, 0x0c, 0xc5, 0x0a, 0x01, 0x01, 0x7f, 0x51, 0x47, 0x00, 0x03,
                                                              0x0f, 0xff, 0x03,
                                                        0x90, 0x00 };
    unsigned char RESPONSE_PPSE_FOUND[]             = { 0x6f, 0x2d, 0x84, 0x0e, 0x31, 0x50, 0x41, 0x59, 0x2e, 0x53, 0x59, 0x53, 0x2e, 0x44, 0x44, 0x46,
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
    unsigned char RESPONSE_GET_DATA_PIN_TC[]        = { 0x9f, 0x17, 0x01,
                                                              0x03,
                                                        0x90, 0x00 };

    unsigned char cmd[NFC_ISO14443_4_COMMAND_BUFFER_LENGTH];
    size_t cLen = 0;
    nfc_result_t result;

    if (!target) {
        qDebug() << "XXXX HCE: processIso144434CommandEvent has null target!" << endl;
        emit message("processIso144434CommandEvent has null target!");
        return;
    }

    memset(cmd, 0, NFC_ISO14443_4_COMMAND_BUFFER_LENGTH);

    qDebug() << "XXXX HCE: Received Iso14443-4 Command message" << endl;
    result = nfc_get_iso14443_4_emulation_command(target, cmd, NFC_ISO14443_4_COMMAND_BUFFER_LENGTH, &cLen);

    if (result != NFC_RESULT_SUCCESS) {
        qDebug() << QString("XXXX HCE: nfc_get_iso14443_4_emulation_command failed: %d").arg(result) << endl;
        emit message(QString("nfc_get_iso14443_4_emulation_command failed: %d").arg(result));
        return;
    }

    if      (COMPARE(cmd, COMMAND_HELLO_POS))              RESPOND(cmd, cLen, true,  RESPONSE_HELLO_POS);
    else if (COMPARE(cmd, SELECT_F00012345210))            RESPOND(cmd, cLen, true,  RESPONSE_OK);
    else if (COMPARE(cmd, SELECT_ISD)) {                   RESPOND(cmd, cLen, true,  RESPONSE_67_00); _gotGpo = false;}
    else if (COMPARE(cmd, SELECT_FILE_MASTER_FILE))        RESPOND(cmd, cLen, true,  RESPONSE_OK);
    else if (COMPARE(cmd, SELECT_FILE_MASTER_FILE_BY_ID))  RESPOND(cmd, cLen, true,  RESPONSE_69_85);
    else if (COMPARE(cmd, SELECT_FILE_EF_ATR_1))           RESPOND(cmd, cLen, true,  RESPONSE_6A_86);
    else if (COMPARE(cmd, SELECT_FILE_EF_ATR_2))           RESPOND(cmd, cLen, true,  RESPONSE_67_00);
    else if (COMPARE(cmd, SELECT_FILE_EF_DIR_1))           RESPOND(cmd, cLen, true,  RESPONSE_6A_86);
    else if (COMPARE(cmd, SELECT_FILE_EF_DIR_2))           RESPOND(cmd, cLen, true,  RESPONSE_6A_86);
    else if (COMPARE(cmd, SELECT_FILE_EF_DIR_3))           RESPOND(cmd, cLen, true,  RESPONSE_67_00);
    else if (COMPARE(cmd, SELECT_MCARD_CREDIT))            RESPOND(cmd, cLen, true,  RESPONSE_MC_FOUND);
    else if (COMPARE(cmd, SELECT_PPSE))                    RESPOND(cmd, cLen, true,  RESPONSE_PPSE_FOUND);
    else if (COMPARE(cmd, READ_RECORD_1) && !_gotGpo)      RESPOND(cmd, cLen, true,  RESPONSE_READ_REC_1);
    else if (COMPARE(cmd, READ_RECORD_2))                  RESPOND(cmd, cLen, true,  RESPONSE_6A_83);
    else if (COMPARE(cmd, SELECT_APP_BY_AID))              RESPOND(cmd, cLen, true,  RESPONSE_SELECT_APP_BY_AID);
    else if (COMPARE(cmd, GET_PROCESSING_OPTIONS)) {       RESPOND(cmd, cLen, true,  RESPONSE_GET_PROCESSING_OPTIONS); _gotGpo = true;}
    else if (COMPARE(cmd, READ_RECORD_SFI_1_1) && _gotGpo) RESPOND(cmd, cLen, true,  RESPONSE_READ_RECORD_SFI_1_1);
    else if (COMPARE(cmd, READ_RECORD_SFI_2_1) && _gotGpo) RESPOND(cmd, cLen, true,  RESPONSE_READ_RECORD_SFI_2_1);
    else if (COMPARE(cmd, READ_RECORD_SFI_2_2) && _gotGpo) RESPOND(cmd, cLen, true,  RESPONSE_READ_RECORD_SFI_2_2);
    else if (COMPARE(cmd, READ_RECORD_SFI_2_3) && _gotGpo) RESPOND(cmd, cLen, true,  RESPONSE_READ_RECORD_SFI_2_3);
    else if (COMPARE(cmd, READ_RECORD_SFI_3_1) && _gotGpo) RESPOND(cmd, cLen, true,  RESPONSE_READ_RECORD_SFI_3_1);
    else if (COMPARE(cmd, READ_RECORD_SFI_3_2) && _gotGpo) RESPOND(cmd, cLen, true,  RESPONSE_READ_RECORD_SFI_3_2);
    else if (COMPARE(cmd, READ_RECORD_SFI_3_3) && _gotGpo) RESPOND(cmd, cLen, true,  RESPONSE_READ_RECORD_SFI_3_3);
    else if (COMPARE(cmd, GET_DATA_PIN_TC)     && _gotGpo) RESPOND(cmd, cLen, true,  RESPONSE_GET_DATA_PIN_TC);
    else if (COMPARE(cmd, GET_CHALLENGE)       && _gotGpo) RESPOND(cmd, cLen, true,  RESPONSE_6D_00);
    else if (COMPARE(cmd, SELECT_XXXX))                    RESPOND(cmd, cLen, false, RESPONSE_6A_82); // catch-all
    else if (COMPARE(cmd, GET_DATA_XXXX))                  RESPOND(cmd, cLen, false, RESPONSE_6A_88); // catch-all
    else                                                   RESPOND(cmd, cLen, false, RESPONSE_6D_00); // reject all others
}

void NfcListener::onStartEmulating()
{

	qDebug() << "XXXX NfcListener::onStartEmulating: " << endl;

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
        emit message("Emulating applet");
        emit emulationStarted();
    } else {
        emit message(QString("Error starting emulation: rc=%1, %2").arg(rc).arg(strerror(errno)));
    }
}

void NfcListener::onStopEmulating()
{

	qDebug() << "XXXX NfcListener::onStopEmulating" << endl;

    nfc_stop_iso14443_4_emulation();
    emit emulationStopped();
}

void NfcListener::onRegisterAid()
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX NfcListener::onRegisterAid: " << endl;

    bool isRegistered = false;
    int rc = 0;
    QByteArray aidBytes;

    if (_aid != "*") // wildcard?
    {
        // sanity check on AID: it must have even length
        if (_aid.size() % 2 != 0) {
            qDebug() << "XXXX NfcListener::onRegisterAid: Rejected AID with odd length" << endl;
            return;
        }
        aidBytes = QByteArray::fromHex(_aid.toAscii());  // convert the hexstring to bytes
    }

    const uint8_t* aidPtr = reinterpret_cast<const uint8_t*>(aidBytes.constData());

    if (_featureSet == 2) {

        rc = nfc_hce_is_aid_registered(aidPtr, aidBytes.size(), &isRegistered);

        if ((rc != NFC_RESULT_SUCCESS) || !isRegistered) {

            rc = nfc_hce_register_aid(aidPtr, aidBytes.size(), APP_CATEGORY_OTHER, HCE_INVOKE_AID_SELECTED);

            if (rc == NFC_RESULT_SUCCESS) {

                qDebug() << "XXXX NfcListener::onRegisterAid: nfc_hce_register_aid() :SUCCESS" << endl;
                emit aidRegistered(_aid);

            } else if (rc == NFC_RESULT_OPERATION_REJECTED) {

                qDebug() << "XXXX NfcListener::onRegisterAid: AID already registered" << endl;
                emit aidRegistered(_aid);
                emit message("AID: " + _aid + " was already registered");

            } else if (rc == NFC_RESULT_UNSUPPORTED_API) {

                qDebug() << "XXXX NfcListener::onRegisterAid: Unsupported API" << endl;
                emit message("AID: " + _aid + " unsupported operation");

            } else {
                qDebug() << "XXXX NfcListener::onRegisterAid: registration failed rc=" << rc << endl;
                emit message("AID: " + _aid + " registration failed");
            }
        } else {

            qDebug() << "XXXX NfcListener::onRegisterAid: nfc_hce_is_aid_registered() : REGISTERED" << endl;
            emit aidRegistered(_aid);
            emit message("AID: " + _aid + " was already registered");
        }

    } else {
        qDebug() << "XXXX NfcListener::onRegisterAid: Feature set doesn't support HCE AID registration"<< endl;
        emit message("HCE AID registration not supported on this device");
    }
#endif

    return;
}

void NfcListener::onUnregisterAid()
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX NfcListener::onUnregisterAid: " << endl;


    bool isRegistered = false;
    int rc = 0;
    QByteArray aidBytes;
    if (_aid != "*") // wildcard?
    {
        // sanity check on AID: it must have even length
        if (_aid.size() % 2 != 0) {
            qDebug() << "XXXX NfcListener::onRegisterAid: Rejected AID with odd length" << endl;
            return;
        }
        aidBytes = QByteArray::fromHex(_aid.toAscii());  // convert the hexstring to bytes
    }
    const uint8_t* aidPtr = reinterpret_cast<const uint8_t*>(aidBytes.constData());
    if (_featureSet == 2) {

        rc = nfc_hce_is_aid_registered(aidPtr, aidBytes.size(), &isRegistered);
        if ((rc == NFC_RESULT_SUCCESS) && isRegistered) {

            qDebug() << "XXXX NfcListener::onUnregisterAid: nfc_hce_is_aid_registered() : REGISTERED" << endl;

            rc = nfc_hce_unregister_aid(aidPtr, aidBytes.size());

            if (rc == NFC_RESULT_SUCCESS) {

                qDebug() << "XXXX NfcListener::onUnregisterAid: nfc_hce_unregister_aid() : SUCCESS" << endl;
                emit aidUnregistered(_aid);

            } else if (rc == NFC_RESULT_OPERATION_REJECTED) {

                qDebug() << "XXXX NfcListener::onUnregisterAid: AID hand't been registered" << endl;
                emit aidUnregistered(_aid);
                emit message("AID: " + _aid + " was not registered");

            } else if (rc == NFC_RESULT_UNSUPPORTED_API) {

                qDebug() << "XXXX NfcListener::onUnregisterAid: Unsupported API" << endl;
                emit message("AID: " + _aid + " unsupported operation");

            } else {
                qDebug() << "XXXX NfcListener::onUnregisterAid: unregistration failed" << endl;
                emit message("AID: " + _aid + " unregistration failure");
            }
        } else {
            qDebug() << "XXXX NfcListener::onUnregisterAid: AID wasn't registered" << endl;
            emit aidUnregistered(_aid);
            emit message("AID: " + _aid + " was not registered");
        }
    } else {
        qDebug() << "XXXX NfcListener::onUnregisterAid: Feature set doesn't support HCE AID registration"<< endl;
        emit message("HCE AID registration not supported on this device");
    }
#endif

    return;
}

bool NfcListener::isAidRegistered()
{
    bool isRegistered = false;

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX NfcListener::isAidRegistered: " << endl;

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
            qDebug() << "XXXX NfcListener::isAidRegistered: nfc_hce_is_aid_registered() : REGISTERED" << endl;
            isRegistered = true;
        } else {
            qDebug() << "XXXX NfcListener::isAidRegistered: AID wasn't registered:" << strerror(errno)  << endl;
        }
    } else {
        qDebug() << "XXXX NfcListener::isAidRegistered: Feature set doesn't support HCE AID registration"<< endl;
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
    qDebug() << "XXXX Received invoke action=" << request.action() << endl;
    qDebug() << "XXXX Received invoke target=" << request.target() << endl;
    qDebug() << "XXXX Received invoke mine-type=" << request.mimeType() << endl;

#if BBNDK_VERSION_AT_LEAST(10,3,0)

    bool launchedByHce = (request.action().compare("bb.action.NOTIFY", Qt::CaseInsensitive) == 0) &&
                         (request.target().compare(HCE_INVOKE_AID_SELECTED, Qt::CaseInsensitive) == 0) &&
                         (request.mimeType().compare("application/vnd.bb.nfc_hce", Qt::CaseInsensitive) == 0);

    if (launchedByHce) {
        qDebug() << "XXXX AID has been selected and launched by HCE" << endl;
        emit message("AID has been selected by reader and app launched by HCE");
    } else {
        qWarning() << "XXXX received invocation request we don't handle:" << endl;
    }
#else
    qWarning() << "XXXX received invocation request we don't handle:" << request.action() << endl;
#endif
}

void NfcListener::setAid(const QString& aid)
{
    _aid = aid;
}

