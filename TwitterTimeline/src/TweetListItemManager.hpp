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

#ifndef TWEETLISTITEMMANAGER_HPP_
#define TWEETLISTITEMMANAGER_HPP_

#include <bb/cascades/ListItemManager>
#include <bb/cascades/ListView>

/*
 * This class is a custom list item manager based on the default
 * ListItemManager class. It is designed to work with TweeItem class
 * to populate the ListView it is assigned to.
 */
class TweetListItemManager  : public bb::cascades::ListItemManager
{
public:
    /*
     * Ctor and Dtor
     */
    TweetListItemManager();
    virtual ~TweetListItemManager();

    /*
     * Implements ListItemManager::createItem
     * @see ListItemManager::createItem()
     */
    virtual bb::cascades::VisualNode* createItem(bb::cascades::ListView* list, const QString& type);

    /*
     * Implements ListItemManager::updateItem
     * @see ListItemManager::updateItem()
     */
    virtual void updateItem(bb::cascades::ListView* list,
                            bb::cascades::VisualNode* control, const QString& type,
                            const QVariantList& indexPath, const QVariant& data);
};

#endif /* TWEETLISTITEMMANAGER_HPP_ */
