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

#define EMIT_APDU_PRINTS false
#define AID_SETTING_KEY "AID"
#define PPSE_SETTING_KEY "PPSE"
#define AID_EMV_PSE                     "315041592E5359532E4444463031" // 1PAY.SYS.DDF01
#define AID_EMV_PPSE                    "325041592E5359532E4444463031" // 2PAY.SYS.DDF01
#define AID_UNION_PAY_DEBIT             "A000000333010101"
#define AID_UNION_PAY_CREDIT            "A000000333010102"
#define AID_UNION_PAY_QUASI_CREDIT      "A000000333010103"
#define AID_UNION_PAY_ELECTRONIC_CASH   "A000000333010106"
#define AID_VISA_CREDIT_DEBIT           "A0000000031010"
#define AID_VISA_ELECTRON               "A0000000032010"
#define AID_VISA_PAY                    "A0000000032020"
#define AID_VISA_PLUS                   "A0000000038010"
#define AID_MASTERCARD_CREDIT_DEBIT     "A0000000041010"
#define AID_MASTERCARD_PAYPASS          "A0000000049999"
#define AID_MASTERCARD_MAESTRO          "A0000000043060"
#define AID_MASTERCARD_CIRRUS           "A0000000046000"
#define AID_MASTERCARD_MAESTRO_UK       "A0000000050001"
#define AID_AMERICAN_EXPRESS            "A00000002501"
#define AID_JAPAN_CREDIT_BUREAU         "A0000000651010"
#define AID_ZKA_GERMANY                 "A0000003591010028001"
#define AID_EAPS_BANCOMAT_ITALY         "A00000035910100380"
#define AID_DISCOVER                    "A0000003241010"
#define AID_INTERAC_CANADA              "A0000002771010"
#define AID_SPAN2_SAUDI_ARABIA          "A00000022820101010"
#define AID_CB_FRANCE_CREDIT            "A0000000421010"
#define AID_CB_FRANCE_DEBIT             "A0000000422010"
#define AID_DANKORT_DENMARK_DEBIT       "A0000001211010"
#define AID_DINERS_CLUB                 "A0000001523010"
#define AID_BANRISUL_BRAZIL             "A0000001544442"
#define AID_RUPAY_INDIA                 "A0000005241010"

#define AID_DEFAULT_VALUE AID_MASTERCARD_CREDIT_DEBIT
#define PPSE_DEFAULT_VALUE AID_EMV_PPSE


#define COMPARE(__c, __p) \
    Utilities::isSameCharArray(__c, __p, sizeof(__p)/sizeof(__p[0]))

#define SEND_RESPONSE(__r, __m) \
    do { \
        QByteArray responseData = QByteArray::fromRawData(reinterpret_cast<const char *>(__r), sizeof(__r)/sizeof(__r[0])); \
        QString responseAsHex = QString::fromAscii(responseData.toHex()); \
        int result = nfc_send_iso14443_4_emulation_command_response(__r, sizeof(__r)/sizeof(__r[0])); \
        if (result != NFC_RESULT_SUCCESS) { \
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:Response APDU: " << responseAsHex << "send failed" << endl; \
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:nfc_send_iso14443_4_emulation_command_response result=" << result << endl; \
            if (__m) emit message(QString("Response APDU: %1 send failed").arg(responseAsHex)); \
            if (__m) emit message(QString("nfc_send_iso14443_4_emulation_command_response result=%d").arg(result)); \
        } else { \
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:Response APDU: " << responseAsHex << "sent successfully" << endl; \
            if (__m) emit message(QString("Response APDU: %1 sent successfully").arg(responseAsHex)); \
        } \
    } while(0)

#define RESPOND(__c, __l, __m, __r) \
    do { \
        QByteArray commandData = QByteArray::fromRawData(reinterpret_cast<const char *>(__c), __l); \
        QString commandAsHex = QString::fromAscii(commandData.toHex()); \
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:Command  APDU: " << commandAsHex << ", Len=" << __l << endl; \
        if (__m) emit message(QString("Command  APDU: %1, len=%2").arg(commandAsHex).arg(__l)); \
        SEND_RESPONSE(__r, __m); \
    } while(0)

#define SEND_RESPONSE1(__r, __m) \
    do { \
        QString responseAsHex = QString::fromAscii(__r.toHex());; \
        int result = nfc_send_iso14443_4_emulation_command_response((uchar_t*) apduOut.constData(), apduOut.size()); \
        if (result != NFC_RESULT_SUCCESS) { \
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:Response APDU: " << responseAsHex << "send failed" << endl; \
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:nfc_send_iso14443_4_emulation_command_response result=" << result << endl; \
            if (__m) emit message(QString("Response APDU: %1 send failed").arg(responseAsHex)); \
            if (__m) emit message(QString("nfc_send_iso14443_4_emulation_command_response result=%d").arg(result)); \
        } else { \
            qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:Response APDU: " << responseAsHex << "sent successfully" << endl; \
            if (__m) emit message(QString("Response APDU: %1 sent successfully").arg(responseAsHex)); \
        } \
    } while(0)

#define RESPOND1(__c, __l, __m, __r) \
    do { \
        QByteArray commandData = QByteArray::fromRawData(reinterpret_cast<const char *>(__c), __l); \
        QString commandAsHex = QString::fromAscii(commandData.toHex()); \
        qDebug() << "XXXX HCE: NfcListener::processIso144434CommandEvent:Command  APDU: " << commandAsHex << ", Len=" << __l << endl; \
        if (__m) emit message(QString("Command  APDU: %1, len=%2").arg(commandAsHex).arg(__l)); \
        SEND_RESPONSE1(__r, __m); \
    } while(0)


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
    void setPPSE(const QString& aid);
    void startListening();
    void stopListening();
    bool isAidRegistered();
    bool isPPSERegistered();

public slots:
    void onRegisterAid();
    void onUnregisterAid();
    void onRegisterPPSE();
    void onUnregisterPPSE();
    void onStartEmulating();
    void onStopEmulating();

private slots:
    void onInvoked(const bb::system::InvokeRequest &);

signals:
    void emulationStarted();
    void emulationStopped();
    void aidRegistered(const QVariant &text);
    void aidUnregistered(const QVariant &text);
    void ppseRegistered(const QVariant &text);
    void ppseUnregistered(const QVariant &text);
    void message(const QVariant &text);
    void featureSetSupported(const QVariant &featureSet);

private:
    bool _initialised;
    void handleNfcEvent(bps_event_t *event);
    void initialise();
    void terminate();
    void event(bps_event_t *event);
    void processIso144434CodeEvent(nfc_event_t *nfcEvent);
    void processIso144434CommandEvent(nfc_target_t *target);
    void appendFCI(QByteArray& apduOut, const QString& ppseAid, const QStringList& paymentAIDs);
    void appendSw1Sw2(QByteArray& buffer, uint16_t sw1sw2);
    bool isGpoCommand(unsigned char *apdu);
    bool isMCGpoCommand(unsigned char* apdu);
    QString getNfcEventName(int event_code);
    QStringList getActivePaymentAIDs();
    QString getAIDFomSelect(const uchar_t* command, size_t commandLength );
    QString hexToString(const QByteArray& array);
    QString hexToString(const uchar_t* command, size_t commandLength);
    QString _aid;
    QString _ppse;
    bb::system::InvokeManager *_invokeManager;
    int _featureSet;
    bool _gotGpo;
};

#endif // ifndef NFCLISTENER_H
