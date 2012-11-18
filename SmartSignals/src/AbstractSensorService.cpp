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

#include "AbstractSensorService.h"
#include <QDateTime>
#include <iostream>

namespace bb {
namespace cascades {
namespace bps {

AbstractSensorService::AbstractSensorService(sensor_type_t sensorType,
		QObject *parent) :
		BlackBerryPlatformService(parent), m_sensorType(sensorType) {
}
AbstractSensorService::~AbstractSensorService() {
}

bool AbstractSensorService::isSupported() {
	return sensor_is_supported(m_sensorType);
}

void AbstractSensorService::setCalibrationEnabled(bool calibrationEnabled) {
	m_calibrationEnabled = calibrationEnabled;
	m_calibrationEnabledSet = true;
}

void AbstractSensorService::setEnableSkipDuplicates(bool enableSkipDuplicates) {
	m_enableSkipDuplicates = enableSkipDuplicates;
	m_enableSkipDuplicatesSet = true;
}

void AbstractSensorService::setEventRate(unsigned int rate) {
	m_rate = rate;
	m_rateSet = true;
}

void AbstractSensorService::setBackgroundEnabled(bool backgroundEnabled) {
	m_backgroundEnabled = backgroundEnabled;
	m_backgroundEnabledSet = true;
}

void AbstractSensorService::requestEvents() {
	if (m_calibrationEnabledSet)
		sensor_set_calibration(m_sensorType, m_calibrationEnabled);
	if (m_rateSet)
		sensor_set_rate(m_sensorType, m_rate);
	if (m_enableSkipDuplicatesSet)
		sensor_set_skip_duplicates(m_sensorType, m_enableSkipDuplicates);
	//if (m_backgroundEnabledSet) sensor_set_background(m_sensorType, m_backgroundEnabled);
	sensor_request_events(m_sensorType);
}

int AbstractSensorService::eventDomain() {
	return sensor_get_domain();
}

unsigned long long AbstractSensorService::getTimestamp(bps_event_t *event) {
	//unsigned long long timestamp = sensor_event_get_timestamp(event);
	unsigned long long timestamp = QDateTime::currentMSecsSinceEpoch();
	return timestamp;
}

AbstractSensorService::SensorAccuracy AbstractSensorService::getAccuracy(
		bps_event_t *event) {
	SensorAccuracy accuracy = AccuracyUnreliable;
	switch (sensor_event_get_accuracy(event)) {
	case SENSOR_ACCURACY_UNRELIABLE:
		accuracy = AccuracyUnreliable;
		break;
	case SENSOR_ACCURACY_LOW:
		accuracy = AccuracyLow;
		break;
	case SENSOR_ACCURACY_MEDIUM:
		accuracy = AccuracyMedium;
		break;
	case SENSOR_ACCURACY_HIGH:
		accuracy = AccuracyHigh;
		break;
	}
	return accuracy;
}

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */
