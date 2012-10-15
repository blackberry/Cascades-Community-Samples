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

#include "EventHandler.hpp"
#include "Contact.hpp"
#include "ProfilesDataManager.hpp"

//#include <bbmsp/bbmsp_presence.h>
#include <QtCore/QDebug>
#include <bb/system/SystemDialog>

#include <bps/navigator.h>
#include <errno.h>

EventHandler* EventHandler::m_BPSEventHandlerInstance = NULL;
static const QString logPrefix("EventHandler::");
EventHandler*
EventHandler::getInstance()
{
  if (!m_BPSEventHandlerInstance) {
    m_BPSEventHandlerInstance = new EventHandler();
  }
  return m_BPSEventHandlerInstance;
}

EventHandler::EventHandler()
  : m_wasRegistrationEventReceived(false)
{
  // These two calls must be made in this order:  bps_initialize() must precede
  // bbmsp_request_events().
  qDebug() << logPrefix << "EventHandler(): bps initialize" << bps_initialize();
  
  // Prepare the library for listening to its events; 0 indicates that we wish
  // to listen for all events.
  const bbmsp_result_t rc = bbmsp_request_events(0);
  if (rc != BBMSP_SUCCESS) {
    qDebug() << logPrefix << "EventHandler(): request events failed " << rc;
    return;
  }
  // Start listening for events which come from the domain ID for the BBM Social
  // Platform.
  subscribe(bbmsp_get_domain());
}

void
EventHandler::getContactList()
{
  qDebug() << logPrefix << "getContactList(): contact list call made from contact manager" <<  bbmsp_contact_list_get();
  qDebug() << logPrefix << "getContactList(): request contact events ->" << bbmsp_event_contact_list_register_event();
}

void
EventHandler::event(bps_event_t *event)
{
  // This method listens for events and, based on the category of each event,
  // marshals it.
  qDebug() << logPrefix
           << "************************************** EVENT RECEIVED **************************************";
  int eventCategory = 0;
  bbmsp_event_type_t eventType;
  bbmsp_event_t* bbmspEvent = 0;

  // We know that \a event it is a bbmsp event because we've registered only for
  // events coming from the BBM Social Platform, so we can extract the category
  // and the type.
  bbmsp_event_get_category(event, &eventCategory);
  bbmsp_event_get_type(event, (int*) (&eventType));
  bbmsp_event_get(event, &bbmspEvent);

  // Pop up a dialog to inform the user we got an event before we got a
  // registration event.
  if (not wasRegistrationEventReceived() &&
    eventCategory != BBMSP_REGISTRATION) {
    bb::system::SystemDialog errorDialog("OK");
    errorDialog.setTitle("Error");
    errorDialog.setBody("Received event=" + QString::number(eventCategory) +
                        " prior to receiving a registration event.");
    errorDialog.show();
  }
  switch (eventCategory) {
  case BBMSP_REGISTRATION:
    qDebug() << logPrefix << "event(): Registration Event on thread:"
             << (int) QThread::currentThreadId();
    m_wasRegistrationEventReceived = true;
    handleRegistrationEvent(bbmspEvent, eventType);
    break;

  case BBMSP_USER_PROFILE:
    // This event will occur upon successful registration of the application
    // with the BBM SP Server, or when profile information has changed.
    qDebug() << logPrefix << "event(): Profile List Event on thread:"
             << (int) QThread::currentThreadId();
    handleUserProfileEvent(bbmspEvent, eventType);
    break;

  case BBMSP_CONNECTION:
    qDebug() << logPrefix << "event(): Connection List Event on thread:"
             << (int) QThread::currentThreadId();
    break;

  case BBMSP_CONTACT_LIST:
    qDebug() << logPrefix << "event(): Contact List Event on thread:"
             << (int) QThread::currentThreadId();
    handleContactListEvent(bbmspEvent, eventType);
    break;


  default:
    break;
  }
}

void
EventHandler::handleRegistrationEvent(bbmsp_event_t* event,
                                              bbmsp_event_type_t eventType)
{
  // We have only one type of action for a registration event.
  if (eventType == BBMSP_SP_EVENT_ACCESS_CHANGED) {
    qDebug() << logPrefix
      << "handleRegistrationEvent(): BBMSP_SP_EVENT_ACCESS_CHANGED, event="
      << bbmsp_event_access_changed_get_access_error_code(event);
    emit bbmsp_register_callback(event);
  }
}

