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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <id3/tag.h>
#include <id3/utils.h>
#include <id3/misc_support.h>
#include <id3/readers.h>
#include <id3/io_helpers.h>

using namespace bb::cascades;

static AbstractPane *_root = 0;

static const char ** genre_table = ID3_v1_genre_description;

static const int genre_count = ID3_NR_OF_V1_GENRES;

const char *GetGenreFromNum(int genre_id)
{
    if ((genre_id >= 0) && (genre_id < genre_count))
        return genre_table[genre_id];
    else
        return "Unknown";
}

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app), _artist("---"), _album("---"), _title("---"), _bitrate("---"), _frequency("---"), _version("---"), _layer("---"), _genre("---"), _year("---")
{
    qDebug() << "XXXX Starting.....";
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    _root = qml->createRootObject<AbstractPane>();

    qml->setContextProperty("app", this);

    QObject *main = _root->findChild<QObject*>((const QString) "mainpage");
    connect(main, SIGNAL(showMetaData(const QString &)), this, SLOT(showMetaData(const QString &)));

    // Set created root object as the application scene
    app->setScene(_root);

}

void ApplicationUI::extractMp3Tags(const ID3_Tag &myTag)
{

    const Mp3_Headerinfo* mp3info;
    mp3info = myTag.GetMp3HeaderInfo();

    bool v1 = myTag.HasV1Tag();
    bool v2 = myTag.HasV2Tag();

    if (mp3info) {
        _bitrate = QString::number(mp3info->bitrate);
        qDebug() << "XXXX bitrate="<<_bitrate;
        _frequency = QString::number(mp3info->frequency);
        qDebug() << "XXXX frequency="<<_frequency;
        switch (mp3info->version) {
            case MPEGVERSION_2_5:
                _version = QString("MPEG2.5");
                break;
            case MPEGVERSION_2:
                _version = QString("MPEG2");
                break;
            case MPEGVERSION_1:
                _version = QString("MPEG1");
                break;
            default:
                _version = QString("Unknown");
                break;
        }
        switch (mp3info->layer) {
            case MPEGLAYER_III:
                _layer = QString("layer III");
                break;
            case MPEGLAYER_II:
                _layer = QString("layer II");
                break;
            case MPEGLAYER_I:
                _layer = QString("layer I");
                break;
            default:
                _layer = QString("Unknown");
                break;
        }

    }

    ID3_Tag::ConstIterator* iter = myTag.CreateIterator();
    const ID3_Frame* frame = NULL;
    while (NULL != (frame = iter->GetNext())) {
        ID3_FrameID eFrameID = frame->GetID();
        const char* desc = frame->GetDescription();
        if (!desc) desc = "";
        qDebug() << "XXXX frame ID:" << frame->GetTextID();
        qDebug() << "XXXX frame desc::" << desc;
        char *sText = ID3_GetString(frame, ID3FN_TEXT);
        qDebug() << "XXXX text:" << QString(sText);
        switch (eFrameID) {
            case ID3FID_ALBUM: {
                char *sText = ID3_GetString(frame, ID3FN_TEXT);
                _album = QString(sText);
                delete[] sText;
                break;
            }
            case ID3FID_TITLE: {
                char *sText = ID3_GetString(frame, ID3FN_TEXT);
                _title = QString(sText);
                delete[] sText;
                break;
            }
            case ID3FID_LEADARTIST: {
                char *sText = ID3_GetString(frame, ID3FN_TEXT);
                _artist = QString(sText);
                delete[] sText;
                break;
            }
            case ID3FID_YEAR: {
                char *sText = ID3_GetString(frame, ID3FN_TEXT);
                _year = QString(sText);
                delete[] sText;
                break;
            }
            case ID3FID_CONTENTTYPE:
            {
              const char* genre_str;
              int genre_id = 255;
              char *sText = ID3_GetString(frame, ID3FN_TEXT);
              sscanf(sText, "(%d)", &genre_id);
              if (genre_id != 255) {
                  genre_str = GetGenreFromNum(genre_id);
              } else {
                  genre_str = sText;
              }
              _genre = QString(genre_str);
              delete [] sText;
              break;
            }

            default: {
                break;
            }
        }
    }
    delete iter;

}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Id3TestApp_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::showMetaData(QString file_url)
{
    qDebug() << "XXXX showMetaData:" << file_url;
    ID3_Tag myTag;
    QString mp3(file_url);
    myTag.Link(mp3.toLatin1().constData(), ID3TT_ALL);

    _artist="";
    _album="";
    _title="";
    _genre="";
    _year="";
    _bitrate="";
    _frequency="";
    _version="";
    _layer="";

    extractMp3Tags(myTag);

    emit signalArtistChanged();
    emit signalAlbumChanged();
    emit signalTitleChanged();
    emit signalGenreChanged();
    emit signalYearChanged();
    emit signalBitrateChanged();
    emit signalFrequencyChanged();
    emit signalVersionChanged();
    emit signalLayerChanged();
}

QString ApplicationUI::getArtist()
{
    qDebug() << "XXXX getArtist:" << _artist;
    return _artist;
}

QString ApplicationUI::getAlbum()
{
    qDebug() << "XXXX getAlbum:" << _album;
    return _album;
}

QString ApplicationUI::getTitle()
{
    qDebug() << "XXXX getTitle:" << _title;
    return _title;
}

QString ApplicationUI::getGenre()
{
    qDebug() << "XXXX getGenre:" << _genre;
    return _genre;
}

QString ApplicationUI::getYear()
{
    qDebug() << "XXXX getYear:" << _genre;
    return _year;
}

QString ApplicationUI::getBitrate()
{
    qDebug() << "XXXX getBitrate:" << _bitrate;
    return _bitrate;
}

QString ApplicationUI::getFrequency()
{
    qDebug() << "XXXX getFrequency:" << _frequency;
    return _frequency;
}

QString ApplicationUI::getVersion()
{
    qDebug() << "XXXX getVersion:" << _version;
    return _version;
}

QString ApplicationUI::getLayer()
{
    qDebug() << "XXXX getLayer:" << _layer;
    return _layer;
}
