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

#ifndef MAGNETOMETERSERVICE_H_
#define MAGNETOMETERSERVICE_H_

#include "AbstractSensorService.h"

namespace bb {
namespace cascades {
namespace bps {

class MagnetometerService: public bb::cascades::bps::AbstractSensorService {
	Q_OBJECT

public:
	MagnetometerService(QObject *parent = 0);
	virtual ~MagnetometerService();

	virtual void handleEvent(bps_event_t *event);

Q_SIGNALS:
	/**
	 * @brief Emitted when magnetometer data is available.
	 *
	 * @details The magnetometer measures the ambient magnetic field in each axis, in micro-Tesla (uT).
	 *
	 * @param x The ambient magnetic field in the x-axis, in micro-Tesla (uT)
	 * @param y The ambient magnetic field in the y-axis, in micro-Tesla (uT)
	 * @param z The ambient magnetic field in the z-axis, in micro-Tesla (uT)
	 * @param timestamp A monotonic timestamp (not date/time) that can be used to synchronize and/or
	 * fuse different sensor events.
	 * @param accuracy The accuracy of the sensor reading.
	 *
	 * @return None.
	 */
	void magnetometerData(float x, float y, float z, unsigned long long timestamp, SensorAccuracy accuracy);
};

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

QML_DECLARE_TYPE(bb::cascades::bps::MagnetometerService)

#endif /* MAGNETOMETERSERVICE_H_ */
