/* Copyright (c) 2012, 2013  BlackBerry Limited.
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


#include "BBMHandler.hpp"

#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/system/SystemDialog>

#include <bb/platform/bbm/Context>
#include <bb/platform/bbm/RegistrationState>
#include <bb/platform/bbm/MessageService>
#include <bb/platform/bbm/UserProfile>

using namespace bb::cascades;
using namespace bb::platform::bbm;
using namespace bb::system;

BBMHandler::BBMHandler(const QUuid &uuid, QObject *parent) :
		QObject(parent), m_isAllowed(false), m_progress(
				BbmRegistrationProgress::NotStarted), m_temporaryError(false), m_statusMessage(
				tr("Please wait while the application connects to BBM.")),
				m_messageService(0)
{
	bool ok = false;
	if (uuid.isNull())
	{
		SystemDialog *uuidDialog = new SystemDialog("OK");
		uuidDialog->setTitle("UUID Error");
		uuidDialog->setBody(
				"Invalid/Empty UUID, please set correctly in tldrapp.cpp");
		ok = connect(uuidDialog,
				SIGNAL(finished(bb::system::SystemUiResult::Type)), this,
				SLOT(dialogFinished(bb::system::SystemUiResult::Type)));
		Q_ASSERT(ok);
		uuidDialog->show();
		return;
	}

	m_context = new Context(uuid, this);

	ok =
			connect(m_context,
					SIGNAL(registrationStateUpdated(bb::platform::bbm::RegistrationState::Type)),
					this,
					SLOT(processRegistrationStatus(bb::platform::bbm::RegistrationState::Type)));

	Q_ASSERT(ok);
}

void BBMHandler::registerApplication()
{
	m_progress = BbmRegistrationProgress::Started;
	processRegistrationStatus(m_context->registrationState());
}

void BBMHandler::processRegistrationStatus(
		const RegistrationState::Type state)
{
	// Based on the state, decide whether we need to register. If we already
	// registered successfully once (i.e. on a previous application run), then
	// we will not call requestRegisterApplication() again.
	qDebug() << "Received a BBM Social Platform registration access state="
			<< state;
	switch (m_progress)
	{
	case BbmRegistrationProgress::Pending:
		if (state != RegistrationState::Pending)
		{
			registrationFinished();
			return;
		}
		// Otherwise, ignore since registration is still in progress.
		break;

	case BbmRegistrationProgress::Started:
		if (m_context->isAccessAllowed())
		{
			// Access is allowed, the application is registered.
			registrationFinished();
			return;
		}
		if (m_context->registrationState() == RegistrationState::Unknown)
		{
			// Status is not yet known. Wait for an event that will deliver the
			// status.
			qDebug() << "BBM Social Platform access state is UNKNOWN; waiting "
					"for the initial status";
			return;
		}
		// Start registration.
		if (m_context->requestRegisterApplication())
		{
			// Registration started. The user will see a dialog informing them
			// that your application is connecting to BBM.
			m_progress = BbmRegistrationProgress::Pending;
			qDebug() << "BBM Social Platform registration started";
			qDebug() << "Verify you are using a valid UUID";
			return;
		}
		// Could not start registration. No dialogs were shown.
		qDebug() << "BBM Social Platform registration could not be started";
		registrationFinished();
		break;

	case BbmRegistrationProgress::Finished:
		if (m_context->isAccessAllowed() != m_isAllowed)
		{
			// Access to the BBM Social Platform has changed.
			registrationFinished();
		}
		break;

	default:
		qDebug() << "Ignoring BBM Social Platform access state=" << state
				<< "when progress=" << m_progress;
		break;
	}
}

void BBMHandler::registrationFinished()
{
	// Finish registration and use the state to decide which message to show
	// the user.
	m_progress = BbmRegistrationProgress::Finished;
	switch (m_context->registrationState())
	{
	case RegistrationState::Allowed:
		m_statusMessage = tr("Application connected to BBM.  Press Continue.");
		m_temporaryError = false;
		break;

	case RegistrationState::BlockedByRIM:
		m_statusMessage = tr("Disconnected by RIM. RIM is preventing this "
				"application from connecting to BBM.");
		m_temporaryError = false;
		break;

	case RegistrationState::BlockedByUser:
		m_statusMessage = tr("Disconnected. Go to Settings -> Security and "
				"Privacy -> Application Permissions and "
				"connect this application to BBM.");
		m_temporaryError = false;
		break;

	case RegistrationState::InvalidUuid:
		// You should be resolving this error at development time.
		m_statusMessage = tr("Invalid UUID. Report this error to the "
				"vendor.");
		m_temporaryError = true;
		break;

	case RegistrationState::MaxAppsReached:
		m_statusMessage = tr("Too many applications are connected to BBM. "
				"Uninstall one or more applications and try "
				"again.");
		m_temporaryError = false;
		break;

	case RegistrationState::Expired:
	case RegistrationState::MaxDownloadsReached:
		m_statusMessage = tr("Cannot connect to BBM. Download this "
				"application from AppWorld to keep using it.");
		m_temporaryError = false;
		break;

	case RegistrationState::NoDataConnection:
		m_statusMessage = tr("Check your Internet connection and try again.");
		m_temporaryError = true;
		break;

	case RegistrationState::Pending:
		// The user will never see this. The BBM Social Platform already
		// displays a "Connecting" dialog.
		m_statusMessage = tr("Connecting to BBM. Please wait.");
		m_temporaryError = false;
		break;

	case RegistrationState::Unknown:
		m_statusMessage = tr("Determining the status. Please wait.");
		m_temporaryError = false;
		break;

	case RegistrationState::Unregistered:
	case RegistrationState::UnexpectedError:
	case RegistrationState::TemporaryError:
	case RegistrationState::CancelledByUser:
	default:
		// If new error codes are added, treat them as temporary errors.
		m_statusMessage = tr("Would you like to connect the application to "
				"BBM?");
		m_temporaryError = true;
		break;
	}

	if (m_context->isAccessAllowed())
	{
		m_isAllowed = true;
	} else
	{
		m_isAllowed = false;
	}
	qDebug() << "Finished BBM Social Platform registration, success="
			<< m_isAllowed << "temporaryError=" << m_temporaryError
			<< " message: " << m_statusMessage;
	emit stateChanged();
}

void BBMHandler::dialogFinished(bb::system::SystemUiResult::Type value)
{
	Application::exit(-1);
}

void BBMHandler::finishRegistration()
{
	qDebug() << "BBM Registration finished!";
	emit registered();
}

void BBMHandler::sendInvite()
{
	if (!m_messageService)
	{
		// Instantiate the MessageService.
		m_messageService = new bb::platform::bbm::MessageService(m_context,
				this);
	}

	// Trigger the invite to download process.
	m_messageService->sendDownloadInvitation();
}

void BBMHandler::setPersonalMessage(const QString& message) {
	if (!m_userProfile) {
		m_userProfile = new bb::platform::bbm::UserProfile(m_context, this);
	}
	m_userProfile->requestUpdatePersonalMessage(message);
}
