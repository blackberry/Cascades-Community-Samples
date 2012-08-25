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

#ifndef TWEETITEM_HPP_
#define TWEETITEM_HPP_

#include <bb/cascades/CustomControl>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/Container>
#include <bb/cascades/TextArea>
#include <bb/cascades/Label>
#include <bb/cascades/ImageView>

/*
 * A customized Item class used for populating the ListView items
 * It is an example of how you can create your version of the default
 * StandardListItem class provided by cascades. It demonstrates how
 * items can be positioned differently, or using different controls such as
 * TextArea instead of labels etc. It can be expanded to show an image in the
 * ListView for example or have a different selected item highlighting behavior.
 */
class TweetItem : public bb::cascades::CustomControl, public bb::cascades::ListItemListener
{
    Q_OBJECT
public:
    /*
     * Ctor & Dtor
     */
    TweetItem(bb::cascades::Container* parent = 0);
    virtual ~TweetItem();

    /*
     * Allows the text and date to be updated
     * @param text - text to update
     * @param date - date to update
     */
    void updateItem(const QString text, QDateTime date);

    /*
     * Implements ListItemListener::select
     * @see ListItemListener::select()
     */
    virtual void select(bool select);

    /*
     * Implements ListItemListener::reset
     * @see ListItemListener::reset()
     */
    virtual void reset(bool selected, bool activated);

    /*
     * Implements ListItemListener::activate
     * @see ListItemListener::activate
     */
    virtual void activate(bool activate);

private:
    bb::cascades::Label* m_dateLabel;
    bb::cascades::TextArea* m_textArea;
    bb::cascades::Container* m_highlightContainer;
};

#endif /* TWEETITEM_HPP_ */
