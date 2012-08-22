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

#ifndef BPSMONITOR_P_H_
#define BPSMONITOR_P_H_

#include "BPSMonitor.h"
#include "BPSEventHandler.h"

class QThread;

namespace bb {
namespace cascades {
namespace bps {

class BPSMonitorPrivate {
private:
	Q_DECLARE_PUBLIC(BPSMonitor);

	BPSMonitorPrivate(BPSMonitor &parent);
	~BPSMonitorPrivate();

	BPSMonitor *const q_ptr;
	BPSEventHandler *m_bpsEventHandler;

	static void append_service(QDeclarativeListProperty<bb::cascades::bps::BlackBerryPlatformService> *list, BlackBerryPlatformService *service);
	void appendService(BlackBerryPlatformService *service);

	VirtualKeyboardService *m_virtualKeyboardService;
	GeolocationService *m_geolocationService;
	AccelerometerService *m_accelerometerService;
	MagnetometerService *m_magnetometerService;
	ProximityService *m_proximityService;
	AzimuthPitchRollService *m_azimuthPitchRollService;
	LightService *m_lightService;
};

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

#endif /* BPSMONITOR_P_H_ */
