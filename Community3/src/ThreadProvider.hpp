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

#ifndef THREADPROVIDER_HPP_
#define THREADPROVIDER_HPP_

#include "ForumRequest.hpp"
#include "URLProvider.hpp"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QVector>
#include <bb/cascades/ArrayDataModel>
#include <bb/data/XmlDataAccess>


class ThreadProvider : public QObject
{
Q_OBJECT

Q_PROPERTY(bb::cascades::DataModel* model READ model NOTIFY threadRequestReady)
Q_PROPERTY(int noOfMessages READ noOfMessages NOTIFY threadRequestReady)
Q_PROPERTY(bool hasSolution READ hasSolution NOTIFY threadRequestReady)
Q_PROPERTY(int firstSolution READ firstSolution NOTIFY threadRequestReady)
Q_PROPERTY(bool canMarkSolved READ canMarkSolved NOTIFY threadRequestReady)

public:

	ThreadProvider(QObject *parent = 0);

	Q_INVOKABLE bool isSolution(QString messageId);

Q_SIGNALS:

	/*
	 * This signal is emitted the thread is ready.
	 */
	void threadRequestReady();

	/*
	 * This signal is emitted when the read thread request fails.
	 * @param error - The error message returned.
	 */
	void threadRequestFailed(QString error);

	//Fired after the delay requested by using scrollListViewDelay.
	void scrollListView();

public Q_SLOTS:

	/*
	 * This slot is to initiate the request to download a thread.
	 * @param threadId - The thread Id to list categories for.
	 * @param pageNo - The page number to view for long threads.
	 */
	void makeThreadRequest(QString threadId, QString pageNo);

	/*
	 * This slot is to receive the signal emitted when the forum network request is complete
	 * @param info - on success, this is the xml reply from the request
	 *               on failure, it is an error string
	 * @param success - true if request succeed, false if not
	 */
	void requestComplete(const QString &info, bool success);

	//Called by forumThread.qml to allow for a timer delay so the ListView has time to load
	//data before scrollToItem is called.
	void scrollListViewDelay();

	//Called by the timer for delay requested by scrollListViewDelay
	void scrollListViewDelayTimer();

private:

	bb::cascades::DataModel* model() const;

	int noOfMessages() const;
	bool hasSolution() const;
	int firstSolution() const;
	bool canMarkSolved() const;

	bb::cascades::ArrayDataModel* mThreadData;
	ForumRequest* mForumRequest;
	int mPageNo;
	int mNoOfMessages;
	QVector<int>* mSolutions;
	bool mHasSolution;
	bool mCanMarkSolved;

};

#endif /* THREADPROVIDER_HPP_ */
