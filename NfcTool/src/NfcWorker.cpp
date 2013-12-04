/* Copyright (c) 2013 BlackBerry Limited.
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
#include <QDate>
#include <bps/bps.h>
#include <nfc/nfc_bps.h>
#include <nfc/nfc_se_access.h>

#include <gulliver.h>

#include <bps/navigator.h>
#include <bps/navigator_invoke.h>

#include "NfcWorker.hpp"
#include "Logger.hpp"
#include "Settings.hpp"
#include "Utilities.hpp"

#include <sys/time.h>

NfcWorker* NfcWorker::_instance;

#include "StateManager.hpp"
// APDUs that I want ISO14 emulation to ignore. My Omnikey reader and its driver generate them automatically, presumably probing the "card" for certain things
unsigned char FILTER_APDU_0[] = { 0x00, 0xa4, 0x00, 0x0c, 0x02, 0x50, 0x31 };
unsigned char FILTER_APDU_1[] = { 0x00, 0xa4, 0x04, 0x00, 0x0c, 0xa0, 0x00, 0x00, 0x00, 0x63, 0x50, 0x4b, 0x43, 0x53, 0x2d, 0x31, 0x35, 0x00 };
unsigned char FILTER_APDU_2[] = { 0x80, 0xf6, 0x00, 0x01, 0x0a };
unsigned char FILTER_APDU_3[] = { 0x80, 0xf6, 0x00, 0x00, 0x08 };
unsigned char FILTER_APDU_4[] = { 0x00, 0xa4, 0x00, 0x0c, 0x02, 0x3f, 0x00 };

int FILTER_APDU_LENGTHS[] = { 7, 18, 5, 5, 7 };

unsigned char RESPONSE_OK[] = { 0x90, 0x00 };

/*
 * BPS_EVENT_TIMEOUT(-1)   == no timeout timeout on BPS blocking waits
 * BPS_EVENT_TIMEOUT(3000) ==  3 seconds timeout on BPS blocking waits
 */
NfcWorker::NfcWorker(QObject *parent) :
        QObject(parent), BPS_EVENT_TIMEOUT(3000), _failedToInitialize(false), _timeToDie(false), _taskToPerform(NONE_SET), _navigatorExitReceived(false), _emulateNdefMessage(0)
{

    nfc_set_verbosity(2);
}

NfcWorker::~NfcWorker()
{
    qDebug() << "XXXX NfcWorker destructor";
    _timeToDie = true;
    _instance = 0;

    if (_emulateNdefMessage) {
        CHECK(nfc_delete_ndef_message(_emulateNdefMessage, true));
    }
}

NfcWorker* NfcWorker::getInstance()
{
    if (_instance == 0) {
        _instance = new NfcWorker;
    }
    return _instance;
}

void NfcWorker::reset()
{
    qDebug() << "XXXX NfcWorker::reset()";
    _taskToPerform = NONE_SET;
    StateManager* state_mgr = StateManager::getInstance();
    nfc_stop_ndef_tag_emulation();
    nfc_stop_iso14443_4_emulation();
    nfc_unregister_tag_readerwriter();
    nfc_unregister_snep_client();
    if (state_mgr->inLlcpState()) {
        qDebug() << "XXXX unregistering and freeing LLCP resources";
        nfc_llcp_unregister_connection_listener(*_llcp_server);
        nfc_llcp_unregister_connection_listener(*_llcp_client);
        free(_llcp_client);
        free(_llcp_server);
    }
    state_mgr->setDefaultState();
}

void NfcWorker::startEventLoop()
{
    initialize();
    readTag(); // we're interested in reading tags right from the start
    listen();
}

void NfcWorker::readTag()
{
    qDebug() << "XXXX NfcWorker::readTag() starts...";
    prepareToReadNdefTagViaInvoke();
    qDebug() << "XXXX NfcWorker::readTag() ends...";
}

void NfcWorker::emulateTag(const QVariant &uri, const QVariant &text)
{
    qDebug() << "XXXX NfcWorker::emulateTag() starts...";
    prepareToEmulateTag(uri, text);
    qDebug() << "XXXX NfcWorker::emulateTag() ends...";
}

void NfcWorker::stopEmulatingTag()
{
    qDebug() << "XXXX NfcWorker::stopEmulatingTag() starts...";
    prepareToStopEmulation();
    qDebug() << "XXXX NfcWorker::stopEmulatingTag() ends...";
}

void NfcWorker::emulateEcho()
{
    qDebug() << "XXXX NfcWorker::emulateEcho() starts...";
    prepareToEmulateEcho();
    qDebug() << "XXXX NfcWorker::emulateEcho() ends...";
}

void NfcWorker::writeUriTag(const QVariant &uri)
{
    qDebug() << "XXXX NfcWorker::writeUriTag(" << uri << ") starts...";
    prepareToWriteNdefUriTag(uri);
    qDebug() << "XXXX NfcWorker::writeUriTag ends...";
}

void NfcWorker::writeSpTag(const QVariant &sp_uri, const QVariant &sp_text)
{
    qDebug() << "XXXX NfcWorker::writeSpTag(" << sp_uri << "," << sp_text << ") starts...";
    prepareToWriteNdefSpTag(sp_uri, sp_text);
    qDebug() << "XXXX NfcWorker::writeSpTag ends...";
}

void NfcWorker::writeTextTag(const QVariant &text)
{
    qDebug() << "XXXX NfcWorker::writeTextTag(" << text << ") starts...";
    prepareToWriteNdefTextTag(text);
    qDebug() << "XXXX NfcWorker::writeTextTag ends...";
}

void NfcWorker::writeCustomTag(const QVariant &domain, const QVariant &type, const QVariant &payload)
{
    qDebug() << "XXXX NfcWorker::writeCustomTag(" << domain << "," << type << "," << payload << ") starts...";
    prepareToWriteNdefCustomTag(domain, type, payload);
    qDebug() << "XXXX NfcWorker::writeCustomTag ends...";
}

void NfcWorker::sendVcard(const QVariant &first_name, const QVariant &last_name, const QVariant &address, const QVariant &email, const QVariant &mobile)
{
    qDebug() << "XXXX NfcWorker::sendVcard(" << first_name << "," << last_name << "," << address << "," << email << "," << mobile << ") starts...";
    prepareToSendVcard(first_name, last_name, address, email, mobile);
    qDebug() << "XXXX NfcWorker::writeCustomTag ends...";
}

void NfcWorker::readIso15693()
{
    qDebug() << "XXXX NfcWorker::readIso15693 starts...";
    prepareToDoIso15693Read();
    qDebug() << "XXXX NfcWorker::readIso15693 ends...";
}

void NfcWorker::writeIso15693(const QVariant &data)
{
    qDebug() << "XXXX NfcWorker::writeIso15693 starts...";
    prepareToDoIso15693Write(data);
    qDebug() << "XXXX NfcWorker::writeIso15693 ends...";
}

void NfcWorker::readGvb()
{
    qDebug() << "XXXX NfcWorker::readGvb starts...";
    prepareToDoGvbRead();
    qDebug() << "XXXX NfcWorker::readGvb ends...";
}

void NfcWorker::readTagDetails()
{
    qDebug() << "XXXX NfcWorker::readTagDetails starts...";
    prepareToReadTagDetails();
    qDebug() << "XXXX NfcWorker::readTagDetails ends...";
}

/*
 * This method initialises the interface to BPS
 */
