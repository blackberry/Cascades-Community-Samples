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

#ifndef LOCATIONENGINE_H_
#define LOCATIONENGINE_H_

#include <iostream>

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <QtLocationSubset/QGeoPositionInfo>
#include <QtLocationSubset/QGeoPositionInfoSource>

#include <QtLocationSubset/QGeoSatelliteInfo>
#include <QtLocationSubset/QGeoSatelliteInfoSource>

#include <bb/cascades/controls/page.h>
#include <bb/multimedia/MediaPlayer.hpp>
#include <stdio.h>

using namespace std;
using namespace QtMobilitySubset;
using namespace bb::cascades;
using namespace bb::multimedia;

class LocationEngine: public QObject {
	Q_OBJECT



public:
	LocationEngine(QObject* parent, bool satInfo);
	Q_INVOKABLE void startUpdates();
	Q_INVOKABLE void requestUpdate();
	Q_INVOKABLE void stopUpdates();
	QGeoPositionInfoSource* positionSource();
	bool isSoundEnabled();
	void setSoundEnabled(bool enabled);
	Q_INVOKABLE void resetEngine(QString type);

public Q_SLOTS:
	void positionUpdated(const QGeoPositionInfo & pos);
    void positionUpdateTimeout();

    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> & satellites);
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> & satellites);

private:
    bool soundEnabled;
    MediaPlayer* player;
    Page* _locationPage;
    QGeoPositionInfoSource *_positionSource;
    QGeoSatelliteInfoSource *_satelliteSource;
    void printField( QString & key, QVariant & value );
    void dumpRawPositionData( QString & label, QVariantMap & rawDat );
    void logQString(QString msg);
};

#endif /* LOCATIONENGINE_H_ */
