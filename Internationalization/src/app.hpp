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

#ifndef APP_H
#define APP_H

#include <QtCore/QObject>
#include <QtCore/QMetaType>

#include <bb/cascades/Event>
#include <bb/cascades/UiObject>
#include <bb/cascades/Control>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>

class App : public QObject
{
    Q_OBJECT

public:
    App();

    /*
     * Sets the cascades::Application pointer to allow translation to be
     * changed at runtime from the App class.
     *
     * @param app - the pointer to bb::cascades::Application
     * @param translator - pointer to the current translator
     * @param locale - the locale the application was launched with
     */
    void setApplication(bb::cascades::Application* app, QTranslator* translator, QString currentLocale);

    /*
     * Refreshes the UI with the specified locale
     *
     * @param locale - the locale to change to
     */
    Q_INVOKABLE void updateLocale(QString locale);

    /*
     * Allows the current language to be retrieved from QML
     *
     * @return the current language (translated)
     */
    Q_INVOKABLE QString getCurrentLanguage();

    /*
     * Workaround to force keyboard to hide when readonly text area is touched
     */
    Q_INVOKABLE void suppressKeyboard();

    /*
     * Exposed method to the QML to load a new xml document into the XmlDataModel
     *
     * @param name - the name of the xml to load (not the full filename)
     */
    Q_INVOKABLE void loadXML(QString name);

private:
    bb::cascades::Application* m_app;
    bb::cascades::AbstractPane* m_root;
    QString m_currentLocale;
    QString m_currentXML;
    QTranslator* m_translator;
};

#endif // ifndef APP_H
