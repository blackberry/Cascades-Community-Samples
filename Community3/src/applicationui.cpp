/* Copyright (c) 2014 BlackBerry Ltd.
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
 *
 * The Lithium REST APIs are owned by Lithium Technologies, Inc.
 * (http://www.lithium.com) and are not part of the “Works” licensed
 * hereunder pursuant to the Apache 2.0 license.
 */

#include "applicationui.hpp"
#include "CategoryProvider.hpp"
#include "BoardProvider.hpp"
#include "ThreadListProvider.hpp"
#include "ThreadProvider.hpp"
#include "Credentials.hpp"
#include "Authenticator.hpp"
#include "MessageWorker.hpp"
#include "bbm/BBMHandler.hpp"
#include "EULA.hpp"
#include "Searcher.hpp"
#include "URLProvider.hpp"
#include "BoardProperties.hpp"
#include "ApplicationSettings.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>

using namespace bb::cascades;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
, mAuthenticator (Authenticator::Instance())
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    if(!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }
    // initial load
    onSystemLanguageChanged();

    //Kick off BBM Registration.
    //ToDo: Define your own UUID here. You can generate one here: http://www.guidgenerator.com/
	const QString uuid(QLatin1String(""));
	BBMHandler *bbmHandler = new BBMHandler(uuid, app);
	bbmHandler->registerApplication();

    // The model for populating the forum data are registered, so that it and all its
    // properties can be accessed directly from QML.
    qmlRegisterType<CategoryProvider>("com.msohm.CategoryProvider", 1, 0, "CategoryProvider");
    qmlRegisterType<BoardProvider>("com.msohm.BoardProvider", 1, 0, "BoardProvider");
    qmlRegisterType<ThreadListProvider>("com.msohm.ThreadListProvider", 1, 0, "ThreadListProvider");
    qmlRegisterType<ThreadProvider>("com.msohm.ThreadProvider", 1, 0, "ThreadProvider");
    qmlRegisterType<Credentials>("com.msohm.Credentials", 1, 0, "Credentials");
    qmlRegisterType<MessageWorker>("com.msohm.MessageWorker", 1, 0, "MessageWorker");
    qmlRegisterType<EULA>("com.msohm.EULA", 1, 0, "EULA");
    qmlRegisterType<Searcher>("com.msohm.Searcher", 1, 0, "Searcher");
    qmlRegisterType<URLProvider>("com.msohm.URLProvider", 1, 0, "URLProvider");
    qmlRegisterType<BoardProperties>("com.msohm.BoardProperties", 1, 0, "BoardProperties");
    qmlRegisterType<ApplicationSettings>("com.msohm.ApplicationSettings", 1, 0, "ApplicationSettings");

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    //Expose the BBM Registration handler to main.qml.
	qml->setContextProperty("bbmHandler", bbmHandler);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    app->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("BBCommunityApp_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}
