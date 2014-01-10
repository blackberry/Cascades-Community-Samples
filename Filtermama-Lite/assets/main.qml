/* Copyright (c) 2013 Chad Tetreault
 * http://palebanana.com - @chadtatro
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

import bb.cascades 1.0

TabbedPane {
    id: tabs
    showTabsOnActionBar: true
    peekEnabled: false

    property variant filePicked: false
    property variant filePath: false
    property variant fileSaved: false
    property variant thumbnailsLoaded: false
    property variant lastTab: null

    attachedObjects: [
        ComponentDefinition {
            id: aboutSheetDefinition
            AboutSheet {
            }
        },

        BasePage {
            id: basePage
        }
    ]

    Menu.definition: MenuDefinition {
        // Add any remaining actions
        actions: [
            ActionItem {
                title: qsTr("About") + Retranslate.onLanguageChanged
                imageSource: "asset:///images/about.png"
                onTriggered: {
                    var about = aboutSheetDefinition.createObject(app)
                    about.open();
                }
            },

            ActionItem {
                title: qsTr("Help") + Retranslate.onLanguageChanged
                imageSource: "asset:///images/help.png"
                onTriggered: {
                    app.invoke("sys.browser", "bb.action.OPEN", "*", "http://palebanana.com");
                }
            },

            ActionItem {
                title: qsTr("Follow Me!") + Retranslate.onLanguageChanged
                imageSource: "asset:///images/twitter.png"
                onTriggered: {
                    app.invoke("com.twitter.urihandler", "bb.action.VIEW", "*", "twitter:connect:@chadtatro");
                }
            },

            ActionItem {
                title: qsTr("Invite Friends") + Retranslate.onLanguageChanged
                imageSource: "asset:///images/bbm.png"
                enabled: bbmHandler.allowed
                onTriggered: {
                    bbmHandler.sendInvite();
                }
            }
        ]
    }

    /* ==================================================
     *      feed
     * ==================================================
     */

    Tab {
        id: tabFeed
        title: qsTr("Feed") + Retranslate.onLanguageChanged
        imageSource: "asset:///images/feed.png"
        Feed {
        }
    }

    /* ==================================================
     *      processing
     * ==================================================
     */

    Tab {
        id: tabProcess
        title: qsTr("Process") + Retranslate.onLanguageChanged
        imageSource: "asset:///images/process.png"

        Filtering {
            id: filterPage
        }

        // update the webview
        function updateImage(imgData) {
            filterPage.updateWebview(imgData);
        }
    }

    /* ==================================================
     *     if no photo picked, go back to last active tab
     * ==================================================
     */

    onActiveTabChanged: {
        if (activeTab === tabProcess) {
            if (! filePicked) {
                basePage.filepicker.open();
            }
        } else {
            lastTab = activeTab;
        }
    }

    onCreationCompleted: {
        lastTab = tabFeed;
    }
}
