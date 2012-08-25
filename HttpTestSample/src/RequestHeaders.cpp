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

#include "RequestHeaders.hpp"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <bb/data/jsondataaccess>
#include "AppSettings.hpp"

/**
 * RequestHeaders
 *
 * In this class you will learn the following:
 * -- How to use QNetworkAccessManager to make a network request
 * -- How to setup a secure connection with QSslConfiguration
 * -- How to read a network response with QNetworkReply
 * -- How to parse JSON data using JsonDataAccess
 * -- How to read the headers in an http response
 */

RequestHeaders::RequestHeaders(QObject* parent)
    : QObject(parent)
{
}

RequestHeaders::~RequestHeaders()
{
}

/**
 * RequestHeaders::getRequest()
 *
 * Setup an http get request using SSL if configured
 */
void RequestHeaders::getRequest()
{
    QNetworkAccessManager* netManager = new QNetworkAccessManager();
    if (!netManager)
    {
        qDebug() << "Unable to create QNetworkAccessManager!";
        emit complete("Unable to create QNetworkAccessManager!");
        return;
    }

    QUrl url("http://httpbin.org/get");
    QNetworkRequest req(url);
    if (AppSettings::isUsingHttps())
    {
        req.setUrl(QUrl("https://httpbin.org/get"));
        QSslConfiguration config = req.sslConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        config.setProtocol(QSsl::TlsV1);
        req.setSslConfiguration(config);
    }

    QNetworkReply* ipReply = netManager->get(req);
    connect(ipReply, SIGNAL(finished()), this, SLOT(onGetReply()));
}

/**
 * RequestHeaders::onGetReply()
 *
 * SLOT
 * Handles the http response by parsing JSON and printing out the response http headers
 */
void RequestHeaders::onGetReply()
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
                //"args": {},
                //"headers": {
                //  "Accept": "*/*",
                //  "Connection": "close",
                //  "Content-Length": "",
                //  "Content-Type": "",
                //  "Host": "httpbin.org",
                //  "User-Agent": "curl/7.19.7 (universal-apple-darwin10.0) libcurl/7.19.7 OpenSSL/0.9.8l zlib/1.2.3"
                //},
                //"origin": "24.127.96.129",
                //"url": "http://httpbin.org/get"

                bb::data::JsonDataAccess ja;
                QVariant jsonva = ja.loadFromBuffer(buffer);
                QMap<QString, QVariant> jsonreply = jsonva.toMap();

                // locate the header array
                QMap<QString, QVariant>::iterator it = jsonreply.find("headers");
                if (it != jsonreply.end())
                {
                    // print everything in header array
                    QMap<QString, QVariant> headers = it.value().toMap();
                    for (QMap<QString, QVariant>::iterator hdrIter = headers.begin(); hdrIter != headers.end(); hdrIter++)
                    {
                        if (hdrIter.value().toString().trimmed().isEmpty())
                        {
                            continue;    //skip empty values
                        }

                        response += hdrIter.key() + QString(": ") + hdrIter.value().toString() + QString("\r\n");
                    }
                }

                // print everything else
                for (it = jsonreply.begin(); it != jsonreply.end(); it++)
                {
                    if (it.value().toString().trimmed().isEmpty())
                    {
                        continue;    //skip empty values
                    }

                    response += it.key() + QString(": ") + it.value().toString() + QString("\r\n");
                }
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
        response = "Unable to retrieve request headers";
    }

    emit complete(response);
}
