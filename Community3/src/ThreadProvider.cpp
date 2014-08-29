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

#include "ThreadProvider.hpp"
#include "ForumRequest.hpp"
#include "Authenticator.hpp"
#include "ReadMarker.hpp"

#include <bb/cascades/ItemGrouping>
#include <QtCore/QThread>


ThreadProvider::ThreadProvider(QObject *parent)
: mThreadData(new bb::cascades::ArrayDataModel(this))
, mForumRequest(new ForumRequest())
, mSolutions(new QVector<int>())
{
	setParent(parent);

	QObject::connect(mForumRequest, SIGNAL(requestComplete(QString, bool)), this,
				SLOT(requestComplete(QString, bool)));

	mPageNo = 1;
}

void ThreadProvider::makeThreadRequest(QString threadId, QString pageNo)
{
	mPageNo = pageNo.toInt();

	mThreadData->clear();

	const QString queryUrl = URLProvider::getForumURL() + QString::fromLatin1("messages/id/%1/thread?restapi.format_detail=full_list_element&message_viewer.message_sort_order=thread_ascending&page_size=10&page=%2").arg(threadId, pageNo);

	mForumRequest->makeRequest(queryUrl);

}

void ThreadProvider::requestComplete(const QString &info, bool success)
{

	if (!success)
	{
		emit threadRequestFailed(info);
	}
	//User is allowed to mark thread as solved.
	else if (info.contains("<response status=\"success\">\n  <value type=\"boolean\">true</value>\n</response>"))
	{
		mCanMarkSolved = true;
		//We now have everything needed to display the thread.
		emit threadRequestReady();
	}
	//User is not allowed to mark thread as solved.
	else if (info.contains("<response status=\"success\">\n  <value type=\"boolean\">false</value>\n</response>"))
	{
		mCanMarkSolved = false;
		//We now have everything needed to display the thread.
		emit threadRequestReady();
	}
	else
	{
		bb::data::XmlDataAccess xda;
		mHasSolution = false;

		QVariant list = xda.loadFromBuffer(info, "/response/thread/messages/count");
		mNoOfMessages = list.value<QVariant>().toMap().value(".data").value<QString>().toInt();

		list = xda.loadFromBuffer(info, "/response/thread/messages/linear/message");

		if (xda.hasError())
		{
			qDebug() << info;
			emit threadRequestFailed(xda.error().errorMessage());
		}
		else
		{
			Authenticator* auth = Authenticator::Instance();

			mThreadData->clear();

			//Use a QVariantList if there are multiple entries returned,
			//otherwise use a QVariant.
			if (list.canConvert<QVariantList>())
			{
				mThreadData->insert(0, list.value<QVariantList>());

				QVariant solutionList =  xda.loadFromBuffer(info, "/response/thread/solutions/solution");

				//Error happens if there aren't any solutions.
				if (!xda.hasError())
				{
					//Use a QVariantList if there are multiple solutions returned,
					//otherwise use a QVariant.
					if (solutionList.canConvert<QVariantList>())
					{
						QList<QVariant> solList = solutionList.value<QVariantList>();

						int solutionSize =  solList.size();

						for (int count = 0; count < solutionSize; count++)
						{
							mSolutions->append(solList.at(count).toMap().value("id").toMap().value(".data").value<QString>().toInt());
						}
					}
					else
					{
						mSolutions->append(solutionList.toMap().value("id").toMap().value(".data").value<QString>().toInt());
					}

					mHasSolution = true;
				}

				//If the user is logged in, see if the have the permission to
				//mark a post in the thread as the solution.
				//Only need to check the first reply.  Can't use the first
				//message because it can't be marked as the solution, only replies.
				//If the user has permission to mark the first reply as the solution,
				//then they'll have the ability to mark any reply as the solution.
				if (auth->authenticated())
				{
					QList<QVariant> canSolveList = list.value<QVariantList>();
					int messageIdToCheck = canSolveList.at(1).toMap().value("id").toMap().value(".data").value<QString>().toInt();
					const QString queryUrl = URLProvider::getForumURL() + QString::fromLatin1("messages/id/%1/solutions/solution/mark/allowed").arg(messageIdToCheck);
					mForumRequest->makeRequest(queryUrl);
				}
				else
				{
					//If not logged in they can't mark the thread as solved.
					mCanMarkSolved = false;
					emit threadRequestReady();
				}
			}
			else
			{
				mThreadData->insert(0, list.value<QVariant>());
				//No replies to check mark as solution permissions if there is only 1 post.
				emit threadRequestReady();
			}

			//If the user is logged in, mark the retrieved messages as read.
			if (auth->authenticated())
			{
				ReadMarker* readMarker = new ReadMarker(this);
				QVariant message;
				int messageId;

				for (int count = mThreadData->size() - 1; count >= 0; count--)
				{
					message = mThreadData->value(count);
					messageId =  message.value<QVariant>().toMap().value("id").toMap().value(".data").value<QString>().toInt();
					readMarker->markRead(messageId);
				}
			}
		}
	}
}

void ThreadProvider::scrollListViewDelay()
{
	QTimer::singleShot(1000, this, SLOT(scrollListViewDelayTimer()));
}

void ThreadProvider::scrollListViewDelayTimer()
{
	emit scrollListView();
}

bb::cascades::DataModel* ThreadProvider::model() const
{
    return mThreadData;
}

int ThreadProvider::noOfMessages() const
{
	return mNoOfMessages;
}

bool ThreadProvider::isSolution(QString messageId)
{
	return mSolutions->contains(messageId.toInt());
}

bool ThreadProvider::hasSolution() const
{
	return mHasSolution;
}

int ThreadProvider::firstSolution() const
{
	if (mHasSolution)
	{
		return mSolutions->first();
	}
	else
	{
		return 0;
	}
}

bool ThreadProvider::canMarkSolved() const
{
	return mCanMarkSolved;
}
