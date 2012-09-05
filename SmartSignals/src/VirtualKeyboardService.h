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

#ifndef VIRTUALKEYBOARDSERVICE_H_
#define VIRTUALKEYBOARDSERVICE_H_

#include "BlackBerryPlatformService.h"

namespace bb {
namespace cascades {
namespace bps {

class VirtualKeyboardService : public BlackBerryPlatformService {
	Q_OBJECT

public:
	VirtualKeyboardService(QObject *parent = 0);
	~VirtualKeyboardService();

	virtual void requestEvents();
	virtual int eventDomain();
	virtual void handleEvent(bps_event_t *event);

Q_SIGNALS:
	void keyboardVisible();
	void keyboardHidden();
	void keyboardInfo(int height);
};

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

QML_DECLARE_TYPE(bb::cascades::bps::VirtualKeyboardService)

#endif /* VIRTUALKEYBOARDSERVICE_H_ */
