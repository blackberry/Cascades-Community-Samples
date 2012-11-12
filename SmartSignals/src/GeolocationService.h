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

#ifndef GEOLOCATIONSERVICE_H_
#define GEOLOCATIONSERVICE_H_

#include "BlackBerryPlatformService.h"

namespace bb {
namespace cascades {
namespace bps {

class GeolocationService: public BlackBerryPlatformService {
Q_OBJECT
Q_PROPERTY( unsigned int period READ period WRITE setPeriod )

public:
	GeolocationService(QObject *parent = 0);
	~GeolocationService();

	virtual void requestEvents();
	virtual int eventDomain();
	virtual void handleEvent(bps_event_t *event);

	/* Sets the period (in seconds) at which geolocation events are reported.
	 * If the period is set to 0, a single geolocation event is delivered, and
	 * no more geolocation events are delivered subsequently.
	 */
	unsigned int period();
	void setPeriod(unsigned int period);

Q_SIGNALS:
	void cancel();
	void locationUpdate(double latitude, double longitude, double accuracy,
			bool isCoarse);
	void altitudeUpdate(double altitude, bool isAltitudeValid,
			double altitudeAccuracy, bool isAltitudeAccuracyValid);
	void headingUpdate(double heading, bool isHeadingValid);
	void speedUpdate(double speed, bool isSpeedValid);

private:
	unsigned int m_period;
};

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

QML_DECLARE_TYPE(bb::cascades::bps::GeolocationService)

#endif /* GEOLOCATIONSERVICE_H_ */
