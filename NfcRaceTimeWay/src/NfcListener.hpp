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

#ifndef NFCLISTENER_H
#define NFCLISTENER_H

#define CHECK(rc) NfcListener::checkReturnCode((rc), __LINE__, __FILE__, __PRETTY_FUNCTION__)

#include <QtCore/QObject>
#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>
#include <QtNfcSubset/qndefmessage.h>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <bps/bps.h>
#include <nfc/nfc.h>
#include <nfc/nfc_ndef.h>

#include <bb/core/AbstractBpsEventHandler.hpp>

#include <bps/navigator.h>
#include <bps/navigator_invoke.h>

/*!
 * @brief Application GUI object
 */
class NfcListener : public QObject, public bb::AbstractBpsEventHandler
{
    Q_OBJECT

public:
    NfcListener();
	virtual ~NfcListener();
	void interruptBpsWaitLoop(unsigned int code);

private:
	static void checkReturnCode(int rc, int line, const char *file, const char *func);
	bool _failedToInitialize;
	nfc_ndef_message_t *_emulateNdefMessage;
	void handleNdefRequest(const QtMobilitySubset::QNdefMessage message);
	void handleNfcEvent(bps_event_t *event);
	nfc_ndef_record_t* makeCustomRecord(QString domain, QString type, QString content);
	void initialize();
	void listen();
	void terminate();
	void event(bps_event_t *event);

public slots:
	void startTagEmulation(const QString &tagData);
    void updateTagEmulation(const QString &tagData);
    void stopTagEmulation();
	void startListening();
	void stopListening();
	void receivedInvokeRequest(const bb::system::InvokeRequest& request);

signals:
	void raceTagDetected(const QVariant &content);
	void tagEmulationSelectEvent();
	void tagEmulationReadEvent();
	void tagEmulationLeftEvent();
};

#endif // ifndef NFCLISTENER_H
