/*
 * Copyright (c) 2014 BlackBerry Limited.
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
#include "HTTPImage.hpp"
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>

#include <bb/cascades/Button>
#include <bb/system/CardDoneMessage>
#include <QtGui/QImage>
#include <bb/system/SystemToast>
using namespace bb::cascades;
using namespace bb::system;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) : QObject(app), m_InvokeManager(new InvokeManager(this))
{
    connect(m_InvokeManager, SIGNAL(invoked(const bb::system::InvokeRequest &)), this, SLOT(onInvoked(const bb::system::InvokeRequest &)));


    		m_pTranslator = new QTranslator(this);
    	    m_pLocaleHandler = new LocaleHandler(this);

    	    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    	    connect(m_InvokeManager, SIGNAL(invoked(const bb::system::InvokeRequest &)), this, SLOT(onInvoked(const bb::system::InvokeRequest &)));


    	    // This is only available in Debug builds
    	    Q_ASSERT(res);
    	    // Since the variable is not used in the app, this is added to avoid a
    	    // compiler warning
    	    Q_UNUSED(res);

    	    // initial load
    	    onSystemLanguageChanged();
    	    qmlRegisterType<HTTPImage>("com.example.httpimage", 1, 0, "HTTPImage");
    	    // Create scene document from main.qml asset, the parent is set
    	    // to ensure the document gets destroyed properly at shut down.
    	    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    	    qml->setContextProperty("application", this);
    	    // Create root object for the UI
    	    AbstractPane *root = qml->createRootObject<AbstractPane>();

    	    // Set created root object as the application scene
    	    app->setScene(root);

    	    connect(root, SIGNAL(cancel()), this, SLOT(onCancel()));
    	    connect(root, SIGNAL(select(const QString&)), this, SLOT(onSelect(const QString&)));


}

QString ApplicationUI::findImage(const QVariant item)
{
    // The item variable is the xml data from rss-feed
    QVariantMap itemData = item.toMap();

    // Some streams have the image data in a media:content container, of so let's get it
    if (itemData.contains("media:content")) {
        QVariantMap itemMediaMap;

        if (itemData["media:content"].canConvert<QVariantList>()) {
            QVariantList itemMediaList = itemData["media:content"].toList();

            if (itemMediaList.size() > 0) {
                itemMediaMap = itemMediaList.at(1).toMap();
            } else {
                itemMediaMap = itemMediaList.at(0).toMap();
            }

        } else if (itemData["media:content"].canConvert<QVariantMap>()) {
            itemMediaMap = itemData["media:content"].toMap();

        } else {
            // Well, there was some new way we have not encountered before
            qDebug() << "could not convert " << itemData["media:content"];
        }
        // Sometimes it's an URL
        if (itemMediaMap.contains("url")) {
            return itemMediaMap["url"].toString();
        }
    // If the image data is not in media:content it could be in content:encoded, let's check here
    } else {
        // We might find the image data in either the content:encoded or the description part
        QString description;
        if (itemData.contains("content:encoded")) {
            description = itemData["content:encoded"].toString();
        }

        if (description.isNull() && itemData.contains("description")) {
            description = itemData["description"].toString();
        }

        if (!description.isNull()) {
            // Grab the first img tag in the description and use it as source
            int firstIndex = description.indexOf(QRegExp("<img [^>]*/>"));
            int sourceIndexStart = description.indexOf("src=\"", firstIndex) + 5;
            int sourceIndexEnd = description.indexOf('"', sourceIndexStart);

            QString imageUrl = description.mid(sourceIndexStart, sourceIndexEnd - sourceIndexStart);

            if (!imageUrl.isEmpty() && imageUrl.contains("http:")) {
                return imageUrl;
            }
        }
    }
    // We found nothing, return with sadness
    return "";
}

void ApplicationUI::onInvoked(const bb::system::InvokeRequest &req) {
    qDebug() << __PRETTY_FUNCTION__;
    qDebug() << req.metadata() << req.source().installId();

}
void ApplicationUI::onCancel()
{
    if(m_InvokeManager->startupMode() == bb::system::ApplicationStartupMode::LaunchApplication) {
        Application::instance()->requestExit();
    } else { // Its a card
        CardDoneMessage response;
        response.setReason("Cancel");
        m_InvokeManager->sendCardDone(response);
        qDebug() << "Sending sendCardDone message to invoking parent. Response data: " << response.data();
    }
}

void ApplicationUI::onSelect(const QString& path)
{
    qDebug() << "onSelect" << path;

    if(m_InvokeManager->startupMode() == bb::system::ApplicationStartupMode::LaunchApplication) {
            //Application::instance()->requestExit();
            bb::system::SystemToast *toast = new bb::system::SystemToast(this);
            toast->setPosition(bb::system::SystemUiPosition::MiddleCenter);
            toast->setBody("Image saved to your pictures.");
            toast->show();
        } else { // Its a card
            CardDoneMessage response;
            response.setReason("Done");
            response.setDataType("image/png");
            response.setData("file://"+path);
            m_InvokeManager->sendCardDone(response);
            qDebug() << "Sending sendCardDone message to invoking parent. Response data: " << response.data();
        }
}


void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("expressgur_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}


