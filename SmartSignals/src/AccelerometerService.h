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

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include "AbstractSensorService.h"

namespace bb {
namespace cascades {
namespace bps {

class AccelerometerService : public AbstractSensorService {
	Q_OBJECT

public:
    AccelerometerService(QObject *parent = 0);
	virtual ~AccelerometerService();

	virtual void handleEvent(bps_event_t *event);

Q_SIGNALS:
	/**
	 * @brief Emitted when accelerometer data is available.
	 *
	 * @details The accelerometer measures the acceleration on each axis.
	 *
	 * @param x Acceleration minus Gx on the x-axis, in SI units (m/s^2)
	 * @param y Acceleration minus Gy on the y-axis, in SI units (m/s^2)
	 * @param z Acceleration minus Gz on the z-axis, in SI units (m/s^2)
	 * @param timestamp A monotonic timestamp (not date/time) that can be used to synchronize and/or
	 * fuse different sensor events.
	 * @param accuracy The accuracy of the sensor reading.
	 *
	 * @return None.
	 */
	void accelerometerData(float x, float y, float z, unsigned long long timestamp, SensorAccuracy accuracy);
};

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

QML_DECLARE_TYPE(bb::cascades::bps::AccelerometerService)

#endif /* ACCELEROMETER_H_ */
