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

#include "ProfileBoxesListPage.hpp"

#include <bb/cascades/QmlDocument>
#include <bb/cascades/LongPressHandler>
#include <QtCore/QVariantList>

#include "ProfileBox.hpp"
#include "ProfileBoxesManager.hpp"
#include "NewProfileBoxPage.hpp"
#include "Global.hpp"

using namespace bb::cascades;

ProfileBoxesListPage::ProfileBoxesListPage()
{
	QmlDocument* qmlContent = QmlDocument::create(
			"asset:///ProfileBoxesList.qml");
	if (qmlContent)
	{
		Control* content = qmlContent->createRootObject<Control>();
		this->setContent(content);
	}

	retrieveProfileItemsUsingHardcodedIds();

	m_listView = this->findChild<ListView*>("profileBoxesListView");

	QObject::connect(m_listView, SIGNAL(triggered(QVariantList)), this,
			SLOT(itemSelected(QVariantList)));

	qmlRegisterType<ProfileBox>("CustomComponents", 1, 0, "ProfileBox");
	m_listView->setDataModel(ProfileBoxesManager::getDataModel());

	ActionItem* newProfileBoxItem = ActionItem::create().title(
			"New Profile Box");
	this->addAction(newProfileBoxItem);
	QObject::connect(newProfileBoxItem, SIGNAL(triggered()), this,
			SLOT(createNewProfileBox()));

	m_deleteItem = ActionItem::create().title("Delete Off");
	this->addAction(m_deleteItem);
	QObject::connect(m_deleteItem, SIGNAL(triggered()), this,
			SLOT(toggleDeletion()));
	m_isInDeleteMode = false;
}

// TODO: Not yet implemented
void ProfileBoxesListPage::retrieveProfileItems()
{
}

// TODO: Note yet implemented.
// This function tests the retrieval of an icon and a profile box item from the database
// using a hard-coded item id.  An item in the database must to be added first.
void ProfileBoxesListPage::retrieveProfileItemsUsingHardcodedIds()
{
}

void ProfileBoxesListPage::createNewProfileBox()
{
	m_isInDeleteMode = false;
	m_deleteItem->setTitle("Delete Off");
	NewProfileBoxPage *newProfileBoxPage = new NewProfileBoxPage();
	g_navPane->push(newProfileBoxPage);
}

void ProfileBoxesListPage::toggleDeletion()
{
	m_isInDeleteMode = !m_isInDeleteMode;

	if (m_isInDeleteMode)
	{
		m_deleteItem->setTitle("Delete On");
	} else
	{
		m_deleteItem->setTitle("Delete Off");
	}
}

void ProfileBoxesListPage::itemSelected(QVariantList indexPath)
{
	if (m_isInDeleteMode)
	{
		ProfileBoxesManager::getInstance().removeProfileBox(indexPath);
	}
}

