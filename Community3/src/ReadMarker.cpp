/* Copyright (c) 2014 BlackBerry Ltd.
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
 *
 * The Lithium REST APIs are owned by Lithium Technologies, Inc.
 * (http://www.lithium.com) and are not part of the “Works” licensed
 * hereunder pursuant to the Apache 2.0 license.
 */

#include "ReadMarker.hpp"

#include <QDebug>

ReadMarker::ReadMarker(QObject *parent)
: mForumRequest(new ForumRequest())
{
	setParent(parent);

//Not used.
//	QObject::connect(mForumRequest, SIGNAL(requestComplete(QString, bool)), this,
//					SLOT(requestComplete(QString, bool)));
}


void ReadMarker::markRead(int messageId)
{
	const QString readUrl = URLProvider::getForumURL() + QString::fromLatin1("messages/id/%1/read/mark").arg(messageId);
	mForumRequest->makeRequest(readUrl, true);

}

void ReadMarker::markRead(int messageIds[])
{
	int size = size=sizeof(messageIds)/sizeof(messageIds[0]);

	for (int count = 0; count < size; count++)
	{
		markRead(messageIds[count]);
	}
}

void ReadMarker::requestComplete(const QString &info, bool success)
{
	//Not yet used.
}
