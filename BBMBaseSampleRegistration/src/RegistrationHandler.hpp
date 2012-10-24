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

#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QDebug>

#include <bbmsp/bbmsp.h>
#include <bbmsp/bbmsp_events.h>
#include <bbmsp/bbmsp_context.h>
#include <bps/bps.h>

#include <bb/cascades/Button>
#include <bb/cascades/Page>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/TextArea>




class RegistrationHandler: public bb::cascades::Page
{

	Q_OBJECT

public slots:
  void appRegister();
  void registrationCallback(bbmsp_event_t* event);
  void continueToMainAppScreen();

public:
	RegistrationHandler();
	virtual ~RegistrationHandler(){};

private:
	void checkRegistrationAccess(bbmsp_access_error_codes_t errorCode);
	void registrationFailed();
	bb::cascades::TextArea* m_statusText;
	bb::cascades::Button* m_continueButton;
};

#endif /* REGISTRATIONHANDLER_H_ */
