/*
 * Copyright (c) 2013-2014 BlackBerry Limited.
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
#include <bb/system/InvokeManager>

using namespace bb::cascades;
using namespace bb::system;

ApplicationUI::ApplicationUI() :
        QObject(),
        m_translator(new QTranslator(this)),
        m_localeHandler(new LocaleHandler(this)),
        m_invokeManager(new InvokeManager(this))
{
    // Start the headless part right away
    InvokeRequest request;
    request.setTarget("com.example.TranslateHLService");
    request.setAction("com.example.TranslateHLService.START");
    m_invokeManager->invoke(request);

    /*
     * bb::cascades::LocaleHandler
     *
     * When the language or locale of the device changes, this class is responsible
     * for dynamically updating text with any necessary changes.
     *
     * Tip: It relies on bb::cascades, which is not accessible in an headless app.
     *
     * This connect() function will call onSystemLanguageChanged() whenever the user
     * changes language or locale.
     *
     */
    if (!QObject::connect(m_localeHandler, SIGNAL(systemLanguageChanged()),
            this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }

    // initial load
    // Call onSystemLanguageChanged() directly to load the right translation on startup
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Make app available to the qml.
    qml->setContextProperty("app", this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    Application::instance()->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    // Remove old translation
    QCoreApplication::instance()->removeTranslator(m_translator);

    // Initiate, load and install the application translation files.
    /*
     * QLocale().name()
     *
     * Returns the language and country of this locale as a string of the
     * form "language_country", where language is a lowercase, two-letter
     * ISO 639 language code, and country is an uppercase, two- or three-letter
     * ISO 3166 country code.
     */
    QString locale_string = QLocale().name();

    // Append the locale to the name of the app to build the ts file name
    QString file_name = QString("TranslateHL_%1").arg(locale_string);

    // Try to load the correct ts file, if successful install the translation
    if (m_translator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_translator);
    }
}

void ApplicationUI::onSystemLanguageChanged(const QString& newLocale)
{
    // This function is used only for simulating a system language change

    // Remove old translation
    QCoreApplication::instance()->removeTranslator(m_translator);

    // Append the locale to the name of the app to build the ts file name
    QString file_name = QString("TranslateHL_%1").arg(newLocale);

    // Try to load the correct ts file, if successful install the translation
    if (m_translator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_translator);
    }
}

void ApplicationUI::invokeSettings()
{
    // Open the Settings card at the language section
    InvokeRequest request;
    request.setUri("settings://language");
    request.setTarget("sys.settings.card");
    request.setAction("bb.action.OPEN");
    m_invokeManager->invoke(request);
}

void ApplicationUI::sendToHeadless(const QString& command, QString parameters)
{
    // Start the headless part right away
    InvokeRequest request;
    request.setTarget("com.example.TranslateHLService");
    request.setAction("com.example.TranslateHLService." + command.toUpper());
    request.setData(parameters.toUtf8());
    m_invokeManager->invoke(request);

    // Only used during app development
    if (command == "SHUTDOWN")
        bb::Application::instance()->quit();
}
