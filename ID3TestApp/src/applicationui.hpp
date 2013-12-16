/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <id3/tag.h>
#include <id3/utils.h>
#include <id3/misc_support.h>
#include <id3/readers.h>
#include <id3/io_helpers.h>

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

class QTranslator;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI: public QObject
{
    Q_OBJECT

    Q_PROPERTY(
            QString artist
            READ getArtist
            NOTIFY signalArtistChanged)

    Q_PROPERTY(
            QString album
            READ getAlbum
            NOTIFY signalAlbumChanged)

    Q_PROPERTY(
            QString title
            READ getTitle
            NOTIFY signalTitleChanged)

    Q_PROPERTY(
            QString genre
            READ getGenre
            NOTIFY signalGenreChanged)

    Q_PROPERTY(
            QString year
            READ getYear
            NOTIFY signalYearChanged)

    Q_PROPERTY(
            QString bitrate
            READ getBitrate
            NOTIFY signalBitrateChanged)

    Q_PROPERTY(
            QString frequency
            READ getFrequency
            NOTIFY signalFrequencyChanged)

    Q_PROPERTY(
            QString version
            READ getVersion
            NOTIFY signalVersionChanged)

    Q_PROPERTY(
            QString layer
            READ getLayer
            NOTIFY signalLayerChanged)

public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI()
    {
    }
private slots:
    void onSystemLanguageChanged();
    QString getArtist();
    QString getAlbum();
    QString getTitle();
    QString getGenre();
    QString getYear();
    QString getBitrate();
    QString getFrequency();
    QString getVersion();
    QString getLayer();

    signals:
    void signalArtistChanged();
    void signalAlbumChanged();
    void signalTitleChanged();
    void signalGenreChanged();
    void signalYearChanged();
    void signalBitrateChanged();
    void signalFrequencyChanged();
    void signalVersionChanged();
    void signalLayerChanged();

public slots:
    void showMetaData(const QString file_url);

private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
    void extractMp3Tags(const ID3_Tag &myTag);

    QString _artist;
    QString _album;
    QString _title;
    QString _genre;
    QString _year;
    QString _bitrate;
    QString _frequency;
    QString _version;
    QString _layer;
};

#endif /* ApplicationUI_HPP_ */
