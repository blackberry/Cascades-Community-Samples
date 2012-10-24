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


#ifndef REGISTRATIONHANDLER_H_
#define REGISTRATIONHANDLER_H_

#include <bb/platform/bbm/Context>
#include <bb/platform/bbm/RegistrationState>

#include <bb/cascades/TextArea>
#include <bb/cascades/Button>
#include <bb/cascades/Page>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/TextArea>

#include <QtCore/QObject>
#include <QtCore/QMetaType>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>
#include <assert.h>

class RegistrationHandler : public bb::cascades::Page
{
  Q_OBJECT

public slots:
  void appRegister();
  void registrationStatus(bb::platform::bbm::RegistrationState::Type state);
  void continueToMainAppScreen();

public:
  RegistrationHandler();

private:
  void checkRegistrationAccess();
  void registrationFailed();
  bb::cascades::Button* m_continueButton;
  bb::cascades::TextArea* m_statusText;
  bb::platform::bbm::Context* m_context;
  QString m_uuid;
};

Q_DECLARE_METATYPE(RegistrationHandler*)

#endif /* REGISTRATIONHANDLER_H_ */
