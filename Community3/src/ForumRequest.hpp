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

#ifndef FORUMREQUEST_HPP_
#define FORUMREQUEST_HPP_

#include <QtCore/QObject>
#include <QtCore/QString>

class ForumRequest : public QObject
{
    Q_OBJECT

public:
    ForumRequest(QObject *parent = 0);

    /*
     * Makes a network call to retrieve the forum REST data for the specified URL
     * @param url - the url to connect to
     * @requiresAuthentication - Flag to say if the user must be logged in to access the URL.
     * @see onForumReply
     */
    void makeRequest(const QString url, bool requiresAuthentication = false, QString subject = "", QString body = "");


public: Q_SIGNALS:
    /*
     * This signal is emitted when the request is complete
     * @param info - on success, this is the json reply from the request
     *               on failure, it is an error string
     * @param success - true if request succeed, false if not
     */
    void requestComplete(const QString &info, bool success);

    private Q_SLOTS:
        /*
         * Callback handler for QNetworkReply finished() signal
         */
        void onForumReply();

        void authenticationUpdate(bool authenticationSuccessful);

        void onForumLocaleChanged();

private:
        QString mUrl;
        bool mCancelPendingRequests;  //If the forum local changed, stop any pending requests to prevent race condition using session key on previous forum URL, triggering a new authentication attempt.
		bool mRequiresAuthentication;

};


#endif /* FORUMREQUEST_HPP_ */