void NfcWorker::initialize()
{

    qDebug() << "XXXX NfcWorker::initialize() starts...";

    _eventLog = EventLog::getInstance();
    QObject::connect(this, SIGNAL(event_log_needed()), _eventLog, SLOT(show()));
    qDebug() << "XXXX NfcWorker::initialize() event log connection made";

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
void NfcWorker::listen()
{
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
void NfcWorker::interruptBpsWaitLoop(unsigned int code)
{
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

void NfcWorker::checkReturnCode(int rc, int line, const char *file, const char *func)
{
    if (rc != BPS_SUCCESS) {
        qDebug() << "XXXX Error code " << rc << " in function " << func << " on line " << line << " in " << file;
        StateManager* state_mgr = StateManager::getInstance();
        if (state_mgr->isEventLogShowing()) {
            emit message(QString("Error %1 [%2]").arg(rc).arg(line));
        }
    }
}

void NfcWorker::prepareToReadNdefTagViaInvoke()
{
    if (_failedToInitialize) {
        return;
    }
    qDebug() << "XXXX NfcWorker::prepareToReadNdefTagViaInvoke entered ...";
    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setReadState(true);
    _taskToPerform = READ_NDEF_TAG;
}

void NfcWorker::prepareToEmulateTag(const QVariant &the_uri, const QVariant &the_text)
{
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

void NfcWorker::prepareToStopEmulation()
{
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

void NfcWorker::prepareToEmulateEcho()
{
    if (_failedToInitialize) {
        return;
    }

    qDebug() << "XXXX NfcWorker::prepareToEmulateEcho entered ...";

    qDebug() << "XXXX setting inIso14EmulationState=true";
    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setIso14443_4_EmulationState(true);

    emit clearMessages();

    _taskToPerform = EMULATE_ECHO;

    /**
     * Unique identifier as defined by ISO 14443-3 Type A specification.
     * @c identifier can be either:
     *  - @c NULL so that an internal unique identifier is created by the NFC
     *    driver upon calling @c nfc_start_iso14443_4_emulation().
     *  - a 4-byte array in which the first byte is within the range 0xH0-0xH7
     *    or 0xH9-0xHE, where 'H' can be any hex value.
     */
    char validId[] = { 0x4E, 0x01, 0x02, 0x03 };
    char* identifier = validId;

    /**
     * Size of @c identifier (in bytes). This is currently not used and must be
     * set to 0 by the application.
     */
    size_t identifier_size = 4;

    /**
     * Historical bytes as defined by ISO 14443-4 Type A specification.
     */
    char dataStr[] = "Testing_data";
    char* applicationData = dataStr;

    /**
     * Size of @c applicationData (in bytes)
     */
    size_t applicationData_size = 13;

    nfc_iso14443_4_card_t card_type_A;
    card_type_A.type = ISO14443_4TYPE_A;
    card_type_A.info.A.identifier = identifier;
    card_type_A.info.A.identifier_size = identifier_size;
    card_type_A.info.A.applicationData = applicationData;
    card_type_A.info.A.applicationData_size = applicationData_size;

    const nfc_iso14443_4_card_t* p_card_type_A = &card_type_A;
    int rc = nfc_start_iso14443_4_emulation(p_card_type_A);
    if (rc == NFC_RESULT_SUCCESS) {
        emit message(QString("Emulating echo applet"));
        emit message(QString("Ignoring certain APDUs"));
    } else {
        emit message(QString("Error starting emulation %1").arg(rc));
        _taskToPerform = NONE_SET;
    }
}

void NfcWorker::prepareToWriteNdefUriTag(const QVariant &the_uri)
{
    if (_failedToInitialize) {
        return;
    }

    QString uri = the_uri.toString();

    qDebug() << "XXXX NfcWorker::prepareToWriteNdefUriTag entered ...";
    qDebug() << "XXXX URI: " << uri;

    qDebug() << "XXXX setting inDetectAndWriteState=true";
    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setDetectAndWriteState(true);

    emit message(QString("Writing URI: %1").arg(uri));

    _ndefUri = uri;
    _taskToPerform = WRITE_URI_TAG;

    qDebug() << "XXXX registering readerwriter NDEF_TAG target";
    CHECK(nfc_register_tag_readerwriter(TAG_TYPE_NDEF));

}

void NfcWorker::prepareToWriteNdefTextTag(const QVariant &the_text)
{
    if (_failedToInitialize) {
        return;
    }

    QString text = the_text.toString();

    qDebug() << "XXXX NfcWorker::prepareToWriteNdefTextTag entered ...";
    qDebug() << "XXXX Text: " << text;

    qDebug() << "XXXX setting inDetectAndWriteState=true";
    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setDetectAndWriteState(true);

    emit message(QString("Writing Text: '%1'").arg(text));

    _ndefText = text;
    _taskToPerform = WRITE_TEXT_TAG;

    CHECK(nfc_register_tag_readerwriter(TAG_TYPE_NDEF));
}

void NfcWorker::prepareToWriteNdefSpTag(const QVariant &the_sp_uri, const QVariant &the_sp_text)
{
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

    emit message(QString("Writing SP URI: '%1' Text: '%2'").arg(sp_uri).arg(sp_text));

    _ndefSpUri = sp_uri;
    _ndefSpText = sp_text;
    _taskToPerform = WRITE_SP_TAG;

    CHECK(nfc_register_tag_readerwriter(TAG_TYPE_NDEF));

    qDebug() << "XXXX NfcWorker::prepareToWriteNdefSpTag done ...";

}

void NfcWorker::prepareToWriteNdefCustomTag(const QVariant &the_domain, const QVariant &the_type, const QVariant &the_payload)
{

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

    emit message(QString("Writing Domain: '%1' Type: '%2' Payload: '%3'").arg(domain).arg(type).arg(payload));

    _ndefDomain = domain;
    _ndefType = type;
    _ndefPayload = payload;
    _taskToPerform = WRITE_CUSTOM_TAG;

    CHECK(nfc_register_tag_readerwriter(TAG_TYPE_NDEF));
}

void NfcWorker::prepareToDoIso15693Read()
{
    if (_failedToInitialize) {
        return;
    }

    qDebug() << "XXXX NfcWorker::prepareToDoIso15693Read entered ...";

    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setDetectAndWriteState(false);

    if (!state_mgr->isEventLogShowing()) {
        qDebug() << "XXXXX NfcWorker emiting event_log_needed";
        emit event_log_needed();
    }

    _taskToPerform = READ_ISO15693;

    qDebug() << "XXXX registering readerwriter TAG_TYPE_ISO_15693_3 target";
    CHECK(nfc_register_tag_readerwriter(TAG_TYPE_ISO_15693_3));

}

void NfcWorker::prepareToDoIso15693Write(const QVariant &the_data)
{
    if (_failedToInitialize) {
        return;
    }

    qDebug() << "XXXX NfcWorker::prepareToDoIso15693Write entered ...";

    QString data = the_data.toString();
    _data = data;

    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setDetectAndWriteState(true);

    if (!state_mgr->isEventLogShowing()) {
        qDebug() << "XXXXX NfcWorker emiting event_log_needed";
        emit event_log_needed();
    }

    _taskToPerform = WRITE_ISO15693;

    qDebug() << "XXXX registering readerwriter TAG_TYPE_ISO_15693_3 target";
    CHECK(nfc_register_tag_readerwriter(TAG_TYPE_ISO_15693_3));

}

void NfcWorker::prepareToDoGvbRead()
{
    if (_failedToInitialize) {
        return;
    }

    qDebug() << "XXXX NfcWorker::prepareToDoGvbRead entered ...";

    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setDetectAndWriteState(false);

    if (!state_mgr->isEventLogShowing()) {
        qDebug() << "XXXXX NfcWorker emiting event_log_needed";
        emit event_log_needed();
    }

    _taskToPerform = READ_GVB;

    qDebug() << "XXXX registering readerwriter TAG_TYPE_ISO_14443_3 target";
    CHECK(nfc_register_tag_readerwriter(TAG_TYPE_ISO_14443_3));
}

void NfcWorker::prepareToReadTagDetails()
{
    if (_failedToInitialize) {
        return;
    }

    qDebug() << "XXXX NfcWorker::prepareToReadTagDetails entered ...";

    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setDetectAndWriteState(true);

    if (!state_mgr->isEventLogShowing()) {
        qDebug() << "XXXXX NfcWorker emiting event_log_needed";
        emit event_log_needed();
    }

    _taskToPerform = READ_TAG_DETAILS;

    qDebug() << "XXXX registering readerwriter TAG_TYPE_ISO_14443_3 target";
    CHECK(nfc_register_tag_readerwriter(TAG_TYPE_NDEF));
}

void NfcWorker::prepareToSendVcard(const QVariant &the_first_name, const QVariant &the_last_name, const QVariant &the_address, const QVariant &the_email, const QVariant &the_mobile)
{

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

void NfcWorker::startLlcp()
{

    if (_failedToInitialize) {
        return;
    }

    qDebug() << "XXXX NfcWorker::startLlcp entered ...";

    _taskToPerform = LLCP;

    _llcp_server = (nfc_llcp_connection_listener_t*) malloc(sizeof(nfc_llcp_connection_listener_t));

    nfc_result_t server_result = nfc_llcp_register_connection_listener(NFC_LLCP_SERVER, 0, // let the platform assign the SAP #
            "urn:nfc:sn:llcpdemo", _llcp_server);

    if (server_result != NFC_RESULT_SUCCESS) {
        qDebug() << "XXXX ERROR establishing LLCP server: " << Utilities::getOperationResultName(server_result);
        return;
    }

    qDebug() << "XXXX NfcWorker::startLlcp registered LLCP server ...";

    _llcp_client = (nfc_llcp_connection_listener_t*) malloc(sizeof(nfc_llcp_connection_listener_t));

    nfc_result_t client_result = nfc_llcp_register_connection_listener(NFC_LLCP_CLIENT, 0, "urn:nfc:sn:llcpdemo", _llcp_client);

    if (client_result != NFC_RESULT_SUCCESS) {
        qDebug() << "XXXX ERROR establishing LLCP client: " << Utilities::getOperationResultName(client_result);
        return;
    }

    qDebug() << "XXXX NfcWorker::startLlcp registered LLCP client ...";

    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setLlcpState(true);

}

void NfcWorker::prepIso7816CardTest(const QVariant &aid, bool select_only, const QVariant &hex_cla, const QVariant &hex_ins, const QVariant &hexp1p2, const QVariant &hex_lc,
        const QVariant &hex_command, const QVariant &hex_le)
{

    qDebug() << "XXXX prepIso7816CardTest: INS=" << hex_ins.toString();

    if (_failedToInitialize) {
        return;
    }

    qDebug() << "XXXX setting inDetectAndWriteState=true";
    StateManager* state_mgr = StateManager::getInstance();
    state_mgr->setDetectAndWriteState(true);

    emit message(QString("Bring card into proximity"));

    _aid = aid.toString();
    _select_only = select_only;
    _hex_cla = hex_cla.toString();
    _hex_ins = hex_ins.toString();
    qDebug() << "XXXX prepIso7816CardTest: _hex_ins=" << _hex_ins;
    _hexp1p2 = hexp1p2.toString();
    _hex_lc = hex_lc.toString();
    _hex_command = hex_command.toString();
    _hex_le = hex_le.toString();

    _taskToPerform = CARD_APDU_EXCHANGE;

    qDebug() << "XXXX registering readerwriter ISO_14443_4 target";
    CHECK(nfc_register_tag_readerwriter(TAG_TYPE_ISO_14443_4));

}

void NfcWorker::doIso7816Test(const QVariant &aid, bool select_only, const QVariant &hex_cla, const QVariant &hex_ins, const QVariant &hexp1p2, const QVariant &hex_lc, const QVariant &hex_command,
        const QVariant &hex_le)
{
    qDebug() << "XXXX doIso7816Test. select_only=" << select_only;
    emit message(QString("ISO7816-4 test starts"));

    _aid = aid.toString();
    _hex_cla = "";
    _hex_ins = "";
    _hexp1p2 = "";
    _hex_lc = "";
    _hex_command = "";
    _hex_le = "";
    QString apdu = "";

    emit message(QString("AID: '%1'").arg(_aid));
    if (!select_only) {
        _hex_cla = hex_cla.toString();
        _hex_ins = hex_ins.toString();
        _hexp1p2 = hexp1p2.toString();
        _hex_lc = hex_lc.toString();
        _hex_command = hex_command.toString();
        _hex_le = hex_le.toString();
        apdu = _hex_cla;
        apdu.append(_hex_ins);
        apdu.append(_hexp1p2);
        apdu.append(_hex_lc);
        apdu.append(_hex_command);
        apdu.append(_hex_le);
        emit message(QString("Le: '%1'").arg(_hex_le));
        emit message(QString("COMMAND: '%1'").arg(_hex_command));
        emit message(QString("Lc: '%1'").arg(_hex_lc));
        emit message(QString("P1 P2: '%1'").arg(_hexp1p2));
        emit message(QString("INS: '%1'").arg(_hex_ins));
        emit message(QString("CLA: '%1'").arg(_hex_cla));
        emit message(QString("APDU: '%1'").arg(apdu));
        emit message(QString("ISO7816-4 request APDU:"));
    }
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

    Utilities::hexToIntArray(_aid, the_aid);
    Utilities::hexToIntArray(apdu, the_apdu);

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
    int sim_readers_found = 0;
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
        free(phReaders);
        return;
    }

    // Deallocate the array for holding the readers.
    free(phReaders);

    // Open a session with the SIM Reader
    // Note: You may hold onto this session for the lifetime
    // of you application.
    rc = nfc_se_reader_open_session(uiccSeReaderID, &hSESession);
    if (rc != NFC_RESULT_SUCCESS) {
        qDebug() << QString("XXXX ERROR opening session:%1").arg(rc);
        emit message(QString("ERROR opening session:%1").arg(rc));
        emit message(QString(Utilities::getOperationResultName(rc)));
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
        emit message(QString(Utilities::getOperationResultName(rc)));
        return;
    }

    emit message(QString("opened logical channel with applet"));

    if (!select_only) {
        // send APDU command
        emit message(QString("transmit:%1").arg(apdu));

        rc = nfc_se_channel_transmit_apdu(seChannel, the_apdu, apdu_size, &exchangeResponseLen);
        if (rc != NFC_RESULT_SUCCESS) {
            qDebug() << QString("XXXX ERROR transmitting APDU:%1").arg(rc);
            emit message(QString("ERROR transmitting APDU:%1:%2").arg(rc).arg(Utilities::getOperationResultName(rc)));
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
    }

    // Close the channel
    if (NFC_RESULT_SUCCESS != nfc_se_channel_close_channel(seChannel)) {
        emit message(QString("nfc_se_channel_close_channel failed"));
    }
    emit message(QString("ISO7816-4 test ends"));

}

void NfcWorker::exchangeApduWithCard(bps_event_t *event)
{

    qDebug() << "XXXX exchangeApduWithCard";

    _taskToPerform = NONE_SET;

    int rc = NFC_RESULT_SUCCESS;
    uint16_t code = bps_event_get_code(event);
    nfc_event_t *nfcEvent;

    QString apdu = "";

    if (!_select_only) {
        apdu = _hex_cla;
        apdu.append(_hex_ins);
        apdu.append(_hexp1p2);
        apdu.append(_hex_lc);
        apdu.append(_hex_command);
        apdu.append(_hex_le);
        emit message(QString("Le: '%1'").arg(_hex_le));
        emit message(QString("COMMAND: '%1'").arg(_hex_command));
        emit message(QString("Lc: '%1'").arg(_hex_lc));
        emit message(QString("P1 P2: '%1'").arg(_hexp1p2));
        emit message(QString("INS: '%1'").arg(_hex_ins));
        emit message(QString("CLA: '%1'").arg(_hex_cla));
        emit message(QString("APDU: '%1'").arg(apdu));
        emit message(QString("ISO7816-4 request APDU:"));
    }

    int aid_size = _aid.length() / 2;
    uchar_t the_aid[aid_size];
    Utilities::hexToIntArray(_aid, the_aid);

    int apdu_size = apdu.length() / 2;

    if (NFC_TAG_READWRITE_EVENT == code) {
        qDebug() << "XXXX NfcWorker::exchangeApduWithCard - Target Read Write Event";

        nfc_target_t* target;

        emit message(QString("Card detected"));

        rc = nfc_get_nfc_event(event, &nfcEvent);
        if (rc != NFC_RESULT_SUCCESS) {
            QString msg = QString("Error: NFC result %1:%2").arg(rc).arg(Utilities::getOperationResultName(rc));
            emit message(QString("ERROR: "));
            return;
        }

        rc = nfc_get_target(nfcEvent, &target);
        if (rc != NFC_RESULT_SUCCESS) {
            QString msg = QString("Error: NFC result %1:%2").arg(rc).arg(Utilities::getOperationResultName(rc));
            emit message(QString("ERROR: "));
            return;
        }

        rc = selectByAID(the_aid, aid_size, target);
        if (rc != NFC_RESULT_SUCCESS) {
            QString msg = QString("Error: NFC result %1:%2").arg(rc).arg(Utilities::getOperationResultName(rc));
            emit message(QString("ERROR: "));
            return;
        }

        if (!_select_only) {
            // construct and send the  the user specified command
            uint8_t the_command[apdu_size];
            Utilities::hexToIntArray(apdu, the_command);
            rc = exchangeApdu(the_command, apdu_size, target);
            if (rc != NFC_RESULT_SUCCESS) {
                QString msg = QString("Error: NFC result %1:%2").arg(rc).arg(Utilities::getOperationResultName(rc));
                emit message(QString("ERROR: "));
                return;
            }
        }

        CHECK(nfc_destroy_target(target));

    } else {
        qDebug() << "XXXX NfcWorker::exchangeApduWithCard - NFC BPS event that we didn't register for: " << code;
    }
    CHECK(nfc_unregister_tag_readerwriter());
}

int NfcWorker::miscCommand(uchar_t* the_command, int cmd_size, nfc_target_t* target)
{
    int apdu_size = 4 + (_hex_lc.size() / 2) + cmd_size + (_hex_le.size() / 2);
    qDebug() << "XXXX APDU size=" << apdu_size;
    uchar_t misc_command[apdu_size];
    misc_command[0] = Utilities::hexToInt(_hex_cla.at(0), _hex_cla.at(1)); // CLA
    misc_command[1] = Utilities::hexToInt(_hex_ins.at(0), _hex_ins.at(1)); // INS
    misc_command[2] = Utilities::hexToInt(_hexp1p2.at(0), _hexp1p2.at(1)); // P1
    misc_command[3] = Utilities::hexToInt(_hexp1p2.at(2), _hexp1p2.at(3)); // P2
    int inx = 4;
    if ((_hex_lc.size() / 2) == 1) {
        misc_command[inx++] = Utilities::hexToInt(_hex_lc.at(0), _hex_lc.at(1));
    }
    if ((_hex_command.size() / 2) > 0) {
        for (int i = 0; i < cmd_size; i++) {
            misc_command[inx] = the_command[i];
            inx++;
        }
    }
    if ((_hex_le.size() / 2) == 1) {
        misc_command[inx] = Utilities::hexToInt(_hex_le.at(0), _hex_le.at(1));
    }
    return exchangeApdu(misc_command, apdu_size, target);
}

int NfcWorker::selectByAID(uchar_t* the_aid, int aid_size, nfc_target_t* target)
{
    int apdu_size = 5 + aid_size;
    uchar_t select_command[apdu_size];
    select_command[0] = 0x00; // CLA
    select_command[1] = 0xA4; // INS
    select_command[2] = 0x04; // P1
    select_command[3] = 0x00; // P2
    select_command[4] = aid_size; // Lc
    int j = 5;
    for (int i = 0; i < aid_size; i++) {
        select_command[j] = the_aid[i];
        j++;
    }
    return exchangeApdu(select_command, apdu_size, target);
}

int NfcWorker::exchangeApdu(uchar_t* the_apdu, int apdu_size, nfc_target_t* target)
{
    QByteArray requestData = QByteArray::fromRawData(reinterpret_cast<const char *>(the_apdu), apdu_size);
    QString requestAsHex = QString::fromAscii(requestData.toHex());
    emit message(QString("APDU request:"));
    emit message(QString("%1").arg(requestAsHex));
    qDebug() << "XXXX request:" << requestAsHex;

    int MAX_RESPONSE_SIZE = 256;
    size_t rlength;
    // Allocate response buffer
    // (max size determined by application)
    uchar_t response[MAX_RESPONSE_SIZE];
    int rc = nfc_tag_transceive(target, TAG_TYPE_ISO_14443_4, the_apdu, apdu_size, response, MAX_RESPONSE_SIZE, &rlength);
    if (rc == NFC_RESULT_SUCCESS) {
        QByteArray responseData = QByteArray::fromRawData(reinterpret_cast<const char *>(response), rlength);
        QString responseAsHex = QString::fromAscii(responseData.toHex());
        emit message(QString("APDU response:"));
        emit message(QString("%1").arg(responseAsHex));
        qDebug() << "XXXX response:" << responseAsHex;
    } else {
        emit message(QString("ERROR transmitting APDU:%1:%2").arg(rc).arg(Utilities::getOperationResultName(rc)));
        emit message(Utilities::getOperationResultName(rc));
        qDebug() << "XXXX " << QString("ERROR transmitting APDU:%1:%2").arg(rc).arg(Utilities::getOperationResultName(rc));
    }
    return rc;
}

/*
 * All detected NFC events are handled here other than NDEF Read Tag events
 */
void NfcWorker::handleNfcEvent(bps_event_t *event)
{
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

        case READ_ISO15693:
            qDebug() << "XXXX Handling an NFC event in READ_ISO15693 state";
            handleIso15693TagEvent(event);
            break;

        case WRITE_ISO15693:
            qDebug() << "XXXX Handling an NFC event in READ_ISO15693 state";
            handleIso15693TagEvent(event);
            break;

        case READ_NDEF_TAG:
            qDebug() << "XXXX Handling an NFC event in READ_NDEF_TAG state";
            handleNfcReadNdefTagEvent(event);
            break;

        case CARD_APDU_EXCHANGE:
            qDebug() << "XXXX Handling an NFC event in CARD_APDU_EXCHANGE state";
            exchangeApduWithCard(event);
            break;

        case SEND_VCARD:
            qDebug() << "XXXX Handling an NFC event in SEND_VCARD state";
            handleSendVcardEvent(event);
            break;

        case LLCP:
            qDebug() << "XXXX Handling an NFC event in LLCP state";
            handleLlcpEvent(event);
            break;

        case EMULATE_TAG:
            qDebug() << "XXXX Handling an NFC event in EMULATE_TAG state";
            handleEmulateNfcEvent(event);
            break;

        case EMULATE_ECHO:
            qDebug() << "XXXX Handling an NFC event in EMULATE_ECHO state";
            handleEmulateEchoEvent(event);
            break;

        case READ_GVB:
            qDebug() << "XXXX Handling an NFC event in READ_GVB state";
            handleGvbEvent(event);
            break;

        case READ_TAG_DETAILS:
            qDebug() << "XXXX Handling an NFC event in READ_TAG_DETAILS state";
            handleReadTagDetailsEvent(event);
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
void NfcWorker::handleNavigatorEvent(bps_event_t *event)
{

    if (NAVIGATOR_EXIT == bps_event_get_code(event)) {
        qDebug() << "XXXX Navigator Exit event requested";
        _timeToDie = true;
        _navigatorExitReceived = true;
        _taskToPerform = NONE_SET;
        return;
    }

}

void NfcWorker::handleTagReadInvocation(const QByteArray data)
{

    qDebug() << "XXXX handleTagReadInvocation";
    nfc_ndef_message_t *ndefMessage;

    CHECK(nfc_create_ndef_message_from_bytes(reinterpret_cast<const uchar_t *>(data.data()), data.length(), &ndefMessage));

    // cause event log to be displayed now

    StateManager* state_mgr = StateManager::getInstance();
    emit clearMessages();
    if (!state_mgr->isEventLogShowing()) {
        qDebug() << "XXXXX NfcWorker emiting event_log_needed";
        emit event_log_needed();
    }

    parseNdefMessage(ndefMessage);

    CHECK(nfc_delete_ndef_message(ndefMessage, true));
}

/*
 * This method processed an NFC Event when we're intending to read an NDEF Tag
 */
void NfcWorker::handleNfcReadNdefTagEvent(bps_event_t *event)
{
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

        }

        CHECK(nfc_destroy_target(target));

    } else {
        qDebug() << "XXXX NfcWorker::handleNfcReadNdefTagEvent - NFC BPS event that we didn't register for: " << code;
    }
}

/*
 * This method processed an NFC Event when we're intending to Write a Custom Tag
 *
 * NDEF_TNF_EXTERNAL
 */

void NfcWorker::handleNfcWriteCustomTagEvent(bps_event_t *event)
{
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
        displayTagInformation(target, false);
        qDebug() << "XXXX NfcWorker::handleWriteCustomTagEvent - Preparing to write Custom: DOMAIN=" << _ndefDomain << ", TYPE=" << _ndefType;
        myNdefRecord = makeCustomRecord(_ndefDomain, _ndefType, _ndefPayload);
        CHECK(nfc_create_ndef_message(&myNdefMessage));
        CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));
        CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
        CHECK(nfc_delete_ndef_message(myNdefMessage, true));
        CHECK(nfc_destroy_target(target));
        CHECK(nfc_unregister_tag_readerwriter());
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
void NfcWorker::handleNfcWriteSpTagEvent(bps_event_t *event)
{
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
        displayTagInformation(target, false);
        qDebug() << "XXXX NfcWorker::handleWriteSpTagEvent - Preparing to write Sp: URI=" << _ndefSpUri << ", Text=" << _ndefSpText;
        spNdefRecord = makeSpRecord();
        CHECK(nfc_create_ndef_message(&myNdefMessage));
        CHECK(nfc_set_sp_uri(spNdefRecord, _ndefSpUri.toUtf8().constData()));
        CHECK(nfc_add_sp_title(spNdefRecord, Settings::LANG_EN, _ndefSpText.toUtf8().constData()));
        CHECK(nfc_add_ndef_record(myNdefMessage, spNdefRecord));
        CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
        CHECK(nfc_delete_ndef_message(myNdefMessage, true));
        CHECK(nfc_destroy_target(target));
        CHECK(nfc_unregister_tag_readerwriter());
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
void NfcWorker::handleNfcWriteTextTagEvent(bps_event_t *event)
{
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
        displayTagInformation(target, false);
        qDebug() << "XXXX NfcWorker::handleWriteTextTagEvent - Preparing to write Text: " << _ndefText;
        myNdefRecord = makeTextRecord(Settings::LANG_EN, _ndefText);

        CHECK(nfc_create_ndef_message(&myNdefMessage));
        CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));
        CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
        CHECK(nfc_delete_ndef_message(myNdefMessage, true));
        CHECK(nfc_destroy_target(target));
        CHECK(nfc_unregister_tag_readerwriter());
        emit message(QString("Tag Type Written Text: %1").arg(_ndefText));
    } else {
        qDebug() << "XXXX NfcWorker::handleNfcWriteTextTagEvent - NFC BPS event that we didn't register for: " << code;
    }
}

/*
 * This method processed an NFC Event when we're intending to Write a Uri Tag
 */
void NfcWorker::handleNfcWriteUriTagEvent(bps_event_t *event)
{
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
        qDebug() << "XXXX displaying tag info";
        displayTagInformation(target, false);
        qDebug() << "XXXX done displaying tag info";
        qDebug() << "XXXX NfcWorker::handleWriteUriTagEvent - Preparing to write URI: " << _ndefUri;
        myNdefRecord = makeUriRecord(Settings::NfcRtdUriPrefixNone, _ndefUri);

        CHECK(nfc_create_ndef_message(&myNdefMessage));
        CHECK(nfc_add_ndef_record(myNdefMessage, myNdefRecord));
        CHECK(nfc_write_ndef_message_to_tag(target, myNdefMessage, false));
        CHECK(nfc_delete_ndef_message(myNdefMessage, true));
        CHECK(nfc_destroy_target(target));
        CHECK(nfc_unregister_tag_readerwriter());
        emit message(QString("Tag Type Written URI: %1").arg(_ndefUri));
    } else {
        qDebug() << "XXXX NfcWorker::handleWriteUriTagEvent - NFC BPS event that we didn't register for: " << code;
    }
}

void NfcWorker::handleIso15693TagEvent(bps_event_t *event)
{
    uint16_t code = bps_event_get_code(event);
    qDebug() << "XXXX NfcWorker::handleIso15693TagEvent - processing event code " << code;

    nfc_event_t *nfcEvent;
    nfc_target_t* target;

    if (NFC_TAG_READWRITE_EVENT == code) {
        qDebug() << "XXXX NfcWorker::handleIso15693TagEvent - Target Read Write Event";

        CHECK(nfc_get_nfc_event(event, &nfcEvent));
        CHECK(nfc_get_target(nfcEvent, &target));
        displayTagInformation(target, false);

        if (_taskToPerform == WRITE_ISO15693) {

            qDebug() << "XXXX NfcWorker::handleIso15693TagEvent - Preparing to write to ISO15693 tag";
//			emit message(QString("Writing %1").arg(_data));
            emit message(QString("Test 2"));
            emit message(QString("Writing 0x 01 02 03 04"));

            int MAX_RESPONSE_SIZE = 10;

            // Allocate response buffer
            // (max size determined by application)
            uchar_t response[MAX_RESPONSE_SIZE];

            // command is:
            // Flags : 1 byte / 8 bits
            // command_code='21' : 1 byte / 8 bits
            // UID (optional) : 8 bytes / 64 bits
            // Block number : 1 byte / 8 bits
            // Data : Block length bytes. For my tag this is 4 bytes.
            // CRC16 : 16 bits per ISO/IEC 13239

            // our simple "write 0x01 02 03 04 bytes to block zero command is therefore:
            //
            // 00 21 00 01 02 03 04

//            a)	ALL data has to be prepared by client (including flags, CRC�)
//            b)	For "UID optional" � this means that client is not required to specify it but then client must indicate that it is not available in the "flags" field.

            // Write request and get response
            int clength = 7;
            uint8_t command[clength];
            QString cmd_hex = "00210001020304";
            Utilities::hexToIntArray(cmd_hex, command);

//			uint8_t test[4] = { 1, 2, 3, 4 };

            size_t rlength; // provided by NFC API

            emit message(QString("Writing %1").arg(cmd_hex));
            int rc = nfc_tag_transceive(target, TAG_TYPE_ISO_15693_3, command, clength, response, MAX_RESPONSE_SIZE, &rlength);

            if (rc == NFC_RESULT_SUCCESS) {
                QByteArray responseData = QByteArray::fromRawData(reinterpret_cast<const char *>(response), rlength);
                QString responseAsHex = QString::fromAscii(responseData.toHex());
                emit message(QString("APDU response: %1").arg(responseAsHex));
            } else {
                emit message(QString("ERROR transmitting APDU:%1").arg(rc));
                emit message(Utilities::getOperationResultName(rc));
            }
        }

        CHECK(nfc_destroy_target(target));
        CHECK(nfc_unregister_tag_readerwriter());

    } else {
        qDebug() << "XXXX NfcWorker::handleIso15693TagEvent - NFC BPS event that we didn't register for: " << code;
    }
}

void NfcWorker::handleGvbEvent(bps_event_t *event)
{
    uint16_t code = bps_event_get_code(event);
    qDebug() << "XXXX NfcWorker::handleGvbEvent - processing event code " << code;

    const int TRANSACTION_NUMBER = 2;

    //=============================================================================================
    //  Background Notes:
    //
    //  Typical form of a transaction record (16 Bytes)
    //  Bytes 0-7 are in clear andhave some useful information
    //  Bytes 8-15 are encrypted and it's unclear what they represent
    //
    //  E.g.
    //
    //  c8001002 0aab9f30 = 5fe9bacd 79961be0
    //
    //  On a Mifare UL card the last 2 transctions only are recorded
    //
    //  first 64 bits ( 2 x int ) are parsed as:
    //
    //  11001000 00000000 00010000 00000010 // 00001010 10101011 10011111 00110000 =
    //  11001 000000000000001 000000000010  // 000 01010101010111 00111110011 0000 =
    //  |     |               |                |   |              |           |
    //  |     |               |                |   |              |           +- Unknown Use
    //  |     |               |                |   |              +------------- Minutes from 00:00 hours
    //  |     |               |                |   +---------------------------- Days from epoch 1 Jan 1997
    //  |     |               |                +-------------------------------- Type (Purchase, Check-in, ...)
    //  |     |               +------------------------------------------------- Network (GVB, RET, ... )
    //  |     +----------------------------------------------------------------- Transaction number (1, 2, ... )
    //  +----------------------------------------------------------------------- Unknown use
    //
    //=============================================================================================

    typedef struct _Transaction
    {
        union
        {
            uchar_t pages[16];
            struct
            {
                unsigned int transdata0;
                unsigned int transdata1;
                unsigned int transdata2;
                unsigned int transdata3;
            };
        };
    } Transaction;

    Transaction transactions[TRANSACTION_NUMBER];

    nfc_event_t *nfcEvent;
    nfc_target_t* target;
    int rc;

    uint8_t commands[TRANSACTION_NUMBER][2] = { { 0x30, 0x04 }, // read block of 16 bytes from page  0x04
            { 0x30, 0x08 } // read block of 16 bytes from page  0x08
    };

    size_t rlength;

    if (NFC_TAG_READWRITE_EVENT == code) {
        qDebug() << "XXXX NfcWorker::handleGvbEvent - Target Read Write Event";

        CHECK(nfc_get_nfc_event(event, &nfcEvent));
        CHECK(nfc_get_target(nfcEvent, &target));

        if (_taskToPerform == READ_GVB) {
            qDebug() << "XXXX NfcWorker::handleGvbEvent - Preparing to read GVB tag";

            tag_variant_type_t variant;
            rc = nfc_get_tag_variant(target, &variant);

            if ((rc == NFC_RESULT_SUCCESS) && (variant == TAGVARIANT_MIFARE_UL)) {

                for (int i = 0; i < TRANSACTION_NUMBER; i++) {
                    rc = nfc_tag_transceive(target, TAG_TYPE_ISO_14443_3, commands[i], sizeof(commands[i]), transactions[i].pages, sizeof(transactions[i].pages), &rlength);

                    if (rc == NFC_RESULT_SUCCESS) {

                        QByteArray responseData = QByteArray::fromRawData(reinterpret_cast<const char *>(transactions[i].pages), rlength);
                        QString responseAsHex = QString::fromAscii(responseData.toHex());

                        qDebug() << "XXXX NfcWorker::handleGvbEvent - APDU response:" << responseAsHex;

                        /// painful since ints are different endian :-(

                        unsigned int counter = EXTRACT_CNTR(ENDIAN_RET32(transactions[i].transdata0));
                        unsigned int location = EXTRACT_LOCN(ENDIAN_RET32(transactions[i].transdata0));
                        unsigned int type = EXTRACT_TYPE(ENDIAN_RET32(transactions[i].transdata1));
                        unsigned int days = EXTRACT_DAYS(ENDIAN_RET32(transactions[i].transdata1));
                        unsigned int minutes = EXTRACT_MINS(ENDIAN_RET32(transactions[i].transdata1));

                        QDate epochDate(1997, 1, 1);

                        if (counter > 0) {
                            unsigned int hour = minutes / 60;
                            unsigned int mins = minutes % 60;

                            emit message(QString("Transaction number: %1").arg(counter));
                            emit message(QString("Date and time: %1 %2:%3").arg(epochDate.addDays(days).toString("d MMM yyyy")).arg(hour, 2, 10, QChar('0')).arg(mins, 2, 10, QChar('0')));

                            QString locText;

                            switch (location) {
                                case 2:
                                    locText = "Amsterdam (GVB)";
                                    break;
                                case 5:
                                    locText = "Rotterdam (RET)";
                                    break;
                                default:
                                    locText = "*UNKNOWN*";
                                    break;
                            }

                            emit message(QString("Transport Network: %1").arg(locText));

                            QString typeText;

                            switch (type) {
                                case 0:
                                    typeText = "Purchase";
                                    break;
                                case 1:
                                    typeText = "Check-in";
                                    break;
                                case 2:
                                    typeText = "Check-out";
                                    break;
                                case 3:
                                    typeText = "Transfer";
                                    break;
                                default:
                                    typeText = "*UNKNOWN*";
                                    break;
                            }
                            emit message(QString("Transaction type: %1").arg(typeText));
                        }
                    } else {
                        emit message(QString("ERROR transmitting APDU:%1").arg(rc));
                        emit message(Utilities::getOperationResultName(rc));
                    }
                }
            } else {
                qDebug() << "XXXX NfcWorker::handleGvbEvent - target must be MiFare UL";
                emit message(QString("Error - Target not MiFare UL"));
            }
        }

        CHECK(nfc_destroy_target(target));
        CHECK(nfc_unregister_tag_readerwriter());

    } else {
        qDebug() << "XXXX NfcWorker::handleGvbEvent - NFC BPS event that we didn't register for: " << code;
    }
}

void NfcWorker::handleReadTagDetailsEvent(bps_event_t *event)
{
    uint16_t code = bps_event_get_code(event);
    qDebug() << "XXXX NfcWorker::handleReadTagDetailsEvent - processing event code " << code;


    if (NFC_TAG_READWRITE_EVENT == code) {
        nfc_event_t *nfcEvent;
        nfc_target_t* target;
        qDebug() << "XXXX NfcWorker::handleReadTagDetailsEvent - Target Read Write Event";
        CHECK(nfc_get_nfc_event(event, &nfcEvent));
        CHECK(nfc_get_target(nfcEvent, &target));
        qDebug() << "XXXX displaying tag info";
        displayTagInformation(target, true);
        qDebug() << "XXXX done displaying tag info";
        CHECK(nfc_destroy_target(target));
    }

}

void NfcWorker::handleSendVcardEvent(bps_event_t *event)
{
    uint16_t code = bps_event_get_code(event);
    qDebug() << "XXXX NfcWorker::handleSendVcardEvent - processing event code " << code;

    nfc_event_t *nfcEvent;
    nfc_target_t* target;
    nfc_ndef_record_t* myNdefRecord;
    nfc_ndef_message_t* myNdefMessage;

    if (NFC_SNEP_CONNECTION_EVENT == code) {

        QString mimeType = QString("text/x-vCard");
        QString vCard =
                QString("BEGIN:VCARD\n").append("VERSION:3.0\n").append("N:").append(_last_name).append(";").append(_first_name).append("\n").append("FN:").append(_first_name).append(" ").append(
                        _last_name).append("\n").append("ADR;TYPE=WORK:").append(_address).append("\n").append("TEL;TYPE=CELL:").append(_mobile).append("\n").append("EMAIL;TYPE=INTERNET:").append(
                        _email).append("\n").append("END:VCARD");

        CHECK(nfc_get_nfc_event(event, &nfcEvent));
        CHECK(nfc_get_target(nfcEvent, &target));

        qDebug() << "XXXX NfcWorker::handleSendVcardEvent - Preparing to send vCard: FIRST NAME=" << _first_name << ", LAST NAME=" << _last_name << ", ADDRESS=" << _address << ", EMAIL=" << _email
                << ", MOBILE=" << _mobile;
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

    CHECK(nfc_unregister_snep_client());
    qDebug() << "XXXX SendVcard done";
}

void NfcWorker::handleLlcpEvent(bps_event_t *event)
{
    uint16_t code = bps_event_get_code(event);
    qDebug() << "XXXX NfcWorker::handleLlcpEvent - processing event code " << code;

    QByteArray llcp_bytes_received;
    QString llcp_message;

    nfc_event_t *nfcEvent;
    nfc_target_t* target;

    switch (code) {
        case NFC_LLCP_CONNECTION_EVENT:
            qDebug() << "XXXX NfcWorker::handleLlcpEvent - got NFC_LLCP_CONNECTION_EVENT";

            // this means the two devices have been brought together and an LLCP connection of some sort has been requested

            CHECK(nfc_get_nfc_event(event, &nfcEvent));
            CHECK(nfc_get_target(nfcEvent, &target));

            nfc_llcp_connection_listener_t* listener;
            listener = (nfc_llcp_connection_listener_t*) malloc(sizeof(nfc_llcp_connection_listener_t));

            // this gives us access to a handle relating to the llcp connection listener
            CHECK(nfc_llcp_get_connection_status(target, listener));

            qDebug() << "XXXX NfcWorker::handleLlcpEvent - LLCP target's listener=" << *listener;

            // what we'll do next is find out which of our client and server connections this event relates to
            qDebug() << "XXXX NfcWorker::handleLlcpEvent - LLCP client listener=" << *_llcp_client;
            qDebug() << "XXXX NfcWorker::handleLlcpEvent - LLCP server listener=" << *_llcp_server;

            if (*listener == *_llcp_client) {
                // the remote device connection is a client so expect it to send some data to us over this connection
                qDebug() << "XXXX NfcWorker::handleLlcpEvent - requesting read of LLCP client connection";
                // max data we can read is the maximum information unit (MIU)
                unsigned int* miu;
                miu = (unsigned int*) malloc(sizeof(unsigned int*));
                CHECK(nfc_llcp_get_remote_miu(target, miu));
                // this incidates we want data to be read for us. We'll get a NFC_LLCP_READ_COMPLETE_EVENT when this has been done and be able to retrieve the result of the read operation at that point
                CHECK(nfc_llcp_read(target, *miu));
                emit logMessageHere("Ready to receive data over LLCP");
                free (miu);
            } else {
                if (*listener == *_llcp_server) {
                    // the remote device connection is a server so we can send text to the other device here if we want to
                    if (_llcp_send) {
                        qDebug() << "XXXX NfcWorker::handleLlcpEvent - writing to remote LLCP server:" << _llcp_text;
                        QByteArray llcp_bytes = _llcp_text.toAscii();
                        int llcp_length = llcp_bytes.length();
                        uchar_t* llcp_data = reinterpret_cast<uchar_t*>(llcp_bytes.data());
                        nfc_llcp_write(target, llcp_data, llcp_length);
                        emit logMessageHere(QString("SEND: %1").arg(_llcp_text));
                        clearLlcpSendData();
                    }
                }
            }
            qDebug() << "XXXX Destroy NFC Target";
            CHECK(nfc_destroy_target(target));
            free (listener);
            break;
        case NFC_LLCP_READ_COMPLETE_EVENT:
            qDebug() << "XXXX NfcWorker::handleLlcpEvent - got NFC_LLCP_READ_COMPLETE_EVENT";
            // we asked for data to be read from the LLCP server connection and this event indicates that this has been done. Now we collect the result.
            CHECK(nfc_get_nfc_event(event, &nfcEvent));
            CHECK(nfc_get_target(nfcEvent, &target));
            unsigned int* miu;
            miu = (unsigned int*) malloc(sizeof(unsigned int*));
            CHECK(nfc_llcp_get_remote_miu(target, miu));
            size_t buffer_length;
            buffer_length = *miu;
            size_t* llcp_bytes_read;
            llcp_bytes_read = (size_t*) malloc(sizeof(size_t*));
            uchar_t* buffer;
            buffer = (uchar_t*) malloc(sizeof(uchar_t*) * *miu);
            // here's where we copy the result into a buffer
            CHECK(nfc_llcp_get_read_result(target, buffer, buffer_length, llcp_bytes_read));
            llcp_bytes_received = QByteArray(reinterpret_cast<char*>(buffer));
            llcp_message = QString(llcp_bytes_received);
            qDebug() << "XXXX NfcWorker::handleLlcpEvent - received message " << llcp_message;
            emit logMessageHere(QString("RECV: %1").arg(llcp_message));
            qDebug() << "XXXX Destroy NFC Target";
            CHECK(nfc_destroy_target(target));
            free (miu);
            free (llcp_bytes_read);
            free (buffer);
            break;
        case NFC_TARGET_LOST_EVENT:
            qDebug() << "XXXX NfcWorker::handleLlcpEvent - got NFC_TARGET_LOST_EVENT";
            break;
        default:
            qDebug() << "XXXX NfcWorker::handleLlcpEvent - NFC BPS event that we didn't expect: " << code;
    }

    qDebug() << "XXXX handleLlcpEvent done";
}

void NfcWorker::handleEmulateNfcEvent(bps_event_t *event)
{
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

void NfcWorker::handleEmulateEchoEvent(bps_event_t *event)
{
    uint16_t code = bps_event_get_code(event);
    nfc_event_t *nfcEvent;
    nfc_target_t* target;

    qDebug() << "XXXX NfcWorker::handleEmulateEchoEvent - processing event code: " << code;

    if (NFC_ISO14443_4_EVENT_CODE_EVENT == code) {
        qDebug() << "XXXX Echo emulation: selected by reader";
        emit message("Selected by reader");

    } else if (NFC_ISO14443_4_COMMAND_EVENT == code) {
        qDebug() << "XXXX Echo emulation: command received";
        CHECK(nfc_get_nfc_event(event, &nfcEvent));
        CHECK(nfc_get_target(nfcEvent, &target));
        processIso144434EchoCommandEvent(target);
    }
    qDebug() << "XXXX Emulate done";
}

void NfcWorker::processIso144434EchoCommandEvent(nfc_target_t *target)
{
    unsigned char command[NFC_ISO14443_4_COMMAND_BUFFER_LENGTH];
    size_t command_length = 0;
    nfc_result_t result;
    int i;

    if (!target) {
        qDebug() << "processIso144434CommandEvent has null target!";
        emit message("processIso144434CommandEvent has null target!");
        return;
    }

    memset(command, 0, NFC_ISO14443_4_COMMAND_BUFFER_LENGTH);

    qDebug() << "Received Iso14443-4 Command message";
    result = nfc_get_iso14443_4_emulation_command(target, command, NFC_ISO14443_4_COMMAND_BUFFER_LENGTH, &command_length);

    if (result != NFC_RESULT_SUCCESS) {
        qDebug() << QString("nfc_get_iso14443_4_emulation_command failed: %d").arg(result);
        emit message(QString("nfc_get_iso14443_4_emulation_command failed: %d").arg(result));
        return;
    }

    qDebug() << "XXXX command_length=" << command_length;
    QByteArray commandData = QByteArray::fromRawData(reinterpret_cast<const char *>(command), command_length);
    QString commandAsHex = QString::fromAscii(commandData.toHex());
    qDebug() << "XXXX command=" << commandAsHex;

    if (Utilities::isSameCharArray(command, FILTER_APDU_0, FILTER_APDU_LENGTHS[0])) {
        qDebug() << "XXXX filtering unwanted APDU command";
        result = nfc_send_iso14443_4_emulation_command_response(RESPONSE_OK, 2);
        return;
    }
    if (Utilities::isSameCharArray(command, FILTER_APDU_1, FILTER_APDU_LENGTHS[1])) {
        qDebug() << "XXXX filtering unwanted APDU command";
        result = nfc_send_iso14443_4_emulation_command_response(RESPONSE_OK, 2);
        return;
    }
    if (Utilities::isSameCharArray(command, FILTER_APDU_2, FILTER_APDU_LENGTHS[2])) {
        qDebug() << "XXXX filtering unwanted APDU command";
        result = nfc_send_iso14443_4_emulation_command_response(RESPONSE_OK, 2);
        return;
    }
    if (Utilities::isSameCharArray(command, FILTER_APDU_3, FILTER_APDU_LENGTHS[3])) {
        qDebug() << "XXXX filtering unwanted APDU command";
        result = nfc_send_iso14443_4_emulation_command_response(RESPONSE_OK, 2);
        return;
    }
    if (Utilities::isSameCharArray(command, FILTER_APDU_4, FILTER_APDU_LENGTHS[4])) {
        qDebug() << "XXXX filtering unwanted APDU command";
        result = nfc_send_iso14443_4_emulation_command_response(RESPONSE_OK, 2);
        return;
    }

    emit message(QString("Command APDU : %1 len=%2").arg(commandAsHex).arg(command_length));

// Return response which is same as command APDU (echo!)

    size_t response_length = command_length + 2;
    unsigned char response[response_length];

    for (i = 0; i < command_length; i++) {
        response[i] = command[i];
    }
    response[i] = 0x90;
    i++;
    response[i] = 0x00;
    QByteArray responseData = QByteArray::fromRawData(reinterpret_cast<const char *>(response), response_length);
    QString responseAsHex = QString::fromAscii(responseData.toHex());

    emit message(QString("Response APDU : %1").arg(responseAsHex));
    result = nfc_send_iso14443_4_emulation_command_response(response, response_length);
    if (result != NFC_RESULT_SUCCESS) {
        qDebug() << QString("nfc_send_iso14443_4_emulation_command_response response: %d").arg(result);
        emit message(QString("nfc_send_iso14443_4_emulation_command_response response: %d").arg(result));
    }
}

nfc_ndef_record_t* NfcWorker::makeMediaRecord(QString type, QString text)
{
    qDebug() << "XXXX In make media record";

    nfc_ndef_record_t* record = 0;
    int textLen = text.length();
    int totalLen = textLen;
    uchar_t payload[totalLen];
    int offset = 0;
    memcpy(&payload[offset], text.toUtf8().constData(), textLen);

    qDebug() << "XXXX Creating NDEF Record";
    CHECK(nfc_create_ndef_record(NDEF_TNF_MEDIA, type.toUtf8().constData(), payload, totalLen, 0, &record));

    qDebug() << "XXXX Leaving make media record";
    return record;
}

void NfcWorker::parseNdefMessage(nfc_ndef_message_t *ndefMessage)
{

    int ndefMsgCount = 0;

    ndefMsgCount = 1;

    for (int ndefMsgIndex = 0; ndefMsgIndex < ndefMsgCount; ++ndefMsgIndex) {

        qDebug() << "XXXX Processing NDEF Message index: " << ndefMsgIndex;

        unsigned int ndefRecordCount = 0;
        CHECK(nfc_get_ndef_record_count(ndefMessage, &ndefRecordCount));

        for (unsigned int ndefRecordIndex = 0; ndefRecordIndex < ndefRecordCount; ++ndefRecordIndex) {

            qDebug() << "XXXX Processing NDEF Record index: " << ndefRecordIndex;
            nfc_ndef_record_t *ndefRecord;
            CHECK(nfc_get_ndef_record(ndefMessage, ndefRecordIndex, &ndefRecord));

            uchar_t* ndefRecordPayloadData;
            size_t ndefRecordPayloadLength;
            CHECK(nfc_get_ndef_record_payload(ndefRecord, &ndefRecordPayloadData, &ndefRecordPayloadLength));

            tnf_type_t ndefRecordTnf;
            CHECK(nfc_get_ndef_record_tnf(ndefRecord, &ndefRecordTnf));

            char *ndefRecordNdefType;
            CHECK(nfc_get_ndef_record_type(ndefRecord, &ndefRecordNdefType));

            char *ndefRecordIdentifier;
            CHECK(nfc_get_ndef_record_id(ndefRecord, &ndefRecordIdentifier));

            QString ndefRecordPayloadAsHex = QString::fromAscii(reinterpret_cast<const char *>(ndefRecordPayloadData), ndefRecordPayloadLength).toAscii().toHex();

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
                CHECK(nfc_get_sp_title(ndefRecord, Settings::LANG_EN, &utf_title, &found_lang, &rtd_encoding, true));

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
nfc_ndef_record_t* NfcWorker::makeUriRecord(uchar_t prefix, QString uri)
{
    qDebug() << QString("makeUriRecord URI: %1").arg(uri);
    nfc_ndef_record_t* record = 0;
    int len = uri.length();
    uchar_t payload[len + 1];
    payload[0] = prefix;
    memcpy(&payload[1], uri.toUtf8().constData(), len);
    CHECK(nfc_create_ndef_record(NDEF_TNF_WELL_KNOWN, Settings::NfcRtdUri, payload, len + 1, 0, &record));
    return record;
}

/*
 * Build an NDEF Text Record
 *
 */
nfc_ndef_record_t* NfcWorker::makeTextRecord(QString language, QString text)
{
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

    CHECK(nfc_create_ndef_record(NDEF_TNF_WELL_KNOWN, Settings::NfcRtdText, payload, totalLen, 0, &record));
    return record;
}

/*
 * Build an NDEF Sp Record to which we will add a Uri record and one or more Text/Title records later
 *
 */
nfc_ndef_record_t* NfcWorker::makeSpRecord()
{
    qDebug() << "XXXX NfcWorker::makeSpRecord";
    nfc_ndef_record_t* record = 0;
    uchar_t payload[0];
    CHECK(nfc_create_ndef_record(NDEF_TNF_WELL_KNOWN, Settings::NfcRtdSmartPoster, payload, 0, 0, &record));
    return record;
}

/*
 * Build an NDEF Text Record
 *
 */
nfc_ndef_record_t* NfcWorker::makeCustomRecord(QString domain, QString type, QString text)
{
    qDebug() << QString("makeCustomRecord DOMAIN: %1 TYPE: %2 TEXT %3").arg(domain).arg(type).arg(text);
    nfc_ndef_record_t* record = 0;

    int textLen = text.toUtf8().length();
    QString domain_plus_type = domain.append(":");
    domain_plus_type = domain_plus_type.append(type);
    int totalLen = textLen;

    uchar_t payload[totalLen];

    int offset = 0;
    memcpy(&payload[offset], text.toUtf8().constData(), textLen);

    CHECK(nfc_create_ndef_record(NDEF_TNF_EXTERNAL, domain_plus_type.toUtf8().constData(), payload, totalLen, 0, &record));
    return record;
}

unsigned long NfcWorker::getSysTimeMs()
{
    timeval theTime;
    gettimeofday(&theTime, 0);
    return (theTime.tv_sec * 1000) + (theTime.tv_usec / 1000);
}

void NfcWorker::displayTagInformation(nfc_target_t* target, bool display_ndef_details)
{

    struct TagPropertyMap
    {
        target_property_type_t property;
        const char *name;
    } tagPropertyMap[] = { { TAG_PROP_ISO_14443_3_TYPE, "ISO_14443_3 TYPE" }, { TAG_PROP_ISO_14443_4_TYPE, "ISO_14443_4 TYPE" }, { TAG_PROP_NDEF_TYPE, "NDEF TYPE" }, { TAG_PROP_NDEF_LOCKED,
            "NDEF LOCKED" }, { TAG_PROP_NDEF_LOCKABLE, "NDEF LOCKABLE" }, { TAG_PROP_NDEF_FREE_SPACE, "NDEF FREE SPACE" }, { TAG_PROP_NDEF_CAPABLE_TYPE, "NDEF CAPABLE TYPE" }, {
            TAG_PROP_NDEF_CAPABLE_TAG_SIZE, "NDEF CAPABLE TAG SIZE" }, { TAG_PROP_ISO_15693_3_TYPE, "ISO_15693_3 TYPE" }, { TAG_PROP_AFI_SUPPORTED, "AFI SUPPORTED" }, { TAG_PROP_AFI_LOCKED,
            "AFI LOCKED" }, { TAG_PROP_DSFID_SUPPORTED, "DSFID SUPPORTED" }, { TAG_PROP_DSFID_LOCKED, "DSFID LOCKED" }, { TAG_PROP_AFI, "AFI" }, { TAG_PROP_DSFID, "DSFID" }, { TAG_PROP_SECTOR_SIZE,
            "SECTOR SIZE" }, { TAG_PROP_SECTOR_NUMBER, "SECTOR NUMBER" }, { TAG_PROP_KOVIO_TYPE, "KOVIO TYPE" } };

    struct TagVariantMap
    {
        tag_variant_type_t variant;
        const char *name;
    } tagVariantMap[] = { { TAGVARIANT_UNKNOWN, "UNKNOWN" }, { TAGVARIANT_JEWEL, "JEWEL" }, { TAGVARIANT_TOPAZ, "TOPAZ" }, { TAGVARIANT_TOPAZ_512, "TOPAZ 512" }, { TAGVARIANT_MIFARE_UL, "MIFARE UL" },
            { TAGVARIANT_MIFARE_UL_C, "MIFARE UL C" }, { TAGVARIANT_DESFIRE_D40, "DESFIRE D40" }, { TAGVARIANT_DESFIRE_EV1_2K, "DESFIRE EV1 2K" }, { TAGVARIANT_DESFIRE_EV1_4K, "DESFIRE EV1 4K" }, {
                    TAGVARIANT_DESFIRE_EV1_8K, "DESFIRE EV1 8K" }, { TAGVARIANT_TI_TAGIT, "TI TAGIT" }, { TAGVARIANT_ST_LRI_512, "ST LRI 512" }, { TAGVARIANT_ST_LRI_2K, "ST LRI 2K" }, {
                    TAGVARIANT_NXP_ICODE, "NXP ICODE" }, { TAGVARIANT_KOVIO, "KOVIO" }, { TAGVARIANT_MIFARE_1K, "MIFARE 1K" }, { TAGVARIANT_MIFARE_4K, "MIFARE 4K" }, { TAGVARIANT_MIFARE_MINI,
                    "MIFARE MINI" }, { TAGVARIANT_MY_D_MOVE, "MY D MOVE" }, { TAGVARIANT_MY_D_NFC, "MY D NFC" } };

    nfc_result_t rc;
    char buffer[100];
    uchar_t id[20];
    size_t nameLength = 0;
    size_t idLength = 0;
    tag_variant_type_t variant;

    for (uint_t i = 0; i < (sizeof(tagPropertyMap) / sizeof(struct TagPropertyMap)); i++) {
        qDebug() << "XXXX nfc_get_tag_property:" << tagPropertyMap[i].name;
        nfc_result_t rc = nfc_get_tag_property(target, tagPropertyMap[i].property, &buffer[0], sizeof buffer);
        if (rc == NFC_RESULT_SUCCESS) {
            if (strlen(&buffer[0]) > 0) {
                if (strcmp(tagPropertyMap[i].name, "NDEF TYPE") != 0) {
                    qDebug() << "XXXX" << tagPropertyMap[i].name << buffer << endl;
                    emit message(QString("%1: %2").arg(tagPropertyMap[i].name).arg(&buffer[0]));
                } else {
                    qDebug() << "XXXX NFC FORUM TYPE" << buffer << endl;
                    emit message(QString("%1: %2").arg("NFC FORUM TYPE").arg(&buffer[0]));
                }
            }
        } else {
            qDebug() << "XXXX nfc_get_tag_property() for" << tagPropertyMap[i].name << "returned" << rc << endl;
        }
    }

    rc = nfc_get_tag_name(target, &buffer[0], sizeof buffer, &nameLength);
    if (rc == NFC_RESULT_SUCCESS) {
        QString tagName = QString::fromAscii(&buffer[0], nameLength);
        qDebug() << "XXXX Tag Name" << tagName << endl;
        if (nameLength > 0) {
            emit message(QString("Tag name: %1").arg(tagName));
        }
    } else {
        qDebug() << "XXXX nfc_get_tag_name() returned" << rc << endl;
    }

    rc = nfc_get_tag_id(target, &id[0], sizeof id, &idLength);
    if (rc == NFC_RESULT_SUCCESS) {
        QByteArray uId = QByteArray::fromRawData(reinterpret_cast<const char *>(&id[0]), idLength);
        qDebug() << "XXXX Tag id" << uId.toHex() << endl;
        emit message(QString("Tag id: %1").arg(QString::fromAscii(uId.toHex())));
    } else {
        qDebug() << "XXXX nfc_get_tag_id() returned" << rc << endl;
    }

    rc = nfc_get_tag_variant(target, &variant);
    if (rc == NFC_RESULT_SUCCESS) {
        for (uint_t i = 0; i < (sizeof(tagVariantMap) / sizeof(struct TagVariantMap)); i++) {
            if (variant == tagVariantMap[i].variant) {
                qDebug() << "XXXX Tag variant" << tagVariantMap[i].name << endl;
                emit message(QString("Tag variant: %1").arg(tagVariantMap[i].name));
            }
        }
    } else {
        qDebug() << "XXXX nfc_get_tag_variant() returned" << rc << endl;
    }

// try to establish the NDEF content

    if (display_ndef_details) {
        unsigned int ndefMsgCount = 0;
        CHECK(nfc_get_ndef_message_count(target, &ndefMsgCount));
        qDebug() << "XXXX NfcWorker::handleNfcReadNdefTagEvent - target message count=" << ndefMsgCount;

        for (unsigned int ndefMsgIndex = 0; ndefMsgIndex < ndefMsgCount; ++ndefMsgIndex) {

            qDebug() << "XXXX Processing NDEF Message index: " << ndefMsgIndex;

            nfc_ndef_message_t *ndefMessage;
            CHECK(nfc_get_ndef_message(target, ndefMsgIndex, &ndefMessage));

            parseNdefMessage(ndefMessage);

        }
    }

}

void NfcWorker::setLlcpSendFlag()
{
    _llcp_send = true;
    emit logMessageHere("Ready to send message...");
}

void NfcWorker::clearLlcpSendData()
{
    _llcp_send = false;
    _llcp_text = "";
}

void NfcWorker::setLlcpSendText(QString text)
{
    _llcp_text = text;
}
