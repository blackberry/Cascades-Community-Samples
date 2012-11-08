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

#include "NdefFactory.hpp"

using namespace bb::cascades;

NdefFactory* NdefFactory::_instance;

NdefFactory* NdefFactory::getInstance() {
	if (_instance == 0) {
		_instance = new NdefFactory;
	}
	return _instance;
}

NdefFactory::NdefFactory() {

	_prefixMap = new QMap<uchar_t, QString>();

	(*_prefixMap)[Settings::NfcRtdUriPrefixHttpWww]  = QString("http://www.");
	(*_prefixMap)[Settings::NfcRtdUriPrefixHttpsWww] = QString("https://www.");
	(*_prefixMap)[Settings::NfcRtdUriPrefixHttp]     = QString("http://");
	(*_prefixMap)[Settings::NfcRtdUriPrefixHttps]    = QString("https://");
	(*_prefixMap)[Settings::NfcRtdUriPrefixNone]     = QString("");
}

NdefFactory::~NdefFactory() {
	delete _prefixMap;
}

QMap<uchar_t, QString> NdefFactory::getPrefixMap() {
	return *_prefixMap;
}

QByteArray NdefFactory::getNdefUriMessage(const QVariant &uri) {

    qDebug() << "XXXX Entering MainMenu::getNdefUriMessage";

    QString ndefUri = uri.toString();

    qDebug() << "XXXX Uri: " << ndefUri;

    QtMobilitySubset::QNdefRecord ndefRecord = QtMobilitySubset::QNdefRecord();

	ndefRecord.setTypeNameFormat(QtMobilitySubset::QNdefRecord::NfcRtd);
	ndefRecord.setType(Settings::NfcRtdUri);
	ndefRecord.setPayload(ndefUri.toUtf8().prepend(Settings::NfcRtdUriPrefixNone));

	QtMobilitySubset::QNdefMessage ndefMessage = QtMobilitySubset::QNdefMessage(ndefRecord);

	qDebug() << "XXXX Raw NDEF Message: " << ndefMessage.toByteArray().toHex();
	qDebug() << "XXXX Leaving MainMenu::getNdefUriMessage";

	return ndefMessage.toByteArray();
}

QByteArray NdefFactory::getNdefTextMessage(const QVariant &lang, const QVariant &text) {

    qDebug() << "XXXX Entering MainMenu::getNdefTextMessage";

    QString ndefLang = lang.toString();
    QString ndefText = text.toString();

    qDebug() << "XXXX Lang: " << ndefLang;
    qDebug() << "XXXX Text: " << ndefText;

    QtMobilitySubset::QNdefRecord ndefRecord = QtMobilitySubset::QNdefRecord();

    int langLength = (ndefLang.length() <= 63) ? ndefLang.length() : 63;

    QByteArray ndefPayload;
    ndefPayload[0] = langLength;
    ndefPayload.append(ndefLang.left(langLength)).append(ndefText.toUtf8());

	ndefRecord.setTypeNameFormat(QtMobilitySubset::QNdefRecord::NfcRtd);
	ndefRecord.setType(Settings::NfcRtdText);
	ndefRecord.setPayload(ndefPayload);

	QtMobilitySubset::QNdefMessage ndefMessage = QtMobilitySubset::QNdefMessage(ndefRecord);

	qDebug() << "XXXX Raw NDEF Message: " << ndefMessage.toByteArray().toHex();
	qDebug() << "XXXX Leaving MainMenu::getNdefTextMessage";

	return ndefMessage.toByteArray();
}

QByteArray NdefFactory::getNdefTwoTextUriMessage(const QVariant &lang, const QVariant &text1, const QVariant &text2) {
    qDebug() << "XXXX Entering MainMenu::getNdefTwoTextUriMessage";

    QString ndefLang = lang.toString();
    QString ndefText1 = text1.toString();
    QString ndefText2 = text2.toString();

    qDebug() << "XXXX Lang:  " << ndefLang;
    qDebug() << "XXXX Text1: " << ndefText1;
    qDebug() << "XXXX Text2: " << ndefText2;

    QtMobilitySubset::QNdefRecord ndefRecord1 = QtMobilitySubset::QNdefRecord();
    QtMobilitySubset::QNdefRecord ndefRecord2 = QtMobilitySubset::QNdefRecord();

    int langLength = (ndefLang.length() <= 63) ? ndefLang.length() : 63;

    QByteArray ndefPayload1;
    QByteArray ndefPayload2;

    ndefPayload1[0] = langLength;
    ndefPayload1.append(ndefLang.left(langLength)).append(ndefText1.toUtf8());

    ndefPayload2[0] = langLength;
    ndefPayload2.append(ndefLang.left(langLength)).append(ndefText2.toUtf8());

    ndefRecord1.setTypeNameFormat(QtMobilitySubset::QNdefRecord::NfcRtd);
	ndefRecord1.setType(Settings::NfcRtdText);
	ndefRecord1.setPayload(ndefPayload1);

    ndefRecord2.setTypeNameFormat(QtMobilitySubset::QNdefRecord::NfcRtd);
	ndefRecord2.setType(Settings::NfcRtdText);
	ndefRecord2.setPayload(ndefPayload2);

	QList<QtMobilitySubset::QNdefRecord> records;

	records.append(ndefRecord1);
	records.append(ndefRecord2);

	QtMobilitySubset::QNdefMessage ndefMessage = QtMobilitySubset::QNdefMessage(records);

	qDebug() << "XXXX Raw NDEF Message: " << ndefMessage.toByteArray().toHex();
	qDebug() << "XXXX Leaving MainMenu::getNdefTwoTextUriMessage";

	return ndefMessage.toByteArray();
}

