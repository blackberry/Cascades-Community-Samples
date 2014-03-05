/*
 * Copyright (c) 2014 BlackBerry Limited.
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

#include "HTTPImage.hpp"
#include <QtNetwork/QNetworkAccessManager>
#include <bb/cascades/Image>
#include <bb/cascades/Application>


using namespace bb::cascades;

HTTPImage::HTTPImage(QObject *parent) : QObject(parent),
        m_url(QString("")), m_pNetworkReply(0), m_downloadPercentage(0)
{

}

HTTPImage::~HTTPImage()
{

}

QString HTTPImage::url() const
{
    return m_url;
}

void HTTPImage::setUrl(const QString& url)
{
    if (m_url.compare(url) != 0) {
        m_url = url;
        emit urlChanged();

        downloadImage(QUrl(m_url));
    }
    else if (!m_imageData.isNull()) {
    	Image image(m_imageData);
    	emit imageDownloadComplete(QVariant::fromValue(image));
    }
}

int HTTPImage::downloadPercentage() const
{
    return m_downloadPercentage;
}

void HTTPImage::setDownloadPercentage(int percentage)
{
    m_downloadPercentage = percentage;
    emit downloadPercentageChanged(percentage);
}

QString HTTPImage::saveImage(const QString& folder)
{
    QString path("/accounts/1000/shared/photos/" + folder);

    if (path.endsWith('/') == false) {
        path.append('/');
    }

    if (QDir(path).exists() == false && QDir().mkdir(path) == false)
    {
        qCritical() << "Failed to create directory" << path;
        return "";
    }

    QString photoFileName = m_url.split('/', QString::SkipEmptyParts).last();

    //some images have arguments in the url, strip away these
    if (photoFileName.contains('?')) {
        photoFileName = photoFileName.split('?', QString::SkipEmptyParts).first();
        qDebug() << photoFileName;
    }

    QString fullPath(path + photoFileName);
    int sanity = 0;
    QFile* file = NULL;
    while (QFile::exists(fullPath))
    {
        if (sanity++ > 100) {
            goto FILE_ERROR;
        }
        fullPath = fullPath.insert(fullPath.lastIndexOf('.'),'1');
    }
    file = new QFile(fullPath, this);
    if (file->open(QIODevice::WriteOnly)) {
        if(file->write(m_imageData) == -1) {
        	file->close();
        	delete file;
        	file = NULL;
        	goto FILE_ERROR;
        }
        file->close();
        delete file;
        file = NULL;
    } else {
    	goto FILE_ERROR;
    }

    return fullPath;

FILE_ERROR:

	return "";
}

void HTTPImage::downloadImage(const QUrl& url)
{
    bool success;
    m_imageData.clear();

    if (m_pNetworkReply != 0) {
        m_pNetworkReply->abort();
    }
    setDownloadPercentage(0);
    QNetworkAccessManager *netman = new QNetworkAccessManager(this);
    m_pNetworkReply = netman->get(QNetworkRequest (url));
    success = connect(m_pNetworkReply, SIGNAL(finished()), this, SLOT(onImageDownloaded()));
   // ASSERT(success, "Failed connecting network reply finished signal");
    success = connect(m_pNetworkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
   // ASSERT(success, "Failed connecting network reply download progress signal");

    if (m_pNetworkReply->error() != QNetworkReply::NoError) {
        qWarning() << "Could not send request:" << m_pNetworkReply->errorString();
    }
}

void HTTPImage::onImageDownloaded()
{
    if (m_pNetworkReply->error() == QNetworkReply::NoError) {
        m_imageData = m_pNetworkReply->readAll();
        Image image(m_imageData);

        emit imageDownloadComplete(QVariant::fromValue(image));
    }
    else if (m_pNetworkReply->error() != QNetworkReply::OperationCanceledError) {
        qWarning() << "Network reply error: " << m_pNetworkReply->errorString();
    }

    m_pNetworkReply = 0;
}

void HTTPImage::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    double t = bytesReceived / static_cast<double>(bytesTotal);
    setDownloadPercentage(static_cast<int>(t * 100.0));
}
