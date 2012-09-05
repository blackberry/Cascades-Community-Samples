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

#include "BPSMonitor.h"
#include "BPSMonitor_p.h"

#include <iostream>
#include <QThread>

namespace bb {
namespace cascades {
namespace bps {

BPSMonitor::BPSMonitor(QObject *parent) :
		QObject(parent), d_ptr(new BPSMonitorPrivate(*this)) {
}
BPSMonitor::~BPSMonitor() {
	delete d_ptr;
}

BPSMonitorPrivate::BPSMonitorPrivate(BPSMonitor &parent) :
		q_ptr(&parent), m_bpsEventHandler(new BPSEventHandler()) {
}
BPSMonitorPrivate::~BPSMonitorPrivate() {
	delete m_bpsEventHandler;
}

VirtualKeyboardService *BPSMonitor::virtualKeyboardService() {
	return d_ptr->m_virtualKeyboardService;
}
void BPSMonitor::setVirtualKeyboardService(VirtualKeyboardService *service) {
	d_ptr->m_bpsEventHandler->registerService(service);
	d_ptr->m_virtualKeyboardService = service;
}

GeolocationService *BPSMonitor::geolocationService() {
	return d_ptr->m_geolocationService;
}
void BPSMonitor::setGeolocationService(GeolocationService *service) {
	d_ptr->m_bpsEventHandler->registerService(service);
	d_ptr->m_geolocationService = service;
}

AccelerometerService *BPSMonitor::accelerometerService() {
	return d_ptr->m_accelerometerService;
}
void BPSMonitor::setAccelerometerService(AccelerometerService *service) {
	d_ptr->m_bpsEventHandler->registerService(service);
	d_ptr->m_accelerometerService = service;
}

MagnetometerService *BPSMonitor::magnetometerService() {
	return d_ptr->m_magnetometerService;
}

void BPSMonitor::setMagnetometerService(MagnetometerService *service) {
	d_ptr->m_bpsEventHandler->registerService(service);
	d_ptr->m_magnetometerService = service;
}

ProximityService *BPSMonitor::proximityService() {
	return d_ptr->m_proximityService;
}
void BPSMonitor::setProximityService(ProximityService *service) {
	d_ptr->m_bpsEventHandler->registerService(service);
	d_ptr->m_proximityService = service;
}

AzimuthPitchRollService *BPSMonitor::azimuthPitchRollService() {
	return d_ptr->m_azimuthPitchRollService;
}
void BPSMonitor::setAzimuthPitchRollService(AzimuthPitchRollService *service) {
	d_ptr->m_bpsEventHandler->registerService(service);
	d_ptr->m_azimuthPitchRollService = service;
}

LightService *BPSMonitor::lightService() {
	return d_ptr->m_lightService;
}
void BPSMonitor::setLightService(LightService *service) {
	d_ptr->m_bpsEventHandler->registerService(service);
	d_ptr->m_lightService = service;
}

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */
