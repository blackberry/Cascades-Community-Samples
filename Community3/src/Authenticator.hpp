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

#ifndef AUTHENTICATOR_HPP_
#define AUTHENTICATOR_HPP_

#include <QtCore/QObject>
#include <QTime>

#include "Credentials.hpp"
#include "URLProvider.hpp"

class Authenticator : public QObject
{
Q_OBJECT

public:

	static Authenticator* Instance();
	void authenticate();
	bool authenticated();
	QString userId();
	QString sessionKey();
	QString forumLocalLoginAttempted();

public: Q_SIGNALS:

	//Fired when the user is logged into or logged out of the forum.
	void authenticationChanged(bool authenticationSuccessful);

	//Notifies that the user has switched from one forum to another.
   void forumLocaleChanged();

public Q_SLOTS:

	//Slot to receive when login credentials are available.
	void credsReady();

    /*
     * Callback handler for QNetworkReply finished() signal fired after login.
     */
    void onLoginReply();

    /*
	* Callback handler for QNetworkReply finished() signal fired when requesting the currently logged in user.
	*/
   void onCurrentUserReply();

private:

	Authenticator();
	Authenticator(const Authenticator &); // hide copy constructor
	Authenticator& operator=(const Authenticator &);
	void getUserId();

	static Authenticator* mAuthInstance;
	bool mAuthenticated;
	QString mSessionKey, mUsername, mPassword, mUserId;
	Credentials* mCredentials;
	QString mForumLocale;
	bool mAuthenticationInProgress;
	QTime mLastAuthenticationRequest;
};


#endif /* AUTHENTICATOR_HPP_ */
