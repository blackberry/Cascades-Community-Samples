/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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
#include <bb/system/InvokeUpdateTargetFiltersRequest>
#include <bb/system/InvokeTargetFilter>

using namespace bb::cascades;
using namespace bb::system;

ApplicationUI::ApplicationUI() :
        QObject(),
        m_translator(new QTranslator(this)),
        m_localeHandler(new LocaleHandler(this)),
        m_invokeManager(new InvokeManager(this))
{
    // prepare the localization
    if (!QObject::connect(m_localeHandler, SIGNAL(systemLanguageChanged()),
            this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }

    // initial load
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
    QCoreApplication::instance()->removeTranslator(m_translator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("DeferredAutoStart_%1").arg(locale_string);
    if (m_translator->load(file_name, "app/native/qm")) {
    QCoreApplication::instance()->installTranslator(m_translator);
    }
}

void ApplicationUI::enableAutoStart()
{

    //Create the InvokeTargetFilter, which matches the filter that was
    //commented out from the bar-descriptor.xml.
InvokeTargetFilter autoStartFilter;
autoStartFilter.addAction("bb.action.system.STARTED");
autoStartFilter.addMimeType("application/vnd.blackberry.system.event.STARTED");
autoStartFilter.addUri("data://local");

    //Create the InvokeUpdateTargetFilterRequest and add the filter we just created.
InvokeUpdateTargetFiltersRequest updateRequest;
updateRequest.setTarget("com.example.DeferredAutoStartService");
QList<InvokeTargetFilter> filterList;
filterList.append(autoStartFilter);
updateRequest.setTargetFilters(filterList);

    //Use InvokeManager to update the filter.
InvokeManager manager;
manager.updateTargetFilters(updateRequest);


    //At this point the headless entry point of the application will auto run when the BlackBerry Smartphone
    //boots up.  However note that this does not start headless portion.  Left as is, it won't start
    //until next reboot.  The code below starts the headless application.

InvokeRequest request;
request.setTarget("com.example.DeferredAutoStartService");
request.setAction("bb.action.system.STARTED");
manager.invoke(request);
}
