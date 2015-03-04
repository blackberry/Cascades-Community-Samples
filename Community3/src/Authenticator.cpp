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

#include "Authenticator.hpp"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <bb/system/SystemToast>

#include <bb/cascades/QmlDocument>

Authenticator * Authenticator::mAuthInstance(NULL);

Authenticator::Authenticator()
: mAuthenticated (false)
, mSessionKey (QString(""))
, mCredentials (new Credentials(this))
, mAuthenticationInProgress(false)
, mLastAuthenticationRequest(QTime())
{
	//Login to the forum on startup if credentials exist.
	if (mCredentials->hasCredentials())
	{
		authenticate();
	}
}


Authenticator* Authenticator::Instance()
{
	// Only allow one instance of class to be generated.
   if (!mAuthInstance)
   {
	   mAuthInstance = new Authenticator;
   }

   return mAuthInstance;
}


void Authenticator::credsReady()
{
	//Log in with the new credentials.
	authenticate();
}


bool Authenticator::authenticated()
{
	return mAuthenticated;
}

QString Authenticator::sessionKey()
{
	return mSessionKey;
}

void Authenticator::authenticate()
{
	//Don't start another authentication request if one is already in progress.  We give 5 seconds for the
	//current request to finish.
	if (!mAuthenticationInProgress || (mAuthenticationInProgress && (mLastAuthenticationRequest.secsTo(QTime::currentTime()) > 5)))
	{
		mAuthenticationInProgress = true;
		mLastAuthenticationRequest = QTime::currentTime();

		//Always grab the latest username and password.
		mUsername = mCredentials->username();
		mPassword = mCredentials->password();

		//Set authenticated to false to prevent using wrong session key if forum was changed.
		mAuthenticated = false;

		//Only attempt to login if a username has been entered.
		if (mUsername.length() > 0)
		{

			QUrl postData;
			postData.addQueryItem("user.login", mUsername);
			postData.addQueryItem("user.password", mPassword);

			QString loginURL = URLProvider::getForumURL() + "authentication/sessions/login";
			QNetworkRequest request(loginURL);
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

			QNetworkReply* reply = bb::cascades::QmlDocument::defaultDeclarativeEngine()->networkAccessManager()->post(request, postData.encodedQuery());
			QObject::connect(reply, SIGNAL(finished()), this, SLOT(onLoginReply()));
		}
		else
		{
			qDebug() << "No forum credentials, not attempting login.";
			mAuthenticated = false;
			emit authenticationChanged(mAuthenticated);
		}
	}

}

/*
 * ForumRequest::onLoginReply()
 *
 * Callback handler for QNetworkReply finished() signal fired after login.
 */
void Authenticator::onLoginReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    QString response;
    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            const int available = reply->bytesAvailable();
            if (available > 0) {
                const QByteArray buffer = reply->readAll();
                response = QString::fromUtf8(buffer);
            }
        }

        reply->deleteLater();
    }

    if (response.contains("status=\"success\"", Qt::CaseInsensitive))
    {
    	qDebug() << "Auth success!";
    	int valStart = response.indexOf("<value type=\"string\">") + 21;
    	int valEnd = response.indexOf("</value>");
		mSessionKey = response.mid(valStart, (valEnd - valStart));

    	mAuthenticated = true;

    	getUserId();

        bb::system::SystemToast* toast = new bb::system::SystemToast(this);
        toast->setBody(tr("You are now logged into the forum."));
        toast->show();
    }
    else
    {
    	qDebug() << "Auth failed!";
    	mAuthenticated = false;
    	bb::system::SystemToast* toast = new bb::system::SystemToast(this);
		toast->setBody(tr("Failed to log into the forum."));
		toast->show();
    }

    mAuthenticationInProgress = false;
    emit authenticationChanged(mAuthenticated);
}

void Authenticator::getUserId()
{
	QString userIdURL = URLProvider::getForumURL() + "authentication/sessions/current/user";
	QNetworkRequest request(userIdURL);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	QUrl postData;
	postData.addQueryItem("restapi.session_key", mSessionKey);

	QNetworkReply* reply = bb::cascades::QmlDocument::defaultDeclarativeEngine()->networkAccessManager()->post(request, postData.encodedQuery());
	QObject::connect(reply, SIGNAL(finished()), this, SLOT(onCurrentUserReply()));
}


/*
 * ForumRequest::onLoginReply()
 *
 * Callback handler for QNetworkReply finished() signal fired when requesting the currently logged in user.
 */
void Authenticator::onCurrentUserReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    QString response;
    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            const int available = reply->bytesAvailable();
            if (available > 0) {
                const QByteArray buffer = reply->readAll();
                response = QString::fromUtf8(buffer);
            }
        }

        reply->deleteLater();
    }

    if (response.contains("status=\"success\"", Qt::CaseInsensitive))
    {
    	int valStart = response.indexOf("<id type=\"int\">") + 15;
    	int valEnd = response.indexOf("</id>");
    	mUserId = "id/" + response.mid(valStart, (valEnd - valStart));
    }
    else
    {
    	mUserId = "";
    }
}


QString Authenticator::userId()
{
	return mUserId;
}