QByteArray NdefFactory::getNdefSpMessage(const QVariant &uri, const QVariant &text) {

    qDebug() << "XXXX Entering MainMenu::getNdefSpMessage";

    QString ndefText = text.toString();
    QString ndefUri  = uri.toString();

    qDebug() << "XXXX Uri:  " << ndefUri;
    qDebug() << "XXXX Text: " << ndefText;

    nfc_ndef_record_t* spNdefRecord;
	nfc_ndef_message_t* spNdefMessage;

	CHECK(nfc_create_ndef_message(&spNdefMessage));

	spNdefRecord = makeSpRecord();

	CHECK(nfc_set_sp_uri(spNdefRecord, ndefUri.toUtf8().constData()));
	CHECK(nfc_add_sp_title(spNdefRecord, Settings::LANG_EN, ndefText.toUtf8().constData()));
	CHECK(nfc_add_ndef_record(spNdefMessage, spNdefRecord));

	size_t spNdefMessageLength = 0;
	CHECK(nfc_get_ndef_message_byte_length(spNdefMessage, &spNdefMessageLength));

	uchar_t* spNdefData = new uchar_t[spNdefMessageLength];
	CHECK(nfc_get_ndef_message_bytes(spNdefMessage, spNdefData));

	QByteArray spNdefByteArray = QByteArray(reinterpret_cast<const char*>(spNdefData), spNdefMessageLength);

	CHECK(nfc_delete_ndef_message(spNdefMessage, true)); // do we?? -- but when and where?
	delete [] spNdefData; // do we? -- but when and where?

	qDebug() << "XXXX Leaving MainMenu::getNdefSpMessage";

	return spNdefByteArray;
}

nfc_ndef_record_t* NdefFactory::makeSpRecord() {
	qDebug() << "XXXX NfcWorker::makeSpRecord";
	nfc_ndef_record_t* record = 0;
	uchar_t payload[0];
	CHECK(nfc_create_ndef_record(NDEF_TNF_WELL_KNOWN, Settings::NfcRtdSmartPoster, payload, 0, 0, &record));
	return record;
}

QByteArray NdefFactory::getNdefCustomMessage(const QVariant &domain, const QVariant &type, const QVariant &content) {

    qDebug() << "XXXX Entering MainMenu::getNdefCustomMessage";

    QString ndefDomain    = domain.toString();
    QString ndefType      = type.toString();
    QString ndefContent   = content.toString();

    qDebug() << "XXXX Domain:  " << ndefDomain;
    qDebug() << "XXXX Type:    " << ndefType;
    qDebug() << "XXXX Content: " << ndefContent;

    nfc_ndef_record_t* custNdefRecord;
	nfc_ndef_message_t* custNdefMessage;

	custNdefRecord = makeCustomRecord(ndefDomain, ndefType, ndefContent);

	CHECK(nfc_create_ndef_message(&custNdefMessage));
	CHECK(nfc_add_ndef_record(custNdefMessage, custNdefRecord));

	size_t custNdefMessageLength = 0;
	CHECK(nfc_get_ndef_message_byte_length(custNdefMessage, &custNdefMessageLength));

	uchar_t* custNdefData = new uchar_t[custNdefMessageLength];
	CHECK(nfc_get_ndef_message_bytes(custNdefMessage, custNdefData));

	QByteArray custNdefByteArray = QByteArray(reinterpret_cast<const char*>(custNdefData), custNdefMessageLength);

	CHECK(nfc_delete_ndef_message(custNdefMessage, true)); // do we? -- but when and wwhre?
	delete [] custNdefData; // do we? -- but when and where?

    qDebug() << "XXXX Leaving MainMenu::getNdefCustomMessage";

	return custNdefByteArray;
}

