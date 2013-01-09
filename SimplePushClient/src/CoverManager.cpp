/* Copyright (c) 2012 Research In Motion Limited.
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
*/

#include "CoverManager.hpp"

#include <bb/cascades/QmlDocument>

/**
 *  This class extends SceneCover so we can display information on the tiled/
 *  thumbnailed application.
 */

CoverManager::CoverManager(PushManager *pushManager) :
		SceneCover(this), m_pushManager(pushManager) {

	//Whenever a push is received we want to update the cover, this will listen for data changes
	connect(m_pushManager, SIGNAL(pushListChanged(const QVariantList &)), this,
			SLOT(pushListUpdated(const QVariantList &)));

	//Create a new Container based off LastPushSceneCover.qml
	QmlDocument *qml =
			QmlDocument::create("asset:///LastPushSceneCover.qml").parent(this);
	m_pushMessageCover = qml->createRootObject<Container>();

	//This call sets the Container to be the displayable for the SceneCover
	setContent (m_pushMessageCover);

}

CoverManager::~CoverManager() {
}

//If the data store changes this method updates the SceneCover with the latest push message
void CoverManager::pushListUpdated(const QVariantList &pushList) {
	if (!pushList.isEmpty()) {
		//Read in the last message from the list
		QVariant lastReceivedPushMessageVariant = pushList.last();
		QString lastReceivedPushMessage;
		QStringList messageParts;
		lastReceivedPushMessage = lastReceivedPushMessageVariant.toString();
		qDebug() << "Last push received: " << lastReceivedPushMessage;
		messageParts = lastReceivedPushMessage.split('|');
		if (messageParts.size() != 4) {
			qDebug() << "Invalid list length. Expected 4, received: "
					<< messageParts.size();

		} else {
			m_pushMessageCover->setProperty("priority", messageParts[0].toInt());
			m_pushMessageCover->setProperty("title", messageParts[1]);
			m_pushMessageCover->setProperty("body", messageParts[2]);
			setDescription(messageParts[3]);
		}
		return;
	} else { //If the list is empty let's display some data to let the user know
		m_pushMessageCover->setProperty("priority", 13013);
		m_pushMessageCover->setProperty("title", "");
		m_pushMessageCover->setProperty("body", "");
		setDescription("-");
	}
}
