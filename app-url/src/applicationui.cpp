/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
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

using namespace bb::cascades;

ApplicationUI::ApplicationUI() :
        QObject(),
        m_invokeManager(new bb::system::InvokeManager(this))
{
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

    //setup invokemanager stuff
    m_invokeManager->connect(m_invokeManager,
                            SIGNAL(invoked(const bb::system::InvokeRequest&)),
                            this,
                            SLOT(handleInvoke(const bb::system::InvokeRequest&)));

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    switch(m_invokeManager->startupMode()) {
        case bb::system::ApplicationStartupMode::LaunchApplication:
            // An application can initialize if it
            // was launched from the home screen
            // Set created root object as the application scene
            Application::instance()->setScene(root);
            break;
        case bb::system::ApplicationStartupMode::InvokeApplication:
            // If the application is invoked,
            // it must wait until it receives an invoked(..) signal
            // so that it can determine the UI that it needs to initialize
            break;
        default:
            // What app is it and how did it get here?
            break;
        }


}

void ApplicationUI::handleInvoke(const bb::system::InvokeRequest & request)
{
    qDebug() << "handle invoke: " << request.uri().host();

    if( request.uri().host().compare("configuration") == 0) {
           qDebug() << "configuration settings: ";
           qDebug() << "Host: " << request.uri().queryItemValue("host");
           qDebug() << "Port: " << request.uri().queryItemValue("port");
           qDebug() << "HTTPS: " << request.uri().queryItemValue("https");
           qDebug() << "Email: " << request.uri().queryItemValue("email");
           qDebug() << "Password: " << request.uri().queryItemValue("password");

           QmlDocument *qml = QmlDocument::create("asset:///config.qml").parent(this);

           // Create root object for the UI
           AbstractPane *root = qml->createRootObject<AbstractPane>();

           // Set created root object as the application scene
           Application::instance()->setScene(root);

           //populate values
           root->findChild<bb::cascades::Label*>("hostText")->setText(request.uri().queryItemValue("host").prepend("Host:  "));
           root->findChild<bb::cascades::Label*>("portText")->setText(request.uri().queryItemValue("port").prepend("Port:  "));
           root->findChild<bb::cascades::Label*>("httpsText")->setText(request.uri().queryItemValue("https").prepend("HTTPS:  "));
           root->findChild<bb::cascades::Label*>("emailText")->setText(request.uri().queryItemValue("email").prepend("Email:  "));
           root->findChild<bb::cascades::Label*>("passwordText")->setText(request.uri().queryItemValue("password").prepend("Password:  "));
    }
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("ConfigurationByURI_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}
