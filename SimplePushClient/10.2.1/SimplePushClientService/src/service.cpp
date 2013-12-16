/* Copyright (c) 2013 BlackBerry Limited.
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

#include "service.hpp"
#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/network/PushStatus>
#include <bb/network/PushPayload>

using namespace bb::platform;
using namespace bb::system;
using namespace bb::network;

// The following strings must be updated to use the values for your Push Services account
const QString Service::BLACKBERRY_PUSH_APPLICATION_ID =
		"Application ID";
const QString Service::BLACKBERRY_PUSH_URL = "http://cpXXXX.pushapi.eval.blackberry.com";

//Update this value as well as the invoke-target id in the bar-descriptor.xml file
const QString Service::BLACKBERRY_INVOKE_TARGET_ID =
		"com.example.SimplePushClientService";

Service::Service(bb::Application * app) :
		QObject(app), m_invokeManager(new InvokeManager(this)), m_pushService(
				new PushService(BLACKBERRY_PUSH_APPLICATION_ID,
						BLACKBERRY_INVOKE_TARGET_ID, app)) {

	m_shouldChannelBeCreated = false;

	//Initiate the session creation so we can start receiving push messages
	m_pushService->createSession();

	//createSession is asynchronous so once completed we can proceed with creating the channel
	connect(m_pushService,
			SIGNAL(createSessionCompleted (const bb::network::PushStatus &)),
			SLOT(createSessionCompleted(const bb::network::PushStatus&)));

	//The InvokeManager will handle the invocation where the push data is received
	m_invokeManager->connect(m_invokeManager,
			SIGNAL(invoked(const bb::system::InvokeRequest&)), this,
			SLOT(handleInvoke(const bb::system::InvokeRequest&)));

	//createChannel is asynchronous so once completed can reister to launch
	connect(m_pushService,
			SIGNAL(createChannelCompleted (const bb::network::PushStatus &, const QString &)),
			SLOT(createChannelCompleted(const bb::network::PushStatus &, const QString &)));

}

/**
 * When the app gets invoked this method will be called and passed the InvokeRequest
 */
void Service::handleInvoke(const bb::system::InvokeRequest & request) {
	if (request.action().compare("bb.action.system.STARTED") == 0) {
		qint64 channelCreationDate =
				m_settings.value("channelValidDate", 0).toLongLong();

		bool isTheChannelStale = QDateTime::fromMSecsSinceEpoch(
				channelCreationDate).daysTo(QDateTime::currentDateTime()) > 45;
		bool doesTheChannelNeedToBeCreated = (channelCreationDate == 0);

		m_shouldChannelBeCreated = isTheChannelStale
				|| doesTheChannelNeedToBeCreated;

	} else if (request.action().compare("bb.action.PUSH") == 0) {
		//If the app is here we know the InvokeRequest contains push data
		PushPayload payload(request);
		if (payload.isValid()) {

			QVariant channelCreationDate = QVariant(
					QDateTime::currentMSecsSinceEpoch());
			m_settings.setValue("channelValidDate", channelCreationDate);

			log("payload is valid, processing now");
			if (payload.isAckRequired()) { //This section is only useful for Push Plus
				log("ACK required, sending");
				m_pushService->acceptPush(payload.id());
			}
			//Read all the push data from the payload
			QString message = payload.data();
			//Pass it to our method which will store the push in a nice format
			logPush(message);

			//For the purposes of this sample only we are expecting that all push data
			// is formatted into 3 parts using a '|' character as a delimiter.
			QStringList messageParts = message.split('|');
			if (messageParts.size() != 3) {
				log(
						"Invalid list length. Expected 3, received "
								+ messageParts.size());
				return;
			}

			handlePush(messageParts.at(1), messageParts.at(2));
		}
	} else if (request.action().compare(
			"com.samples.simplepushclient.CLEAR_HISTORY") == 0) {
		m_settings.setValue("pushList", "");
		qDebug() << bb::Application::instance()->requestExit();
	}
}

/**
 * Once the session is created successfully we can then create the channel (register
 * with the BlackBerry Push Service)
 */
void Service::createSessionCompleted(const bb::network::PushStatus& status) {
	if (!status.isError() && m_pushService) {
		log("Session creation completed successfully");
		if (m_shouldChannelBeCreated) {
			m_pushService->createChannel(BLACKBERRY_PUSH_URL);
		}
	} else {
		log("Session creation failed: " + status.errorDescription());
	}
}

/**
 * If the channel was created successfully we can then register to launch the app
 * so the app opens when a push arrives.
 */
void Service::createChannelCompleted(const bb::network::PushStatus& status,
		const QString& token) {
	Q_UNUSED(token);
	if (!status.isError() && m_pushService) {
		log("Channel creation completed successfully");
		QVariant channelCreationDate = QVariant(
				QDateTime::currentMSecsSinceEpoch());
		m_settings.setValue("channelValidDate", channelCreationDate);
		m_pushService->registerToLaunch();
	} else {
		log("Channel creation failed: " + status.errorDescription());
	}
	qDebug() << bb::Application::instance()->requestExit();
}

// Split the push into its various parts then store to QSettings
void Service::logPush(const QString & pushMessage) {
	m_pushList = m_settings.value("pushList").toList();
	QString pushMessageWithDate = pushMessage;
	pushMessageWithDate = pushMessageWithDate.append(
			"|" + QDateTime::currentDateTime().toString("h:mm:ss ap"));
	m_pushList.append(pushMessageWithDate);
	m_settings.setValue("pushList", m_pushList);
}

/**
 * Handle the push message:
 *-        Fires off a system notification
 *        o        Adds a splat to the app
 *        o        Does whatever the system specifies (sounds, LED, vibration)
 */

void Service::handlePush(const QString & title, const QString & body) {
	Notification* notification = new Notification();
	notification->setTitle(title);
	notification->setBody(body);

	InvokeRequest invokeRequest;
	invokeRequest.setTarget("com.example.SimplePushClient");

	notification->setInvokeRequest(invokeRequest);
	notification->notify();
	qDebug() << bb::Application::instance()->requestExit();
}

//A log method to output to the console
void Service::log(const QString &toLog) {
	qDebug() << toLog;
}

