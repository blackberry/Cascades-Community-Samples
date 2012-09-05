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

#ifndef PROXIMITYSERVICE_H_
#define PROXIMITYSERVICE_H_

#include "AbstractSensorService.h"

namespace bb {
namespace cascades {
namespace bps {

class ProximityService: public bb::cascades::bps::AbstractSensorService {
	Q_OBJECT

public:
	ProximityService(QObject *parent = 0);
	virtual ~ProximityService();

	virtual void handleEvent(bps_event_t *event);

Q_SIGNALS:
	void proximityData(float proximity, unsigned long long timestamp, SensorAccuracy accuracy);
};

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

QML_DECLARE_TYPE(bb::cascades::bps::ProximityService)

#endif /* PROXIMITYSERVICE_H_ */
