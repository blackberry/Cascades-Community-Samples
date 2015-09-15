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

#include "service.hpp"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>

#include <QTimer>

using namespace bb::platform;
using namespace bb::system;

Service::Service() :
        QObject(),
        m_translator(new QTranslator(this)),
        m_invokeManager(new InvokeManager(this)),
        m_myBpsHandler(new MyBpsHandler())
{
    qDebug() << "Headless starting...";

    // Listen for Invoked signal
    m_invokeManager->connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

    // Listen for Locale change signal
    connect(m_myBpsHandler, SIGNAL(localeUpdated(const QString&)), this, SLOT(onSystemLanguageChanged(const QString&)));

    // Allow the app to send notifications to Hub
    NotificationDefaultApplicationSettings settings;
    settings.setPreview(NotificationPriorityPolicy::Allow);
    settings.apply();
}

void Service::onSystemLanguageChanged(const QString& newLocale)
{
    qDebug() << "Service::onSystemLanguageChanged()" << newLocale;

    // Append the locale code (ie: en_US, fr_FR, etc)
    QString file_name = QString("TranslateHLService_%1").arg(newLocale);
    qDebug() << "File name:" << file_name;

    if (m_translator->load(file_name, "app/native/qm")) {
        qDebug() << "File loaded successfully";
        QCoreApplication::instance()->installTranslator(m_translator);

        // Notify the hub with a localized string
        Notification::clearEffectsForAll();
        Notification::deleteAllFromInbox();
        Notification* notify = new Notification(this);
        notify->setTitle(tr("Default language used : English")); // <-- This will be translated if a translation is found
        notify->setBody(tr("The app isn't translated in the system language selected, the app will use the default one : English")); // <-- This will be translated if a translation is found
        notify->notify();
    }
}

void Service::handleInvoke(const bb::system::InvokeRequest & request)
{
    qDebug() << "Service::handleInvoke()" << request.action() << request.data() << request.metadata() << request.target() << request.uri();

    if (request.action().compare("com.example.TranslateHLService.SHUTDOWN") == 0) {
        // Used in development mode to force headless to stop to update it on next start
        bb::Application::instance()->quit();
    }
    else if (request.action().compare("com.example.TranslateHLService.LANGUAGE_CHANGE") == 0) {
        // Simulate a locale change signal (only used for headless)
        onSystemLanguageChanged(request.data());
    }
}
