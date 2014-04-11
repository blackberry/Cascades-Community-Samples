/* Copyright (c) 2014 Research In Motion Limited.
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

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QtCore/QString>
#include <QtCore/QDebug>

class DataManager: public QObject {

Q_OBJECT

public:
	static DataManager* getInstance();
	Q_INVOKABLE QString getUserid();
	Q_INVOKABLE QString getPassword();

	Q_INVOKABLE void setUserid(QString userid);
	Q_INVOKABLE void setPassword(QString password);

private:
	DataManager();
	virtual ~DataManager();
	static DataManager* _instance;
    QString user_id;
    QString password;

    static QString KEY_USER_ID;
    static QString KEY_PASSWORD;

};

#endif // ifndef DATAMANAGER_H
