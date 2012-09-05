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

#ifndef BPSEVENTHANDLER_H_
#define BPSEVENTHANDLER_H_

#include <bb/AbstractBpsEventHandler>
#include "BlackBerryPlatformService.h"
namespace bb {
namespace cascades {
namespace bps {
class BPSEventHandler: public bb::AbstractBpsEventHandler {
public:
	BPSEventHandler();
	virtual ~BPSEventHandler();

	void setServices(const QList<BlackBerryPlatformService*> &services) {
		m_services = services;
	}
	QList<BlackBerryPlatformService*> &services() {
		return m_services;
	}

	virtual void event(bps_event_t *event);
	void registerService(BlackBerryPlatformService *service);


private:
	QList<BlackBerryPlatformService*> m_services;
};
} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

#endif /* BPSEVENTHANDLER_H_ */
