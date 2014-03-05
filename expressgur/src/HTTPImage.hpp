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
 
#pragma once

#include <QObject>
#include <QUrl>

class QNetworkReply;

/**
 * Class that takes care of downloading single images over HTTP from flickr backend
 */
class HTTPImage : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged);
    Q_PROPERTY(int downloadPercentage READ downloadPercentage NOTIFY downloadPercentageChanged);

public:
    HTTPImage(QObject *parent = 0);
    virtual ~HTTPImage();

    QString url() const;
    void setUrl(const QString& url);

    int downloadPercentage() const;

    Q_INVOKABLE QString saveImage(const QString& folder);

signals:
    void urlChanged();
    void imageDownloadComplete(const QVariant& image);
    void downloadPercentageChanged(int value);

private slots:
    void onImageDownloaded();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    void setDownloadPercentage(int percent);
    void downloadImage(const QUrl& url);

    QString m_url;
    QNetworkReply *m_pNetworkReply;
    int m_downloadPercentage;
    QByteArray m_imageData;
};
