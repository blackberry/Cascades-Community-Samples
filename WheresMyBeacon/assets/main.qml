/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

Page {
    
    Container {
        // ======== Identity ===============
        
        id: mainPage
        objectName: "mainPage"
        
        // ======== Properties =============
        
        // ======== SIGNAL()s ==============
        
        signal startListening()
        signal stopListening()
        
        // ======== SLOT()s ================
        
        function onMessage(text) {
            logMessage(text);
        }
        
        // ======== Local functions ========
        
        function logMessage(message) {
            log.text += (qsTr("\n") + message );
        }
        
        layout: StackLayout {
        }
        
        topPadding: 10
        leftPadding: 30
        rightPadding: 30
        
        Label {
            text: qsTr("Where's My Beacon")
            textStyle {
                base: SystemDefaults.TextStyles.BigText
                fontWeight: FontWeight.Bold
            }
        }
        
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: startListeningButton
                text: "Start"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.startListening();
                }
            }
            Button {
                id: stopListeningButton
                text: "Stop"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.stopListening();
                }
            }
        }
        
        Container {
            topPadding: 20
            leftPadding: 20
            rightPadding: 20
            bottomPadding: 20
            
            ListView {
                id: beacon_list
                objectName: "beacon_list"

                dataModel: app.model
                
                listItemComponents: [
                    ListItemComponent {
                        type: "listItem"
                        StandardListItem {
                            title: ListItemData.UUID
                            description: "Maj: " + ListItemData.MAJOR + " Min: " + ListItemData.MINOR + " RSSI: " + ListItemData.RSSI + " P-Loss: " + ListItemData.LOSS
                        }
                    }
                ]
                
                function itemType(data, indexPath) {
                    if (indexPath.length == 1) {
                        return "header";
                    } else {
                        return "listItem";
                    }
                }
                accessibility.name: "TODO: Add property content"
            }
        }
        Logger {
            id: log
            visible: false
        }
    }
}
