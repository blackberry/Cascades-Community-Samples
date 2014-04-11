/* Copyright (c) 2014 Research In Motion Limited.
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

#include "DataManager.hpp"
#include <QSettings>
#include "SecurityManager.hpp"
#include "Utilities.hpp"
#include "husha2.h"
#include "sbreturn.h"


DataManager* DataManager::_instance;
QString DataManager::KEY_USER_ID="user_id";
QString DataManager::KEY_PASSWORD="password";


DataManager::DataManager() {
    QSettings settings;
	SecurityManager* sec_mgr = SecurityManager::getInstance();
	int result = SB_SUCCESS;

    // get user_id from QSettings or set to the default value of the hex representation of the sha-2 hash of "guest"
	unsigned char message_digest_user_id[SB_SHA256_DIGEST_LEN];
	QString user_id_hash="";
	result = sec_mgr->makeHash("guest", message_digest_user_id);
	if (result == SB_SUCCESS) {
		QByteArray user_id_hash_data = QByteArray::fromRawData(reinterpret_cast<const char *>(message_digest_user_id), SB_SHA256_DIGEST_LEN);
		QString user_id_hash_as_hex = QString::fromAscii(user_id_hash_data.toHex());
		user_id = settings.value(KEY_USER_ID, user_id_hash_as_hex).toString();
	} else {
		qDebug() << "XXXX ERROR: could not establish current user_id value from QSettings";
	}

    // get password from QSettings or set to the default value of the hex representation of the sha-2 hash of "password"
	unsigned char message_digest_password[SB_SHA256_DIGEST_LEN];
	QString password_hash="";
	result = sec_mgr->makeHash("password", message_digest_password);
	if (result == SB_SUCCESS) {
		QByteArray password_hash_data = QByteArray::fromRawData(reinterpret_cast<const char *>(message_digest_password), SB_SHA256_DIGEST_LEN);
		QString password_hash_as_hex = QString::fromAscii(password_hash_data.toHex());
		password = settings.value(KEY_PASSWORD, password_hash_as_hex).toString();
	} else {
		qDebug() << "XXXX ERROR: could not establish current password value from QSettings";
	}

}

DataManager::~DataManager() {
	_instance = 0;
}

DataManager* DataManager::getInstance() {
	if (_instance == 0) {
		_instance = new DataManager;
	}
	return _instance;
}

QString DataManager::getUserid() {
	return user_id;
}

QString DataManager::getPassword() {
	return password;
}


void DataManager::setUserid(QString new_userid) {
    QSettings settings;
	SecurityManager* sec_mgr = SecurityManager::getInstance();
	unsigned char message_digest[SB_SHA256_DIGEST_LEN];
	QString user_id_hash="";
	int result = sec_mgr->makeHash(new_userid, message_digest);
	if (result == SB_SUCCESS) {
		QByteArray user_id_hash_data = QByteArray::fromRawData(reinterpret_cast<const char *>(message_digest), SB_SHA256_DIGEST_LEN);
		QString user_id_hash_as_hex = QString::fromAscii(user_id_hash_data.toHex());
	    settings.setValue(KEY_USER_ID,user_id_hash_as_hex);
	    qDebug() << "XXXX user ID hash stored in QSettings:" << user_id_hash_as_hex;
	    user_id = user_id_hash_as_hex;
	} else {
		qDebug() << "XXXX ERROR: could not set new user_id in QSettings";
	}
}

void DataManager::setPassword(QString new_password) {
    QSettings settings;
	SecurityManager* sec_mgr = SecurityManager::getInstance();
	unsigned char message_digest[SB_SHA256_DIGEST_LEN];
	QString password_hash="";
	int result = sec_mgr->makeHash(new_password, message_digest);
	if (result == SB_SUCCESS) {
		QByteArray password_hash_data = QByteArray::fromRawData(reinterpret_cast<const char *>(message_digest), SB_SHA256_DIGEST_LEN);
		QString password_hash_as_hex = QString::fromAscii(password_hash_data.toHex());
	    settings.setValue(KEY_PASSWORD,password_hash_as_hex);
	    qDebug() << "XXXX password hash stored in QSettings:" << password_hash_as_hex;
	    password = password_hash_as_hex;
	} else {
		qDebug() << "XXXX ERROR: could not set new password in QSettings";
	}
}

