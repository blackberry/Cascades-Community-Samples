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

#include "Credentials.hpp"
#include "Authenticator.hpp"

Credentials::Credentials(QObject *parent)
: mSettings("Your Company", "Community 3")  //ToDo: Enter your company and app name here.  Use same value in URLProvider.
{
	setParent(parent);
}

void Credentials::setCredentials(QString username, QString password)
{
	mSettings.setValue("ForumUsername", username);
	mSettings.setValue("ForumPassword", password);

	//Re-authenticate with the new credentials.
	Authenticator* auth = Authenticator::Instance();
	auth->authenticate();
}

QString Credentials::username()
{
	return mSettings.value("ForumUsername").toString();
}

QString Credentials::password()
{
	return mSettings.value("ForumPassword").toString();
}

bool Credentials::hasCredentials()
{
	if (mSettings.contains("ForumUsername") && mSettings.contains("ForumPassword"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Credentials::loggedIn()
{
	//Re-authenticate with the new credentials.
	Authenticator* auth = Authenticator::Instance();
	return auth->authenticated();
}
