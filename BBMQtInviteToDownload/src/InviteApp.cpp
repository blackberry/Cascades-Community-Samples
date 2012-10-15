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

#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include "InviteApp.hpp"
#include "RegistrationHandler.hpp"
#include "InviteToDownload.hpp"

using namespace bb::cascades;

InviteApp::InviteApp()
{

	// The InviteToDownload object used to trigger the invite to download process.
	qmlRegisterType<InviteToDownload>(
			"com.bbmqtinvitetodownload.invitetodownload", 1, 0,
			"InviteToDownload");

	RegistrationHandler* page;
	page = new RegistrationHandler();

	if (page)
	{
		Application::instance()->setScene(page);
	}
}
