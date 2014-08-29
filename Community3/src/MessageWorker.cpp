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

#include "MessageWorker.hpp"
#include "Authenticator.hpp"
#include "ReadMarker.hpp"

#include <QDebug>

MessageWorker::MessageWorker(QObject *parent)
: mForumRequest(new ForumRequest())
{
	setParent(parent);

	QObject::connect(mForumRequest, SIGNAL(requestComplete(QString, bool)), this,
					SLOT(requestComplete(QString, bool)));
}

void MessageWorker::postMessage(QString boardId, QString subject, QString body)
{
	const QString postUrl = URLProvider::getForumURL() + QString::fromLatin1("boards/id/%1/messages/post").arg(boardId);
	mForumRequest->makeRequest(postUrl, true, subject, formatBody(body));
}


void MessageWorker::postReply(QString messageId, QString subject, QString body)
{
	//Add "Re:" if it's not already there.
	if (!subject.startsWith(tr("Re:"), Qt::CaseInsensitive))
	{
		subject.prepend(tr("Re: "));
	}

	const QString postUrl = URLProvider::getForumURL() + QString::fromLatin1("messages/id/%1/reply").arg(messageId);
	mForumRequest->makeRequest(postUrl, true, subject, formatBody(body));
}

QString MessageWorker::formatBody(QString body)
{
	body = "<p>" + body + "</p>";

	//Replace newlines with paragraph tags.
	body.replace("\n", "</p><p>");

	//Add a &nbsp; to empty paragraph tags so they trigger a line break.
	body.replace("<p></p>", "<p>&nbsp;</p>");
	return body;

}

void MessageWorker::kudosMessage(QString messageId, QString authorId)
{
	Authenticator* auth = Authenticator::Instance();

	if (auth->authenticated() && authorId.contains(auth->userId(), Qt::CaseInsensitive))
	{
		emit postComplete(false, tr("You cannot like your own posts."));
	}
	else
	{
		const QString kudosUrl = URLProvider::getForumURL() + QString::fromLatin1("messages/id/%1/kudos/give").arg(messageId);
		mForumRequest->makeRequest(kudosUrl, true);
	}
}

void MessageWorker::markAsSolution(QString messageId)
{
	const QString solutionURL = URLProvider::getForumURL() + QString::fromLatin1("messages/id/%1/solutions/solution/mark/").arg(messageId);
	mForumRequest->makeRequest(solutionURL, true);
}

void MessageWorker::requestComplete(const QString &info, bool success)
{
	if (!success)
	{
		qDebug() << "Post response1: " << info << " success: " << success;
		emit postComplete(success, info);
	}
	else if (info.contains("status=\"success\"", Qt::CaseInsensitive))
	{
		emit postComplete(true, "");

		//Reply from posting messages contains the message Id.
		//Mark that as read.
		int loc = info.indexOf("<id type=\"int\">");
		if (loc > 0)
		{
			QString id =  info.mid(loc + 15);
			loc = id.indexOf("</id>");
			ReadMarker* readMarker = new ReadMarker(this);
			readMarker->markRead(id.mid(0, loc).toInt());
		}

	}
	else if (info.trimmed().isEmpty())
	{
		emit postComplete(false, tr("Unknown error."));
		qDebug() << "Post response2: " << info << " success: " << success;
	}
	else
	{
		emit postComplete(false, info);
		qDebug() << "Post response3: " << info << " success: " << success;
	}

}
