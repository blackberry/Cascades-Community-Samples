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

#include "BPSEventHandler.h"
#include <bps/event.h>

namespace bb {
namespace cascades {
namespace bps {
BPSEventHandler::BPSEventHandler() :
		AbstractBpsEventHandler() {
}

BPSEventHandler::~BPSEventHandler() {
}

void BPSEventHandler::registerService(BlackBerryPlatformService *service) {
	m_services << service;
	service->requestEvents();
	subscribe(service->eventDomain());
}

void BPSEventHandler::event(bps_event_t *event) {
	if (event != NULL) {
		// find the proper service object and let it handle the event
		int domain = bps_event_get_domain(event);
		for (int i = 0; i < m_services.size(); ++i) {
			BlackBerryPlatformService *service = m_services.at(i);
			if (domain == service->eventDomain()) {
				service->handleEvent(event);
			}
		}
	}
}
} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

