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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/ListView>
#include <bb/cascades/TextArea>
#include <bb/cascades/Container>
#include <bb/cascades/Button>
#include <bb/data/JsonDataAccess>

#include "App.hpp"
#include "TwitterRequest.hpp"
#include "Tweet.hpp"
#include "TweetListItemManager.hpp"

using namespace bb::cascades;
using namespace bb::data;

/*
 * This application demonstrates how to retrieve a twitter feed and populate a listview with StandardListItem
 * and a ListView with ListItemManger from the json data. Also demonstrates how to parse json into a GroupDataModel
 */

/*
 * Default constructor
 */
App::App()
    : m_twitterResponse("")
    , m_requestInProgress(false)
    , m_viewType(App::Basic)
{
    QmlDocument* qml = QmlDocument::create("main.qml");
    qml->setContextProperty("app", this);

    Page* root = qml->createRootNode<Page>();
    m_navPane = NavigationPane::create();
    m_navPane->push(root);

    Application::setScene(m_navPane);
}

/*
 * App::reset
 *
 * Clears the cached twitter response from the previous request
 */
void App::reset()
{
    m_twitterResponse = "";
    qDebug() << "app reset!";
}

/*
 *  App::getTimeline(QString screenName)
 *
 *  Initiates an http request to retrieve the timeline belonging to the twitter name
 *  referred to by "screenName" parameter. When the network request is complete
 *  onTwitterTimeline method is called with the result
 */
void App::getTimeline(QString screenName)
{
    //sanitize screenname
    QStringList list = screenName.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (list.count() <= 0)
    {
        QString error = "please enter a valid screen name";
        showError(error);
        return;
    }
    QString twitterid = list[0];
    if (twitterid.startsWith('@'))
    {
        twitterid = twitterid.remove(0, 1);
    }

    m_requestInProgress = true;
    TwitterRequest* tr = new TwitterRequest();
    tr->getTimeline(twitterid);
    connect(tr, SIGNAL(complete(QString, bool)), this, SLOT(onTwitterTimeline(QString, bool)));
}

/*
 * App::changeView(QString viewType)
 *
 * This QINVOKABLE method is called from the QML to when the segmented control changes
 * the view type from Standard to Custom. It sets a flag in the class which is used to
 * display the ListView
 */
void App::changeView(QString viewType)
{
    if (viewType.compare("rich", Qt::CaseInsensitive) == 0)
    {
        m_viewType = App::Rich;
    }
    else
    {
        m_viewType = App::Basic;
    }
}

/*
 * App::showError(QString error)
 *
 * This QINVOKABLE method makes visible the container for displaying error and sets
 * the error text.
 */
void App::showError(QString error)
{
    TextArea* errorLabel = m_navPane->findChild<TextArea*>("errorLabel");
    if (errorLabel)
    {
        errorLabel->setText(error);
    }

    Container* errorView = m_navPane->findChild<Container*>("failureView");
    if (errorView)
    {
        errorView->setVisible(true);
    }

    Button* timelineButton = m_navPane->findChild<Button*>("timelineBtn");
    if (timelineButton)
    {
        timelineButton->setEnabled(true);
    }
}

/*
 * App::onTwitterTimeline(QString info, bool success)
 *
 * Slot handler for receiving the data from the twitter network request
 * made in App::getTimeline(). On success it navigates to the appropriate
 * ListView and on failure it displays the error string from the failed
 * request.
 *
 * info - contains the json response (success) or error text (failure)
 * success - boolean flag indicating success of failure of the request
 */
void App::onTwitterTimeline(QString info, bool success)
{
    if (success)
    {
        switch (m_viewType)
        {
            case Rich:
            {
                QmlDocument* qml = QmlDocument::create("CustomTimelineView.qml");
                qml->setContextProperty("app", this);
                Page* root = qml->createRootNode<Page>();
                m_navPane->push(root);
            }
            break;
            case Basic:
            default:
            {
                QmlDocument* qml = QmlDocument::create("StandardTimelineView.qml");
                qml->setContextProperty("app", this);
                Page* root = qml->createRootNode<Page>();
                m_navPane->push(root);
            }
            break;
        }
        m_twitterResponse = info;
        parseResponse();
    }
    else
    {
        showError(info);
    }

    Button* timelineButton = m_navPane->findChild<Button*>("timelineBtn");
    if (timelineButton)
    {
        timelineButton->setEnabled(true);
    }
}

