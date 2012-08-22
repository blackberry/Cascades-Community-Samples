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

#include "GeolocationService.h"
#include <iostream>
#include <bps/geolocation.h>

namespace bb {
namespace cascades {
namespace bps {

GeolocationService::GeolocationService(QObject *parent) :
		BlackBerryPlatformService(parent) {
}

GeolocationService::~GeolocationService() {
}

void GeolocationService::requestEvents() {
	geolocation_request_events(0);
}

int GeolocationService::eventDomain() {
	return geolocation_get_domain();
}

unsigned int GeolocationService::period() {
	return m_period;
}
void GeolocationService::setPeriod(unsigned int period) {
	m_period = period;
	geolocation_set_period(m_period);
}

void GeolocationService::handleEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	if (code == GEOLOCATION_CANCEL) {
		Q_EMIT cancel();
	} else if (code == GEOLOCATION_INFO) {
		double latitude = geolocation_event_get_latitude(event);
		double longitude = geolocation_event_get_longitude(event);
		double accuracy = geolocation_event_get_accuracy(event); // accuracy in meters
		bool isCoarse = geolocation_event_is_coarse(event);
		Q_EMIT locationUpdate(latitude, longitude, accuracy, isCoarse);

		double altitude = geolocation_event_get_altitude(event);
		bool isAltitudeValid = geolocation_event_is_altitude_valid(event);
		double altitudeAccuracy = geolocation_event_get_altitude_accuracy(
				event);
		bool isAltitudeAccuracyValid =
				geolocation_event_is_altitude_accuracy_valid(event);
		Q_EMIT altitudeUpdate(altitude, isAltitudeValid, altitudeAccuracy,
				isAltitudeAccuracyValid);

		double heading = geolocation_event_get_heading(event);
		bool isHeadingValid = geolocation_event_is_heading_valid(event);
		Q_EMIT headingUpdate(heading, isHeadingValid);

		double speed = geolocation_event_get_speed(event);
		bool isSpeedValid = geolocation_event_is_speed_valid(event);
		Q_EMIT speedUpdate(speed, isSpeedValid);
	}
}

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */
