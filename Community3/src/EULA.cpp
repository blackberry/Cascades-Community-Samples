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

#include "EULA.hpp"

#include <QDebug>
#include <QDir>
#include <bb/data/JsonDataAccess>


EULA::EULA(QObject *parent)
: mLocal(QLocale())
,  mSettings("MSohm", "BlackBerry Community App")
, mTitle (QString(""))
, mBody (QString(""))
, mAgreebutton (QString(""))
, mDisagreebutton (QString(""))
{
	setParent(parent);

	qDebug() << "The size is: " << mTitle.size();

	QString loc = mLocal.bcp47Name().left(2);

	QString appFolder(QDir::homePath());
	appFolder.chop(4);
	appFolder.append("/app/native/assets/eula/eula.json");

	bb::data::JsonDataAccess jda;
	QVariantList eulas = jda.load(appFolder).toList();
	QString enTitle, enBody, enAgreebutton, enDisagreebutton;

	for (int count = eulas.count() - 1; count >= 0; --count)
	{
		QVariantMap map = eulas.at(count).toMap();
		if (map.value("locale").toString().contains(loc))
		{
			mTitle = map.value("title").toString();
			mBody = map.value("body").toString();
			mAgreebutton = map.value("button1").toString();
			mDisagreebutton = map.value("button2").toString();
		}
		else if (map.value("locale").toString().contains("en"))
		{
			enTitle = map.value("title").toString();
			enBody = map.value("body").toString();
			enAgreebutton = map.value("button1").toString();
			enDisagreebutton = map.value("button2").toString();
		}

	}

	//Use English as the default if the language is not found.
	if (mTitle.size() == 0)
	{
		mTitle = enTitle;
		mBody = enBody;
		mAgreebutton = enAgreebutton;
		mDisagreebutton = enDisagreebutton;
	}
}

void EULA::agreeToEULA()
{
	mSettings.setValue("AgreedToEULA", true);

}

bool EULA::agreedTo()
{
	if (mSettings.contains("AgreedToEULA"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

QString EULA::eulaTitle()
{
	return mTitle;
}

QString EULA::eulaBody()
{
	return mBody;
}

QString EULA::eulaAgreeButton()
{
	return mAgreebutton;
}

QString EULA::eulaDisagreeButton()
{
	return mDisagreebutton;
}
