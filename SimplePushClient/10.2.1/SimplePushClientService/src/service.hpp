/* Copyright (c) 2013 BlackBerry Limited.
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

#ifndef SERVICE_H_
#define SERVICE_H_

#include <QObject>
#include <QSettings>
#include <QVariantList>

#include <bb/network/PushService>

namespace bb {
class Application;
namespace platform {
class Notification;
class NotificationDialog;
}
namespace system {
class InvokeManager;
class InvokeRequest;
}
namespace network {
class PushStatus;
class PushService;
}
}

class Service: public QObject {
	Q_OBJECT
public:
	Service(bb::Application * app);

	enum PushPriority {
		Low = 0, Medium = 1, High = 2
	};

private slots:
	void handleInvoke(const bb::system::InvokeRequest &);
	void createSessionCompleted(const bb::network::PushStatus&);
	void createChannelCompleted(const bb::network::PushStatus&, const QString&);

private:
	void triggerNotification();

	static const QString BLACKBERRY_PUSH_APPLICATION_ID;
	static const QString BLACKBERRY_PUSH_URL;
	static const QString BLACKBERRY_INVOKE_TARGET_ID;

	bool m_shouldChannelBeCreated;

	bb::system::InvokeManager* m_invokeManager; //Created to handle only push invocations
	bb::network::PushService* m_pushService; //The service that will handle push registration/reception
	QSettings m_settings; //Persistent store for the app
	QVariantList m_pushList; //Handle to a usable list of all push messages

	void log(const QString &);
	void logPush(const QString &);
	void handlePush(const QString &, const QString &);

};

#endif /* SERVICE_H_ */
