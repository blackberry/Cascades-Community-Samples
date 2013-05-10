/*
 * ODataNetworkManager.cpp
 *
 *  Created on: 2013-03-26
 *      Author: Daniel Baxter
 */

#include "ODataNetworkManager.h"
#include <qnetworkrequest.h>
#include <JsonDataAccess.hpp>
#include <XmlDataAccess.hpp>
#include <qbuffer.h>
/*
 * CONSTRUCTORS / DESTRUCTORS
 */

ODataNetworkManager::ODataNetworkManager()
: mNetAccessManager(NULL)
{
    mNetAccessManager = new QNetworkAccessManager(this);
}

ODataNetworkManager::~ODataNetworkManager() {
    delete mNetAccessManager;
}

/*
 * PUBLIC FUNCTIONS
 */

void ODataNetworkManager::read(QString url) {
    QUrl qurl(url);

    QNetworkRequest req(qurl);
    QNetworkReply* reply = mNetAccessManager->get(req);
    connect(reply, SIGNAL(finished()), this, SLOT(onReadReply()));
}

void ODataNetworkManager::create(QString url, QByteArray dataModel) {
    QUrl qurl(url);

    QNetworkRequest req(qurl);
    req.setRawHeader("Content-Type", "application/atom+xml");

    QNetworkReply* reply = mNetAccessManager->post(req, dataModel);
    connect(reply, SIGNAL(finished()), this, SLOT(onCreateReply()));
}

void ODataNetworkManager::update(QString url, QByteArray dataModel) {
    QUrl qurl(url);

    QNetworkRequest req(qurl);
    req.setRawHeader("Content-Type", "application/atom+xml");

    QBuffer* data = new QBuffer();
    data->setData(dataModel);

    QNetworkReply* reply = mNetAccessManager->sendCustomRequest(req, "MERGE", data);
    connect(reply, SIGNAL(finished()), this, SLOT(onUpdateReply()));

}

void ODataNetworkManager::del(QString url) {
    QUrl qurl(url);

    QNetworkRequest req(qurl);
    QNetworkReply* reply = mNetAccessManager->deleteResource(req);
    connect(reply, SIGNAL(finished()), this, SLOT(onDeleteReply()));
}

/*
 * SLOTS
 */

void ODataNetworkManager::onReadReply() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());


    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response;

            response = reply->readAll();

            QByteArray contentType = reply->rawHeader("Content-Type");

            // determine what the response is encoded in
            if (contentType.contains(TYPE_ATOM)) {
                bb::data::XmlDataAccess xda;
                emit atomReady(xda.loadFromBuffer(response));
            }
            else if (contentType.contains(TYPE_JSON)) {
                bb::data::JsonDataAccess jda;
                emit jsonReady(jda.loadFromBuffer(response));
            }
            else if (contentType.contains(TYPE_XML)) {
                bb::data::XmlDataAccess xda;
                emit xmlReady(xda.loadFromBuffer(response));
            }
        }
        else {
            handleError(reply);
        }
    }
    else {
        handleErrorNoReply();
    }
}

void ODataNetworkManager::onCreateReply(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());


    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            emit createSuccessful();

            // further object parsing if they want the data out
            QByteArray response;

            response = reply->readAll();

            QByteArray contentType = reply->rawHeader("Content-Type");

            // determine what the response is encoded in
            if (contentType.contains(TYPE_ATOM)) {
                bb::data::XmlDataAccess xda;
                emit createAtomReady(xda.loadFromBuffer(response));
            }
            else if (contentType.contains(TYPE_JSON)) {
                bb::data::JsonDataAccess jda;
                emit createJsonReady(jda.loadFromBuffer(response));
            }
            else if (contentType.contains(TYPE_XML)) {
                bb::data::XmlDataAccess xda;
                emit createXmlReady(xda.loadFromBuffer(response));
            }
        }
        else {
            handleError(reply);
        }
    }
    else {
        handleErrorNoReply();
    }
}

void ODataNetworkManager::onUpdateReply(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());


    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            emit updateSuccessful();
        }
        else {
            handleError(reply);
        }
    }
    else {
        handleErrorNoReply();
    }
}

void ODataNetworkManager::onDeleteReply(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());


    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            emit deleteSuccessful();
        }
        else {
            handleError(reply);
        }
    }
    else {
        handleErrorNoReply();
    }
}


void ODataNetworkManager::handleError(QNetworkReply* reply){
    emit networkError(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), reply->errorString());

    /* for debugging errors just uncomment
    // LOG the error to the console
    fprintf(stderr, "Error: Code %d - %s\n", reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), reply->errorString().toUtf8().constData());

    QByteArray response;
    response = reply->readAll();
    fprintf(stderr, "Response: %s\n", response.constData());
    */
}

void ODataNetworkManager::handleErrorNoReply(){
    emit networkError(0, "No Reply");
}
