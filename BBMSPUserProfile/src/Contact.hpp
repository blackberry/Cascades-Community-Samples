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

#ifndef CONTACT_HPP_
#define CONTACT_HPP_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDebug>

#include "bbmsp/bbmsp_contactlist.h"


class Contact : public QObject {
	Q_OBJECT

	Q_PROPERTY(QString handle READ getHandle NOTIFY handleChanged)
	Q_PROPERTY(QString ppid READ getPpid NOTIFY ppidChanged)
	Q_PROPERTY(QString appVersion READ getAppVersion NOTIFY appVersionChanged)
	Q_PROPERTY(QString displayName READ getDisplayName NOTIFY displayNameChanged)
	Q_PROPERTY(QString personalMessage READ getPersonalMessage NOTIFY personalMessageChanged)
	Q_PROPERTY(QString statusMessage READ getStatusMessage NOTIFY statusMessageChanged)

signals:
    void handleChanged(QVariant);
    void ppidChanged(QVariant);
    void appVersionChanged(QVariant);
    void displayNameChanged(QVariant);
    void personalMessageChanged(QVariant);
    void statusMessageChanged(QVariant);

public:
	Contact(){}
	Contact(bbmsp_contact_t* contact);
	virtual ~Contact(){}

	QString getHandle();
	QString getPpid();
	QString getAppVersion();
	QString getDisplayName();
	QString getPersonalMessage();
	QString getStatusMessage();
	bbmsp_result_t requestContactAvatar();
	QString getAvatarMimeType();
	bbmsp_image_t* m_contact_avatar;

private:
	bbmsp_contact_t* m_bbmsp_contact;
};

Q_DECLARE_METATYPE(Contact *);

#endif /* CONTACT_HPP_ */
