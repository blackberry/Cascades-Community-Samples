/* Copyright (c) 2013 BlackBerry.
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

#include "FreemiumSampleApp.hpp"

#include <qt4/QtCore/QUuid>

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

FreemiumSampleApp::FreemiumSampleApp(bb::cascades::Application *app) :
		QObject(app), m_purchaseStore(new PurchaseStore(this)) {

	QmlDocument::defaultDeclarativeEngine()->rootContext()->setContextProperty(
			"PurchaseStore", m_purchaseStore);

	//Expose this class to QML using the handle "mainApp", this will let our QML call
	// the code needed to use BBM Invites
	QmlDocument::defaultDeclarativeEngine()->rootContext()->setContextProperty(
			"mainApp", this);

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	// set created root object as a scene
	app->setScene(root);

	// All code below here allows the app to invite users via BBM to download the app
	m_context = new bb::platform::bbm::Context(
			QUuid("bf38c081-c104-4660-a958-618392541a24"), this); //UUID was randomly generated for dev testing
	if (m_context->registrationState()
			!= bb::platform::bbm::RegistrationState::Allowed) {
		connect(m_context, SIGNAL(
				registrationStateUpdated(
						bb::platform::bbm::RegistrationState::Type)), this,
				SLOT(
						registrationStateUpdated(
								bb::platform::bbm::RegistrationState::Type)));
		m_context->requestRegisterApplication();
	}
}

// This method is exposed to QML so we can initiate a BBM download
// invite without switching contexts
void FreemiumSampleApp::inviteUserToDownloadViaBBM() {
	if (m_context->registrationState()
			== bb::platform::bbm::RegistrationState::Allowed) {
		m_messageService->sendDownloadInvitation();
	}
}

void FreemiumSampleApp::registrationStateUpdated(
		bb::platform::bbm::RegistrationState::Type state) {
	if (state == bb::platform::bbm::RegistrationState::Allowed) {
		m_messageService = new bb::platform::bbm::MessageService(m_context,
				this);
	} else if (state == bb::platform::bbm::RegistrationState::Unregistered) {
		m_context->requestRegisterApplication();
	}
}
