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
#include "ProfilesDataManager.hpp"
#include "ProfilePage.hpp"
#include "Global.hpp"
#include "UpdateProfilePage.hpp"

#include <bb/cascades/ActionItem>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Button>
#include <bb/cascades/TapHandler>
#include <bb/cascades/Container>
#include <bb/cascades/NavigationPaneProperties>

using namespace bb::cascades;

ProfilePage::ProfilePage()
{

  // Inflate the UI components.
  QmlDocument* qmlContent = QmlDocument::create("asset:///profile.qml");
  if (qmlContent) {
    Control* content = qmlContent->createRootObject<Control>();
    this->setContent(content);
  }

  // Grab references to the UI components.
  m_statusMessageLabel = this->findChild<TextArea*>("statusMessage");
  m_personalMessageLabel = this->findChild<TextArea*>("personalMessage");
  m_busyIconImageView = this->findChild<ImageView*>("busyIcon");
  m_displayName = this->findChild<TextArea*>("nameLabel");
  m_avatarImageView = this->findChild<ImageView*>("avatarImageView");

  // Grab the user's Profile data and populate the fields.
  UserProfile* userProfile = ProfilesDataManager::getInstance()->getUserProfile();

  m_displayName->setText(userProfile->getDisplayName());

  QString personalMsgString = "Personal Message: " + userProfile->getPersonalMessage();
  m_personalMessageLabel->setText(personalMsgString);

  m_busyIconImageView->setVisible(userProfile->getStatus());

  QString statusMsgString = "Status Message: " + userProfile->getStatusMessage();
  m_statusMessageLabel->setText(statusMsgString);

  TextArea* ppidLabel = this->findChild<TextArea*>("ppid");
  QString ppidString = "Ppid: " + userProfile->getPpid();
  ppidLabel->setText(ppidString);

  TextArea* handleLabel = this->findChild<TextArea*>("handle");
  QString handleString = "Handle: " + userProfile->getHandle();
  handleLabel->setText(handleString);

  TextArea* appVersionLabel = this->findChild<TextArea*>("appVersion");
  QString appVersionString = "App version: " + userProfile->getAppVersion();
  appVersionLabel->setText(appVersionString);

  TextArea* platformVersionLabel = this->findChild<TextArea*>(
      "platformVersion");
  QString platformVersionString = "Platform version: "
      + QString::number(userProfile->getPlatformVersion(), 10);
  platformVersionLabel->setText(platformVersionString);

  // Retrieve the button for fetching the user's avatar.
  Button* getAvatarButton = this->findChild<Button*>("getAvatarButton");
  QObject::connect(getAvatarButton,
                   SIGNAL(clicked()),
                   ProfilesDataManager::getInstance()->getUserProfile(),
                   // TODO:  Maybe move requestUserAvatar() to UserProfile
                   SLOT(requestUserAvatar()));



  // Notify the UI that the User Profile has changed.
  QObject::connect(EventHandler::getInstance(),
                   SIGNAL(userProfileChanged()),
                   this,
                   // TODO:  Move profileChanged() to the ProfileDataManager
                   SLOT(profileChanged()));

  ActionItem* updateItem = ActionItem::create().title("Update profile");
  this->addAction(updateItem);
  QObject::connect(updateItem, SIGNAL(triggered()), this,
                   SLOT(updateProfile()));

}

void
ProfilePage::profileChanged()
{
  // Grab the updated profile information and populate the on-screen fields.
  UserProfile* userProfile = ProfilesDataManager::getInstance()->getUserProfile();

  m_displayName->setText(userProfile->getDisplayName());

  QString personalMsgString = "Personal Message: " + userProfile->getPersonalMessage();
  m_personalMessageLabel->setText(personalMsgString);

  m_busyIconImageView->setVisible(userProfile->getStatus());

  QString statusMsgString = "Status Message: " + userProfile->getStatusMessage();
  m_statusMessageLabel->setText(statusMsgString);

  QFile tempFile(QDir::homePath() + "/profileAvatar.jpg");
  if(tempFile.exists())
  {
    m_avatarImageView->setImageSource(QUrl::fromLocalFile(QDir::homePath() + "/profileAvatar.jpg"));
  }

}

void
ProfilePage::updateProfile()
{
  UpdateProfilePage *updateProfilePage = new UpdateProfilePage();
  g_navPane->push(updateProfilePage);
}




