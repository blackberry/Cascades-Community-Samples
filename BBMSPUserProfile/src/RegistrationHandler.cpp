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
#include "EventHandler.hpp"
#include "Global.hpp"
#include "ProfilePage.hpp"

#include <bbmsp/bbmsp.h>
#include <bbmsp/bbmsp_events.h>
#include <bbmsp/bbmsp_context.h>
#include <bps/bps.h>

#include <bb/cascades/QmlDocument>
#include <bb/cascades/Application>


using namespace bb::cascades;

RegistrationHandler::RegistrationHandler()
{

	  // Create the UI for the registration page and its components.
	  QmlDocument* qmlContent = QmlDocument::create("asset:///registration.qml");
	  if (qmlContent) {
	    Control* content = qmlContent->createRootObject<Control>();
	    this->setContent(content);
	  }

	  // Console Setup -- link BbmspBpsEventHandler to registrationCallback().
	  m_statusText = this->findChild<TextArea*>("statusText");

	  // Retrieve the button
	  m_continueButton = this->findChild<Button*>("continueButton");

	  // Connect the callback for the button.
	  QObject::connect(m_continueButton,
			 SIGNAL(clicked()),
			 this,
			 SLOT(continueToMainAppScreen()));

	  //Setup the registration callback.
	  QObject ::connect(EventHandler::getInstance(),
			SIGNAL(bbmsp_register_callback(bbmsp_event_t*)),
			this,
			SLOT(registrationCallback(bbmsp_event_t*)));

	this->appRegister();
}


void RegistrationHandler::checkRegistrationAccess(bbmsp_access_error_codes_t accessErrorCode)
{
  qDebug() << "Access error code = " << accessErrorCode;

  // This retrieves the registration status of the application.  This determines
  // what level of access we have to the BBM Social Platform library.

  switch(accessErrorCode) {
    // The application has not yet registered with the BBM Social Platform
    // server.

  case BBMSP_ACCESS_ALLOWED:
	qDebug() << "Access allowed";
	m_statusText->setText("BBM SP registration succeeded.  Press Continue.");
	m_continueButton->setVisible(true);
    break;

    case BBMSP_ACCESS_UNREGISTERED:
    	qDebug() << "Unregistered";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_UNKNOWN:
    	qDebug() << "Access failed: access unknown";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_PENDING:
    	qDebug() << "Access failed: access pending";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_BLOCKED_BY_USER:
    	qDebug() << "Access failed: blocked by user";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_BLOCKED_BY_RIM:
    	qDebug() << "Access failed: blocked by RIM";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_UNEXPECTED_ERROR:
    	qDebug() << "Access failed: unexpected error";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_INVALID_UUID:
    	qDebug() << "Access failed: invalid UUID";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_TEMPORARY_ERROR:
    	qDebug() << "Access failed: temporary error";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_MAX_DOWNLOADS_REACHED:
    	qDebug() << "Access failed: max downloads reached";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_EXPIRED:
    	qDebug() << "Access failed: access expired";
    	this->registrationFailed();
      break;

    case BBMSP_ACCESS_CANCELLED_BY_USER:
    	qDebug() << "Access failed: cancelled by user";
    	this->registrationFailed();
      break;

    default:
    	qDebug() << "Access failed: unknown error";
    	this->registrationFailed();
      break;
    }

}

void RegistrationHandler::registrationFailed()
{
	//A generic error message is provided here.
	//You could provide a different error for each failure code to instruct the
	//user on how to continue.
	m_statusText->setText("BBM SP registration failed.  Registration is required connect with BlackBerry Messenger.  Please restart the application to try again.");
	m_continueButton->setVisible(false);
}

void RegistrationHandler::appRegister()
{

  // Attempt to register the application with the following UUID.
  // Every application is required to have its own unique UUID.
  //TODO:  YOU MUST CHANGE THIS UUID!
  //Change this when creating your own application.
  const std::string UUID = "cc6ee480-ec94-11e1-aff1-0800200c9a66";

  // The attempt to register will trigger another set of events, the first of
  // which is the Registration response.  The other events are the Profile event
  // and an event which indicates that the library has received a list of
  // contacts.
  bbmsp_result_t rc = bbmsp_register(UUID.c_str());
  qDebug() << "registered -> " << rc;

}

void RegistrationHandler::registrationCallback(bbmsp_event_t* event)
{
  this->checkRegistrationAccess(bbmsp_event_access_changed_get_access_error_code(event));
}

void RegistrationHandler::continueToMainAppScreen()
{
	g_navPane = new NavigationPane;

	//Your applications main screen would be displayed here:
	Page* initialPage;
	initialPage = new ProfilePage();
	g_navPane->push(initialPage);
	Application::instance()->setScene(g_navPane);
}
