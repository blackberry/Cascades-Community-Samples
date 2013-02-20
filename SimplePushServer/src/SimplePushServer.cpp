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

#include "SimplePushServer.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <QFile>
#include <QVariantList>
#include <QUuid>
#include <QSettings>
#include <qt4/QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNodeList>

using namespace bb::cascades;

const QString SimplePushServer::BLACKBERRY_PUSH_APPLICATION_ID =
		"Application ID";
const QString SimplePushServer::BLACKBERRY_PUSH_URL =
		"https://cp###.pushapi.eval.blackberry.com/";
const QString SimplePushServer::BLACKBERRY_PUSH_PASSWORD = "password";

//Don't change this unless you also change in the template files
const QString SimplePushServer::BLACKBERRY_PUSH_BOUNDARY = "asdfglkjhqwert";

SimplePushServer::SimplePushServer(bb::cascades::Application *app) :
		QObject(app) {
	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
	qml->setContextProperty("simplePushServer", this);

	// create root object for the UI
	mainAppPage = qml->createRootObject<Page>();
	// set created root object as a scene
	app->setScene(mainAppPage);

	logTextArea = mainAppPage->findChild<TextArea*>("logTextArea");

	QString tempForAddresses;
	populateAddresses(tempForAddresses);
	if (!tempForAddresses.isEmpty()) {
		mainAppPage->setProperty("havePinsBeenReceived", true);
	}
}

/**
 * Push a message to a single user. This will create an address QString with just one
 * PIN specified as the address.
 */
void SimplePushServer::pushMessageToUser(const QString &userPin,
		const int priority, const QString &title, const QString &body) {

	QString papFormattedAddress = QString(userPin);
	papFormatAddress(papFormattedAddress);
	pushMessageToSpecifiedUsers(papFormattedAddress, priority, title, body);
}

/**
 * Push a message to all users known to the app (stored in QSettings). This
 * method will retrieve all users stored then add them to a QString and request
 * the message be sent.
 */
void SimplePushServer::pushMessageToUserList(const int priority,
		const QString &title, const QString &body) {
	QString papFormattedAddresses;
	populateAddresses(papFormattedAddresses);
	pushMessageToSpecifiedUsers(papFormattedAddresses, priority, title, body);
}

/**
 * Both pushMessageToUser() and pushMessageToUserList() will call this method
 *
 */
void SimplePushServer::pushMessageToSpecifiedUsers(
		const QString &papFormattedAddresses, const int priority,
		const QString &title, const QString &body) {
	QString pushMessage;
	generatePushMessage(pushMessage, priority, title, body);

	disconnect(&networkAccessManager, 0, 0, 0);
	connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this,
			SLOT(pushMessageResponse(QNetworkReply*)));

	QNetworkRequest networkRequest = generateBasicNetworkRequest(
			"mss/PD_pushRequest");
	networkRequest.setRawHeader("Content-Type",
			QString(
					"multipart/related; type=\"application/xml\"; boundary="
							+ BLACKBERRY_PUSH_BOUNDARY).toAscii());

	QString postData = readFile("pap_push.template");
	generatePushMessagePostData(postData, papFormattedAddresses, pushMessage);
	networkAccessManager.post(networkRequest, postData.toAscii());
	qDebug() << postData;
}

void SimplePushServer::requestSubscribedUserList() {
	disconnect(&networkAccessManager, 0, 0, 0);
	connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this,
			SLOT(subscriptionQueryResponse(QNetworkReply*)));

	QNetworkRequest networkRequest = generateBasicNetworkRequest(
			"mss/PD_cpSubQuery");
	networkRequest.setRawHeader("Content-Type",
			QString("application/xml").toAscii());

	QString postData = readFile("pap_subscription.template");
	generateSubscriptionQueryPostData(postData);
	networkAccessManager.post(networkRequest, postData.toAscii());
	qDebug() << postData;
}

