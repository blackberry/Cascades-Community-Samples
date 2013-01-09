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

#include "PushManager.hpp"
#include <bb/network/PushPayload>
#include <bb/system/SystemUiButton>
#include <bb/system/ApplicationStartupMode>
#include <bb/IconBadge>

using namespace bb::cascades;
using namespace bb::platform;
using namespace bb::network;
using namespace bb::system;

// The following strings must be updated to use the values for your Push Services account
const QString PushManager::BLACKBERRY_PUSH_APPLICATION_ID =
		"Application ID";
const QString PushManager::BLACKBERRY_PUSH_URL =
		"http://pushapi.eval.blackberry.com";

//Update this value as well as the invoke-target id in the bar-descriptor.xml file
const QString PushManager::BLACKBERRY_INVOKE_TARGET_ID = "com.samples.push.simple";

PushManager::PushManager(Application *app) :
		app(app) {

	hasBeenFullScreened = false; //This bool tracks when the app has been fullscreened

	//Create a push service so the device knows how to invoke our app when a push arrives
	m_pushService = new PushService(BLACKBERRY_PUSH_APPLICATION_ID,
			BLACKBERRY_INVOKE_TARGET_ID);

	//Set up our NotificationDialog with a button to dismiss
	notificationDialog.appendButton(new SystemUiButton("OK"));

	//The InvokeManager will handle the invocation where the push data is received
	invokeManager = new InvokeManager(this);
	connect(invokeManager,
			SIGNAL(invoked(const bb::system::InvokeRequest&)), this,
			SLOT(invoked(const bb::system::InvokeRequest&)));

	//createSession is asynchronous so once completed we can proceed with creating the channel
	connect(m_pushService,
			SIGNAL(createSessionCompleted (const bb::network::PushStatus &)),
			this,
			SLOT(createSessionCompleted(const bb::network::PushStatus&)));

	//createChannel is asynchronous so once completed can reister to launch
	connect(m_pushService,
			SIGNAL(createChannelCompleted (const bb::network::PushStatus&,
							const QString &)), this,
			SLOT(createChannelCompleted(const bb::network::PushStatus&,
							const QString &)));

	//Connect to the app's fullscreen() method so we know if the app has been used by the user yet
	connect(app, SIGNAL(fullscreen()), this, SLOT(appFullScreened()));

	//Initiate the session creation so we can start reciving push messages
	m_pushService->createSession();
}

PushManager::~PushManager() {
}

/**
 * Once the session is created successfully we can then create the channel (register
 * with the BlackBerry Push Service)
 */
void PushManager::createSessionCompleted(
		const bb::network::PushStatus& status) {
	if (!status.isError() && m_pushService) {
		log("Session creation completed successfully");

		//If the app was launched by a push incoming then we know the channel is already created
		if (appWasLaunchedFromRibbon()) {
			m_pushService->createChannel(
					QUrl(BLACKBERRY_PUSH_URL));
		}
	} else {
		log("Session creation failed: " + status.errorDescription());
	}
}

/**
 * If the channel was created successfully we can then register to launch the app
 * so the app opens when a push arrives.
 */
void PushManager::createChannelCompleted(const bb::network::PushStatus& status,
		const QString& token) {
	Q_UNUSED(token);
	if (!status.isError() && m_pushService) {
		log("Channel creation completed successfully");
		m_pushService->registerToLaunch();
	} else {
		log("Channel creation failed: " + status.errorDescription());
	}
}

/**
 * When the app gets invoked this method will be called and passed the InvokeRequest
 */
void PushManager::invoked(const InvokeRequest& request) {
	//Check whether the app was invoked via a push
	if (request.action().compare("bb.action.PUSH") != 0) {
		log("Not a push invocation :("); //Nope, so we return
		return;
	}
	//If the app is here we know the InvokeRequest contains push data
	PushPayload payload(request);
	if (payload.isValid()) {
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

		//The first part of the push denotes the priority of the message
		switch (messageParts.at(0).toInt()) {
		case PushManager::Low:
			handleLowPriorityPush();
			break;
		case PushManager::Medium:
			handleMediumPriorityPush(messageParts.at(1), messageParts.at(2));
			break;
		case PushManager::High:
			handleHighPriorityPush(messageParts.at(1), messageParts.at(2));
			break;
		default:
			break;
		}
	}
}

//Check whether the app was launched by the user of via invocation
bool PushManager::appWasLaunchedFromRibbon() {
	// If the application was launched after receiving a push then we know the channel
	// is already created and our application is already registered to launch, otherwise
	// we should likely create these.
	return (invokeManager->startupMode()
			== ApplicationStartupMode::LaunchApplication);
}

// Split the push into its various parts then store to QSettings
void PushManager::logPush(const QString & pushMessage) {
	pushList = settings.value("pushList").toList();
	QString pushMessageWithDate = pushMessage;
	pushMessageWithDate = pushMessageWithDate.append(
			"|" + QDateTime::currentDateTime().toString("h:mm:ss ap"));
	pushList.append(pushMessageWithDate);
	settings.setValue("pushList", pushList);

	//This 'emit' lets anyone listening know that the DataModel has been updated
	emit pushListChanged(pushList);
}

/**
 * Low priority messages will:
 *-	Add a splat to the app icon
 *-	Closes the app if it has not yet been fullscreened by the user
 */

void PushManager::handleLowPriorityPush() {
	app->setIconBadge(bb::IconBadge::Splat);
	if (!appWasLaunchedFromRibbon() && !hasBeenFullScreened) {
		app->requestExit();
	}
}

/**
 * Medium priority messages will:
 *-	Fires off a system notification
 *	o	Adds a splat to the app
 *	o	Does whatever the system specifies (sounds, LED, vibration)
 *-	Closes the app if it has not yet been fullscreened by the user
 */
void PushManager::handleMediumPriorityPush(const QString & title,
		const QString & body) {
	Q_UNUSED(title);
	Q_UNUSED(body);
	notification.notify();
	if (!appWasLaunchedFromRibbon() && !hasBeenFullScreened) {
		app->requestExit();
	}
}

/**
 * High priority messages will:
 *	Fire off a repeating system notification via a NotificationDialog (global)
 *	o	Adds a splat to the app
 *	o	Does whatever the system specifies (sounds, LED, vibration)
 *		+	Repeats the set action non-stop until the user dismisses the dialog
 *-	App stays open
 */

void PushManager::handleHighPriorityPush(const QString & title,
		const QString & body) {
	notificationDialog.cancel();
	notificationDialog.setTitle(title);
	notificationDialog.setBody(body);
	notificationDialog.setRepeat(true);
	notificationDialog.show();
}

//When the app goes full screen let's clear the notificiations and toggle
// the tracking variable
void PushManager::appFullScreened() {
	notification.clearEffectsForAll();
	app->setIconBadge(bb::IconBadge::None);
	hasBeenFullScreened = true;
}

//Wipe out the data store erasing all existing push messages
void PushManager::clearPushList() {
	log("Clearing push list from cache");
	settings.clear();
	pushList = settings.value("pushList").toList();
	emit pushListChanged(pushList);
}

//This method retrieves all push messages, updating the list, then notifies
// all listeners of the new list.
void PushManager::updatePushList() {
	pushList = settings.value("pushList").toList();
	emit pushListChanged(pushList);
}

//A log method to output to the console
void PushManager::log(const QString &toLog) {
	qDebug() << toLog;
}

