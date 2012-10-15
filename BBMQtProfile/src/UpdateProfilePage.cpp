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

#include "UpdateProfilePage.hpp"

#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>

using namespace bb::cascades;

UpdateProfilePage::UpdateProfilePage(bb::platform::bbm::UserProfile* userProfile) : m_userProfile(userProfile)
{
  QmlDocument* qmlContent = QmlDocument::create("asset:///UpdateProfile.qml");
  if (qmlContent) {
    Control* content = qmlContent->createRootObject<Control>();
    this->setContent(content);
  }

  m_personalMessageField = this->findChild<TextField*>("personalMessageField");
  m_statusMessageField = this->findChild<TextField*>("statusMessageField");
  m_showBusyCheckBox = this->findChild<CheckBox*>("showBusyCheckBox");

  Button* saveStatusButton = this->findChild<Button*>("saveStatusButton");
  Button* savePersonalMsgButton = this->findChild<Button*>(
    "savePersonalMsgButton");

  // The user may set a custom status (instead of 'Available', etc.).
  QObject::connect(saveStatusButton,
                   SIGNAL(clicked()),
                   this,
                   SLOT(saveStatus()));

  // The user may also set a personal message (e.g. 'I'm at the beach').
  QObject::connect(savePersonalMsgButton,
                   SIGNAL(clicked()),
                   this,
                   SLOT(savePersonalMessage()));
}

void
UpdateProfilePage::saveStatus()
{
  qDebug() << "SAVE STATUS BUTTON PRESSED";
  QString statusMessageString = m_statusMessageField->text();
  bool showBusy = m_showBusyCheckBox->isChecked();
  m_userProfile->requestUpdateStatus((showBusy? bb::platform::bbm::UserStatus::Busy : bb::platform::bbm::UserStatus::Available), statusMessageString);
}

void
UpdateProfilePage::savePersonalMessage()
{
  qDebug() << "SAVE PERSONAL MSG BUTTON PRESSED";
  QString personalMessageString = m_personalMessageField->text();

  m_userProfile->requestUpdatePersonalMessage(personalMessageString);
}

