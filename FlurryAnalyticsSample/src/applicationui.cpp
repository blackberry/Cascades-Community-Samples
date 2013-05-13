/* Copyright (c) 2013 BlackBerry.
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
#include <QtLocationSubset/QGeoPositionInfoSource>
#include <QtLocationSubset/QGeoCoordinate>

#include <Flurry.h>

using namespace bb::cascades;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {
	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	// Expose this class to QML such that our Q_INVOKABLE methods defined in applicationui.hpp
	// can be called from our QML classes.
	qml->setContextProperty("cpp", this);

	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	// set created root object as a scene
	app->setScene(root);

	// The code from here down to "void ApplicationUI::logEvent" is used to determine the device
	// location and log the result to the Flurry server. Any errors in the process will also be
	// logged
	QGeoPositionInfoSource *source =
			QGeoPositionInfoSource::createDefaultSource(this);

	if (source) {
		bool positionUpdatedConnected = connect(source,
				SIGNAL(positionUpdated (const QGeoPositionInfo &)), this,
				SLOT(positionUpdated (const QGeoPositionInfo &)));

		if (positionUpdatedConnected) {
			source->requestUpdate();
		} else {
			qDebug() << "positionUpdated connection failed";
			Flurry::Analytics::LogError("positionUpdated connection failed");
		}
	} else {
		qDebug() << "Failed to create QGeoPositionInfoSource";
		Flurry::Analytics::LogError(
				"QGeoPositionInfoSource::createDefaultSource(this) failed");
	}
}

void ApplicationUI::positionUpdated(const QGeoPositionInfo &update) {
	if (!update.isValid()) {
		Flurry::Analytics::LogError(
				"positionUpdated returned invalid location fix");
		return;
	}
	QGeoCoordinate coordinate = update.coordinate();
	Flurry::Analytics::SetLocation(coordinate.latitude(),
			coordinate.longitude(),
			update.attribute(QGeoPositionInfo::HorizontalAccuracy),
			update.attribute(QGeoPositionInfo::VerticalAccuracy));
}

// Log a standard or timed event
void ApplicationUI::logEvent(const QString & eventName, bool timedEvent) {
	qDebug() << "Logging event: " << eventName;
	qDebug() << "Timed: " << timedEvent;
	Flurry::Analytics::LogEvent(eventName, timedEvent);
}

// End a timed event
void ApplicationUI::endTimedEvent(const QString & eventName) {
	qDebug() << "Logging end timed event: " << eventName;
	Flurry::Analytics::EndTimedEvent(eventName);
}

// Set the user's age if provided
void ApplicationUI::setAge(int age) {
	if (age > 0 && age < 200) {
		Flurry::Analytics::SetAge(age);
	} else {
		Flurry::Analytics::LogError(
				QString("setAge received invalid age value: %1").arg(age));
	}
}

// Set the user's gender if provided (Flurry accepts either 'F' or 'M')
void ApplicationUI::setGender(const QString & gender) {
	if (gender.toUpper() == "M" || gender.toUpper() == "F") {
		Flurry::Analytics::SetGender(gender);
	} else {
		Flurry::Analytics::LogError(
				QString("setGender received invalid gender value: %1").arg(
						gender));
	}
}

// Set the user's ID age if needed in your app
void ApplicationUI::setUserId(const QString & userId) {
	if (userId.length() > 1) {
		Flurry::Analytics::SetUserID(userId);
	} else {
		Flurry::Analytics::LogError("setUserId received an empty user ID");
	}
}
