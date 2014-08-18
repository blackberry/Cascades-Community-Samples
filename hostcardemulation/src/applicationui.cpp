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

ApplicationUI::ApplicationUI()
    : QObject()
    , _orgName("BlackBerry Ltd")
    , _appName("HostCardEmulation Settings")
    , _nfcListener(new NfcListener(this))
{
    _translator = new QTranslator(this);
    _pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(_pLocaleHandler, SIGNAL(systemLanguageChanged()),
                                           this, SLOT(onSystemLanguageChanged()));
    Q_ASSERT(res);
    Q_UNUSED(res);

    onSystemLanguageChanged();

    _qml = QmlDocument::create("asset:///main.qml").parent(this);
    _root = _qml->createRootObject<AbstractPane>();
    _mainPage = _root->findChild<QObject*>("mainPage");

    // ============== Message to be sent to page

    if(!QObject::connect(     this, SIGNAL(message(QVariant)),
                         _mainPage, SLOT(onMessage(QVariant)))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - this connect failed - onMessage" << strerror(errno) << endl;
    }

    if(!QObject::connect(_nfcListener, SIGNAL(message(QVariant)),
                            _mainPage, SLOT(onMessage(QVariant)))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - NfcListener connect failed - onMessage" << strerror(errno) << endl;
    }

    if(!QObject::connect(_nfcListener, SIGNAL(featureSetSupported(QVariant)),
                            _mainPage, SLOT(onFeatureSetSupported(QVariant)))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - NfcListener connect failed - onFeatureSetSupported" << strerror(errno) << endl;
    }

    if(!QObject::connect(     this, SIGNAL(message(QVariant)),
                         _mainPage, SLOT(onMessage(QVariant)))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onMessage" << strerror(errno) << endl;
    }

    if(!QObject::connect(_nfcListener, SIGNAL(emulationStarted()),
                            _mainPage, SLOT(onEmulationStarted()))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onEmulationStarted" << strerror(errno) << endl;
    }

    if(!QObject::connect(_nfcListener, SIGNAL(emulationStopped()),
                            _mainPage, SLOT(onEmulationStopped()))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onEmulationStopped" << strerror(errno) << endl;
    }

    if(!QObject::connect(_nfcListener, SIGNAL(aidRegistered(QVariant)),
                            _mainPage, SLOT(onAidRegistered(QVariant)))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onAidRegistered" << strerror(errno) << endl;
    }

    if(!QObject::connect(_nfcListener, SIGNAL(aidUnregistered(QVariant)),
                            _mainPage, SLOT(onAidUnregistered(QVariant)))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onAidUnregistered" << strerror(errno) << endl;
    }

    // ============== Hook up buttons

    if(!QObject::connect(   _mainPage, SIGNAL(startEmulating()),
                         _nfcListener, SLOT(onStartEmulating()))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onStartEmulating" << strerror(errno) << endl;
    }

    if(!QObject::connect(   _mainPage, SIGNAL(stopEmulating()),
                         _nfcListener, SLOT(onStopEmulating()))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onStopEmulating" << strerror(errno) << endl;
    }

    if(!QObject::connect(   _mainPage, SIGNAL(registerAid()),
                         _nfcListener, SLOT(onRegisterAid()))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onRegisterAid" << strerror(errno) << endl;
    }

    if(!QObject::connect(   _mainPage, SIGNAL(unregisterAid()),
                         _nfcListener, SLOT(onUnregisterAid()))) {
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onUnregisterAid" << strerror(errno) << endl;
    }

    Application::instance()->setScene(_root);

    if (getAid().isNull() || getAid().isEmpty()) {
        saveAid(AID_DEFAULT_VALUE);
    }
    _nfcListener->setAid(getAid());
    _nfcListener->startListening();
}

ApplicationUI::~ApplicationUI()
{
    _nfcListener->stopListening();
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(_translator);
    QString localeString = QLocale().name();
    QString fileName = QString("hostcardemulation_%1").arg(localeString);
    if (_translator->load(fileName, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(_translator);
    }
}

void ApplicationUI::saveAid(QString aid)
{
    qDebug() << "XXXX save: aid=" << aid << endl;

    QSettings qSettings(_orgName, _appName);
    qSettings.setValue(AID_SETTING_KEY, QVariant(aid.trimmed()));
    qSettings.sync();
}

QString ApplicationUI::getAid()
{
    QSettings qSettings(_orgName, _appName);
    if (qSettings.value(AID_SETTING_KEY).isNull()) {
        return QString("");
    }
    return qSettings.value(AID_SETTING_KEY).toString();
}