nfc_ndef_record_t* NdefFactory::makeCustomRecord(QString domain, QString type, QString content) {

	qDebug() << QString("makeCustomRecord DOMAIN: %1 TYPE: %2 CONTENT %3").arg(domain).arg(type).arg(content);

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

QByteArray NdefFactory::getNdefVcardMessage(const QVariant &firstName, const QVariant &lastName,
		                                 const QVariant &address, const QVariant &email,
		                                 const QVariant &mobile ) {

	qDebug() << "XXXX Entering MainMenu::getNdefVcardMessage";

    QString ndefFirstName = firstName.toString();
    QString ndefLastName  = lastName.toString();
    QString ndefAddress   = address.toString();
    QString ndefEmail     = email.toString();
    QString ndefMobile    = mobile.toString();

    qDebug() << "XXXX First Name: " << ndefFirstName;
    qDebug() << "XXXX Last Name:  " << ndefLastName;
    qDebug() << "XXXX Address:    " << ndefAddress;
    qDebug() << "XXXX Email:      " << ndefEmail;
    qDebug() << "XXXX Mobile:     " << ndefMobile;

    QtMobilitySubset::QNdefRecord ndefRecord = QtMobilitySubset::QNdefRecord();

	QString mimeType = QString("text/x-vCard");
	QString vCard =
			QString("BEGIN:VCARD\n")
			.append("VERSION:3.0\n")
			.append("N:").append(ndefLastName).append(";").append(ndefFirstName).append("\n")
			.append("FN:").append(ndefLastName).append(" ").append(ndefFirstName).append("\n")
			.append("ADR;TYPE=WORK:").append(ndefAddress).append("\n")
			.append("TEL;TYPE=CELL:").append(ndefMobile).append("\n")
			.append("EMAIL;TYPE=INTERNET:").append(ndefEmail).append("\n")
			.append("END:VCARD");

    ndefRecord.setTypeNameFormat(QtMobilitySubset::QNdefRecord::Mime);
	ndefRecord.setType(mimeType.toUtf8());
	ndefRecord.setPayload(vCard.toUtf8());

	QtMobilitySubset::QNdefMessage ndefMessage = QtMobilitySubset::QNdefMessage(ndefRecord);

	qDebug() << "XXXX Raw NDEF Message: " << ndefMessage.toByteArray().toHex();
	qDebug() << "XXXX Leaving MainMenu::getNdefVcardMessage";

	return ndefMessage.toByteArray();
}

QString NdefFactory::getUriFromUriRecordPayload(const QByteArray &payload) {

	QString uri;

	qDebug() << "XXXX Uri - raw: " << payload.toHex();

	uchar_t uriType;
	int uriLength;

	uriType = payload[0];
	uriLength = payload.length() - 1;

	qDebug() << "XXXX UriType: " << uriType;
	qDebug() << "XXXX UriLength: " << uriLength;

	uri = QString::fromUtf8(payload.right(uriLength).constData(), uriLength);

	if (prefixMapContains(uriType)) {
		uri.prepend(prefixMapFromType(uriType));
	} else {
		qDebug() << QString("URI Prefix %1 not implemented").arg(uriType);
	}

	qDebug() << "XXXX Uri: " << uri;

	return uri;
}

QString NdefFactory::getTextFromTextRecordPayload(const QByteArray &payload) {

	QString text;

	qDebug() << "XXXX Text - raw: " << payload.toHex();

	uchar_t statusByte = payload[0];
	bool isUtf16 = false;

	qDebug() << "XXXX StatusByte: " << statusByte;

	if ( statusByte & 0x80 ) { // bit 7 is UTF type ( 8 or 16 )
		isUtf16 = true;
	}

	int langLen = statusByte & 0x3F; // lower 6 bits is language length
	int textLength = payload.length() - (langLen + 1);

	qDebug() << "XXXX Lang Length: " << langLen;
	qDebug() << "XXXX Text Length: " << textLength;

	if (isUtf16) {
		text = QString(payload.mid(1+langLen).constData());
	} else {
		text = QString::fromUtf8(payload.mid(1+langLen).constData());
	}

    qDebug() << "XXXX Text       : " << text;

	return text;
}

QString NdefFactory::getLangFromTextRecordPayload(const QByteArray &payload) {

	QString langCode;

	qDebug() << "XXXX Lang - raw: " << payload.toHex();

	uchar_t statusByte = payload[0];
	int langLength = statusByte & 0x3F; // lower 6 bits is language length

    qDebug() << "XXXX StatusByte: " << statusByte;
    qDebug() << "XXXX Lang Length: " << langLength;

	langCode = QString::fromAscii(payload.mid(1,langLength).constData());

    qDebug() << "XXXX Lang Code  : " << langCode;

	return langCode;
}

QString NdefFactory::getEncodingFromTextRecordPayload(const QByteArray &payload) {

	QString textEncoding;

	qDebug() << "XXXX Encoding - raw: " << payload.toHex();

	uchar_t statusByte = payload[0];
	bool isUtf16 = false;

	qDebug() << "XXXX StatusByte: " << statusByte;

	if ( statusByte & 0x80 ) { // bit 7 is UTF type ( 8 or 16 )
		isUtf16 = true;
	}

	if (isUtf16) {
		textEncoding = "UTF-16";
	} else {
		textEncoding = "UTF-8";
	}

    qDebug() << "XXXX Encoding   : " << textEncoding;

	return textEncoding;

}

bool NdefFactory::prefixMapContains(uchar_t uriType) {
	return getPrefixMap().contains(uriType);
}

QString NdefFactory::prefixMapFromType(uchar_t uriType) {
	return (getPrefixMap())[uriType];
}

void NdefFactory::checkReturnCode(int rc, int line, const char *file, const char *func) {
	if (rc != BPS_SUCCESS) {
		qDebug() << "XXXX Error code " << rc << " in function " << func
				<< " on line " << line << " in " << file;
	}
}
