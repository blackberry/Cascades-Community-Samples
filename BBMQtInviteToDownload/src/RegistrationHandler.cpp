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

#include "RegistrationHandler.hpp"
#include <bb/platform/bbm/RegistrationState>
#include <bb/platform/bbm/Context>

#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>
#include <bb/cascades/Application>

#include "Global.hpp"

using namespace bb::cascades;

RegistrationHandler::RegistrationHandler()
{
	// Create the UI for the registration page and its components.
	QmlDocument* qmlContent = QmlDocument::create("asset:///registration.qml");
	if (qmlContent)
	{
		Control* content = qmlContent->createRootObject<Control>();
		this->setContent(content);
	}

	// Console Setup -- link BbmspBpsEventHandler to registrationCallback().
	m_statusText = this->findChild<TextArea*>("statusText");

	// Retrieve the button
	m_continueButton = this->findChild<Button*>("continueButton");

	// Connect the callback for the button.
	QObject::connect(m_continueButton, SIGNAL(clicked()), this,
			SLOT(continueToMainAppScreen()));

	// Attempt to register the application with the following UUID.
	//TODO:  Define your own UUID here.  You can generate one here: http://www.guidgenerator.com/
	m_uuid = QString::fromUtf8("4c907a6e-cbbb-44dd-ac58-75f0408d0167"); //original

	this->appRegister();

}

void RegistrationHandler::appRegister()
{
	qDebug() << "entering appRegister -> " << m_uuid;
	qDebug() << "before new Context";
	m_context = new bb::platform::bbm::Context(QUuid(m_uuid));
	Global::instance()->setContext(m_context);
	QObject::connect(m_context,
			SIGNAL(registrationStateUpdated(bb::platform::bbm::RegistrationState::Type)),
			this,
			SLOT(registrationStatus(bb::platform::bbm::RegistrationState::Type)));
	qDebug() << "after new Context and connect";
	m_context->requestRegisterApplication();
	qDebug() << "registered -> " << m_uuid;
}

void RegistrationHandler::checkRegistrationAccess()
{
	bb::platform::bbm::RegistrationState::Type status =
			bb::platform::bbm::RegistrationState::Unknown;
	if (Global::instance()->getContext())
	{
		status = Global::instance()->getContext()->registrationState();
	}
	registrationStatus(status);
}

void RegistrationHandler::registrationStatus(
		bb::platform::bbm::RegistrationState::Type state)
{
	qDebug() << "Registration Status received";

	switch (state)
	{
	case bb::platform::bbm::RegistrationState::Unregistered:
		qDebug() << ("Unregistered");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::Unknown:
		qDebug() << ("Access failed: unknown reason");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::Allowed:
		qDebug() << "RegistrationPage::registrationStatus Access allowed";
		m_statusText->setText(
				"BBM SP registration succeeded.  Press Continue.");
		m_continueButton->setVisible(true);
		break;

	case bb::platform::bbm::RegistrationState::Pending:
		qDebug() << "RegistrationState::Pending";
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::BlockedByUser:
		qDebug() << ("Access failed: blocked by user");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::BlockedByRIM:
		qDebug() << ("Access failed: blocked by RIM");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::NoDataConnection:
		qDebug() << ("Access failed: no data coverage");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::UnexpectedError:
		qDebug() << ("Access failed: unexpected error");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::InvalidUuid:
		qDebug() << ("Access failed: invalid UUID");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::TemporaryError:
		qDebug() << ("Access failed: temporary error");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::CancelledByUser:
		qDebug() << ("Access cancelled by user");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::MaxDownloadsReached:
		qDebug() << ("Access failed: max downloads reached");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::MaxAppsReached:
		qDebug() << ("Access failed: max apps reached");
		this->registrationFailed();
		break;

	case bb::platform::bbm::RegistrationState::Expired:
		qDebug() << ("Access failed: access expired");
		this->registrationFailed();
		break;

	default:
		qDebug() << ("Access failed: other errors");
		this->registrationFailed();
		break;
	}
}

void RegistrationHandler::registrationFailed()
{
	//A generic error message is provided here.
	//You could provide a different error for each failure code to instruct the
	//user on how to continue.
	m_statusText->setText(
			"BBM SP registration failed.  Registration is required connect with BlackBerry Messenger.  Please restart the application to try again.");
	m_continueButton->setVisible(false);
}

void RegistrationHandler::continueToMainAppScreen()
{
	//Display the invite to download screen.
	Page* invitePage = new Page;
	QmlDocument* qmlContent = QmlDocument::create("asset:///invite.qml");
	if (qmlContent)
	{
		Control* content = qmlContent->createRootObject<Control>();
		invitePage->setContent(content);
		Application::instance()->setScene(invitePage);
	}

}
