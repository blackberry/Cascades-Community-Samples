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

#include <bbmsp/bbmsp_presence.h>
#include <QtCore/QDebug>
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
{
  // These two calls must be made in this order:  bps_initialize() must precede
  // bbmsp_request_events().
  qDebug() << logPrefix << "EventHandler(): bps initialize" << bps_initialize();
  
  // Prepare the library for listening to its events; 0 indicates that we wish
  // to listen for all events.
  qDebug() << logPrefix << "EventHandler(): request events ->" << bbmsp_request_events(0);

  // Start listening for events which come from the domain ID for the BBM Social
  // Platform.
  subscribe(bbmsp_get_domain());
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

  switch (eventCategory) {
  case BBMSP_REGISTRATION:
    qDebug() << logPrefix << "event(): Registration Event on thread:"
             << (int) QThread::currentThreadId();

    handleRegistrationEvent(bbmspEvent, eventType);
    break;

  case BBMSP_USER_PROFILE:
    // This event will occur upon successful registration of the application
    // with the BBM SP Server, or when profile information has changed.
    qDebug() << logPrefix << "event(): Profile List Event on thread:"
             << (int) QThread::currentThreadId();
    //Not used in this sample.
    break;

  case BBMSP_CONNECTION:
    qDebug() << logPrefix << "event(): Connection List Event on thread:"
             << (int) QThread::currentThreadId();
    //Not used in this sample.
    break;

  case BBMSP_CONTACT_LIST:
    qDebug() << logPrefix << "event(): Contact List Event on thread:"
             << (int) QThread::currentThreadId();
    //Not used in this sample.
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
    qDebug() << logPrefix << "handleRegistrationEvent():  BBMSP_SP_EVENT_ACCESS_CHANGED";
    emit bbmsp_register_callback(event);
  }
}




