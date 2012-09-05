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

#include "ProximityService.h"
#include <iostream>

namespace bb {
namespace cascades {
namespace bps {

ProximityService::ProximityService(QObject *parent) : AbstractSensorService(SENSOR_TYPE_PROXIMITY, parent) {
}

ProximityService::~ProximityService() {
}

void ProximityService::handleEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	if (code == SENSOR_PROXIMITY_READING) {
		float proximity = sensor_event_get_proximity(event);
		unsigned long long timestamp = getTimestamp(event);
		SensorAccuracy accuracy = getAccuracy(event);

		Q_EMIT proximityData(proximity, timestamp, accuracy);
	}
}

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */
