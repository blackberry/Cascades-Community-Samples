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

#ifndef UrbanAirshipClient_HPP_
#define UrbanAirshipClient_HPP_

#include "UrbanAirshipAPI.hpp"

#include <QObject>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/ActionItem>
#include <bb/cascades/TextArea>
#include <bb/network/PushService>
#include <bb/network/PushStatus>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/platform/Notification>

namespace bb {
namespace cascades {
class Application;
}
}

using namespace bb::cascades;
using namespace bb::network;
using namespace bb::system;
using namespace bb::platform;


class UrbanAirshipClient: public QObject {
Q_OBJECT
public:
	UrbanAirshipClient(bb::cascades::Application *app);
	virtual ~UrbanAirshipClient() {
	}

public slots:
	void urbanAirshipRegistrationComplete(bool success, QString message);
	void urbanAirshipUnRegistrationComplete(bool success, QString message);
	void createSessionCompleted(const bb::network::PushStatus&);
	void createChannelCompleted(const bb::network::PushStatus&, const QString&);
	void destroyChannelCompleted(const bb::network::PushStatus&);
	void registerToLaunchCompleted(const bb::network::PushStatus&);
	void unregisterFromLaunchCompleted(const bb::network::PushStatus&);
	void handleInvoke(const bb::system::InvokeRequest&);
	void initiateChannelCreation();

private:
	TextArea *m_logger;
	PushService *m_pushService;
	UrbanAirshipAPI m_urbanAirshipAPI;
	InvokeManager m_invokeManager;
	AbstractPane *root;
	ActionItem *m_registerActionItem, *m_unregisterActionItem;
	ActivityIndicator *m_activityIndicator;
	QObject *m_createPushSessionStatus;
	QObject *m_createPushChannelStatus;
	QObject *m_registertoLaunchStatus;
	QObject *m_registerWithUrbanAirshipStatus;

	void log(QString);

	void connectPushServices();
	void connectUrbanAirshipAPIs();

	Notification notification;
};

#endif /* UrbanAirshipClient_HPP_ */
