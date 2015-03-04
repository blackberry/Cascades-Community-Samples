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

#include "ForumRequest.hpp"
#include "Authenticator.hpp"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <bb/cascades/QmlDocument>

ForumRequest::ForumRequest(QObject *parent)
: mCancelPendingRequests(false)
{
	setParent(parent);
	Authenticator* auth = Authenticator::Instance();
	QObject::connect(auth, SIGNAL(authenticationChanged(bool)), this, SLOT(authenticationUpdate(bool)));
	QObject::connect(auth, SIGNAL(forumLocaleChanged()), this, SLOT(onForumLocaleChanged()));
}

/*
 * Makes a network call to retrieve the forum REST data for the specified URL
 * @param url - the url to connect to
 * @see onForumReply
 */

void ForumRequest::authenticationUpdate(bool authenticationSuccessful)
{
	qDebug() << "Auth update.";
	if (authenticationSuccessful && !mCancelPendingRequests)
	{
		qDebug() << "Requesting again....";
		makeRequest(mUrl);
	}
	else if (!mCancelPendingRequests && mRequiresAuthentication)
	{
		emit requestComplete(tr("You must be logged in to perform this action.  Configure your BlackBerry forum credentials by swiping from the top and choosing Setup from a non compose screen."), false);
	}
}

void ForumRequest::makeRequest(const QString url, bool requiresAuthentication, QString subject, QString body)
{
	//qDebug() << "Requesting URL: " << url;
	Authenticator* auth = Authenticator::Instance();
	QString sessionKey = "";
	mUrl = url;
	mRequiresAuthentication = requiresAuthentication;

	//Use the session key if available.
	if (auth->authenticated())
	{
		sessionKey = auth->sessionKey();
	}

	if (mRequiresAuthentication && !auth->authenticated())
	{
		//Attempt authentication.
		QObject::connect(auth, SIGNAL(authenticationChanged(bool)), this, SLOT(authenticationUpdate(bool)));
		mUrl = url;
		auth->authenticate();
	}
	else
	{
		QNetworkRequest request(mUrl);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
		if (sessionKey.size() > 0)
		{
			QUrl postData;
			postData.addQueryItem("restapi.session_key", sessionKey);

			//Add in the subject, body and userId for posting messages.
			if (subject.size() > 0)
			{
				postData.addQueryItem("message.subject", subject);
				postData.addQueryItem("message.body", body);
				postData.addQueryItem("message.author", auth->userId());
			}
			QNetworkReply* reply = bb::cascades::QmlDocument::defaultDeclarativeEngine()->networkAccessManager()->post(request, postData.encodedQuery());
			QObject::connect(reply, SIGNAL(finished()), this, SLOT(onForumReply()));
		}
		else
		{
			QNetworkReply* reply = bb::cascades::QmlDocument::defaultDeclarativeEngine()->networkAccessManager()->get(request);
			QObject::connect(reply, SIGNAL(finished()), this, SLOT(onForumReply()));
		}
	}
}

/*
 * ForumRequest::onForumReply()
 *
 * Callback handler for QNetworkReply finished() signal
 */
void ForumRequest::onForumReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    QString response;
    bool success = false;
    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            const int available = reply->bytesAvailable();
            if (available > 0) {
                const QByteArray buffer = reply->readAll();
                response = QString::fromUtf8(buffer);
                success = true;
            }
        } else {
            response =  tr("Error: %1 status: %2").arg(reply->errorString(), reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString());
        }

        reply->deleteLater();
    }

    if (response.contains("error code=\"301\""))
    {
    	//Session key timed out.  Attempt to re-authenticate.
    	qDebug() << "Invalid session key, re-authenticating...";
    	Authenticator* auth = Authenticator::Instance();
		QObject::connect(auth, SIGNAL(authenticationChanged(bool)), this, SLOT(authenticationUpdate(bool)));
		auth->authenticate();
		return;
    }
    else if (response.contains("error code=\"303\""))
    {
    	response = tr("Request Failed.  Permission denied.");
    }
    else if (response.trimmed().isEmpty()) {
        response = tr("Forum request failed. Check your network connection.");
    }

    emit requestComplete(response, success);
}

void ForumRequest::onForumLocaleChanged()
{
	//If the URLs don't match anymore this is the ForumRequest from the previous forum
	//(set before user changed forum in the options).  Delete this request.
	if (!mUrl.contains(URLProvider::getForumURL()))
	{
		mCancelPendingRequests = true;
		this->deleteLater();
	}
}
