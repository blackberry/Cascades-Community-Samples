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

#include "ThreadListProvider.hpp"
#include "ForumRequest.hpp"

#include <bb/cascades/ItemGrouping>


ThreadListProvider::ThreadListProvider(QObject *parent)
: mThreadsData(new bb::cascades::ArrayDataModel(this))
, mForumRequest(new ForumRequest())
{
	setParent(parent);

	mGotFloatRequest = false;

	QObject::connect(mForumRequest, SIGNAL(requestComplete(QString, bool)), this,
					SLOT(requestComplete(QString, bool)));
}

void ThreadListProvider::makeThreadsRequest(QString boardId, QString pageNo)
{
	mBoardId = boardId;
	mPageNo = pageNo;

	mThreadsData->clear();
	mNumberOfFloatedMessages = 0;

	//Only display floated messages on the first page
	//and not for the all recent posts page.
	if (mPageNo.toInt() > 1 || mBoardId.contains("recentpostspage"))
	{
		mGotFloatRequest = true;
		requestMessages();

	}
	else
	{
		mGotFloatRequest = false;
		requestFloatedMessages();
	}


}

void ThreadListProvider::requestFloatedMessages()
{
	const QString queryUrl =
			URLProvider::getForumURL() + QString::fromLatin1("boards/id/%1/subscriptions/global/float/all/").arg(mBoardId);

	mForumRequest->makeRequest(queryUrl);
}

void ThreadListProvider::requestMessages()
{

	QString queryUrl;
	if (mBoardId.contains("recentpostspage"))
	{
		queryUrl = URLProvider::getForumURL() + QString::fromLatin1("threads/recent/?page_size=10&page=%1").arg(mPageNo);
	}
	else
	{
		queryUrl =
				URLProvider::getForumURL() + QString::fromLatin1("boards/id/%1/topiclist?message_viewer.message_sort_order=thread_descending&page_size=10&page=%2").arg(mBoardId, mPageNo);
	}

	mForumRequest->makeRequest(queryUrl);
}

void ThreadListProvider::requestComplete(const QString &info, bool success)
{
	if (!success)
	{
		emit threadsRequestFailed(info);
	}
	else
	{
		bb::data::XmlDataAccess xda;

		//First receive floated messages, then request the rest.
		if (!mGotFloatRequest)
		{

			mGotFloatRequest = true;

			QVariant list = xda.loadFromBuffer(info, "/response/subscriptions/subscription");

			if (xda.hasError())
			{
				qDebug() << "Error parsing floated topics: " << info;
				//Don't report errors here.  If a board doesn't have any floated messages we'll end here.
				//emit threadsRequestFailed("Float:" + xda.error().errorMessage());

			}
			else
			{
				//Use a QVariantList if there are multiple entries returned,
				//otherwise use a QVariant.
				if (list.canConvert<QVariantList>())
				{
					mThreadsData->insert(0, list.value<QVariantList>());
					emit threadsRequestReady();
				}
				else
				{
					mThreadsData->insert(0, list.value<QVariant>());
					emit threadsRequestReady();
				}
			}

			//Store the number of floated messages.
			mNumberOfFloatedMessages = mThreadsData->size();

			//Make the request for the regular (non floated) messages.
			requestMessages();

		}
		else
		{
			QVariant list;
			//All Recent posts call has a different XML structure
			if (mBoardId.contains("recentpostspage"))
			{
				list = xda.loadFromBuffer(info, "/response/threads/thread");
			}
			else
			{
				list = xda.loadFromBuffer(info, "/response/messages/message");
			}

			if (xda.hasError())
			{
				qDebug() << info;
				emit threadsRequestFailed("Message: " + xda.error().errorMessage());
			}
			else
			{
				//Use a QVariantList if there are multiple entries returned,
				//otherwise use a QVariant.
				if (list.canConvert<QVariantList>())
				{
					mThreadsData->insert(mThreadsData->size(), list.value<QVariantList>());
					emit threadsRequestReady();
				}
				else
				{
					mThreadsData->insert(mThreadsData->size(), list.value<QVariant>());
					emit threadsRequestReady();
				}
			}
		}
	}

}

bb::cascades::DataModel* ThreadListProvider::model() const
{
    return mThreadsData;
}

int ThreadListProvider::numberOfFloatedMessages() const
{
	return mNumberOfFloatedMessages;
}
