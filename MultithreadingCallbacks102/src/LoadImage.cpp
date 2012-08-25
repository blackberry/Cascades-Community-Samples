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

#include "LoadImage.hpp"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

/**
 *  This class implements a image loader to run in its own thread, which will
 *  initialize a network request in asynchronous manner.
 *  after receiving response from the network, it creates a QImage object in its own thread.
 *  Then it signals the interested parties about the result.
 */

/**
 *  Constructor
 */
LoadImage::LoadImage(QObject* parent)
    : QObject(parent)
    , m_imageUrl("")
	, m_executeThread(NULL)
{
}

/**
 *  Destructor
 */
LoadImage::~LoadImage()
{
	//no need to delete m_executeThread
	//it will be deleted when the thread is done
}

/**
 * LoadImage::load(QString imageUrl)
 *
 * Instruct the QNetworkAccessManager to initialize a network request in asynchronous manner.
 *
 * also, setup the signal handler to receive the event indicating the network response.
 */
void LoadImage::load(QString imageUrl)
{
    m_imageUrl = imageUrl;
    QNetworkAccessManager* netManager = new QNetworkAccessManager();
    if (!netManager)
    {
        qDebug() << "Unable to create QNetworkAccessManager!";
        emit complete(m_imageUrl, false, "Unable to create QNetworkAccessManager");
        return;
    }

    QString queryUri = imageUrl;
    QUrl url(queryUri);
    QNetworkRequest req(url);

    QNetworkReply* ipReply = netManager->get(req);
    connect(ipReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));

}

/**
 * LoadImage::onReplyFinished()
 *
 * handler for the signal indicating the response for the previous network request.
 *
 * If the result was a success, it will start the thread of constructing the QImage object.
 */
void LoadImage::onReplyFinished()
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
                m_data = reply->readAll();
                if (!m_executeThread)
                {
                    //do the parsing of the image in a new thread
                	startThread();;
                    success = true;
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
    if (!success)
    {
        if (response.trimmed().isEmpty())
        {
            response = "Download failed. Check internet connection";
            qDebug() << response;
        }
        emit complete(m_imageUrl, success, response);
    }
}

/**
 * void startThread();
 *
 * start the executing thread
 */
void LoadImage::startThread()
{
	if(m_executeThread!=NULL)
		return;

	m_executeThread = new QThread;

	prepareThread(m_executeThread);
	m_executeThread->start();
}

/**
 *
 * void prepareThread(QThread* executeThread);
 *
 * a helper function to prepare the executing thread.
 *.
 */
void LoadImage::prepareThread(QThread* executeThread)
{
	if(parent())
	{
		setParent(NULL); //a QObject with a parent will not be able to move another thread.
	}

	 // Connects the threadss started() signal
	// to the startProcess() slot, causing it to start processing downloaded data.
	connect(executeThread, SIGNAL(started()), this, SLOT(startProcess()));

	// Connect the finishThread signal to trigger thread quit, then delete.
	connect(this, SIGNAL(finishThread()), executeThread, SLOT(quit()));
	connect(this, SIGNAL(finishThread()), this, SLOT(deleteLater()));

	// Make sure the thread object is deleted after execution has finished.
	connect(executeThread, SIGNAL(finished()), executeThread, SLOT(deleteLater()));

	moveToThread(executeThread);

}

/**
 *  This is the method to be execute in the thread which constructs the QImage
 *  Object from the network response.
 *
 */
void LoadImage::startProcess()
{
    //image processing goes here, example could be adding
    //water mark to the downloaded image, or resizing etc

    m_image.loadFromData(m_data);

    emit complete(m_imageUrl, true, "");
}
