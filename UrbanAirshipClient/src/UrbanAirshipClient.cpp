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

#include "UrbanAirshipClient.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Button>
#include <bb/device/HardwareInfo>
#include <bb/network/PushPayload>

using namespace bb::cascades;

UrbanAirshipClient::UrbanAirshipClient(bb::cascades::Application *app) :
		QObject(app) {
	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	// create root object for the UI
	root = qml->createRootObject<AbstractPane>();
	// set created root object as a scene
	app->setScene(root);


	// There are many controls defined in QML that we need to access from C++,
	// this section finds them based on their objectName properties defined in
	// QML and gives them a handle from C++
	m_createPushSessionStatus = root->findChild<QObject*>(
			"createPushSessionStatus");
	m_createPushChannelStatus = root->findChild<QObject*>(
			"createPushChannelStatus");
	m_registertoLaunchStatus = root->findChild<QObject*>(
			"registertoLaunchStatus");
	m_registerWithUrbanAirshipStatus = root->findChild<QObject*>(
			"registerWithUrbanAirshipStatus");
	m_logger = root->findChild<TextArea*>("logger");
	m_registerActionItem = root->findChild<ActionItem*>("registerActionItem");
	m_unregisterActionItem = root->findChild<ActionItem*>(
			"unregisterActionItem");
	m_activityIndicator = root->findChild<ActivityIndicator*>("activityIndicator");

	// If our application gets invoked (via a Push or otherwiser) this section will
	// call our handleInvoke method with the InvokeRequest data
	connect(&m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
			this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

	// Create a new PushService object using values from fields defined in
	// UrbanAirshipAPI.cpp
	m_pushService = new PushService(UrbanAirshipAPI::applicationId(),
			UrbanAirshipAPI::invokeTargetId());
	connectPushServices();
	m_pushService->createSession();

	connectUrbanAirshipAPIs();

	// This section lets us handle when the user presses the ActionItems
	// defined in main.qml here in UrbanAirshipClient.cpp
	connect(m_registerActionItem, SIGNAL(triggered()), this,
			SLOT(initiateChannelCreation()));
	connect(m_registerActionItem, SIGNAL(triggered()), m_activityIndicator,
			SLOT(start()));
	connect(m_unregisterActionItem, SIGNAL(triggered()), m_pushService,
			SLOT(destroyChannel()));
	connect(m_unregisterActionItem, SIGNAL(triggered()), m_activityIndicator,
			SLOT(start()));

	// For this application text is only written to the log area in QML when the
	// transaction has finished (successful or not), so we stop the ActivityIndicator
	// whenever that occurs.
	connect(m_logger, SIGNAL(textChanged(QString)), m_activityIndicator, SLOT(stop()));

	connect(app, SIGNAL(fullscreen()), &notification,
			SLOT(clearEffectsForAll()));
}


// This section connects the various signals emitted by PushServices to slots
// in this class
void UrbanAirshipClient::connectPushServices() {
	connect(m_pushService,
			SIGNAL(createSessionCompleted(const bb::network::PushStatus&)),
			this, SLOT(createSessionCompleted(const bb::network::PushStatus&)));

	connect(m_pushService,
			SIGNAL(createChannelCompleted(const bb::network::PushStatus&,
							const QString &)), this,
			SLOT(createChannelCompleted(const bb::network::PushStatus&,
							const QString &)));

	connect(m_pushService,
			SIGNAL(destroyChannelCompleted(const bb::network::PushStatus&)),
			this, SLOT(
					destroyChannelCompleted(const bb::network::PushStatus&)));

	connect(m_pushService,
			SIGNAL(registerToLaunchCompleted(const bb::network::PushStatus &)),
			this,
			SLOT(registerToLaunchCompleted(const bb::network::PushStatus&)));

	connect(m_pushService,
			SIGNAL(unregisterFromLaunchCompleted(const bb::network::PushStatus &)),
			this, SLOT(unregisterFromLaunchCompleted(
							const bb::network::PushStatus&)));
}

// This section connects the various signals emitted by UrbanAirshipAPI to slots
// in this class
void UrbanAirshipClient::connectUrbanAirshipAPIs() {
	connect(&m_urbanAirshipAPI,
			SIGNAL(urbanAirshipRegistrationResult(bool, QString)), this,
			SLOT(urbanAirshipRegistrationComplete(bool, QString)));
	connect(&m_urbanAirshipAPI,
			SIGNAL(urbanAirshipUnRegistrationResult(bool, QString)), this,
			SLOT(urbanAirshipUnRegistrationComplete(bool, QString)));
}

// Standard log call, for now we use qDebug but the method makes it easy to
// change or remove this functionality in a future version
void UrbanAirshipClient::log(QString logLine) {
	qDebug() << logLine;
}

void UrbanAirshipClient::createSessionCompleted(const PushStatus &status) {
	if (!status.isError()) {
		log("Successfully created session");
		QMetaObject::invokeMethod(m_createPushSessionStatus, "setState", Q_ARG(QVariant, true));
	} else {
		m_logger->setText("Failed to create session: " + status.errorDescription());
		QMetaObject::invokeMethod(m_createPushSessionStatus, "setState", Q_ARG(QVariant, false));
	}
}

void UrbanAirshipClient::initiateChannelCreation() {
	m_pushService->createChannel(QUrl(UrbanAirshipAPI::pushUrl()));
}

void UrbanAirshipClient::createChannelCompleted(const PushStatus &status,
		const QString &token) {
	if (!status.isError() && m_pushService) {
		QMetaObject::invokeMethod(m_createPushChannelStatus, "setState", Q_ARG(QVariant, true));
		log("Successfully created channel");
		m_pushService->registerToLaunch();

		log("Registering with UrbanAirship");
		m_urbanAirshipAPI.setToken(token);
		m_urbanAirshipAPI.urbanAirshipRegisterPIN();
	} else {
		QMetaObject::invokeMethod(m_createPushChannelStatus, "setState", Q_ARG(QVariant, false));
		m_logger->setText("Failed to create channel: " + status.errorDescription());
	}
}

void UrbanAirshipClient::destroyChannelCompleted(const PushStatus &status) {
	if (!status.isError()) {
		QMetaObject::invokeMethod(m_createPushChannelStatus, "setState", Q_ARG(QVariant, false));
		log("Unregistering from launch");
		m_pushService->unregisterFromLaunch();
		log("Unregistering with UrbanAirship");
		m_urbanAirshipAPI.urbanAirshipUnRegisterPIN();
	} else {
		m_logger->setText("Failed to destroy channel: " + status.errorDescription());
	}
}

void UrbanAirshipClient::registerToLaunchCompleted(
		const bb::network::PushStatus &status) {
	if (!status.isError()) {
		QMetaObject::invokeMethod(m_registertoLaunchStatus, "setState", Q_ARG(QVariant, true));
		log("Register to launch completed successfully");
	} else {
		QMetaObject::invokeMethod(m_registertoLaunchStatus, "setState", Q_ARG(QVariant, false));
		m_logger->setText("Failed to register to launch: " + status.errorDescription());
	}
}

void UrbanAirshipClient::unregisterFromLaunchCompleted(
		const bb::network::PushStatus &status) {
	if (!status.isError()) {
		QMetaObject::invokeMethod(m_registertoLaunchStatus, "setState", Q_ARG(QVariant, false));
		log("Unregister to launch completed successfully");
	} else {
		m_logger->setText("Failed to unregister to launch: " + status.errorDescription());
	}
}

void UrbanAirshipClient::urbanAirshipRegistrationComplete(bool success,
		QString message) {
	if (success) {
		QMetaObject::invokeMethod(m_registerWithUrbanAirshipStatus, "setState", Q_ARG(QVariant, true));
		m_logger->setText("Registration with UrbanAirship complete: \n" + message);
	} else {
		QMetaObject::invokeMethod(m_registerWithUrbanAirshipStatus, "setState", Q_ARG(QVariant, false));
		m_logger->setText("Failed to register with UrbanAirship: \n" + message);
	}
}

void UrbanAirshipClient::urbanAirshipUnRegistrationComplete(bool success,
		QString message) {
	if (success) {
		QMetaObject::invokeMethod(m_registerWithUrbanAirshipStatus, "setState", Q_ARG(QVariant, false));
		m_logger->setText("Unregistration with UrbanAirship complete: \n" + message);
	} else {
		m_logger->setText("Failed to unregister with UrbanAirship: \n" + message);
	}
}

void UrbanAirshipClient::handleInvoke(const InvokeRequest &request) {

	log("Invocation received");

	// In this case we don't care about the invocation unless it is of type Push
	if (request.action().compare("bb.action.PUSH") != 0) {
		return;
	}
	PushPayload payload(request);
	if (payload.isValid()) {
		if (payload.isAckRequired()) {
			m_pushService->acceptPush(payload.id());
		}
		m_logger->setText("<html><em><b>Push Received</b></em><br/>Payload: <br/> " + payload.data() + "</html>");
		notification.notify();
	}
}
