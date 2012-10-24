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

#include "UserProfile.hpp"

#include <QtCore/QDebug>



UserProfile::UserProfile()
{
  m_profile = 0;
  // Create an empty profile object for the library to use.
  bbmsp_profile_create(&m_profile);

  // Populate the profile object.  Because we already get the user profile on
  // app start-up, we can safely put this method in the constructor.
  bbmsp_get_user_profile(m_profile);
  qDebug() << "********UserProfile: getting user profile from library*********";
}

QString UserProfile::getHandle() {
  char handle[BBMSP_PROFILE_HANDLE_MAX];
  // Retrieve from the library the handle for the current app.
  if(BBMSP_FAILURE == bbmsp_profile_get_handle(m_profile,
                                               handle,
                                               BBMSP_PROFILE_HANDLE_MAX)){
	  handle[0] = 0;
	  qDebug() << "UserProfile::getHandle()" << "Error:Failed to get Handle.";
  }
  return QString::fromUtf8(handle);
}

QString UserProfile::getPpid() {
  char ppid[BBMSP_PROFILE_PPID_MAX];
  // Retrieve from the library the ppid for the current app.
  if(BBMSP_FAILURE == bbmsp_profile_get_ppid(m_profile, ppid,
                                             BBMSP_PROFILE_PPID_MAX)){
	  ppid[0] = 0;
	  qDebug() << "UserProfile::getPpid() " << "Error:Failed to get Ppid.";
  }
  return QString::fromUtf8(ppid);
}

QString UserProfile::getAppVersion() {
  char appVersion[1025];
  if(BBMSP_FAILURE == bbmsp_profile_get_app_version(m_profile, appVersion, 1024)){
	  appVersion[0] = 0;
	  qDebug() << "UserProfile::getAppVersion() " << "Error:Failed to get AppVersion.";
  }
  return QString::fromUtf8(appVersion);
}

QString UserProfile::getDisplayName() {
  char displayName[BBMSP_PROFILE_DISPLAY_NAME_MAX];
  if(BBMSP_FAILURE ==
      bbmsp_profile_get_display_name(m_profile, displayName,
                                     BBMSP_PROFILE_DISPLAY_NAME_MAX)) {
	  displayName[0] = 0;
	  qDebug() << "UserProfile::getDisplayName(): "
	    "Error: Failed to get Display Name.";
  }
  return QString::fromUtf8(displayName);
}

QString UserProfile::getPersonalMessage() {
  char personalMessage[BBMSP_PROFILE_PERSONAL_MSG_MAX];
  if(BBMSP_FAILURE ==
      bbmsp_profile_get_personal_message(m_profile,
                                         personalMessage,
                                         BBMSP_PROFILE_PERSONAL_MSG_MAX)){
	  personalMessage[0] = 0;
	  qDebug()  << "UserProfile::getPersonalMessage() " << "Error: Failed to get Personal Message.";
  }
  return QString::fromUtf8(personalMessage);
}

QString UserProfile::getStatusMessage() {
  char statusMessage[BBMSP_PROFILE_STATUS_MSG_MAX];
  if(BBMSP_FAILURE ==
      bbmsp_profile_get_status_message(m_profile,
                                       statusMessage,
                                       BBMSP_PROFILE_STATUS_MSG_MAX)){
	  statusMessage[0] = 0;
	  qDebug()  << "UserProfile::getStatusMessage(): "
	    "Error: Failed to get Status Message.";
  }
  return QString::fromUtf8(statusMessage);
}

int UserProfile::getStatus() {
  bbmsp_presence_status_t status;
  if (bbmsp_profile_get_status(m_profile, &status) != BBMSP_SUCCESS) {
    qDebug() << "***ERROR*** cannot get profile status";
  }
  return status;
}

int UserProfile::getPlatformVersion(){
  int version;
  if (bbmsp_profile_get_platform_version(m_profile, &version) !=
      BBMSP_SUCCESS) {
    qDebug() << "***ERROR*** cannot get profile platform version";
  }
  return version;
}

void UserProfile::updateStatusMessage(QString statusMessage, bool showBusy) {
  QByteArray bytes = statusMessage.toUtf8();
  const char* text = bytes.constData();

  bbmsp_set_user_profile_status(showBusy ? BBMSP_PRESENCE_STATUS_BUSY :
                                           BBMSP_PRESENCE_STATUS_AVAILABLE,
                                text);
}

void UserProfile::updatePersonalMessage(QString personalMessage) {

  QByteArray bytes = personalMessage.toUtf8();
  const char* text = bytes.constData();
  bbmsp_set_user_profile_personal_message(text);
}


void UserProfile::setUserProfileStatus(bbmsp_profile_t* profile) {
  char statusMessage[BBMSP_PROFILE_STATUS_MSG_MAX];
  if(BBMSP_FAILURE ==
      bbmsp_profile_get_status_message(profile,
                                       statusMessage,
                                       BBMSP_PROFILE_STATUS_MSG_MAX)) {
	  qDebug() << "UserProfile::setUserProfileStatus(): "
	    "Error: Failed to set User Profile Status.";
	  return;
  }
  bbmsp_presence_status_t status;
  if (bbmsp_profile_get_status(profile, &status) != BBMSP_SUCCESS) {
    qDebug() << "***ERROR*** cannot get profile status";
  }
  qDebug() << "NEW STATUS MESSAGE:   " << statusMessage;
  bbmsp_profile_set_status(m_profile, status, statusMessage);
}

