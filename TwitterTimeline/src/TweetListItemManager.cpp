/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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

#include "TweetListItemManager.hpp"
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/HeaderListItem>
#include <bb/cascades/StandardListItem>
#include <QObject>
#include <qDebug>

#include "Tweet.hpp"
#include "TweetItem.hpp"

using namespace bb::cascades;

/*
 * Default Constructor
 */
TweetListItemManager::TweetListItemManager()
{
}

/*
 * Destructor
 */
TweetListItemManager::~TweetListItemManager()
{
}

/*
 * TweetListItemManager::createItem(ListView* list, const QString& type)
 *
 * Implementation of ListItemManager::createItem()
 * Creates a TweetItem visual node used in the ListView
 */
VisualNode* TweetListItemManager::createItem(ListView* list, const QString& type)
{
    Q_UNUSED(list);
    Q_UNUSED(type);
    TweetItem* titem = new TweetItem();
    return titem;
}

/*
 * TweetListItemManager::updateItem(ListView* list,
                                      VisualNode* control, const QString& type,
                                      const QVariantList& indexPath,
                                      const QVariant& data)

   Implementation of ListItemManager::updateItem()
   Updates an item in the listview using the data provided
 */
void TweetListItemManager::updateItem(ListView* list,
                                      VisualNode* control, const QString& type,
                                      const QVariantList& indexPath,
                                      const QVariant& data)
{
    Q_UNUSED(list);
    Q_UNUSED(type);
    Q_UNUSED(indexPath);

    QObject* obj = qvariant_cast<QObject*>(data);
    Tweet* tweet = qobject_cast<Tweet*>(obj);
    if (tweet)
    {
        TweetItem* titem = static_cast<TweetItem*>(control);
        if (titem)
        {
            titem->updateItem(tweet->getText(), tweet->getDate());
        }
    }
}
