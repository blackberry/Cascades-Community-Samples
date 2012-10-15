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

#include <QtCore/QDebug>

#include <bb/platform/bbm/MessageService>

#include "InviteToDownload.hpp"
#include "Global.hpp"

using namespace bb::cascades;

InviteToDownload::InviteToDownload(QObject *parent) :
		QObject(parent), m_messageService(0)
{
}

InviteToDownload::~InviteToDownload()
{
	delete m_messageService;
}

void InviteToDownload::sendInvite()
{
	if (m_messageService == 0)
	{
		// Instantiate the MessageService.
		m_messageService = new bb::platform::bbm::MessageService(
				Global::instance()->getContext(), this);
	}

	//Trigger the invite to download process.
	m_messageService->sendDownloadInvitation();
}
