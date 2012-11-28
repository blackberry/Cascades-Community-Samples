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

#include <qt4/QtNetwork/QNetworkRequest>
#include <qt4/QtNetwork/QNetworkReply>
#include <bb/device/HardwareInfo>
#include <iostream>

#include "UrbanAirshipAPI.hpp"

const QString UrbanAirshipAPI::HTTPS_GO_URBANAIRSHIP_COM_API_DEVICE_PINS =
		"https://go.urbanairship.com/api/device_pins/";

const QString UrbanAirshipAPI::BLACKBERRY_PUSH_APPLICATION_ID =
		"your blackberry application id goes here";
const QString UrbanAirshipAPI::BLACKBERRY_PUSH_URL =
		"http://pushapi.eval.blackberry.com";

// Urban Airship Push Settings for your application. From the Details tab
// for your application @ http://go.urbanairship.com
const QString UrbanAirshipAPI::URBAN_AIRSHIP_APPKEY = "your urban airship app key goes here";
const QString UrbanAirshipAPI::URBAN_AIRSHIP_APPSECRET =
		"your urban airship app secret goes here";

// This value should match the value used as the "<invoke-target id>" in the
// bar-descriptor.xml file. This needs to be changed to be unique to your
// application.
const QString UrbanAirshipAPI::BLACKBERRY_INVOKE_TARGET_ID =
		"com.urbanairship.client.sample";

// Value used to store the device token persistently using QSettings
const QString UrbanAirshipAPI::BLACKBERRY_TOKEN = "pushRegistrationToken";

UrbanAirshipAPI::UrbanAirshipAPI() {
}

UrbanAirshipAPI::~UrbanAirshipAPI() {
}

// Save the device token, this is needed register and unregister with UrbanAirship
void UrbanAirshipAPI::setToken(const QString & token) {
	QSettings settings;
	settings.setValue(BLACKBERRY_TOKEN, token);
}

QString UrbanAirshipAPI::getToken() {
	QSettings settings;
	QString token = settings.value(BLACKBERRY_TOKEN, "").toString();
	return token;
}

/**
 * Register Device PIN with Urban Airship
 */
void UrbanAirshipAPI::urbanAirshipRegisterAPICall(QByteArray payload =
		QByteArray()) {
	disconnect(&networkAccessManager, 0, 0, 0);
	connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this,
			SLOT(registrationFinished(QNetworkReply*)));
	QNetworkRequest networkRequest = generateBasicNetworkRequestObject();
	if (!payload.isEmpty()) {
		networkRequest.setRawHeader("Content-Type", "application/json");
	}
	networkAccessManager.put(networkRequest, payload);
}

/**
 * Un-Register Device PIN with Urban Airship
 */
void UrbanAirshipAPI::urbanAirshipUnRegisterAPICall() {
	disconnect(&networkAccessManager, 0, 0, 0);
	connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this,
			SLOT(unRegistrationFinished(QNetworkReply*)));
	QNetworkRequest networkRequest = generateBasicNetworkRequestObject();
	networkAccessManager.deleteResource(networkRequest);
}

/**
 * The network calls to register and unregister with UrbanAirship are very
 * similar, this call separates out the similarities
 */
QNetworkRequest UrbanAirshipAPI::generateBasicNetworkRequestObject() {
	QNetworkRequest networkRequest;
	QString url(HTTPS_GO_URBANAIRSHIP_COM_API_DEVICE_PINS + getToken());
	networkRequest.setUrl(url);

	QString login = QString("%1:%2").arg(URBAN_AIRSHIP_APPKEY).arg(
			URBAN_AIRSHIP_APPSECRET);
	QByteArray encoded = login.toAscii().toBase64();

	networkRequest.setRawHeader("Authorization", "Basic " + encoded);
	return networkRequest;
}

/**
 * Register Device PIN with Urban Airship, with an Alias.
 *
 * @param alias device alias (e.g., "David's Device")
 *
 */
void UrbanAirshipAPI::urbanAirshipRegisterPINWithAlias(const QString& alias =
		"none") {
	// Build alias payload
	QString jsonString = QString("{\"alias\":\"%1\"}").arg(alias);
	QByteArray json = jsonString.toAscii();

	// Call the register API with the defined Alias
	urbanAirshipRegisterAPICall(json);
}

/**
 * Public method for unregistering with UrbanAirship
 */
void UrbanAirshipAPI::urbanAirshipUnRegisterPIN() {
	urbanAirshipUnRegisterAPICall();
}

/**
 * Public method for unregistering with UrbanAirship without an alias
 */
void UrbanAirshipAPI::urbanAirshipRegisterPIN() {
	urbanAirshipRegisterAPICall();
}


/**
 * The following 2 slots emit signals when completed which are connected
 * in the main portion of the application, UrbanAirshipClient.cpp
 */
void UrbanAirshipAPI::registrationFinished(QNetworkReply* reply) {
	emit urbanAirshipRegistrationResult(
			(reply->error() == QNetworkReply::NoError), reply->readAll());
	reply->deleteLater();
}

void UrbanAirshipAPI::unRegistrationFinished(QNetworkReply* reply) {
	emit urbanAirshipUnRegistrationResult(
			(reply->error() == QNetworkReply::NoError), reply->readAll());
	reply->deleteLater();
}

