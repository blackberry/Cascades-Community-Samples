/*
 * Copyright (c) 2014 BlackBerry Limited.
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

#include "SecurityManager.hpp"
#include "DataManager.hpp"
#include "Utilities.hpp"
#include "huctx.h"
#include <QString>
#include <QDebug>
#include "husha2.h"
#include "hugse56.h"
#include "sbreturn.h"
#include "sbmem.h"

SecurityManager* SecurityManager::_instance;

SecurityManager::SecurityManager() {
	sbCtx=NULL;                       /* SB Context */
	sha256Context=NULL;               /* SHA-256 Context */
	logged_in=false;
}

SecurityManager::~SecurityManager() {
	_instance = 0;
}

SecurityManager* SecurityManager::getInstance() {
	if (_instance == 0) {
		_instance = new SecurityManager;
	}
	return _instance;
}

int SecurityManager::initSecurity() {

	qDebug() << "XXXX initSecurity";

	int returnCode = SB_SUCCESS; /* Return Code */

	/* Create SB Contexts */

	returnCode = hu_GlobalCtxCreateDefault(&sbCtx);

	if (returnCode != SB_SUCCESS) {
		qDebug() << "XXXX makeHash ERROR creating SB contexts:" << Utilities::intToHex(returnCode);
		return returnCode;
	}

	// Register global context with GSE-C 5.6 Provider
	// GSE = Government Security Edition (SB GSE-C)
	// SB GSE-C is also known as BlackBerry OS Cryptographic Kernel

	returnCode = hu_RegisterSbg56(sbCtx);
	if (returnCode != SB_SUCCESS) {
		qDebug() << "XXXX makeHash ERROR calling hu_RegisterSbg56:" << Utilities::intToHex(returnCode);
		return returnCode;
	}

	returnCode = hu_InitSbg56(sbCtx);
	if (returnCode != SB_SUCCESS) {
		qDebug() << "XXXX makeHash ERROR calling hu_InitSbg56:" << Utilities::intToHex(returnCode);
		return returnCode;
	}

	return returnCode;
}

void SecurityManager::endSecurity() {
	/* Clean up contexts */
	qDebug() << "XXXX endSecurity";
	(void) hu_SHA256End(&sha256Context, NULL, sbCtx);
	hu_GlobalCtxDestroy(&sbCtx);
}

int SecurityManager::makeHash(const QString input_data, unsigned char* messageDigest) {

	qDebug() << "XXXX makeHash:" << input_data << " (" << input_data.length() << ")";

	/* Initialize variables */

	int returnCode = SB_SUCCESS; /* Return Code */

	QByteArray input_bytes = input_data.toUtf8();
	unsigned char* hash_input = reinterpret_cast<unsigned char*>(input_bytes.data());

	returnCode = hu_SHA256Begin((size_t) SB_SHA256_DIGEST_LEN, NULL, &sha256Context, sbCtx);
	if (returnCode != SB_SUCCESS) {
		qDebug() << "XXXX makeHash ERROR initialising SHA-256 context:" << Utilities::intToHex(returnCode);
		return returnCode;
	}

	/* Hash Message */

	returnCode = hu_SHA256Hash(sha256Context, (size_t) input_bytes.length(), hash_input, sbCtx);
	if (returnCode != SB_SUCCESS) {
		qDebug() << "XXXX makeHash ERROR creating hash:" << Utilities::intToHex(returnCode);
		return returnCode;
	}

	/* Complete SHA-256 Hashing */

	returnCode = hu_SHA256End(&sha256Context, messageDigest, sbCtx);
	if (returnCode != SB_SUCCESS) {
		qDebug() << "XXXX makeHash ERROR completing hashing" << Utilities::intToHex(returnCode);
		return returnCode;
	}

	return SB_SUCCESS;

}

bool SecurityManager::isCredentialsOK(QString entered_user_id, QString entered_password) {
	qDebug() << "XXXX isCredentialsOK(" << entered_user_id << "," << entered_password << ")";
	int result = SB_SUCCESS;
	DataManager* data_mgr = DataManager::getInstance();
	QString current_user_id_hash_as_hex = data_mgr->getUserid();
	QString current_password_hash_as_hex = data_mgr->getPassword();
	bool password_ready = false;
	// if DataManager was not able to provide the stored password or a default then it will have returned an empty string
	if (current_password_hash_as_hex.length() > 0) {
		password_ready = true;
	}

	// if we were unable to either
	// a) restore password from QSettings or
	// b) create a default password using the SB API then
	// we have a system error and it's game over

	if (!password_ready) {
		qDebug() << "XXXX SYSTEM ERROR: password not ready";
		return false;
	}

	// hash the user ID entered by the user and compare it with the user_id hash restored from QSettings
	unsigned char message_digest_user_id[SB_SHA256_DIGEST_LEN];
	QString entered_user_id_hash = "";
	result = makeHash(entered_user_id, message_digest_user_id);
	if (result == SB_SUCCESS) {
		QByteArray entered_user_id_data = QByteArray::fromRawData(reinterpret_cast<const char *>(message_digest_user_id), SB_SHA256_DIGEST_LEN);
		QString entered_user_id_hash_as_hex = QString::fromAscii(entered_user_id_data.toHex());
		qDebug() << "XXXX entered user_id hash:" << entered_user_id_hash_as_hex;
		qDebug() << "XXXX current user_id hash:" << current_user_id_hash_as_hex;
		if (current_user_id_hash_as_hex.compare(entered_user_id_hash_as_hex) != 0) {
			qDebug() << "XXXX incorrect credentials entered #1";
			return false;
		}
	} else {
		qDebug() << "XXXX SYSTEM ERROR: failed to calculate hash of entered user_id:" << result;
		return false;
	}

	// hash the password entered by the user and compare it with the password hash restored from QSettings
	unsigned char message_digest_password[SB_SHA256_DIGEST_LEN];
	QString entered_password_hash = "";
	result = makeHash(entered_password, message_digest_password);
	if (result == SB_SUCCESS) {
		QByteArray entered_password_data = QByteArray::fromRawData(reinterpret_cast<const char *>(message_digest_password), SB_SHA256_DIGEST_LEN);
		QString entered_password_hash_as_hex = QString::fromAscii(entered_password_data.toHex());
		qDebug() << "XXXX entered password hash:" << entered_password_hash_as_hex;
		qDebug() << "XXXX current password hash:" << current_password_hash_as_hex;
		if (current_password_hash_as_hex.compare(entered_password_hash_as_hex) != 0) {
			qDebug() << "XXXX incorrect credentials entered #2";
			return false;
		} else {
			return true;
		}
	} else {
		qDebug() << "XXXX SYSTEM ERROR: failed to calculate hash of entered password:" << result;
		return false;
	}
}

void SecurityManager::setLoggedIn(bool new_state) {
	qDebug() << "XXXX SecurityManager::setLoggedIn-" << new_state;
	logged_in = new_state;
	emit loggedInChanged(new_state);
}

bool SecurityManager::isLoggedIn() {
	return logged_in;
}
