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

#ifndef INVITETODOWNLOAD_HPP_
#define INVITETODOWNLOAD_HPP_

#include <QtCore/QObject>

namespace bb
{
namespace platform
{
namespace bbm
{
class Context;
class MessageService;
}
}
}

class InviteToDownload: public QObject
{
Q_OBJECT


public:
	InviteToDownload(QObject *parent = 0);
	~InviteToDownload();

	Q_INVOKABLE void sendInvite();

private:
	bb::platform::bbm::MessageService * m_messageService;

};

#endif /* INVITETODOWNLOAD_HPP_ */
