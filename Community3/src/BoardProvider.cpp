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

#include "BoardProvider.hpp"
#include "ForumRequest.hpp"

#include <bb/cascades/ItemGrouping>


BoardProvider::BoardProvider(QObject *parent)
: mBoardData(new bb::cascades::ArrayDataModel(this))
, mForumRequest(new ForumRequest())
{
	setParent(parent);

	QObject::connect(mForumRequest, SIGNAL(requestComplete(QString, bool)), this,
				SLOT(requestComplete(QString, bool)));
}

void BoardProvider::makeBoardRequest(QString categoryId)
{


	const QString queryUrl = URLProvider::getForumURL() + QString::fromLatin1("categories/id/%1/boards").arg(categoryId);

	mForumRequest->makeRequest(queryUrl);

}

void BoardProvider::requestComplete(const QString &info, bool success)
{
	mBoardData->clear();

	if (!success)
	{
		emit boardRequestFailed(info);
	}
	else
	{
		bb::data::XmlDataAccess xda;
		QVariant list = xda.loadFromBuffer(info, "/response/boards/board");

		if (xda.hasError())
		{
			qDebug() << info;
			emit boardRequestFailed(xda.error().errorMessage());
		}
		else
		{
			//Use a QVariantList if there are multiple entries returned,
			//otherwise use a QVariant.
			if (list.canConvert<QVariantList>())
			{
				mBoardData->insert(0, list.value<QVariantList>());
				emit boardRequestReady();
			}
			else
			{
				mBoardData->insert(0, list.value<QVariant>());
				emit boardRequestReady();
			}
		}
	}

}

bb::cascades::DataModel* BoardProvider::model() const
{
    return mBoardData;
}
