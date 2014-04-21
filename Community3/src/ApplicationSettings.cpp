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

#include "ApplicationSettings.hpp"

ApplicationSettings::ApplicationSettings(QObject *parent)
: mSettings(new QSettings("Your Company", "Your Community 3")) //ToDo: Enter your company and app name here.  Use same value in Credentials.
{
	setParent(parent);
}

QString ApplicationSettings::getForumLocale()
{
	if (mSettings->contains("ForumLocale"))
	{
		return mSettings->value("ForumLocale").toString();
	}
	else
	{
		QString localeName = QLocale::system().name();
		QStringList locale = localeName.split('_');
		return locale[0];
	}
}

void ApplicationSettings::setForumLocale(QString forum)
{
	mSettings->setValue("ForumLocale", forum);
}

int ApplicationSettings::getDefaultTab()
{
	return mSettings->value("DefaultTab", 0).toInt();
}

void ApplicationSettings::setDefaultTab(int defaultTab)
{
	mSettings->setValue("DefaultTab", defaultTab);
}