/*
 * App::parseResponse()
 *
 * Parses the JSON data from the twitter response and populates the ListView.
 *
 * When using the basic view type, the json is key/value pair is dumped into
 * a GroupDataModel and the StandardListItem in the qml is mapped to the creation
 * date and text of the tweet
 *
 * When using the custom view type, elements are manually extracted from the JSON
 * and put into a GroupDataModel which is used to populate the ListItemManager
 */
void App::parseResponse()
{
    if (m_twitterResponse.trimmed().isEmpty())
    {
        return;
    }

    // When using the StandardListItem, take the json response and give it the listview
    // without further changes. The StandardListView item is mapped the following way
    //
    // status = value of "created_at" key in json
    // description = value of "text" key in json
    //
    // see StandardTimelineView.qml for more info
    if (m_viewType == App::Basic)
    {
        ListView* list = m_navPane->findChild<ListView*>("basicTimelineView");
        if (!list)
        {
            qDebug() << "unable to find basicTimelineView";
            return; //if basic timeline list not found do nothing
        }

        // Create a group data model with id as the sorting key
        GroupDataModel* dm = new GroupDataModel(QStringList() << "id_str");
        dm->setGrouping(ItemGrouping::None);

        // parse the json response with JsonDataAccess
        JsonDataAccess ja;
        QVariant jsonva = ja.loadFromBuffer(m_twitterResponse);

        // the qvariant is an array of tweets which is extracted as a list
        QVariantList feed = jsonva.toList();

        // for each object in the array, push the variantmap in its raw form
        // into the ListView
        for (QList<QVariant>::iterator it = feed.begin(); it != feed.end(); it++)
        {
            QVariantMap tweet = it->toMap();
            dm->insert(tweet);
        }

        // set the data model to display
        list->setDataModel(dm);
        list->setVisible(true);
    }

    // When using the CustomListView, key/value pairs in the json response are
    // manually extracted into tweet class, which is used to create a data model and populate the ListView
    else if (m_viewType == App::Rich)
    {
        ListView* list = m_navPane->findChild<ListView*>("richTimelineView");
        if (!list)
        {
            qDebug() << "unable to find richTimelineView";
            return; //if rich timeline list not found do nothing
        }

        GroupDataModel* dm = new GroupDataModel(QStringList() << "date");
        dm->setGrouping(ItemGrouping::None);
        dm->setSortedAscending(false);

        // parse the json response with JsonDataAccess
        JsonDataAccess ja;
        QVariant jsonva = ja.loadFromBuffer(m_twitterResponse);

        // the qvariant is an array of tweets which is extracted as a list
        QVariantList feed = jsonva.toList();

        // for each object in the array, push the variantmap in its raw form
        // into the ListView
        for (QList<QVariant>::iterator it = feed.begin(); it != feed.end(); it++)
        {
            QVariantMap tweet = it->toMap();
            QString text;
            QDateTime date;

            QVariantMap::iterator it = tweet.find("text");
            if (it == tweet.end())
            {
                continue;    //can't find it, go to next object in the list
            }

            text = it.value().toString();

            it = tweet.find("created_at");
            if (it == tweet.end())
            {
                continue;    //can't find it, go to next object in the list
            }

            date = QDateTime::fromString(it.value().toString(), "ddd MMM dd HH:mm:ss +0000 yyyy");

            dm->insert(new Tweet(text, date));
        }

        TweetListItemManager* tweetItemMgr = new TweetListItemManager();

        // set the data model to display
        list->setVisible(true);
        list->setDataModel(dm);
        list->setListItemManager(tweetItemMgr);
    }
}
