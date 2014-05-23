/* Copyright (c) 2012, 2013 BlackBerry Limited.
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

#include "MacAddressHandler.hpp"

#include <QtCore/QDebug>

#include <bps/bps.h>

#include <QtNfcSubset/qndefmessage.h>

//! [0]
MacAddressHandler::MacAddressHandler(QObject *parent) :
        QObject(parent), m_macAddress(tr("<not set>")), mRunning(false)
{
    bps_initialize();
    setRunning(true);
}
//! [0]

MacAddressHandler::~MacAddressHandler()
{
    nfc_unregister_handover_listener();
    nfc_stop_events();
    unsubscribe(nfc_get_domain());
    bps_shutdown();
}

QString MacAddressHandler::macAddress() const
{
    return m_macAddress;
}

void MacAddressHandler::event(bps_event_t *event)
{
    uint16_t code = bps_event_get_code(event);
    nfc_event_t *nfc_event = 0;

    if (nfc_get_nfc_event(event, &nfc_event) != BPS_SUCCESS) {
        qDebug() << "[ERRO] Get NFC event: BPS_FAILURE" << endl;
    }

    nfc_target_t *target = 0;
    nfc_get_target(nfc_event, &target);

    switch (code) {
        case NFC_TARGET_LOST_EVENT: {
            qDebug() << "[INFO] Received NFC_TARGET_LOST_EVENT" << endl;
            emit lostEvent();
        }
            break;
        case NFC_HANDOVER_DETECTED_EVENT: {
            qDebug() << "[INFO] Received NFC_HANDOVER_DETECTED_EVENT" << endl;
            handleNfcHandoverDetectedEvent(target);
        }
            break;
        case NFC_HANDOVER_COMPLETE_EVENT: {
            handleNfcHandoverCompletedEvent(target);
            qDebug() << "[INFO] Received NFC_HANDOVER_COMPLETE_EVENT" << endl;
        }
            break;
        default: {
            qDebug() << "[WARN] Event not handled: " << code << endl;
        }
            break;
    }

    nfc_destroy_target(target);
}

//! [1]
void MacAddressHandler::handleNfcHandoverDetectedEvent(nfc_target_t *target)
{
    nfc_result_t result = nfc_confirm_handover_process(target, true);
    qDebug() << "[INFO] Confirmed Handover " << result <<  endl;
    emit handOver();
}

bool MacAddressHandler::running()
{
    return mRunning;
}

void MacAddressHandler::setRunning(bool running)
{
    if (running != mRunning) {
        mRunning = running;

        // TODO: Check stability of this code, we want to stop listening for nfc in some cases.
        if (mRunning) {
            // Listen to NFC events.
            subscribe(nfc_get_domain());

            const int rc = nfc_request_events();
            if (rc == NFC_RESULT_SUCCESS) {
                qDebug() << "[INFO] Request NFC Events: NFC_RESULT_SUCCESS" << endl;
            } else {
                nfc_stop_events();
                unsubscribe(nfc_get_domain());
                bps_shutdown();
                qDebug() << "[ERRO] Request NFC Events: NFC_RESULT_FAILURE" << endl;
            }

            nfc_register_handover_listener(BLUETOOTH_HANDOVER);
        } else {
            // Don't listen to NFC events.
            nfc_unregister_handover_listener();
            nfc_stop_events();
            unsubscribe(nfc_get_domain());
        }

        emit runningChanged(mRunning);
    }
}

void MacAddressHandler::handleNfcHandoverCompletedEvent(nfc_target_t *target)
{
    unsigned int messageCount = 0;
    nfc_get_ndef_message_count(target, &messageCount);

    if (messageCount > 0) {
        nfc_ndef_message_t *ndefMessage = 0;
        nfc_get_ndef_message(target, 0, &ndefMessage);
        nfc_ndef_record_t *record = 0;
        nfc_get_ndef_record(ndefMessage, 0, &record);

        // The MAC address is in little-endian order
        char *macAddress = 0;

        nfc_get_handover_bluetooth_mac_address(record, &macAddress);

        m_macAddress = QString(macAddress);

        emit macAddressChanged(m_macAddress);
        qDebug() << "[INFO] MAC ADDRESS: " << m_macAddress;
    } else {
        qWarning() << "[ERRO] No NdefMessage's found";
    }
}
//! [1]
