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

#ifndef USERPROFILE_HPP_
#define USERPROFILE_HPP_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QImage>

#include "bbmsp/bbmsp_userprofile.h"

class UserProfile : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString handle READ getHandle NOTIFY handleChanged)
  Q_PROPERTY(QString ppid READ getPpid NOTIFY ppidChanged)
  Q_PROPERTY(QString appVersion READ getAppVersion NOTIFY appVersionChanged)
  Q_PROPERTY(QString displayName READ getDisplayName NOTIFY displayNameChanged)
  Q_PROPERTY(QString personalMessage READ getPersonalMessage NOTIFY personalMessageChanged)
  Q_PROPERTY(QString statusMessage READ getStatusMessage NOTIFY statusMessageChanged)
  Q_PROPERTY(int status READ getStatus NOTIFY statusChanged)
  Q_PROPERTY(int platformVersion READ getPlatformVersion NOTIFY platformVersionChanged)

signals:
  void handleChanged(QVariant);
  void ppidChanged(QVariant);
  void appVersionChanged(QVariant);
  void displayNameChanged(QVariant);
  void personalMessageChanged(QVariant);
  void statusMessageChanged(QVariant);
  void statusChanged(QVariant);
  void platformVersionChanged(QVariant);

public slots:
  // Request the avatar from the library
  void requestUserAvatar();

public:
  UserProfile();

  // Call the library to change the status, personal message, avatar
  void updateStatusMessage(QString statusMessage, bool showBusy);
  void updatePersonalMessage(QString personalMessage);
  bool setAvatar(QString imagePath);

  // These methods are setters when we get call-backs from the lib
  void setUserProfileDisplayName(bbmsp_profile_t* profile);
  void setUserProfileStatus(bbmsp_profile_t* profile);
  void setUserProfilePersonalMessage(bbmsp_profile_t* profile);
  void setUserAvatar(bbmsp_profile_t* profile);

  // These methods are used to retrieve the profile fields from the model
  QString getHandle();
  QString getPpid();
  QString getAppVersion();
  QString getDisplayName();
  QString getPersonalMessage();
  QString getStatusMessage();
  int getStatus();
  int getPlatformVersion();

private:
  bbmsp_profile_t* m_profile;
  bbmsp_image_t* m_avatar;
};

#endif /* USERPROFILE_HPP_ */
