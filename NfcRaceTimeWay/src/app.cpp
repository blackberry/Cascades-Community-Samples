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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <QSettings>

#include "app.hpp"
#include "Timer.hpp"
#include "NfcListener.hpp"

using namespace bb::cascades;

App::App(Application *app) {

	qmlRegisterType<Timer>("CustomTimer", 1, 0, "Timer");

	QCoreApplication::setOrganizationName("Research In Motion");
	QCoreApplication::setOrganizationDomain("rim.com");
    QCoreApplication::setApplicationName("NfcRace Settings");

	_app = app;
    _invokeManager = new bb::system::InvokeManager();
	if (_invokeManager->startupMode() == bb::system::ApplicationStartupMode::InvokeApplication) {
		_launchedByInvoke = true;
	}

	_qml = QmlDocument::create("asset:///main.qml");
    _qml->setContextProperty("_app", this);
    QDeclarativePropertyMap* propertyMap = new QDeclarativePropertyMap;
	propertyMap->insert("LaunchedByInvoke", QVariant(_launchedByInvoke ? "true" : "false"));
	_qml->setContextProperty("_propertyMap", propertyMap);
    _root = _qml->createRootObject<AbstractPane>();

	initialiseListeners();

    _app->setScene(_root);
}

App::~App() {
}

QString App::getValueFor(const QString &objectName, const QString &defaultValue) {

	QSettings settings;

    if (settings.value(objectName).isNull()) {
        return defaultValue;
    }

    return settings.value(objectName).toString();
}

void App::saveValueFor(const QString &objectName, const QString &inputValue)
{
    QSettings settings;
    settings.setValue(objectName, QVariant(inputValue));
}

void App::initialiseListeners() {

	qDebug() << "App::initialiseNfcListener";
	_nfcListener = new NfcListener();
	QObject *obj = _root->findChild<QObject*>((const QString) "regDetails");

	QObject::connect(  _nfcListener,   SIGNAL(raceTagDetected(const QVariant &)),
			                    obj,     SLOT(raceTagDetected(const QVariant &)));

	QObject::connect(_invokeManager,   SIGNAL(invoked(const bb::system::InvokeRequest&)),
			           _nfcListener,     SLOT(receivedInvokeRequest(const bb::system::InvokeRequest&)));

	QObject::connect(          this,   SIGNAL(startListening()),
			           _nfcListener,     SLOT(startListening()));

	QObject::connect(          this,   SIGNAL(stopListening()),
			           _nfcListener,     SLOT(stopListening()));

	QObject::connect(           obj,   SIGNAL(startEmulatingTag(const QString &)),
			           _nfcListener,     SLOT(startTagEmulation(const QString &)));

	QObject::connect(           obj,   SIGNAL(updateEmulatingTag(const QString &)),
			           _nfcListener,     SLOT(updateTagEmulation(const QString &)));

	QObject::connect(           obj,   SIGNAL(stopEmulatingTag()),
			           _nfcListener,     SLOT(stopTagEmulation()));

	QObject::connect(  _nfcListener,   SIGNAL(tagEmulationSelectEvent()),
					            obj,     SLOT(tagEmulationSelectEvent()));

	QObject::connect(  _nfcListener,   SIGNAL(tagEmulationReadEvent()),
			                    obj,     SLOT(tagEmulationReadEvent()));

	QObject::connect(  _nfcListener,   SIGNAL(tagEmulationLeftEvent()),
			                    obj,     SLOT(tagEmulationLeftEvent()));

	emit startListening();
}

void App::exitApp() {
	emit stopListening();
	Application::exit(0);
}
