/**
   Copyright (c) 2013 BlackBerry Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**/
#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/SystemToast>
#include <QSettings>
#include <huaes.h>
#include <sbreturn.h>
#include <hurandom.h>
#include <string.h>

#include "AESParams.hpp"
#include "AESKey.hpp"
#include "DRBG.hpp"
#include "SBError.hpp"

using namespace bb::cascades;
using namespace bb::system;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {
	QSettings settings;

	char montreal[] = "Montr\xc3\xa9" "al";

	_plainText = settings.value("plainText",
			QString::fromUtf8(montreal, strlen(montreal))).toString();

    // bugfix - initial values were being ignored...
	if (!settings.contains("key")) {
		settings.setValue("key",generate());
	}
	if (!settings.contains("iv")) {
		settings.setValue("iv",generate());
	}
	_key = settings.value("key").toString();
	_iv = settings.value("iv").toString();

	_cipherText = settings.value("cipherText", "").toString();
	_recoveredPlainText = settings.value("recoveredPlainText", "").toString();

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	qml->setContextProperty("app", this);

	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	// Set created root object as the application scene
	app->setScene(root);

	if (!isCryptoAvailable()) {
		toast("Need to double check our code - crypto isn't available...");
	}
}

QString ApplicationUI::generate() {
	DRBG drbg(globalContext);
	QByteArray buffer(16, 0);
	int rc = drbg.getBytes(buffer);
	if (rc != SB_SUCCESS) {
		toast(
				QString("Could not generate random bytes %1").arg(
						SBError::getErrorText(rc)));
		return "";
	}
	return toHex(buffer);
}

void ApplicationUI::pad(QByteArray & in) {
	int padLength = 16 - (in.length() % 16);
	for (int i = 0; i < padLength; ++i) {
		in.append((char) padLength);
	}
}

bool ApplicationUI::encrypt() {
	QByteArray in(plainText().toUtf8());
	pad(in);
	QByteArray out(in.length(), 0);

	if (crypt(true, in, out)) {
		setCipherText(toHex(out));
		return true;
	}
	return false;
}

bool ApplicationUI::crypt(bool isEncrypt, const QByteArray & in,
		QByteArray & out) {
	QByteArray key, iv;
	QString fail;

	if (!fromHex(_key, key)) {
		fail += "Key is not valid hex. ";
	}
	if (!fromHex(_iv, iv)) {
		fail += "IV is not valid hex. ";
	}

	if (!fail.isEmpty()) {
		toast(fail);
		return false;
	}

	AESParams params(globalContext);
	if (!params.isValid()) {
		toast(
				QString("Could not create params. %1").arg(
						SBError::getErrorText(params.lastError())));
		return false;
	}

	AESKey aesKey(params, key);
	if (!aesKey.isValid()) {
		toast(
				QString("Could not create a key. %1").arg(
						SBError::getErrorText(aesKey.lastError())));
		return false;
	}

	int rc;
	if (isEncrypt) {
		rc = hu_AESEncryptMsg(params.aesParams(), aesKey.aesKey(), iv.length(),
				(const unsigned char*) iv.constData(), in.length(),
				(const unsigned char *) in.constData(),
				(unsigned char *) out.data(), globalContext.ctx());
	} else {
		rc = hu_AESDecryptMsg(params.aesParams(), aesKey.aesKey(), iv.length(),
				(const unsigned char*) iv.constData(), in.length(),
				(const unsigned char *) in.constData(),
				(unsigned char *) out.data(), globalContext.ctx());
	}
	if (rc == SB_SUCCESS) {
		return true;
	}

	toast(QString("Crypto operation failed. %1").arg(SBError::getErrorText(rc)));
	return false;

}

bool ApplicationUI::decrypt() {
	QByteArray in;
	if (!fromHex(cipherText(), in)) {
		toast("Cipher text is not valid hex");
		return false;
	}
	QByteArray out(in.length(), 0);

	if (crypt(false, in, out)) {
		if (removePadding(out)) {
			QString toUse(QString::fromUtf8(out.constData(), out.length()));
			setRecoveredPlainText(toUse);
			return true;
		}
	}
	return false;
}

bool ApplicationUI::removePadding(QByteArray & out) {
	char paddingLength = out[out.length() - 1];
	if (paddingLength < 1 || paddingLength > 16) {
		toast("Invalid padding length. Were the keys good?");
		out.clear();
		return false;
	}
	if (paddingLength > out.length()) {
		toast("Padding is claiming to be longer than the buffer!");
		out.clear();
		return false;
	}
	for (int i = 1; i < paddingLength; ++i) {
		char next = out[out.length() - 1 - i];
		if (next != paddingLength) {
			toast("Not all padding bytes are correct!");
			out.clear();
			return false;
		}
	}
	out.remove(out.length() - paddingLength, paddingLength);
	return true;
}

QString ApplicationUI::plainText() {
	return _plainText;
}

void ApplicationUI::setPlainText(const QString plainText) {
	if (plainText != _plainText) {
		_plainText = plainText;
		QSettings settings;
		settings.setValue("plainText", plainText);
	}
}

QString ApplicationUI::key() {
	return _key;
}

void ApplicationUI::setKey(const QString key) {
	if (key != _key) {
		_key = key;
		QSettings settings;
		settings.setValue("key", key);
	}
}

QString ApplicationUI::iv() {
	return _iv;
}

void ApplicationUI::setIV(const QString iv) {
	if (iv != _iv) {
		_iv = iv;
		QSettings settings;
		settings.setValue("iv", iv);
	}
}

QString ApplicationUI::cipherText() {
	return _cipherText;
}

void ApplicationUI::setCipherText(const QString cipherText) {
	if (cipherText != _cipherText) {
		_cipherText = cipherText;
		QSettings settings;
		settings.setValue("cipherText", cipherText);
	}
}

QString ApplicationUI::recoveredPlainText() {
	return _recoveredPlainText;
}

void ApplicationUI::setRecoveredPlainText(const QString recoveredPlainText) {
	if (recoveredPlainText != _recoveredPlainText) {
		_recoveredPlainText = recoveredPlainText;
		QSettings settings;
		settings.setValue("recoveredPlainText", recoveredPlainText);
	}
}

QString ApplicationUI::toHex(const QByteArray & in) {
	static char hexChars[] = "0123456789abcdef";

	const char * c = in.constData();
	QString toReturn;
	for (int i = 0; i < in.length(); ++i) {
		toReturn += hexChars[(c[i] >> 4) & 0xf];
		toReturn += hexChars[(c[i]) & 0xf];
	}

	return toReturn;
}

bool ApplicationUI::fromHex(const QString in, QByteArray & toReturn) {
	QString temp(in);
	temp.replace(" ","");
	temp.replace(":","");
	temp.replace(".","");
	QByteArray content(temp.toLocal8Bit());

	const char * c(content.constData());

	if (content.length() == 0 || ((content.length() % 2) != 0)) {
		return false;
	}

	for (int i = 0; i < content.length(); i += 2) {
		char a = c[i];
		char b = c[i + 1];
		a = nibble(a);
		b = nibble(b);
		if (a < 0 || b < 0) {
			toReturn.clear();
			return false;
		}
		toReturn.append((a << 4) | b);
	}
	return true;
}

char ApplicationUI::nibble(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	} else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	return -1;
}

void ApplicationUI::toast(const QString & message) {
	qDebug() << message;
	SystemToast * t(new SystemToast(this));
	t->setBody(message);
	t->show();
}

void ApplicationUI::toast(const char * message) {
	toast(QString(message));
}