void
EventHandler::handleUserProfileEvent(bbmsp_event_t* event,
                                             bbmsp_event_type_t eventType)
{
  if (eventType == BBMSP_SP_EVENT_PROFILE_CHANGED) {
    qDebug() << "User profile event";

    bbmsp_profile_t* profile = 0;

    // Create an empty profile object to pass to the library.
    bbmsp_profile_create(&profile);
    // Request that the library populate the profile object.
    bbmsp_event_profile_changed_get_profile(event, &profile);

    QString presenceString;

    // Extract which type of update occurred in the profile.
    bbmsp_presence_update_types_t updateType;
    if (bbmsp_event_profile_changed_get_presence_update_type
        (event, &updateType) != BBMSP_SUCCESS) {
      qDebug() << "***ERROR*** cannot get profile update type";
    }
    switch (updateType) {
    case BBMSP_DISPLAY_NAME:
    	ProfilesDataManager::getInstance()->getUserProfile()->setUserProfileDisplayName(profile);
      presenceString = "changed display name";
      break;

    case BBMSP_PERSONAL_MESSAGE:
      ProfilesDataManager::getInstance()->getUserProfile()->setUserProfilePersonalMessage(profile);
      presenceString = "changed personal message";
      break;

    case BBMSP_STATUS:
      ProfilesDataManager::getInstance()->getUserProfile()->setUserProfileStatus(profile);
      presenceString = "changed status";
      break;

    case BBMSP_DISPLAY_PICTURE:
      qDebug() << logPrefix << "handleUserProfileEvent():  BBMSP_DISPLAY_PICTURE";
      ProfilesDataManager::getInstance()->getUserProfile()->setUserAvatar(profile);
      presenceString = "changed display picture";
      break;

    default:
      qDebug() << logPrefix << "handleUserProfileEvent(): ERROR: unknown presence event type=" << updateType;
      break;
    }
  }

  emit userProfileChanged();
}

void
EventHandler::handleContactListEvent(bbmsp_event_t* event,
                                             bbmsp_event_type_t eventType)
{
  bool eventSuccessful = false;

  if (eventType == BBMSP_SP_EVENT_CONTACT_CHANGED) {
    eventSuccessful = true;

    //TODO: check for any extra steps
    qDebug() << "Contact changed event";

    bbmsp_contact_t* contact = 0;
    bbmsp_contact_create(&contact);
    bbmsp_event_contact_changed_get_contact(event, &contact);

    char displayName[BBMSP_CONTACT_DISPLAY_NAME_MAX];
    bbmsp_contact_get_display_name(contact, displayName,
                                   BBMSP_CONTACT_DISPLAY_NAME_MAX);

    QString presenceString;
    bbmsp_presence_update_types_t updateType;
    if (bbmsp_event_contact_changed_get_presence_update_type(event, &updateType)
        != BBMSP_SUCCESS) {
      qDebug() << "***ERROR*** cannot get update type";
    }
    switch (updateType) {
    case BBMSP_DISPLAY_NAME:
      presenceString = "changed display name";
      break;

    case BBMSP_DISPLAY_PICTURE:
      presenceString = "changed display picture";
      break;

    case BBMSP_PERSONAL_MESSAGE:
      presenceString = "changed personal message";
      break;

    case BBMSP_STATUS:
      presenceString = "changed status";
      break;

    case BBMSP_INSTALL_APP:
      presenceString = "installed the app";
      break;

    case BBMSP_UNINSTALL_APP:
      presenceString = "uninstalled the app";
      break;

    case BBMSP_INVITATION_RECEIVED:
      qDebug() << logPrefix << "handleContactListEvent():  Connection List Event on thread:"
               << (int) QThread::currentThreadId();
      break;

    default:
      qDebug() << logPrefix << "handleContactListEvent():  ERROR: unknown presence event type=" << updateType;
      break;
    }

    ProfilesDataManager::getInstance()->updateContact(contact, updateType);
  } else if (eventType == BBMSP_SP_EVENT_CONTACT_LIST_FULL) {
    qDebug() << "CONTACT LIST FULL EVENT**************************************";
      eventSuccessful = true;
      bbmsp_contact_list_t* contactlist;
      bbmsp_event_contact_list_get_full_contact_list(event, &contactlist);
      ProfilesDataManager::getInstance()->contactListReceived(eventSuccessful, contactlist);
  }
}


void
EventHandler::newAvatarSelected(QString imagePath)
{
  emit newAvatarImageSelected(imagePath);
}

bool
EventHandler::isAccessAllowed()
{
  return wasRegistrationEventReceived() &&
         bbmsp_is_access_allowed();
}
