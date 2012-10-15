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

#include <bb/platform/bbm/ImageType>
#include "ProfilePage.hpp"
#include "Global.hpp"
#include "UpdateProfilePage.hpp"

#include <bb/cascades/ActionItem>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Button>
#include <bb/cascades/TapHandler>
#include <bb/cascades/Container>
#include <bb/cascades/Image>

using namespace bb::cascades;

ProfilePage::ProfilePage()
{
  // Inflate the UI components.
  QmlDocument* qmlContent = QmlDocument::create("asset:///profile.qml");
  if (qmlContent) {
    Control* content = qmlContent->createRootObject<Control>();
    this->setContent(content);
  }
  if (not Global::instance()->isAccessAllowed()) {
    qDebug() << "Access is not allowed, exiting ProfilePage early.";
    return;
  }
  // Grab references to the UI components.
  m_statusMessageLabel = this->findChild<TextArea*>("statusMessage");
  m_personalMessageLabel = this->findChild<TextArea*>("personalMessage");
  m_busyIconImageView = this->findChild<ImageView*>("busyIcon");
  m_avatarImageView = this->findChild<ImageView*>("avatarImageView");
  m_displayName = this->findChild<TextArea*>("nameLabel");

  // Grab the user's Profile data and populate the fields.
  m_userProfile = new bb::platform::bbm::UserProfile(Global::instance()->getContext(), this);

   m_displayName->setText(m_userProfile->displayName());

  QString personalMsgString = "Personal Message: " + m_userProfile->personalMessage();
  qDebug() << "personalMsgString : " << personalMsgString ;
  m_personalMessageLabel->setText(personalMsgString);
  m_busyIconImageView->setVisible(m_userProfile->status() == bb::platform::bbm::UserStatus::Busy ? true : false  );

  QString statusMsgString = "Status Message: " + m_userProfile->statusMessage();
  m_statusMessageLabel->setText(statusMsgString);

  TextArea* ppidLabel = this->findChild<TextArea*>("ppid");
  QString ppidString = "Ppid: " + m_userProfile->ppId();
  qDebug() << "ppidString : " << ppidString ;
  ppidLabel->setText(ppidString);

  TextArea* handleLabel = this->findChild<TextArea*>("handle");
  QString handleString = "Handle: " + m_userProfile->handle();
  handleLabel->setText(handleString);

  TextArea* appVersionLabel = this->findChild<TextArea*>("appVersion");
  QString appVersionString = "App version: " + m_userProfile->applicationVersion();
  appVersionLabel->setText(appVersionString);

  TextArea* platformVersionLabel = this->findChild<TextArea*>(
      "platformVersion");
  QString platformVersionString = "Platform version: "
      + QString::number(m_userProfile->sdkVersion(), 10);
  platformVersionLabel->setText(platformVersionString);


  // Retrieve the button for fetching the user's avatar.
  Button* getAvatarButton = this->findChild<Button*>("getAvatarButton");
  QObject::connect(getAvatarButton,
                   SIGNAL(clicked()),
                   this,
                   SLOT(requestUserAvatar()));

  QObject::connect(m_userProfile,
                   SIGNAL(displayNameUpdated(const QString)),
                   this,
                   SLOT(profileChanged()));
  QObject::connect(m_userProfile,
                   SIGNAL(personalMessageUpdated(const QString)),
                   this,
                   SLOT(profileChanged()));
  QObject::connect(m_userProfile,
                   SIGNAL(statusUpdated(const bb::platform::bbm::UserStatus::Type, const QString)),
                   this,
                   SLOT(profileChanged()));

  ActionItem* updateItem = ActionItem::create().title("Update profile");
  this->addAction(updateItem);
  QObject::connect(updateItem, SIGNAL(triggered()), this,
                   SLOT(updateProfile()));
}

void
ProfilePage::profileChanged()
{
	qDebug() << "ProfilePage::profileChanged";
  m_displayName->setText(m_userProfile->displayName());


  QString personalMsgString = "Personal Message: " + m_userProfile->personalMessage();
  m_personalMessageLabel->setText(personalMsgString);


  QString statusMsgString = "Status Message: " + m_userProfile->statusMessage();
  m_statusMessageLabel->setText(statusMsgString);

  QFile tempFile(QDir::homePath() + "/profileAvatar.jpg");
  if(tempFile.exists())
  {
    m_avatarImageView->setImageSource(QUrl::fromLocalFile(QDir::homePath() + "/profileAvatar.jpg"));
  }

}

void
ProfilePage::requestUserAvatar()
{
	qDebug() << "ProfilePage::requestAvatar" << m_userProfile->displayPictureMimeType() ;
	QByteArray avatarData = m_userProfile->displayPicture();
	bb::platform::bbm::ImageType::Type avatarMT = m_userProfile->displayPictureMimeType();

	QString fileExt;
	switch (avatarMT) {
		case bb::platform::bbm::ImageType::Bmp:
			fileExt = ".bmp";
			break;
		case bb::platform::bbm::ImageType::Jpg:
			fileExt = ".jpg";
			break;
		case bb::platform::bbm::ImageType::Png:
			fileExt = ".png";
			break;
		case bb::platform::bbm::ImageType::Gif:
			fileExt = ".gif";
			break;
		default:
			fileExt = ".jpg";
			break;
	}

	const QString upf(QDir::homePath() + "/" + m_userProfile->displayName() + fileExt);
	QFile f(upf);
	      if (f.exists())
	          f.remove();
	      if (f.open(QFile::WriteOnly))
	      {
	          f.write(avatarData); // holds only 0..f nothing special.
	      }
	      else
	          qDebug("failed to open file upf");
	      f.close();
	m_avatarImageView->setImageSource(QUrl::fromLocalFile(upf));
	qDebug() << "ProfilePage::requestAvatar" << avatarData << " end";

	qDebug() << "ProfilePage::requestAvatar" << avatarMT << " end";
}

void
ProfilePage::updateProfile()
{
  UpdateProfilePage *updateProfilePage = new UpdateProfilePage(m_userProfile);
  g_navPane->push(updateProfilePage);
}

