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

#include "SimplePushClient.hpp"
#include "CoverManager.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

SimplePushClient::SimplePushClient(bb::cascades::Application *app)
: QObject(app)
{
    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    // set created root object as a scene
    app->setScene(root);

    //Create an instance of our PushManager class. This class will handle all push interactions.
    m_pushManager = new PushManager(app);
    //Whenever the push data is changed (new push received, data store cleared) we need to update the UI
    // this connection allows us to be notified so we can do just that.
    connect(m_pushManager, SIGNAL(pushListChanged(const QVariantList &)), this, SLOT(pushListUpdated(const QVariantList &)));

    //Here we get a handle to our QML Button and provide a way to clear the push data store
    m_clearPushListButton = root->findChild<Button*>("clearPushList");
    connect(m_clearPushListButton, SIGNAL(clicked()), m_pushManager, SLOT(clearPushList()));
    m_pushListView = root->findChild<ListView*>("pushListView");

    //Create our DataModel with the appropriate keys to store the received push messages
    m_pushGroupDataModel = new GroupDataModel(QStringList() << "priority" << "title" << "body" << "timeReceived" );
    m_pushListView->setDataModel(m_pushGroupDataModel);

    //If a push arrives when the app is closed or thumbnailed we may still want to display
    // some of the data to the end-user; for that we use a custom SceneCover implementation
    app->setCover(new CoverManager(m_pushManager));

    //This call triggers a refresh of the push messages pulled from QSettings
    m_pushManager->updatePushList();

}

/** The below method is a Signal that gets triggered any time the push data changes.
 *  This method will iterate through all received pieces of data then format them
 *  into the GroupDataModel instantiated above.
 */

void SimplePushClient::pushListUpdated(const QVariantList &pushList){
	qDebug() << "pushListUpdated. New size: " << pushList.length();
	QString pushMessage;
	QVariantMap pushMessageMap;
	QStringList messageParts;
	m_pushListView->setVisible(pushList.length() > 0);
	m_pushGroupDataModel->clear();
	foreach(QVariant pushMessageVariant, pushList){
		pushMessage = pushMessageVariant.toString();
		qDebug() << "Data from list: " << pushMessage;

		//For the purposes of this sample only we are expecting that all push data
		// is formatted into 3 parts using a '|' character as a delimiter.
		messageParts = pushMessage.split('|');
		if (messageParts.size() != 4){
			qDebug() << "Invalid list length. Expected 4, received: " << messageParts.size() << "\nSkipping this item";
			continue;
		}

		//These next lines populate the QVariantMap then insert to the DataModel
		pushMessageMap["priority"] = messageParts[0];
		pushMessageMap["title"] = messageParts[1];
		pushMessageMap["body"] = messageParts[2];
		pushMessageMap["timeReceived"] = messageParts[3];
		m_pushGroupDataModel->insert(pushMessageMap);
	}
}
