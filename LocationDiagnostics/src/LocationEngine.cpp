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


#include "LocationEngine.h"

LocationEngine::LocationEngine(QObject* parent, bool satInfo) {
	_locationPage = dynamic_cast<Page*>(parent);

	soundEnabled = false;
	player = new MediaPlayer(this);



	_positionSource = QGeoPositionInfoSource::createDefaultSource(this);

	connect(_positionSource, SIGNAL(positionUpdated(const QGeoPositionInfo &)), this, SLOT(positionUpdated(const QGeoPositionInfo &)));
	connect(_positionSource, SIGNAL(updateTimeout()), this, SLOT(positionUpdateTimeout()));

	if(satInfo){
		_satelliteSource = QGeoSatelliteInfoSource::createDefaultSource(this);

		connect(_satelliteSource, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)),
					this, SLOT(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));
		connect(_satelliteSource, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)),
						this, SLOT(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
	}

}


void LocationEngine::startUpdates() {
	_positionSource->startUpdates();
}

void LocationEngine::requestUpdate() {
	_positionSource->requestUpdate(120000);
}

void LocationEngine::stopUpdates() {
	_positionSource->stopUpdates();
}

void LocationEngine::positionUpdated(const QGeoPositionInfo& pos) {
	if(soundEnabled){
		player->setSource(QDir::currentPath() + "/app/native/assets/" + "sounds/pewpew.wav");
		player->play();
	}
	_locationPage->findChild<QObject*>("qllLatitude")->setProperty("text", pos.coordinate().latitude());
	_locationPage->findChild<QObject*>("qllLongitude")->setProperty("text", pos.coordinate().longitude());
	_locationPage->findChild<QObject*>("qllAltitude")->setProperty("text", pos.coordinate().altitude());
	_locationPage->findChild<QObject*>("qllTime")->setProperty("text", pos.timestamp().toString());
	_locationPage->findChild<QObject*>("qllDirection")->setProperty("text", pos.attribute(QGeoPositionInfo::Direction));
	_locationPage->findChild<QObject*>("qllGroundSpeed")->setProperty("text", pos.attribute(QGeoPositionInfo::GroundSpeed));
	_locationPage->findChild<QObject*>("qllVerticalSpeed")->setProperty("text", pos.attribute(QGeoPositionInfo::VerticalSpeed));
	_locationPage->findChild<QObject*>("qllHorizontalAccuracy")->setProperty("text", pos.attribute(QGeoPositionInfo::HorizontalAccuracy));
	_locationPage->findChild<QObject*>("qllVerticalAccuracy")->setProperty("text", pos.attribute(QGeoPositionInfo::VerticalAccuracy));
	_locationPage->findChild<QObject*>("qllMagneticVariation")->setProperty("text", pos.attribute(QGeoPositionInfo::MagneticVariation));

	log("update");
}

QGeoPositionInfoSource* LocationEngine::positionSource() {
	return _positionSource;
}

void LocationEngine::positionUpdateTimeout() {
	log("timeout");
}

void LocationEngine::positioningMethodChanged(int optionIndex) {
}

void LocationEngine::satellitesInUseUpdated(const QList<QGeoSatelliteInfo>& satellites) {
	log("satInUseUpdate");
}

bool LocationEngine::isSoundEnabled() {
	return soundEnabled;
}

void LocationEngine::setSoundEnabled(bool enabled) {
	soundEnabled = enabled;
}

void LocationEngine::satellitesInViewUpdated(const QList<QGeoSatelliteInfo>& satellites) {
	log("satInViewUpdate");
}

void LocationEngine::resetEngine(QString type) {
	if(soundEnabled){
		player->stop();
		player->setSource(QDir::currentPath() + "/app/native/assets/" + "sounds/thunder.wav");
		player->play();
	}

	_positionSource->stopUpdates();
	_positionSource->setProperty("reset", type );
	_positionSource->startUpdates();
}

void LocationEngine::log(QString msg) {
	QString text = _locationPage->findChild<QObject*>("logs")->property("text").toString();
	_locationPage->findChild<QObject*>("logs")->setProperty("text", text + "\n" + msg);
}








