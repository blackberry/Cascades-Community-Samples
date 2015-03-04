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

#include "BoardProperties.hpp"


BoardProperties::BoardProperties(QObject *parent)
: mForumRequest(new ForumRequest())
{
	setParent(parent);

	QObject::connect(mForumRequest, SIGNAL(requestComplete(QString, bool)), this,
					SLOT(requestComplete(QString, bool)));
}

void BoardProperties::getBoardName(QString boardId)
{

	const QString queryUrl = URLProvider::getForumURL() + (boardId);

	mForumRequest->makeRequest(queryUrl);

}


void BoardProperties::requestComplete(const QString &info, bool success)
{

	if (success && info.contains("status=\"success\"", Qt::CaseInsensitive))
	{
		int valStart = info.indexOf("<title type=\"string\">") + 21;
		int valEnd = info.indexOf("</title>");
		emit boardNameReady(info.mid(valStart, (valEnd - valStart)));
	}

}
