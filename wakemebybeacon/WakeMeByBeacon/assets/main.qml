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

        property bool scanningStarted: false
        property bool serviceRunning: false
        
        // ======== SIGNAL()s ==============

        signal addScanRequest(string address)
        signal enableScanInvokeRequest()
        signal disableScanInvokeRequest()
        signal removeScanRequest(string address)
        signal startHeadlessService()
        signal stopHeadlessService()

        // ======== SLOT()s ================

        function onMessage(text) {
            logMessage(text);
        }

        function onScanStateChanged(scanning) {
            if (scanning) {
                scanningStarted = true;
                logMessage("Scanning in progress")
            } else {
                scanningStarted = false;
                logMessage("Scanning stopped")
            }
        }

        function onStartedStateChanged(running) {
            if (running) {
                serviceRunning = true;
                logMessage("Headless Service Running")
            } else {
                serviceRunning = false;
                logMessage("Headless Service Not Running")
            }
        }
        //	void altBeaconEnteredRange(const QVariant &timeStamp, const QVariant &macAddress, const QVariant &companyCode,
        // const QVariant &beaconId, const QVariant &reserved, const QVariant &rssi, const QVariant &loss);

        function onIBeaconEnteredRange(timeStamp, macAddress, uuid, major, minor, rssi, loss) {
            logMessage("iBeacon entered:\n UUID: " + uuid + "\n Major: " + major + " Minor: " + minor + " Loss: " + loss);
        }

        function onIBeaconExitedRange(timeStamp, macAddress, uuid, major, minor, rssi, loss) {
            logMessage("iBeacon exited:\n UUID: " + uuid + "\n Major: " + major + " Minor: " + minor + " Loss: " + loss);
        }

        function onAltBeaconEnteredRange(timeStamp, macAddress, companyCode, companyName, beaconId, reserved, rssi, loss) {
            if (companyName != "") {
                logMessage("AltBeacon entered:\n ID: " + beaconId + "\n Company Name: " + companyName + " Reserved: " + reserved + " Loss: " + loss);
            } else {
                logMessage("AltBeacon entered:\n ID: " + beaconId + "\n Company Code: " + companyCode + " Reserved: " + reserved + " Loss: " + loss);
            }
        }
        
        function onAltBeaconExitedRange(timeStamp, macAddress, companyCode, companyName, beaconId, reserved, rssi, loss) {
            if (companyName != "") {
                logMessage("AltBeacon exited:\n ID: " + beaconId + "\n Company Name: " + companyName + " Reserved: " + reserved + " Loss: " + loss);
            } else {
                logMessage("AltBeacon exited:\n ID: " + beaconId + "\n Company Code: " + companyCode + " Reserved: " + reserved + " Loss: " + loss);
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
            text: qsTr("Wake Me By Beacon")
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
                id: enableScanButton
                text: "Enable Scan"
                enabled: !mainPage.scanningStarted && mainPage.serviceRunning
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.addScanRequest("00:00:00:00:00:00");
                    mainPage.enableScanInvokeRequest();
                }
            }
            Button {
                id: disableScanButton
                text: "Disable Scan"
                enabled: mainPage.scanningStarted
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.removeScanRequest("00:00:00:00:00:00");
                    mainPage.disableScanInvokeRequest();
                }
            }
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: startServiceButton
                text: "Start Service"
                enabled: !mainPage.serviceRunning
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.startHeadlessService();
                }
            }
            Button {
                id: stopServiceButton
                text: "Stop Service"
                enabled: mainPage.serviceRunning && !mainPage.scanningStarted
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.stopHeadlessService();
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