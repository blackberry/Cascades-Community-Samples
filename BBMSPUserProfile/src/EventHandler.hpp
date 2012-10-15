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


#ifndef BBMSPBPSEVENTHANDLER_HPP_
#define BBMSPBPSEVENTHANDLER_HPP_

#include <bb/AbstractBpsEventHandler>

#include <QtCore/QObject>
#include <QtCore/QThread>

#include <bbmsp/bbmsp.h>
#include <bbmsp/bbmsp_events.h>
#include <bbmsp/bbmsp_context.h>
#include <bbmsp/bbmsp_contactlist.h>
#include <bbmsp/bbmsp_userprofile.h>

class EventHandler : public QObject, public bb::AbstractBpsEventHandler
{
  Q_OBJECT

signals:
  void newAvatarImageSelected(QString imagePath);
  void bbmsp_register_callback(bbmsp_event_t* event);
  void bbmsp_profile_changed_callback(bbmsp_profile_t* profile);
  void bbmspGetEcoidCallback(QString newEcoid);
  void userProfileChanged();

public:
  void event(bps_event_t *event);
  static EventHandler* getInstance();
  void newAvatarSelected(QString imagePath);
  void getContactList();
  // Return true if a registration event was received; otherwise, return false.
  bool wasRegistrationEventReceived()
  { return m_wasRegistrationEventReceived; }

  // If access to platform is not allowed, log a message.
  // Return true if we successfully registered with platform; otherwise,
  // return false.
  bool isAccessAllowed();

private:
  EventHandler();
  virtual ~EventHandler(){}
  void handleRegistrationEvent(bbmsp_event_t* event,
                               bbmsp_event_type_t eventType);
  void handleUserProfileEvent(bbmsp_event_t* event,
                              bbmsp_event_type_t eventType);
  void handleContactListEvent(bbmsp_event_t* event,
                              bbmsp_event_type_t eventType);

  // The app must wait to receive a registration event before it can use other
  // APIs.
  // If the event indicates the app is Unregistered, the app can only call
  // bbmsp_register() and wait to receive another registration event before
  // calling other APIs.
  // If the event indicates the app is successfully registered, the app can
  // call any other API.
  // If the event indicates the app is not successfully registered, the app
  // cannot call any other API.
  bool m_wasRegistrationEventReceived;

private:
  static EventHandler* m_BPSEventHandlerInstance;
};

#endif /* BBMSPBPSEVENTHANDLER_HPP_ */
