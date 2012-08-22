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

#ifndef BLACKBERRYPLATFORMSERVICE_H_
#define BLACKBERRYPLATFORMSERVICE_H_

#include <QObject>
#include <QtDeclarative/qdeclarative.h>

struct bps_event_t;

namespace bb {
namespace cascades {
namespace bps {

class BlackBerryPlatformService : public QObject {
	Q_OBJECT

public:
	explicit BlackBerryPlatformService(QObject *parent = 0) : QObject(parent) {}
	virtual ~BlackBerryPlatformService() {}

	virtual void requestEvents() = 0;
	virtual int eventDomain() = 0;
	virtual void handleEvent(bps_event_t *event) = 0;

private:
	Q_DISABLE_COPY(BlackBerryPlatformService)
};

} /* namespace bps */
} /* namespace cascades */
} /* namespace bb */

QML_DECLARE_TYPE(bb::cascades::bps::BlackBerryPlatformService)

#endif /* BLACKBERRYPLATFORMSERVICE_H_ */
