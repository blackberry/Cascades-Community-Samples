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


#ifndef UPDATEPROFILEPAGE_HPP_
#define UPDATEPROFILEPAGE_HPP_

#include <bb/platform/bbm/UserProfile>
#include <QtCore/QObject>
#include <bb/cascades/Page>

#include <bb/cascades/CheckBox>
#include <bb/cascades/TextField>
#include <bb/cascades/Button>

class UpdateProfilePage: public bb::cascades::Page {
  Q_OBJECT

public slots:
  void saveStatus();
  void savePersonalMessage();

public:
  UpdateProfilePage(bb::platform::bbm::UserProfile* userProfile);

private:
  bb::cascades::TextField* m_personalMessageField;
  bb::cascades::TextField* m_statusMessageField;
  bb::cascades::CheckBox* m_showBusyCheckBox;
  bb::platform::bbm::UserProfile * m_userProfile;

};

#endif /* UPDATEPROFILEPAGE_HPP_ */
