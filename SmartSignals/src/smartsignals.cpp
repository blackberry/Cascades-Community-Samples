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

#include <QtCore/QObject>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>

#include <bb/cascades/Application>
#include <bb/cascades/Container>

#include "SmartSignalsApp.hpp"
#include "BPSMonitor.h"
#include "VirtualKeyboardService.h"
#include "GeolocationService.h"
#include "AccelerometerService.h"
#include "MagnetometerService.h"
#include "ProximityService.h"

using ::bb::cascades::Application;

int main(int argc, char **argv) {
	const char *uri = "bb.cascades.bps";
	qmlRegisterType<bb::cascades::bps::BPSMonitor>(uri, 1, 0, "BPSMonitor");
	qmlRegisterInterface<bb::cascades::bps::BlackBerryPlatformService>("BlackBerryPlatformService");
	qmlRegisterType<bb::cascades::bps::VirtualKeyboardService>(uri, 1, 0, "VirtualKeyboardService");
	qmlRegisterType<bb::cascades::bps::GeolocationService>(uri, 1, 0, "GeolocationService");
	qRegisterMetaType<bb::cascades::bps::AccelerometerService::SensorAccuracy>("SensorAccuracy");
	qmlRegisterType<bb::cascades::bps::AccelerometerService>(uri, 1, 0, "AccelerometerService");
	qmlRegisterType<bb::cascades::bps::MagnetometerService>(uri, 1, 0, "MagnetometerService");
	qmlRegisterType<bb::cascades::bps::ProximityService>(uri, 1, 0, "ProximityService");
	qmlRegisterType<bb::cascades::bps::AzimuthPitchRollService>(uri, 1, 0, "AzimuthPitchRollService");
	qmlRegisterType<bb::cascades::bps::LightService>(uri, 1, 0, "LightService");

	Application app(argc, argv);

	QTranslator translator;
	QString locale_string = QLocale().name();
	QString filename = QString("SmartSignals_%1").arg(locale_string);
	if (translator.load(filename, "app/native/qm")) {
		app.installTranslator(&translator);
	}

	SmartSignalsApp mainApp;

	return Application::exec();
}
