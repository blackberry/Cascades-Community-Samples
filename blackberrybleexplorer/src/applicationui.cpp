/* Copyright (c) 2013 BlackBerry Limited.
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

#include "DevicesManager.hpp"
#include "ServicesManager.hpp"
#include "CharacteristicsManager.hpp"
#include "Timer.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Container>
#include <bb/cascades/SceneCover>

#include <bb/device/DisplayInfo>
#include <unistd.h>

using namespace bb::cascades;

static AbstractPane *_root = 0;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    qmlRegisterType<bb::device::DisplayInfo>("bb.displayInfo", 1, 0, "DisplayInfo");
    qmlRegisterType<Timer>("CustomTimer", 1, 0, "Timer");

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // want to instantiate these singletons here first so they're hooked
    // into QObject hierarchy under this QObject.

    DevicesManager *dm = DevicesManager::getInstance(this);
    ServicesManager *sm = ServicesManager::getInstance(this);
    CharacteristicsManager *cm = CharacteristicsManager::getInstance(this);
    DataContainer *dc = DataContainer::getInstance();

    Q_ASSERT(sm != NULL);
    Q_ASSERT(cm != NULL);

    // initial load
    onSystemLanguageChanged();

    qmlRegisterType<QTimer>("utils", 1, 0, "QTimer");
    qmlRegisterType<QPropertyAnimation>("bb.cascades", 1, 0, "QPropertyAnimation");

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    QmlDocument::defaultDeclarativeEngine()->rootContext()->setContextProperty("app", this);
    QmlDocument::defaultDeclarativeEngine()->rootContext()->setContextProperty("data", dc);
    QmlDocument::defaultDeclarativeEngine()->rootContext()->setContextProperty("smgr", sm);
    QmlDocument::defaultDeclarativeEngine()->rootContext()->setContextProperty("cmgr", cm);
    QmlDocument::defaultDeclarativeEngine()->rootContext()->setContextProperty("cmgrModel", cm->model());

    // set up the application's cover
    qDebug() << "XXXX setting up active frame";
    QmlDocument *qmlCover = QmlDocument::create("asset:///cover.qml").parent(this);
    qmlCover->setContextProperty("data", dc);
    Container *coverContainer = qmlCover->createRootObject<Container>();
    SceneCover *cover = SceneCover::create().content(coverContainer);
    Application::instance()->setCover(cover);

    // Create root object for the UI
    _root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    app->setScene(_root);
    QObject *mainPage = _root->findChild<QObject*>((const QString) "mainPage");

    if (mainPage) {
        qDebug() << "XXXX found deviceCarousel";
        QObject::connect(dm, SIGNAL(setDeviceCount(QVariant)), mainPage, SLOT(setDeviceCount(QVariant)), Qt::QueuedConnection);
        QObject::connect(dm, SIGNAL(setDeviceCount(QVariant)), coverContainer, SLOT(setDeviceCount(QVariant)), Qt::QueuedConnection);
        QObject::connect(dm, SIGNAL(finishedScanningForDevices()), mainPage, SLOT(stopActivityIndicator()), Qt::QueuedConnection);
        QObject::connect(dm, SIGNAL(startedScanningForDevices()), mainPage, SLOT(startActivityIndicator()), Qt::QueuedConnection);
    } else {
        qDebug() << "XXXX NOT found deviceCarousel";
    }

    findBleDevices();

}

void ApplicationUI::findBleDevices() {
    DevicesManager *dm = DevicesManager::getInstance(this);
    _future = new QFuture<void>;
    _watcher = new QFutureWatcher<void>;
    _mutex.lock();
    *_future = QtConcurrent::run(dm, &DevicesManager::findBleDevices);
    _watcher->setFuture(*_future);
    QObject::connect(_watcher, SIGNAL(finished()), this, SLOT(finishedSearching()));
}

void ApplicationUI::finishedSearching() {
    qDebug() << "YYYY BLE device search thread has finished running";
    _mutex.unlock();
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("BlackBerryBleExplorer_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::deviceSelected(int index) {
    QString selected_addr = DataContainer::getInstance()->getDeviceAddr(index);
    qDebug() << "XXXX deviceSelected:" << selected_addr;
    emit deviceSelected(QVariant(index), QVariant(selected_addr));
}
