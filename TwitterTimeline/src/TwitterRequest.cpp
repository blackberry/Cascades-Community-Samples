/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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

#include "TwitterRequest.hpp"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

/*
 * Default constructor
 */
TwitterRequest::TwitterRequest()
{
}

/*
 * Destructor
 */
TwitterRequest::~TwitterRequest()
{
}

/*
 * TwitterRequest::getTimeline(QString screenname)
 *
 * Makes a network call to retrieve the twitter feed for the specified screenname
 */
void TwitterRequest::getTimeline(QString screenname)
{
    QNetworkAccessManager* netManager = new QNetworkAccessManager();
    if (!netManager)
    {
        qDebug() << "Unable to create QNetworkAccessManager!";
        emit complete("Unable to create QNetworkAccessManager!", false);
        return;
    }

    QString queryUri = "http://api.twitter.com/1/statuses/user_timeline.json?include_entities=true&include_rts=true&screen_name=";
    queryUri += screenname;
    QUrl url(queryUri);
    QNetworkRequest req(url);

    QNetworkReply* ipReply = netManager->get(req);
    connect(ipReply, SIGNAL(finished()), this, SLOT(onTimelineReply()));
}

/*
 * TwitterRequest::onTimelineReply()
 *
 * Callback handler for QNetworkReply finished() signal
 */
void TwitterRequest::onTimelineReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString response;
    bool success = false;
    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            int available = reply->bytesAvailable();
            if (available > 0)
            {
                int bufSize = sizeof(char) * available + sizeof(char);
                QByteArray buffer(bufSize, 0);
                int read = reply->read(buffer.data(), available);
                response = QString::fromUtf8(buffer);
                success = true;
            }
        }
        else
        {
            response =  QString("Error: ") + reply->errorString() + QString(" status:") + reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
            qDebug() << response;
        }
        reply->deleteLater();
    }
    if (response.trimmed().isEmpty())
    {
        response = "Twitter request failed. Check internet connection";
        qDebug() << response;
    }
    emit complete(response, success);
}
