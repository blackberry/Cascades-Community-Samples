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

#ifndef SEARCHER_HPP_
#define SEARCHER_HPP_

#include "ForumRequest.hpp"
#include "URLProvider.hpp"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <bb/cascades/ArrayDataModel>
#include <bb/data/XmlDataAccess>


class Searcher : public QObject
{
Q_OBJECT

Q_PROPERTY(bb::cascades::DataModel* model READ model NOTIFY searchRequestReady)

public:

Searcher(QObject *parent = 0);

Q_SIGNALS:

	/*
	 * This signal is emitted the thread list is ready.
	 */
	void searchRequestReady();

	/*
	 * This signal is emitted when the thread request fails.
	 * @param error - The error message returned.
	 */
	void searchRequestFailed(QString error);

	public Q_SLOTS:

	/*
	 * This slot is to initiate the search request.
	 */
	void makeSearchRequest(QString categoryId, QString boardId, QString pageNo, QString terms, QString phrase,
			QString oneOrMoreTerms, QString withoutTerms);

	//Locates posts by the currently logged in user.
	void findUsersPosts(QString pageNo);

	void requestComplete(const QString &info, bool success);

	private:

	bb::cascades::DataModel* model() const;

	bb::cascades::ArrayDataModel* mSearchData;
	ForumRequest* mForumRequest;

};


#endif /* SEARCHER_HPP_ */
