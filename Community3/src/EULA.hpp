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

#ifndef EULA_HPP_
#define EULA_HPP_

#include <QtCore/QObject>
#include <QLocale>
#include <QSettings>

class EULA : public QObject
{
	Q_OBJECT;

	Q_PROPERTY(bool agreedTo READ agreedTo);
	Q_PROPERTY(QString eulaTitle READ eulaTitle);
	Q_PROPERTY(QString eulaBody READ eulaBody);
	Q_PROPERTY(QString eulaAgreeButton READ eulaAgreeButton);
	Q_PROPERTY(QString eulaDisagreeButton READ eulaDisagreeButton);


public:

	EULA(QObject *parent = 0);

public Q_SLOTS:

	void agreeToEULA();


private:
	bool agreedTo();
	QString eulaTitle();
	QString eulaBody();
	QString eulaAgreeButton();
	QString eulaDisagreeButton();

	QLocale mLocal;
	QSettings mSettings;
	QString mTitle, mBody, mAgreebutton, mDisagreebutton;

};


#endif /* EULA_HPP_ */
