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

#ifndef BLACKBERRYPLATFORMSERVICESMONITOR_H_
#define BLACKBERRYPLATFORMSERVICESMONITOR_H_

#include <QtCore/QObject>
#include <QtDeclarative/qdeclarative.h>

struct bps_event_t;

#include "BlackBerryPlatformService.h"
#include "VirtualKeyboardService.h"
#include "GeolocationService.h"
#include "AccelerometerService.h"
#include "MagnetometerService.h"
#include "ProximityService.h"
#include "AzimuthPitchRollService.h"
#include "LightService.h"

namespace bb {
namespace cascades {
namespace bps {

class BPSMonitorPrivate;
class BPSMonitor: public QObject {
	Q_OBJECT

	Q_PROPERTY(bb::cascades::bps::VirtualKeyboardService* virtualKeyboardService READ virtualKeyboardService WRITE setVirtualKeyboardService)
	Q_PROPERTY(bb::cascades::bps::GeolocationService* geolocationService READ geolocationService WRITE setGeolocationService)
	Q_PROPERTY(bb::cascades::bps::AccelerometerService* accelerometerService READ accelerometerService WRITE setAccelerometerService)
	Q_PROPERTY(bb::cascades::bps::MagnetometerService* magnetometerService READ magnetometerService WRITE setMagnetometerService)
	Q_PROPERTY(bb::cascades::bps::ProximityService* proximityService READ proximityService WRITE setProximityService)
	Q_PROPERTY(bb::cascades::bps::AzimuthPitchRollService* azimuthPitchRollService READ azimuthPitchRollService WRITE setAzimuthPitchRollService)
	Q_PROPERTY(bb::cascades::bps::LightService* lightService READ lightService WRITE setLightService)

public:
	explicit BPSMonitor(QObject *parent = 0);
	~BPSMonitor();

	//Q_INVOKABLE void start();
	//Q_INVOKABLE void stop();

	VirtualKeyboardService *virtualKeyboardService();
	void setVirtualKeyboardService(VirtualKeyboardService *service);

	GeolocationService *geolocationService();
	void setGeolocationService(GeolocationService *service);

	AccelerometerService *accelerometerService();
	void setAccelerometerService(AccelerometerService *service);

	MagnetometerService *magnetometerService();
	void setMagnetometerService(MagnetometerService *service);

	ProximityService *proximityService();
	void setProximityService(ProximityService *service);

	AzimuthPitchRollService *azimuthPitchRollService();
	void setAzimuthPitchRollService(AzimuthPitchRollService *service);

	LightService *lightService();
	void setLightService(LightService *service);

private:
	Q_DECLARE_PRIVATE(BPSMonitor);
	BPSMonitorPrivate *const d_ptr;
};


} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

QML_DECLARE_TYPE(bb::cascades::bps::BPSMonitor)

#endif /* BLACKBERRYPLATFORMSERVICESMONITOR_H_ */
