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

#include "AccelerometerService.h"
#include <iostream>

namespace bb {
namespace cascades {
namespace bps {

AccelerometerService::AccelerometerService(QObject *parent) : AbstractSensorService(SENSOR_TYPE_ACCELEROMETER, parent) {
}

AccelerometerService::~AccelerometerService() {
}

void AccelerometerService::handleEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	if (code == SENSOR_ACCELEROMETER_READING) {
		float x, y, z;
		sensor_event_get_xyz(event, &x, &y, &z);
		unsigned long long timestamp = getTimestamp(event);
		SensorAccuracy accuracy = getAccuracy(event);

		Q_EMIT accelerometerData(x, y, z, timestamp, accuracy);
	}
}

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */
