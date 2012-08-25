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

#include "TweetItem.hpp"
#include <bb/cascades/DockLayout>
#include <bb/cascades/DockLayoutProperties>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/VerticalAlignment>
#include <bb/cascades/HorizontalAlignment>
#include <bb/cascades/SystemDefaults>

using namespace bb::cascades;

/*
 * Constructor - draws the layout of each item in the ListView
 *
 * Creates a Label for the date and a multi-line text area for the description for each
 * twitter item. Can be expanded to show an image or any additional data from the JSON response.
 */
TweetItem::TweetItem(Container* parent)
    : CustomControl(parent)
{
    // Dock layout with margins inside.
    Container* itemContainer = new Container();
    DockLayout* itemLayout = new DockLayout();
    itemContainer->setLayout(itemLayout);

    // A background Container that will hold the item content Container.
    Container* backgroundContainer = new Container();
    DockLayout* backgroundLayout = new DockLayout();
    backgroundContainer->setLayout(backgroundLayout);
    backgroundContainer->setLayoutProperties(
        DockLayoutProperties::create().horizontal(HorizontalAlignment::Center));

    // A Colored Container will be used to show if an item is highlighted.
    m_highlightContainer = new Container();
    m_highlightContainer->setBackground(Color::fromARGB(0xff75b5d3));
    m_highlightContainer->setLayoutProperties(DockLayoutProperties::create()
            .horizontal(HorizontalAlignment::Center));
    m_highlightContainer->setOpacity(0.0);
    m_highlightContainer->setPreferredWidth(760.0f);
    m_highlightContainer->setPreferredHeight(200.0f);

    // Content Container, text with padding to get alignment on background image.
    Container* contentContainer = new Container();
    StackLayout* contentLayout = new StackLayout();
    contentLayout->setLayoutDirection(LayoutDirection::TopToBottom);
    contentContainer->setLayout(contentLayout);

    // A list item label, docked to the center, the text is set in updateItem.
    m_dateLabel = Label::create().text(" ").layoutProperties(
                      StackLayoutProperties::create().horizontal(HorizontalAlignment::Center));
    m_dateLabel->textStyle()->setBase(SystemDefaults::TextStyles::subtitleText());

    m_textArea = TextArea::create().text(" ").layoutProperties(
                     StackLayoutProperties::create().vertical(VerticalAlignment::Center));
    m_textArea->textStyle()->setBase(SystemDefaults::TextStyles::smallText());
    m_textArea->setPreferredHeight(200);
    m_textArea->setEditable(false);
    m_textArea->setBackgroundVisible(false);

    // Add the Label to the content.
    contentContainer->add(m_dateLabel);
    contentContainer->add(m_textArea);

    // Add the content to the full item container.
    backgroundContainer->add(m_highlightContainer);
    backgroundContainer->add(contentContainer);

    // Finally add the background Container to the item Container.
    itemContainer->add(backgroundContainer);

    setRoot(itemContainer);
}

/*
 * Deconstructor
 */
TweetItem::~TweetItem()
{
}

/*
 * TweetItem::updateItem(const QString text, QDateTime date)
 *
 * Updates the date and description controls with new data
 */
void TweetItem::updateItem(const QString text, QDateTime date)
{
    m_dateLabel->setText(date.toString());
    m_textArea->setText(text);
}

/*
 * TweetItem::select(bool select)
 *
 * Applies a different a color and opacity if the item is selected in the listview
 */
void TweetItem::select(bool select)
{
    // When an item is selected show the colored highlight Container
    if (select)
    {
        m_highlightContainer->setOpacity(0.9f);
        m_textArea->textStyle()->setColor(Color::White);
        m_dateLabel->textStyle()->setColor(Color::Black);
    }
    else
    {
        m_textArea->textStyle()->resetColor();
        m_dateLabel->textStyle()->resetColor();
        m_highlightContainer->setOpacity(0.0f);
    }
}

/*
 * TweetItem::reset(bool selected, bool activated)
 *
 * Resets the item's display properties depending on whether it is selected or not
 */
void TweetItem::reset(bool selected, bool activated)
{
    Q_UNUSED(activated);

    // Since items are recycled the reset function is where we have
    // to make sure that item state, defined by the arguments, is correct.
    select(selected);
}

/*
 * TweetItem::activate(bool activate)
 *
 * Called when an item is activated in the UI
 */
void TweetItem::activate(bool activate)
{
    Q_UNUSED(activate);
    // There is no special activate state
}
