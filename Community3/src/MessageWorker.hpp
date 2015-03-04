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

#ifndef MESSAGEWORKER_HPP_
#define MESSAGEWORKER_HPP_

#include "ForumRequest.hpp"
#include "URLProvider.hpp"

#include <QtCore/QObject>
#include <QtCore/QString>

class MessageWorker : public QObject
{
Q_OBJECT

public:

MessageWorker(QObject *parent = 0);

Q_SIGNALS:

	/*
	 * This signal is emitted after the post attempt is complete.
	 */
	void postComplete(bool success, QString error);


public Q_SLOTS:

	/*
	 * This slot is used to post a new message.
	 * @param boardId - The board Id to post the message to.
	 * @param subject - The subject of the post.
	 * @param body - The body of the post
	 */
	void postMessage(QString boardId, QString subject, QString body);

	/*
	 * This slot is used to post a reply.
	 * @param messageId - The message Id of the post to reply to.
	 * @param subject - The subject of the post.
	 * @param body - The body of the post
	 */
	void postReply(QString messageId, QString subject, QString body);


	/*
		 * This slot is used to kudos a message.
		 * @param messageId - The message Id of the post to kudos.
		 * @param authorId - The forum Id of the person who authored the post being liked.
		 */
	void kudosMessage(QString messageId, QString authorId);

	/*
	 * This slot is used to mark a message as the solution.
	 * @param messageId - The message Id of the post to mark as the solution.
	 */
	void markAsSolution(QString messageId);

	/*
	 * This slot is to receive the signal emitted when the forum network request is complete
	 * @param info - on success, this is the xml reply from the request
	 *               on failure, it is an error string
	 * @param success - true if request succeed, false if not
	 */
	void requestComplete(const QString &info, bool success);


private:

	ForumRequest* mForumRequest;

	QString formatBody(QString body);

};


#endif /* MESSAGEWORKER_HPP_ */