void UserProfile::setUserProfilePersonalMessage(bbmsp_profile_t* profile) {
  char personalMessage[1025];
  if(BBMSP_FAILURE == bbmsp_profile_get_personal_message(profile, personalMessage, 1024)){
	  qDebug()  << "UserProfile::setUserProfilePersonalMessage " << "Error: Failed to set user Profile Personal Message.";
	  return;
  }
  qDebug() << "NEW PERSONAL MESSAGE:   " << personalMessage;
  bbmsp_profile_set_personal_message(m_profile, personalMessage);
}

void UserProfile::requestUserAvatar() {
  qDebug() << "Trying to get user avatar";
  m_avatar = 0;
  bbmsp_image_create_empty(&m_avatar);
  bbmsp_result_t rc = bbmsp_profile_get_display_picture(m_profile, m_avatar);
}

void UserProfile::setUserAvatar(bbmsp_profile_t* profile) {
  qDebug() << "Got user avatar call back";
  m_avatar = 0;
    bbmsp_image_create_empty(&m_avatar);
    bbmsp_result_t rc = bbmsp_profile_get_display_picture(profile, m_avatar);
    if(rc == BBMSP_SUCCESS){
      QFile tempFile(QDir::homePath() + "/profileAvatar.jpg");
      qDebug() << QDir::homePath();
      if(tempFile.exists())
      {
        tempFile.remove();
      }

      if(tempFile.open(QFile::ReadWrite))
      {
        char* data = bbmsp_image_get_data(m_avatar);
        int size = bbmsp_image_get_data_size(m_avatar);
        QByteArray byteArray(data,size);
        tempFile.write(byteArray);
      }else{
        qDebug() << "error in opening the file in read write";
      }
      tempFile.close();
    }
}

bool UserProfile::setAvatar(QString imagePath) {
	int maxWidth = 333;
	int maxHeight = 333;
	int maxFileSize = 32768;

	bbmsp_result_t rc = BBMSP_FAILURE;

	// Get the file extension.
	int index = imagePath.lastIndexOf(".") + 1;
	QString fileExtension = imagePath.mid(index, imagePath.length() - index);
	qDebug() << "fileExtension: " << fileExtension;

	QString tempAvatar = QString("/accounts/1000/shared/photos"); // this one works but users can see it, which is why we delete it afterwards
	tempAvatar.append("/bbmSdkAvatar.");
	tempAvatar.append(fileExtension);
	QFile tFile(tempAvatar);
	tFile.remove();

	QImage image(imagePath);
	QImage scaledImage;
	scaledImage = image.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio);
	scaledImage.save(tempAvatar);

	QFile tempFile(tempAvatar);
	if(tempFile.open(QFile::ReadOnly)){
		qDebug() << "read: size: " << tempFile.size();

		QDataStream in(&tempFile);
		QByteArray imageData;

		char bytes[100];
		int bytesRead;
		while(not in.atEnd()){
			bytesRead = in.readRawData(bytes, 100);
			if(bytesRead <= 0) {
			  qDebug() << "setAvatar: Error while reading file";
			  return false;
			}
			imageData.append(bytes, bytesRead);
		}

		qDebug() << "setAvatar: data size: " << imageData.size() << " data: " << imageData.data();

		// Create the icon object and register the icon
		bbmsp_image_t* bbmspImage = NULL;
		bbmspImage=0;
		bbmsp_image_create_empty(&bbmspImage);
		rc = bbmsp_image_create(&bbmspImage, BBMSP_IMAGE_TYPE_JPG,
											 imageData.data(), imageData.size());

		qDebug() << "setAvatar: Created bbmsp_image_t: rc=" << rc;
		if(rc == BBMSP_SUCCESS) {
			rc = bbmsp_set_user_profile_display_picture(bbmspImage);
			bbmsp_image_destroy(&bbmspImage);
			qDebug() << "setAvatar: rc=" << rc;
		}

	}

	qDebug() << "out: rc: " << rc;

	return (rc==BBMSP_ASYNC || rc==BBMSP_SUCCESS)?true:false;
}

void UserProfile::setUserProfileDisplayName(bbmsp_profile_t* profile) {
  //get display name from received profile
  char displayName[BBMSP_PROFILE_DISPLAY_NAME_MAX];
  if (BBMSP_FAILURE ==
      bbmsp_profile_get_display_name(profile, displayName,
                                     BBMSP_PROFILE_DISPLAY_NAME_MAX)){
	  qDebug()  << "UserProfile::setUserProfileDisplayName(): "
	    "Error: Failed to set Profile Display Name.";
	  return;
  }

  qDebug() << "NEW DISPLAY NAME:   " << displayName;
  //set display name for local profile
  // TODO:  Determine whether we should support the following method call --
  // the profile name should change only in the BBM UI.
  // bbmsp_profile_set_display_name(m_profile, displayName);
}
