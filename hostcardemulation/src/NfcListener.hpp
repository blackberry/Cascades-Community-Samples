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

#ifndef NFCLISTENER_H
#define NFCLISTENER_H

#include "Utilities.hpp"

#include <QObject>
#include <errno.h>
#include <bps/bps.h>
#include <nfc/nfc.h>
#include <bb/core/AbstractBpsEventHandler.hpp>
#include <bb/system/InvokeManager>

#define HCE_INVOKE_AID_SELECTED "com.example.HostCardEmulation.AID_SELECTED"

#define COMPARE(__c, __p) \
    Utilities::isSameCharArray(__c, __p, sizeof(__p)/sizeof(__p[0]))

#define RESPOND(__c, __l, __m, __r) \
    do { \
        qDebug() << "XXXX HCE: command_length=" << __l << endl; \
        QByteArray commandData = QByteArray::fromRawData(reinterpret_cast<const char *>(__c), __l); \
        QString commandAsHex = QString::fromAscii(commandData.toHex()); \
        qDebug() << "XXXX HCE: command=" << commandAsHex << endl; \
        if (__m) emit message(QString("Command APDU : %1 len=%2").arg(commandAsHex).arg(__l)); \
        QByteArray responseData = QByteArray::fromRawData(reinterpret_cast<const char *>(__r), sizeof(__r)/sizeof(__r[0])); \
        QString responseAsHex = QString::fromAscii(responseData.toHex()); \
        if (__m) emit message(QString("Response APDU : %1").arg(responseAsHex)); \
        int result = nfc_send_iso14443_4_emulation_command_response(__r, sizeof(__r)/sizeof(__r[0])); \
        if (result != NFC_RESULT_SUCCESS) { \
            qDebug() << QString("XXXX HCE: nfc_send_iso14443_4_emulation_command_response response: %d").arg(result) << endl; \
            if (__m) emit message(QString("nfc_send_iso14443_4_emulation_command_response response: %d").arg(result)); \
        } else { \
            qDebug() << "XXXX Response sent successfully" << endl; \
            if (__m) emit message("Response Sent Successfully"); \
        } \
    } while(0)


#define AID_SETTING_KEY "AID"
#define AID_DEFAULT_VALUE "F00012345210"

class NfcListener
    : public QObject
    , public bb::AbstractBpsEventHandler
{
    Q_OBJECT

public:
    NfcListener(QObject *obj = 0);
	virtual ~NfcListener();
	bool initialised();
    void setAid(const QString& aid);
    void startListening();
    void stopListening();

public slots:
    void onRegisterAid();
    void onUnregisterAid();
    void onStartEmulating();
    void onStopEmulating();

private slots:
    void onInvoked(const bb::system::InvokeRequest &);

signals:
    void emulationStarted();
    void emulationStopped();
    void aidRegistered(const QVariant &text);
    void aidUnregistered(const QVariant &text);
    void message(const QVariant &text);
    void featureSetSupported(const QVariant &featureSet);

private:
    bool _initialised;
    void handleNfcEvent(bps_event_t *event);
    void initialise();
    void terminate();
    void event(bps_event_t *event);
    void processIso144434CommandEvent(nfc_target_t *target);
    bool isAidRegistered();

    QString _aid;
    bb::system::InvokeManager *_invokeManager;
    int _featureSet;
    bool _gotGpo;
};

#endif // ifndef NFCLISTENER_H
