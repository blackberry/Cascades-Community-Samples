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

#ifndef URBANAIRSHIPAPI_HPP_
#define URBANAIRSHIPAPI_HPP_

#include <QObject>
#include <qt4/QtNetwork/QNetworkAccessManager>

class UrbanAirshipAPI: public QObject {
	Q_OBJECT
public:
	UrbanAirshipAPI();
	virtual ~UrbanAirshipAPI();

	void urbanAirshipUnRegisterPIN();
	void urbanAirshipRegisterPIN();
	void urbanAirshipRegisterPINWithAlias(const QString& alias);
	void setToken(const QString & token);

	static QString applicationId() {
		return BLACKBERRY_PUSH_APPLICATION_ID;
	}

	static QString pushUrl() {
		return BLACKBERRY_PUSH_URL;
	}

	static QString invokeTargetId() {
		return BLACKBERRY_INVOKE_TARGET_ID;
	}

private slots:
	void registrationFinished(QNetworkReply* reply);
	void unRegistrationFinished(QNetworkReply* reply);

	signals:
	void urbanAirshipRegistrationResult(bool registrationSuccess,
			QString resultMessage = "");
	void urbanAirshipUnRegistrationResult(bool registrationSuccess,
			QString resultMessage = "");

private:
	static const QString HTTPS_GO_URBANAIRSHIP_COM_API_DEVICE_PINS;
	static const QString URBAN_AIRSHIP_REGISTER_PIN;
	static const QString URBAN_AIRSHIP_UNREGISTER_PIN;
	static const QString BLACKBERRY_PUSH_APPLICATION_ID;
	static const QString BLACKBERRY_PUSH_URL;
	static const QString URBAN_AIRSHIP_APPKEY;
	static const QString URBAN_AIRSHIP_APPSECRET;
	static const QString BLACKBERRY_INVOKE_TARGET_ID;
	static const QString BLACKBERRY_TOKEN;

	QNetworkAccessManager networkAccessManager;

	void urbanAirshipRegisterAPICall(QByteArray payload);
	void urbanAirshipUnRegisterAPICall();
	QString getToken();

	QNetworkRequest generateBasicNetworkRequestObject();

};

#endif /* URBANAIRSHIPAPI_HPP_ */
