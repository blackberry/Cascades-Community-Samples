/* Copyright (c) 2013 BlackBerry.
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

#ifndef FreemiumSampleApp_HPP_
#define FreemiumSampleApp_HPP_

#include <QObject>
#include <bb/platform/bbm/Context>
#include <bb/platform/bbm/MessageService>

#include "PurchaseStore.hpp"

namespace bb {
namespace cascades {
class Application;
}
}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class FreemiumSampleApp: public QObject {
Q_OBJECT
public:
	FreemiumSampleApp(bb::cascades::Application *app);
	virtual ~FreemiumSampleApp() {
	}

public Q_SLOTS:
	void registrationStateUpdated(
			bb::platform::bbm::RegistrationState::Type state);

	//Expose this function so it can be called from QML
	Q_INVOKABLE
	void inviteUserToDownloadViaBBM();

private:
	//All fields here used for BBM invitation support
	bb::platform::bbm::Context *m_context;
	bb::platform::bbm::MessageService *m_messageService;
	PurchaseStore *m_purchaseStore;
};

#endif /* FreemiumSampleApp_HPP_ */
