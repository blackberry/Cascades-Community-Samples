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

#ifndef APPLICATIONSETTINGS_HPP_
#define APPLICATIONSETTINGS_HPP_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QDebug>
#include <QSettings>
#include <Qlocale>

class ApplicationSettings : public QObject
{
Q_OBJECT

public:

	ApplicationSettings(QObject *parent = 0);

	Q_INVOKABLE QString getForumLocale();
	Q_INVOKABLE void setForumLocale(QString forumLocale);
	Q_INVOKABLE int getDefaultTab();
	Q_INVOKABLE void setDefaultTab(int defaultTab);


private:
	QSettings* mSettings;

};


#endif /* APPLICATIONSETTINGS_HPP_ */
