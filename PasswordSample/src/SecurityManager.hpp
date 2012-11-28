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
#ifndef SecurityManager_h
#define SecurityManager_h

#include "huctx.h"
#include "sbdef.h"
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QDebug>


class SecurityManager : public QObject {

Q_OBJECT

public:
	static SecurityManager* getInstance();
	Q_INVOKABLE bool isPasswordOK(QString user_id, QString password);
	Q_INVOKABLE void setLoggedIn(bool logged_in_state);
	Q_INVOKABLE bool isLoggedIn();
	int initSecurity();
	void endSecurity();
	int makeHash(const QString input_data, unsigned char* messageDigest);

signals:
	void loggedInChanged(const QVariant &logged_in);

private:
	SecurityManager();
	virtual ~SecurityManager();
	static SecurityManager* _instance;
	sb_GlobalCtx sbCtx;                       /* SB Context */
	sb_Context sha256Context;                 /* SHA-256 Context */
	bool logged_in;
};

#endif // ifndef SecurityManager_h
