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

#include "Contact.hpp"

Contact::Contact(bbmsp_contact_t* contact)
{
	bbmsp_contact_create(&m_bbmsp_contact);
	bbmsp_contact_copy(m_bbmsp_contact, contact);
}

QString Contact::getHandle()
{
	char handle[BBMSP_CONTACT_HANDLE_MAX];
	bbmsp_contact_get_handle(m_bbmsp_contact, handle, BBMSP_CONTACT_HANDLE_MAX);
	return QString::fromUtf8(handle);
}

QString Contact::getPpid()
{
	char ppid[BBMSP_CONTACT_PPID_MAX];
	bbmsp_contact_get_ppid(m_bbmsp_contact, ppid, BBMSP_CONTACT_PPID_MAX);
	return QString::fromUtf8(ppid);
}

QString Contact::getAppVersion()
{
	char appVersion[1025];
	bbmsp_contact_get_app_version(m_bbmsp_contact, appVersion, 1024);
	return QString::fromUtf8(appVersion);
}

QString Contact::getDisplayName()
{
	char displayName[BBMSP_CONTACT_DISPLAY_NAME_MAX];
	bbmsp_contact_get_display_name(m_bbmsp_contact, displayName,
			BBMSP_CONTACT_DISPLAY_NAME_MAX);
	return QString::fromUtf8(displayName);
}

QString Contact::getPersonalMessage()
{
	char personalMessage[BBMSP_CONTACT_PERSONAL_MSG_MAX];
	bbmsp_contact_get_personal_message(m_bbmsp_contact, personalMessage,
			BBMSP_CONTACT_PERSONAL_MSG_MAX);
	return QString::fromUtf8(personalMessage);

}

QString Contact::getStatusMessage()
{
	char statusMessage[BBMSP_CONTACT_STATUS_MSG_MAX];
	bbmsp_contact_get_status_message(m_bbmsp_contact, statusMessage,
			BBMSP_CONTACT_STATUS_MSG_MAX);
	return QString::fromUtf8(statusMessage);
}

QString Contact::getAvatarMimeType()
{
	//get the avatar type
	bbmsp_image_type_t type = bbmsp_image_get_type(m_contact_avatar);
	if (type == BBMSP_IMAGE_TYPE_JPG)
	{
		return "jpeg";
	} else if (type == BBMSP_IMAGE_TYPE_PNG)
	{
		return "png";
	} else if (type == BBMSP_IMAGE_TYPE_GIF)
	{
		return "gif";
	} else if (type == BBMSP_IMAGE_TYPE_BMP)
	{
		return "bmp";
	}
}

bbmsp_result_t Contact::requestContactAvatar()
{
	m_contact_avatar = 0;
	bbmsp_image_create_empty(&m_contact_avatar);

	bbmsp_result_t rc = bbmsp_contact_get_display_picture(m_bbmsp_contact,
			m_contact_avatar);
	if (rc == BBMSP_SUCCESS)
	{
		QFile tempFile(
				QDir::homePath() + "/contact_" + this->getDisplayName() + "."
						+ this->getAvatarMimeType());
		if (tempFile.exists())
		{
			tempFile.remove();
		}

		if (tempFile.open(QFile::ReadWrite))
		{
			char* data = bbmsp_image_get_data(m_contact_avatar);
			int size = bbmsp_image_get_data_size(m_contact_avatar);
			QByteArray byteArray(data, size);
			tempFile.write(byteArray);
		} else
		{
			qDebug() << "error in opening the file in read write";
		}
		tempFile.close();
	}
	return rc;
}
