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
#include "DataManager.hpp"
#include "Utilities.hpp"
#include "SecurityManager.hpp"
#include "husha2.h"
#include "sbreturn.h"
#include <QSettings>


DataManager* DataManager::_instance;
QString DataManager::KEY_USER_ID="user_id";
QString DataManager::KEY_PASSWORD="password";
QString DataManager::KEY_FIRST_NAME="first_name";
QString DataManager::KEY_LAST_NAME="last_name";


DataManager::DataManager() {
    QSettings settings;
    // get user_id from QSettings or set to the default value of "guest"
    user_id = settings.value(KEY_USER_ID, "guest").toString();

    // get password from QSettings or set to the default value of the hex representation of the sha-2 hash of "password"
	unsigned char message_digest[SB_SHA256_DIGEST_LEN];
	QString password_hash="";
	SecurityManager* sec_mgr = SecurityManager::getInstance();
	int result = sec_mgr->makeHash("password", message_digest);
	if (result == SB_SUCCESS) {
		QByteArray password_hash_data = QByteArray::fromRawData(reinterpret_cast<const char *>(message_digest), SB_SHA256_DIGEST_LEN);
		QString password_hash_as_hex = QString::fromAscii(password_hash_data.toHex());
		password = settings.value(KEY_PASSWORD, password_hash_as_hex).toString();
	} else {
		qDebug() << "XXXX ERROR: could not establish current password value from QSettings";
	}

	first_name = settings.value(KEY_FIRST_NAME, "").toString();
	last_name = settings.value(KEY_LAST_NAME, "").toString();
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

QString DataManager::getFirstname() {
	return first_name;
}

QString DataManager::getLastname() {
	return last_name;
}

void DataManager::setUserid(QString new_userid) {
    QSettings settings;
    settings.setValue(KEY_USER_ID,new_userid);
    user_id = new_userid;
}

void DataManager::setPassword(QString new_password) {
    QSettings settings;
	unsigned char message_digest[SB_SHA256_DIGEST_LEN];
	QString password_hash="";
	SecurityManager* sec_mgr = SecurityManager::getInstance();
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

void DataManager::setFirstname(QString new_first_name) {
    QSettings settings;
    settings.setValue(KEY_FIRST_NAME,new_first_name);
    first_name = new_first_name;
}

void DataManager::setLastname(QString new_last_name) {
    QSettings settings;
    settings.setValue(KEY_LAST_NAME,new_last_name);
    last_name = new_last_name;
}
