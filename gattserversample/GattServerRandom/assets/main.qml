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

import bb.cascades 1.3

Page {

    attachedObjects: [
        AboutSheet {
            id: aboutInfo
        }
    ]

    Menu.definition: MenuDefinition {
        actions: [
            ActionItem {
                title: "About"
                imageSource: "images/about.png"

                onTriggered: {
                    aboutInfo.open();
                }
            }
        ]
    }

    Container {
        // ======== Identity ===============

        id: mainPage
        objectName: "mainPage"

        // ======== Properties =============

        property bool serviceRunning: false
        property bool serviceConnected: false
        
        // ======== SIGNAL()s ==============

        signal startGattService()
        signal stopGattService()

        // ======== SLOT()s ================

        function onMessage(text) {
            logMessage(text);
        }

        function onStartedStateChanged(running) {
            if (running) {
                serviceRunning = true;
                logMessage("GATT Service Running")
            } else {
                serviceRunning = false;
                logMessage("GATT Service Not Running")
            }
        }

        function onConnectionStateChanged(connected) {
            if (connected) {
                serviceConnected = true;
                logMessage("GATT Service Connected")
            } else {
                serviceConnected = false;
                logMessage("GATT Service Not Connected")
            }
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
            text: qsTr("GATT Server")
            textStyle {
                base: SystemDefaults.TextStyles.BigText
                fontWeight: FontWeight.Bold
            }
            horizontalAlignment: HorizontalAlignment.Center
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: startServiceButton
                text: "Start Service"
                enabled: !mainPage.serviceRunning && !mainPage.serviceConnected
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.startGattService();
                }
            }
            Button {
                id: stopServiceButton
                text: "Stop Service"
                enabled: mainPage.serviceRunning && !mainPage.serviceConnected
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.stopGattService();
                }
            }
        }
        Container {
            topPadding: 20
            leftPadding: 20
            rightPadding: 20
            bottomPadding: 20
        }
        Logger {
            id: log
            visible: true
        }
    }
}