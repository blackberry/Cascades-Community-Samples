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

#include "Searcher.hpp"
#include "Authenticator.hpp"

Searcher::Searcher(QObject *parent)
: mSearchData(new bb::cascades::ArrayDataModel(this))
, mForumRequest(new ForumRequest())
{
	setParent(parent);

	QObject::connect(mForumRequest, SIGNAL(requestComplete(QString, bool)), this,
						SLOT(requestComplete(QString, bool)));
}

void Searcher::makeSearchRequest(QString categoryId, QString boardId, QString pageNo, QString terms, QString phrase,
				QString oneOrMoreTerms, QString withoutTerms)
{
	mSearchData->clear();
	QString queryUrl;

	if (categoryId.length() > 0)
	{
		//Search a category.
		queryUrl =
				URLProvider::getForumURL() + QString::fromLatin1("categories/id/%1/search/messages?q=%2&phrase=%3&one_or_more=%4&without=%5&page_size=10&page=%6").arg(categoryId, terms, phrase, oneOrMoreTerms, withoutTerms, pageNo);

	}
	else if (boardId.length() > 0)
	{
		//Search a board.
		queryUrl =
				URLProvider::getForumURL() + QString::fromLatin1("boards/id/%1/search/messages?q=%2&phrase=%3&one_or_more=%4&without=%5&page_size=10&page=%6").arg(boardId, terms, phrase, oneOrMoreTerms, withoutTerms, pageNo);

	}
	else
	{
		//Search the whole community.
		queryUrl =
				URLProvider::getForumURL() + QString::fromLatin1("search/messages?q=%1&phrase=%2&one_or_more=%3&without=%4&page_size=10&page=%5").arg(terms, phrase, oneOrMoreTerms, withoutTerms, pageNo);

	}

	mForumRequest->makeRequest(queryUrl);

}

void Searcher::findUsersPosts(QString pageNo)
{
	Authenticator* auth = Authenticator::Instance();
	if (auth->authenticated())
	{
		mSearchData->clear();
		QString queryUrl =
				URLProvider::getForumURL() + QString::fromLatin1("users/%1/posts/messages?message_viewer.message_sort_order=thread_descending&page_size=10&page=%2").arg(auth->userId(), pageNo);
		mForumRequest->makeRequest(queryUrl);
	}
	else
	{
		emit searchRequestFailed(tr("You must be logged into view your posts. Swipe down from the top of the screen and choose Settings to log in."));
	}
}

void Searcher::requestComplete(const QString &info, bool success)
{
	if (!success)
	{
		emit searchRequestFailed(info);
	}
	else
	{
		//If the results contains message/, there are results.
		if (info.contains("/message", Qt::CaseInsensitive))
		{
			bb::data::XmlDataAccess xda;

			QVariant list = xda.loadFromBuffer(info, "/response/messages/message");

			if (xda.hasError())
			{
				qDebug() << info;
				emit searchRequestFailed("Message: " + xda.error().errorMessage());
			}
			else
			{
				//Use a QVariantList if there are multiple entries returned,
				//otherwise use a QVariant.
				if (list.canConvert<QVariantList>())
				{
					mSearchData->insert(0, list.value<QVariantList>());
					emit searchRequestReady();
				}
				else
				{
					mSearchData->insert(0, list.value<QVariant>());
					emit searchRequestReady();
				}
			}
		}
		else
		{
			emit searchRequestFailed(tr("No messages found matching your search criteria."));
		}
	}
}

bb::cascades::DataModel* Searcher::model() const
{
    return mSearchData;
}
