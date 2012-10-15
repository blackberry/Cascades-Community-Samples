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

#ifndef EVENTHANDLER_HPP_
#define EVENTHANDLER_HPP_

#include <bb/AbstractBpsEventHandler>

#include <QtCore/QObject>
#include <QtCore/QThread>

#include <bbmsp/bbmsp.h>
#include <bbmsp/bbmsp_events.h>
#include <bbmsp/bbmsp_context.h>

class EventHandler : public QObject, public bb::AbstractBpsEventHandler
{
  Q_OBJECT

signals:
  void bbmsp_register_callback(bbmsp_event_t* event);

public:
  void event(bps_event_t *event);
  static EventHandler* getInstance();
  void newAvatarSelected(QString imagePath);
  void getContactList();

private:
  EventHandler();
  virtual ~EventHandler(){}
  void handleRegistrationEvent(bbmsp_event_t* event,
                               bbmsp_event_type_t eventType);

private:
  static EventHandler* m_BPSEventHandlerInstance;
};

#endif /* EVENTHANDLER_HPP_ */
