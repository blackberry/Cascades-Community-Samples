/* Copyright (c) 2014 BlackBerry Ltd.
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
 *
 * The Lithium REST APIs are owned by Lithium Technologies, Inc.
 * (http://www.lithium.com) and are not part of the “Works” licensed
 * hereunder pursuant to the Apache 2.0 license.
 */

#ifndef CREDENTIALS_HPP_
#define CREDENTIALS_HPP_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QDebug>
#include <QSettings>
#include <sbreturn.h>
#include <huaes.h>
#include <QList>
#include <QVariantMap>

class Credentials : public QObject
{
	Q_OBJECT;

	Q_PROPERTY(QString username READ username);
	Q_PROPERTY(QString password READ password);
	Q_PROPERTY(bool loggedIn READ loggedIn);

	public:
		Credentials(QObject *parent = 0);

		QString username();
		QString password();
		bool loggedIn();
		bool hasCredentials();

	public: Q_SIGNALS:

public Q_SLOTS:

	//Used to set the username and password.
	void setCredentials(QString username, QString password);

	private:

	QSettings mSettings;


};

#endif /* CREDENTIALS_HPP_ */
