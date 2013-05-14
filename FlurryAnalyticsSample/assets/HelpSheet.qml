/* Copyright (c) 2013 BlackBerry.
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

/**
 * The HelpSheet displays a list of FAQ including potential answers to each. All interactions
 * with the questions are logged as well as when the user flags the answer as adequately
 * answering the question.
 */

Sheet {
    id: helpSheet
    objectName: "helpSheet"

    content: Page {
        titleBar: TitleBar {
            title: "Help FAQ"
            dismissAction: ActionItem {
                title: "Close"
                onTriggered: {
                    close();
                }
            }
        }
        Container {
            layout: DockLayout {

            }
            ImageView {
                imageSource: "asset:///images/crumpledPage.png"
            }
            BackgroundContainer {
            }
            ListView {
                id: listView
                dataModel: XmlDataModel {
                    source: "models/faq.xml" //See assets/models/faq.xml
                }
                listItemComponents: ListItemComponent {
                    type: "faq"
                    //Add a new ExpandableElement for every Q in the FAQ.xml file
                    ExpandableElement {
                        objectName: ListItemData.objectName
                        headerText: ListItemData.question
                        bodyText: ListItemData.answer
                        answerButtonText: ListItemData.answerButton
                        
                        //Track all changes to visibility of the ExpandableElement body
                        onBodyVisibleChanged: {
                            if (bodyVisible) {
                                ListItem.view.logEvent(objectName + " expanded");
                            } else {
                                ListItem.view.logEvent(objectName + " minimized");
                            }
                        }
                        //Track which Qs were answered by the body As
                        onAnswered: {
                            ListItem.view.logEvent(objectName + " question answered!");
                        }
                    }
                }
                
                // The close and logEvent functions are re-written here so they can be accessed
                // by the ListView controls that exist in a separate context.
                function close() {
                    helpSheet.close();
                }
                function logEvent(message) {
                    cpp.logEvent(message);
                }
            }
        }
    }
    
    // Start and stop time events when the sheet is displayed/closed respectively
    onOpened: {
        cpp.logEvent(objectName + " page displayed", true);
    }
    onClosed: {
        cpp.endTimedEvent(objectName + " page displayed");
    }
}
