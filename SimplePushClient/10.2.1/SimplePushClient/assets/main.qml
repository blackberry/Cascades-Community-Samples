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

import bb.cascades 1.2

/*
 * This is the main Pane for our app. It displays all received push messages
 * in a list and allows the user to click the messages to expand the message
 * to a Page where more details are able to be displayed.
 */

NavigationPane {
    id: navigationPane

    /* This ComponentDefinition lets us dynamically create a Page to push
     * whenever needed. In this case we use it to display the push message.
     */
    attachedObjects: [
        ComponentDefinition {
            id: pushMessageDetailsPage
            source: "PushMessageDetailsPage.qml"
        }
    ]
    Page {
        Container {
            bottomPadding: 20.0
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            Container {
                layout: DockLayout {
                }
                horizontalAlignment: HorizontalAlignment.Fill
                preferredHeight: maxHeight

                //The below ListView will be used to organize our push messages
                ListView {
                    id: pushListView
                    objectName: "pushListView"
                    listItemComponents: [
                        ListItemComponent {
                            type: "item"
                            StandardListItem {
                                title: ListItemData.title
                                description: ListItemData.body
                                status: ListItemData.timeReceived
                            }
                        },
                        ListItemComponent {
                            type: "header"
                            //PriorityHeader is a custom component, gives a better visual
                            // representation of the push priority
                            PriorityHeader {
                                priority: ListItemData
                            }
                        }
                    ]
                    onTriggered: {
                        // When a push message is selected we grab the data and push it
                        // in a new page.
                        var selectedData = pushListView.dataModel.data(indexPath);
                        var detailsPage = pushMessageDetailsPage.createObject();
                        detailsPage.priority = selectedData.priority;
                        detailsPage.title = selectedData.title;
                        detailsPage.body = selectedData.body;
                        detailsPage.timeReceived = selectedData.timeReceived;
                        navigationPane.push(detailsPage);
                    }
                }
                //When no data is available we use this Label to let the user know
                Label {
                    text: "No push data received"
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    visible: ! pushListView.visible
                }
            }
            //Clicking this button will delete all push messages
            Button {
                objectName: "clearPushList"
                text: "Clear Push List"
                horizontalAlignment: HorizontalAlignment.Center
            }
        }
    }
}