QNetworkRequest SimplePushServer::generateBasicNetworkRequest(
		const QString & urlSuffix) {
	QNetworkRequest networkRequest;
	networkRequest.setUrl(QUrl(BLACKBERRY_PUSH_URL + urlSuffix));
	QString login = QString("%1:%2").arg(BLACKBERRY_PUSH_APPLICATION_ID).arg(
			BLACKBERRY_PUSH_PASSWORD);
	QByteArray encoded = login.toAscii().toBase64();
	networkRequest.setRawHeader("Authorization", "Basic " + encoded);
	return networkRequest;
}

QString SimplePushServer::readFile(const QString& fileName) {
	QFile file("app/native/assets/templates/" + fileName);
	file.open(QIODevice::ReadOnly);
	QByteArray toPost = file.readAll();
	return toPost;
}

void SimplePushServer::generatePushMessagePostData(QString& templateFileData,
		const QString &address, const QString &pushData) {

	templateFileData.replace("$(boundary)", BLACKBERRY_PUSH_BOUNDARY);
	templateFileData.replace("$(pushid)",
			QUuid::createUuid().toString().right(18).left(17));
	templateFileData.replace("$(username)", BLACKBERRY_PUSH_APPLICATION_ID);
	templateFileData.replace("$(addresses)", address);
	templateFileData.replace("$(deliveryMethod)", "unconfirmed");
	templateFileData.replace("$(headers)", "Content-Type: text/plain");
	templateFileData.replace("$(content)", pushData);
	templateFileData.replace("\r\n", "EOL");
	templateFileData.replace("\n", "EOL");
	templateFileData.replace("EOL", "\r\n");
}

void SimplePushServer::generateSubscriptionQueryPostData(
		QString& templateFileData) {
	templateFileData.replace("$(username)", BLACKBERRY_PUSH_APPLICATION_ID);
	templateFileData.replace("\r\n", "EOL");
	templateFileData.replace("\n", "EOL");
	templateFileData.replace("EOL", "\r\n");
}

void SimplePushServer::populateAddresses(QString& papFormattedAddresses) {
	QSettings settings;
	QVariant temp = settings.value("pins");
	QString tempAddr;
	if (!temp.isNull()) {
		QVariantList addressList = temp.toList();
		foreach(QVariant address, addressList){
		tempAddr = QString(address.toString());
		papFormatAddress(tempAddr);
		papFormattedAddresses.append(tempAddr);
	}
}
}

void SimplePushServer::papFormatAddress(QString &address) {
	address = QString("<address address-value=\"%1\"/>\r\n").arg(address);
}

void SimplePushServer::generatePushMessage(QString& pushMessage,
		const int priority, const QString &title, const QString &body) {
	pushMessage = QString("%1|%2|%3").arg(priority).arg(title).arg(body);
}

void SimplePushServer::log(const QString& message) {
	logTextArea->setText(message);
	qDebug() << message;
}

void SimplePushServer::pushMessageResponse(QNetworkReply* reply) {
	if (reply->error() == QNetworkReply::NoError) {
		log("Push response: \n" + reply->readAll());
	} else {
		log("Failed to send Push: \n" + reply->errorString());
	}
}

void SimplePushServer::subscriptionQueryResponse(QNetworkReply* reply) {
	if (reply->error() == QNetworkReply::NoError) {
		QDomDocument doc;
		doc.setContent(reply->readAll());
		QDomNodeList list = doc.elementsByTagName("address");
		QVariantList pinList;
		QString listOfPins;
		for (int i = 0; i < list.size(); ++i) {
			pinList << list.at(i).toElement().attribute("address-value");
			listOfPins.append(
					list.at(i).toElement().attribute("address-value") + "\n");
		}

		log(QString("PINs received: %1\n%2").arg(list.size()).arg(listOfPins));

		if (list.size() > 0) {
			mainAppPage->setProperty("havePinsBeenReceived", true);
		}

		QSettings settings;
		settings.clear();
		settings.setValue("pins", pinList);
	} else {
		log("Failed to receive PINs: \n" + reply->errorString());
	}
}
