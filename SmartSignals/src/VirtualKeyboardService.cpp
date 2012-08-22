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

#include "VirtualKeyboardService.h"
#include <iostream>
#include "bps/virtualkeyboard.h"

namespace bb {
namespace cascades {
namespace bps {

VirtualKeyboardService::VirtualKeyboardService(QObject *parent) : BlackBerryPlatformService(parent) {
}

VirtualKeyboardService::~VirtualKeyboardService() {
}

void VirtualKeyboardService::requestEvents() {
	virtualkeyboard_request_events(0);
}

int VirtualKeyboardService::eventDomain() {
	return virtualkeyboard_get_domain();
}

void VirtualKeyboardService::handleEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	if (code == VIRTUALKEYBOARD_EVENT_VISIBLE) {
		Q_EMIT keyboardVisible();
	} else if (code == VIRTUALKEYBOARD_EVENT_HIDDEN) {
		Q_EMIT keyboardHidden();
	} else if (code == VIRTUALKEYBOARD_EVENT_INFO) {
		int height = virtualkeyboard_event_get_height(event);
		Q_EMIT keyboardInfo(height);
	} else {
		std::cout << "Unhandled keyboard event code: " << code << std::endl;
	}
}

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */
