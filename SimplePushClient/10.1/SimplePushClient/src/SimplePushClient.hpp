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

#ifndef SimplePushClient_HPP_
#define SimplePushClient_HPP_

#include <QObject>

#include <bb/cascades/Button>
#include <bb/cascades/ListView>
#include <bb/cascades/GroupDataModel>

#include "PushManager.hpp"

using namespace bb::cascades;

namespace bb {
namespace cascades {
class Application;
}
}

class SimplePushClient: public QObject {
Q_OBJECT
public:
	SimplePushClient(bb::cascades::Application *app);
	virtual ~SimplePushClient() {
	}

private slots:
	void pushListUpdated(const QVariantList &pushList);

private:
	PushManager *m_pushManager; //Handle all push interactions
	Button *m_clearPushListButton; //Button to clear the push data store
	ListView *m_pushListView; //ListView that displays all push data to the user
	GroupDataModel *m_pushGroupDataModel; //This DataModel stores all received push data

	void populatePushGroupDataModel();

};

#endif /* SimplePushClient_HPP_ */
