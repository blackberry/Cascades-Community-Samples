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

#ifndef APP_H
#define APP_H

#include <QtCore/QObject>
#include <bb/cascades/NavigationPane>

class App : public QObject
{
    Q_OBJECT

    /* defines the view types of the list view used to show twitter feeds
     * Basic : ListView with a StandardListItem
     * Rich : ListView with a custom ListItemManager
     */
    enum ViewType { Basic, Rich };

public:
    App();

    /*
     * Called by the QML to get a twitter feed for the screen nane
     */
    Q_INVOKABLE void getTimeline(QString screenName);

    /*
     * Called by the QML to notify the app of view changes
     */
    Q_INVOKABLE void changeView(QString viewType);

    /*
     * Allows the QML to reset the state of the application
     */
    Q_INVOKABLE void reset();

public slots:
    /*
     * Handles the complete signal from TwitterRequest when
     * the request is complete
     * @see TwitterRequest::complete()
     */
    void onTwitterTimeline(QString info, bool success);

private:
    /*
     * Parses the JSON response from the twitter request
     */
    void parseResponse();

    /*
     * Makes visible and shows error text on the main screen
     */
    void showError(QString error);

private:
    bb::cascades::NavigationPane* m_navPane;
    QString m_twitterResponse;
    bool m_requestInProgress;
    ViewType m_viewType;
};

#endif // ifndef APP_H
