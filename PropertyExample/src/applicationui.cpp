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

using namespace bb::cascades;

static AbstractPane *_root = 0;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
{

    _some_text = QString("Hello I am a property");

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

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    _root = qml->createRootObject<AbstractPane>();

    // expose the property to QML
    qml->setContextProperty("my_prop", this);

    // Set created root object as the application scene
    app->setScene(_root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("PropertyExample_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

QString ApplicationUI::getMyProperty() {
    logMsg("getMyProperty called");
    return _some_text;
}

void ApplicationUI::setMyProperty(QString some_text) {
    logMsg("setMyProperty called:" + some_text);
    _some_text = some_text;
    logMsg("emiting signalMyPropertyChanged");
    emit signalMyPropertyChanged();
}

void ApplicationUI::resetMyProperty() {
    logMsg("resetMyProperty called");
    setMyProperty("Hello I am a property");
}


void ApplicationUI::logMsg(const QVariant &msg) {
    qDebug() << "XXXX " << msg;
    QMetaObject::invokeMethod(_root, "logMessage", Q_ARG(QVariant, msg));
}
