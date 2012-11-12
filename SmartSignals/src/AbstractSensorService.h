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

#ifndef ABSTRACTSENSORSERVICE_H_
#define ABSTRACTSENSORSERVICE_H_

#include "BlackBerryPlatformService.h"
#include <bps/sensor.h>

namespace bb {
namespace cascades {
namespace bps {

class AbstractSensorService: public BlackBerryPlatformService {
	Q_OBJECT
	Q_ENUMS(SensorAccuracy)

public:
    /**
     * The accuracy levels for a sensor reading.
     */
    enum SensorAccuracy {
        AccuracyUnreliable = 0,
        AccuracyLow = 1,
        AccuracyMedium = 2,
        AccuracyHigh = 3
    };

	AbstractSensorService(sensor_type_t sensorType, QObject *parent = 0);
	virtual ~AbstractSensorService();

	Q_INVOKABLE bool isSupported();
	Q_INVOKABLE void setCalibrationEnabled(bool calibrationEnabled);
	Q_INVOKABLE void setEnableSkipDuplicates(bool enableSkipDuplicates);
	Q_INVOKABLE void setEventRate(unsigned int rate);
	Q_INVOKABLE void setBackgroundEnabled(bool backgroundEnabled);

	virtual void requestEvents();
	virtual int eventDomain();

protected:
	SensorAccuracy getAccuracy(bps_event_t *event);
	unsigned long long getTimestamp(bps_event_t *event);

private:
	sensor_type_t m_sensorType;
	bool m_calibrationEnabled;
	bool m_calibrationEnabledSet;
	bool m_enableSkipDuplicates;
	bool m_enableSkipDuplicatesSet;
	unsigned int m_rate;
	bool m_rateSet;
	bool m_backgroundEnabled;
	bool m_backgroundEnabledSet;
};

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

Q_DECLARE_METATYPE( bb::cascades::bps::AbstractSensorService::SensorAccuracy );
QML_DECLARE_TYPE(bb::cascades::bps::AbstractSensorService)

#endif /* ABSTRACTSENSORSERVICE_H_ */
