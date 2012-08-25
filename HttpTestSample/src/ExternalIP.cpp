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

#include "ExternalIP.hpp"
#include <QUrl>
#include <QDebug>
#include <QSettings>
#include <bb/data/jsondataaccess>
#include "AppSettings.hpp"

/**
 * ExternalIP
 *
 * In this class you will learn the following:
 * -- How to use QNetworkAccessManager to make a network request
 * -- How to setup a secure connection with QSslConfiguration
 * -- How to read a network response with QNetworkReply
 * -- How to parse JSON data using JsonDataAccess
 */

ExternalIP::ExternalIP(QObject* parent)
    : QObject(parent)
{
}

ExternalIP::~ExternalIP()
{
}

/**
 * ExternalIP::getIP()
 *
 * Make a network request to httpbin.org/ip to return your external IP address in JSON
 * format
 */
void ExternalIP::getIP()
{
    QNetworkAccessManager* netManager = new QNetworkAccessManager();
    if (!netManager)
    {
        qDebug() << "Unable to create QNetworkAccessManager!";
        emit complete("Unable to create QNetworkAccessManager!");
        return;
    }

    QUrl url("http://httpbin.org/ip");
    QNetworkRequest req(url);

    // Check App settings to see if we should use SSL
    if (AppSettings::isUsingHttps())
    {
        req.setUrl(QUrl("https://httpbin.org/ip"));
        QSslConfiguration config = req.sslConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        config.setProtocol(QSsl::TlsV1);
        req.setSslConfiguration(config);
    }

    QNetworkReply* ipReply = netManager->get(req);

    connect(ipReply, SIGNAL(finished()), this, SLOT(onGetReply()));
}

/**
 * ExternalIP::onGetReply()
 *
 * SLOT
 * Parse the network response if we get one
 */
void ExternalIP::onGetReply()
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

                // the data from reply is in a json format e.g
                // {"origin": "24.127.96.129"}

                bb::data::JsonDataAccess ja;
                QVariant jsonva = ja.loadFromBuffer(buffer);
                QMap<QString, QVariant> externalip = jsonva.toMap();
                QMap<QString, QVariant>::iterator it;
                for (it = externalip.begin(); it != externalip.end(); it++)
                {
                    response = it.value().toString();
                }
            }
        }
        else
        {
            //get http status code
            int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            //do some error management
            response = "Http Error: " + QString::number(httpStatus);
        }
        reply->deleteLater();
    }
    if (response.trimmed().isEmpty())
    {
        response = "Unable to retrieve ip address";
    }
    emit complete(response);
}
