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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <audio/audio_manager_event.h>


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
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI()
    {
    }

    Q_INVOKABLE void startAudioManagerEventProcessing();
    Q_INVOKABLE void stopAudioManagerEventProcessing();

private slots:
    void onSystemLanguageChanged();
    void finishedAudioManagerEventProcessing();
public slots:
    void cleanUpOnExit();
private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
    void logMsg(const QVariant &msg);
    void processAudioManagerEvents();
    QString audioManagerEventName(audio_manager_event_type_t event);
    QFuture<void> *_future;
    QFutureWatcher<void> *_watcher;

};

#endif /* ApplicationUI_HPP_ */
