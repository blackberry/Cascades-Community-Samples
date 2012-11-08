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
#ifndef NDEFFACTORY_HPP_
#define NDEFFACTORY_HPP_

#define CHECK(rc) NdefFactory::checkReturnCode((rc), __LINE__, __FILE__, __PRETTY_FUNCTION__)

#include <QObject>

#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>
#include <QtNfcSubset/qndefmessage.h>

#include <bps/bps.h>
#include <nfc/nfc.h>
#include <nfc/nfc_ndef.h>

#include <sys/types.h>

#include "Settings.hpp"

class NdefFactory : public QObject {
    Q_OBJECT

public:
	virtual ~NdefFactory();

	static NdefFactory* getInstance();

	Q_INVOKABLE QByteArray getNdefUriMessage(const QVariant &uri);
	Q_INVOKABLE QByteArray getNdefTextMessage(const QVariant &lang, const QVariant &text);
	Q_INVOKABLE QByteArray getNdefSpMessage(const QVariant &uri, const QVariant &text);
	Q_INVOKABLE QByteArray getNdefCustomMessage(const QVariant &domain, const QVariant &type, const QVariant &content);
	Q_INVOKABLE QByteArray getNdefVcardMessage(const QVariant &firstName, const QVariant &lastName,
			                                   const QVariant &address, const QVariant &email,
			                                   const QVariant &mobile );
	Q_INVOKABLE QByteArray getNdefTwoTextUriMessage(const QVariant &lang,
			                                        const QVariant &text1,
			                                        const QVariant &text2);
	QMap<uchar_t, QString> getPrefixMap();
    QString getUriFromUriRecordPayload(const QByteArray &payload);
    QString getTextFromTextRecordPayload(const QByteArray &payload);
    QString getLangFromTextRecordPayload(const QByteArray &payload);
    QString getEncodingFromTextRecordPayload(const QByteArray &payload);

private:
	NdefFactory();
	static NdefFactory* _instance;
	static void checkReturnCode(int rc, int line, const char *file, const char *func);
	nfc_ndef_record_t* makeSpRecord();
	nfc_ndef_record_t* makeCustomRecord(QString domain, QString type, QString content);
	QMap<uchar_t, QString> *_prefixMap;
	bool prefixMapContains(const uchar_t uriType);
	QString prefixMapFromType(uchar_t uriType);
};

#endif /* NDEFFACTORY_HPP_ */
