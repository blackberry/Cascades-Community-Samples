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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <QFuture>
#include <QFutureWatcher>
#include <unistd.h>
#include <audio/audio_manager_event.h>

using namespace bb::cascades;

static AbstractPane *_root = 0;
static bool audio_manager_events_required = false;
static     audio_manager_event_context_t *event_context;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    if (!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }
    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    _root = qml->createRootObject<AbstractPane>();

    // expose this object to QML
    qml->setContextProperty("ame", this);

    // Set created root object as the application scene
    app->setScene(_root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("AudioManagerEvents_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::logMsg(const QVariant &msg)
{
    qDebug() << "XXXX " << msg;
    QMetaObject::invokeMethod(_root, "logMessage", Q_ARG(QVariant, msg));
}

// called from QML
void ApplicationUI::startAudioManagerEventProcessing()
{
    qDebug() << "XXXX startAudioManagerEventProcessing()";

    audio_manager_events_required = true;

    _future = new QFuture<void>;
    _watcher = new QFutureWatcher<void>;
    *_future = QtConcurrent::run(this, &ApplicationUI::processAudioManagerEvents);
    _watcher->setFuture(*_future);
    QObject::connect(_watcher, SIGNAL(finished()), this, SLOT(finishedAudioManagerEventProcessing()));
}

// called from QML
void ApplicationUI::stopAudioManagerEventProcessing()
{
    qDebug() << "XXXX stopAudioManagerEventProcessing()";

    audio_manager_events_required = false;

    audio_manager_release_event_context(&event_context);

    logMsg("Stopping AudioManager Event capture");
}

void ApplicationUI::processAudioManagerEvents()
{
    logMsg("Starting audio event processing");

    int rc = 0;
    int errno = 0;
    void * event_params;
    audio_manager_event_type_t event_type;

    logMsg("Getting audio manager event context");
    rc = audio_manager_get_event_context(&event_context);
    if (rc == 0) {
        logMsg("audio_manager_get_event_context - OK");
    } else {
        logMsg(QString("audio_manager_get_event_context - rc=%1 errno=%2 : %3").arg(rc).arg(errno).arg(strerror(errno)));
        audio_manager_events_required = false;
        return;
    }

    logMsg("Got audio manager event context");

    logMsg("Registering for Audio Manager Events");

    audio_manager_add_routing_change_event(event_context);

    logMsg("Waiting for Audio Manager Events");

    while (audio_manager_events_required) {

        errno = 0;
        rc = audio_manager_get_event(event_context, &event_type, &event_params);
        if (rc == 0) {
            qDebug() << "XXXX audio_manager_get_event - OK";
            logMsg(audioManagerEventName(event_type));
            // indicate this event can be disposed of
            audio_manager_dispatch_event(event_context, event_type, event_params);
        } else {
            logMsg(QString("audio_manager_get_event - rc=%1 errno=%2 : %3").arg(rc).arg(errno).arg(strerror(errno)));
            audio_manager_events_required = false;
            return;
        }

    }

    qDebug() << "XXXX processAudioManagerEvents() - exiting";
}

void ApplicationUI::finishedAudioManagerEventProcessing()
{
    qDebug() << "XXXX finishedAudioManagerEventProcessing()";
    logMsg("Finished capturing audio events");

}

void ApplicationUI::cleanUpOnExit()
{
    qDebug() << "XXXX Clean up on application exit";
    audio_manager_events_required = false;
    audio_manager_release_event_context(&event_context);

}

QString ApplicationUI::audioManagerEventName(audio_manager_event_type_t event_type)
{
    if (event_type == AUDIO_ROUTING_CHANGE) {
        return QString("AUDIO_ROUTING_CHANGE");
    }
    if (event_type == AUDIO_CONCURRENCY_CHANGE) {
        return QString("AUDIO_CONCURRENCY_CHANGE");
    }
    if (event_type == AUDIO_DEVICE_CHANGE) {
        return QString("AUDIO_DEVICE_CHANGE");
    }
    if (event_type == AUDIO_VOLUME_CHANGE) {
        return QString("AUDIO_VOLUME_CHANGE");
    }
    if (event_type == AUDIO_HEADPHONE_BOOST_CHANGE) {
        return QString("AUDIO_HEADPHONE_BOOST_CHANGE");
    }
    if (event_type == AUDIO_VOICE_VOLUME_CHANGE) {
        return QString("AUDIO_VOICE_VOLUME_CHANGE");
    }
    if (event_type == AUDIO_VOICE_SERVICE_CHANGE) {
        return QString("AUDIO_VOICE_SERVICE_CHANGE");
    }
    if (event_type == AUDIO_DEVICE_BUTTON_CHANGE) {
        return QString("AUDIO_DEVICE_BUTTON_CHANGE");
    }
    if (event_type == AUDIO_HEADPHONE_UNSAFE_ZONE_CHANGE) {
        return QString("AUDIO_HEADPHONE_UNSAFE_ZONE_CHANGE");
    }
    if (event_type == AUDIO_STAT_CHANGE) {
        return QString("AUDIO_STAT_CHANGE");
    }
    if (event_type == AUDIO_VOICE_OPTION_CHANGE) {
        return QString("AUDIO_VOICE_OPTION_CHANGE");
    }
    if (event_type == AUDIO_HEADPHONE_OUTPUT_VOLUME_REGULATION_CHANGE) {
        return QString("AUDIO_HEADPHONE_OUTPUT_VOLUME_REGULATION_CHANGE");
    }
    if (event_type == AUDIO_EVENT_TYPE_COUNT) {
        return QString("AUDIO_EVENT_TYPE_COUNT");
    }
    return QString("UNKNOWN EVENT");

}
