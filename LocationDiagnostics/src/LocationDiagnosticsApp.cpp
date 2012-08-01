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

#include "LocationDiagnosticsApp.h"

LocationDiagnosticsApp::LocationDiagnosticsApp() {
	// We set up the application Organization and name, this is used by QSettings
	// when saving values to the persistent store.
	QCoreApplication::setOrganizationName("Shadid Haque");
	QCoreApplication::setApplicationName("Location Diagnostics");

	// Then we load the application
	QmlDocument *qml = QmlDocument::create().load("main.qml");
	qml->setContextProperty("_locationDiagnostics", this);

	if (!qml->hasErrors()) {

		appPage = qml->createRootNode<NavigationPane>();
		if (appPage) {
			Application::setScene(appPage);
		}
	}
}

LocationDiagnosticsApp::~LocationDiagnosticsApp() {
}

QString LocationDiagnosticsApp::getValueFor(const QString &objectName,
		const QString &defaultValue) {
	QSettings settings;

	// If no value has been saved return the value.
	if (settings.value(objectName).isNull()) {
		return defaultValue;
	}

	// Otherwise return the value stored in the settings object.
	return settings.value(objectName).toString();
}

void LocationDiagnosticsApp::saveValueFor(const QString &objectName,
		const QString &inputValue) {
	// A new value is saved to the application settings object.
	QSettings settings;
	settings.setValue(objectName, QVariant(inputValue));
}

void LocationDiagnosticsApp::startLocationSession(bool trackingMode) {
	QmlDocument *qml = QmlDocument::create().load("LocationPage.qml");

	Page* locationPage = qml->createRootNode<Page>();

	LocationEngine* engine = new LocationEngine(locationPage, true);

	qml->setContextProperty("_engine", engine);
	qml->setContextProperty("appPage", appPage);

	appPage->push(locationPage);

	int method = appPage->findChild<QObject*>("qddPositionMethods")->property("selectedIndex").value<int>();
	if(method==0){
		engine->positionSource()->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
	} else if(method==1){
		engine->positionSource()->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
	} else if(method==2){
		engine->positionSource()->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
	} else if(method==3){
		engine->positionSource()->setProperty("provider", "gnss");
		engine->positionSource()->setProperty("fixType", "gps_autonomous");
	} else if(method==4){
		engine->positionSource()->setProperty("provider", "network");
		engine->positionSource()->setProperty("fixType", "cellsite");
	} else if(method==5){
		engine->positionSource()->setProperty("provider", "network");
		engine->positionSource()->setProperty("fixType", "wifi");
	}

	int assistMethod = appPage->findChild<QObject*>("qddAssistance")->property("selectedIndex").value<int>();
	if(assistMethod==1){
		QString url = appPage->findChild<QObject*>("qtaPDEURL")->property("text").toString();
		if(url.length()>0){
			engine->positionSource()->setProperty("qtaPDEURL", url);
		}
	} else if(assistMethod==2){
		QString url = appPage->findChild<QObject*>("qtaSLPURL")->property("text").toString();
		if(url.length()>0){
			engine->positionSource()->setProperty("slpURL", url);
		}
	} else if(assistMethod==3){
		QString id = appPage->findChild<QObject*>("qtaAppID")->property("text").toString();
		QString pass = appPage->findChild<QObject*>("qtaAppPassword")->property("text").toString();
		if(id.length()>0){
			engine->positionSource()->setProperty("qtaAppID", id);
		}
		if(pass.length()>0){
			engine->positionSource()->setProperty("qtaAppPassword", pass);
		}
	}

	int freq = appPage->findChild<QObject*>("qwdfrequency")->property("value").value<int>()*1000;
	engine->positionSource()->setUpdateInterval(freq);

	bool bgMode = appPage->findChild<QObject*>("qtlbBGMode")->property("checked").value<bool>();
	engine->positionSource()->setProperty("canRunInBackground", bgMode);

	bool soundEnabled = appPage->findChild<QObject*>("qtlbSound")->property("checked").value<bool>();
	if(soundEnabled){
		engine->setSoundEnabled(true);
	}

	if(trackingMode){
		engine->startUpdates();
	} else{
		engine->requestUpdate();
	}
}



