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

#include "PostHttp.hpp"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QDebug>
#include "AppSettings.hpp"

/**
 * PostHttp
 *
 * In this class you will learn the following:
 * -- How to use QNetworkAccessManager to make a network request
 * -- How to set Http headers for your request
 * -- How to setup a secure connection with QSslConfiguration
 * -- How to read a network response with QNetworkReply
 * -- How to parse JSON data using JsonDataAccess
 */

PostHttp::PostHttp(QObject* parent)
    : QObject(parent)
{
}

PostHttp::~PostHttp()
{
}

/**
 * PostHttp::post
 *
 * Make a network request to httpbin.org/post with POST data and get
 * the response
 */
void PostHttp::post(QString body)
{
    QNetworkAccessManager* netManager = new QNetworkAccessManager();
    if (!netManager)
    {
        qDebug() << "Unable to create QNetworkAccessManager!";
        emit complete("Unable to create QNetworkAccessManager!");
        return;
    }

    QUrl url("http://httpbin.org/post");
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    if (AppSettings::isUsingHttps())
    {
        req.setUrl(QUrl("https://httpbin.org/post"));
        QSslConfiguration config = req.sslConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        config.setProtocol(QSsl::TlsV1);
        req.setSslConfiguration(config);
    }

    QNetworkReply* ipReply = netManager->post(req, body.toAscii());
    connect(ipReply, SIGNAL(finished()), this, SLOT(onGetReply()));
}

/**
 * PostHttp::onGetReply()
 *
 * SLOT
 * Read and return the http response from our http post request
 */
void PostHttp::onGetReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString response;
    if (reply)
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            int available = reply->bytesAvailable();
            if (available > 0)
            {
                QByteArray buffer(reply->readAll());
                response = QString(buffer);
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
        response = "Unable to retrieve post response";
    }
    emit complete(response);
}
