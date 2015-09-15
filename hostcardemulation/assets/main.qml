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

        property bool emulationStarted: false
        property bool aidRegistered: false
        property bool ppseRegistered: false
        property bool hceSupported: false
        property bool nfcSupported: false
        property bool featureSetSupported: false
        
        // ======== SIGNAL()s ==============

        signal startEmulating()
        signal stopEmulating()
        signal registerAid()
        signal unregisterAid()
        signal registerPPSE()
        signal unregisterPPSE()

        // ======== SLOT()s ================

        function onMessage(text) {
            logMessage(text);
        }

        function onEmulationStopped() {
            mainPage.emulationStarted = false;
            console.debug("XXXX HCE: In onEmulationStopped()");
            logMessage("HCE Stopped");
        }

        function onEmulationStarted() {
            console.debug("XXXX HCE: In onEmulationStarted()");
            mainPage.emulationStarted = true;
            logMessage("HCE Started");
        }
        
        function onAidRegistered(aid) {
            console.debug("XXXX HCE: In onAidRegistered()");
            mainPage.aidRegistered = true;
            logMessage("HCE AID Registered: " + aid);
        }
        
        function onAidUnregistered(aid) {
            console.debug("XXXX HCE: In onAidUnregistered()");
            mainPage.aidRegistered = false;
            logMessage("HCE AID Unregistered: " + aid);
        }
        
        function onPPSERegistered(aid) {
            console.debug("XXXX HCE: In onPPSERegistered()");
            mainPage.ppseRegistered = true;
            logMessage("HCE PPSE AID Registered: " + aid);
        }
        
        function onPPSEUnregistered(aid) {
            console.debug("XXXX HCE: In onPPSEUnregistered()");
            mainPage.ppseRegistered = false;
            logMessage("HCE PPSE AID Unregistered: " + aid);
        }

        function onFeatureSetSupported(featureSet) {
            console.debug("XXXX HCE: In onFeatureSetSupported() - " + featureSet);
            
            if (featureSet >= 0) {
                logMessage("Device supports feature set " + featureSet);
            }

            if (featureSet == 0) {
                mainPage.featureSetSupported = true;
                mainPage.nfcSupported = false;
                mainPage.hceSupported = false;
                logMessage("NFC not supported");
            } else if (featureSet == 1) {
                mainPage.featureSetSupported = true;
                mainPage.nfcSupported = true;
                mainPage.hceSupported = false;
                logMessage("HCE AID registration not supported");
            } else if (featureSet == 2) {
                mainPage.featureSetSupported = true;
                mainPage.nfcSupported = true;
                mainPage.hceSupported = true;
                logMessage("HCE AID registration supported");
            } else {
                mainPage.featureSetSupported = false;
                mainPage.nfcSupported = true;
                mainPage.hceSupported = false;
                logMessage("Unknown NFC / HCE AID registration supported");
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
            text: qsTr("Host Card Emulation")
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
                id: registerAidButton
                text: "HCE Register AID"
                enabled: mainPage.nfcSupported && mainPage.hceSupported && !mainPage.aidRegistered && !mainPage.emulationStarted
                visible: mainPage.featureSetSupported
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.registerAid();
                }
            }
            Button {
                id: unregisterAidButton
                text: "HCE Unregister AID"
                enabled: mainPage.nfcSupported && mainPage.hceSupported && mainPage.aidRegistered && !mainPage.emulationStarted
                visible: mainPage.featureSetSupported
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.unregisterAid();
                }
            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: registerPPSEButton
                text: "HCE Register PPSE"
                enabled: mainPage.nfcSupported && mainPage.hceSupported && !mainPage.ppseRegistered && !mainPage.emulationStarted
                visible: mainPage.featureSetSupported
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.registerPPSE();
                }
            }
            Button {
                id: unregisterPPSEButton
                text: "HCE Unregister PPSE"
                enabled: mainPage.nfcSupported && mainPage.hceSupported && mainPage.ppseRegistered && !mainPage.emulationStarted
                visible: mainPage.featureSetSupported
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.unregisterPPSE();
                }
            }
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: startEmulationButton
                text: "Start HCE"
                enabled: mainPage.nfcSupported && !mainPage.emulationStarted && (!(mainPage.ppseRegistered || mainPage.aidRegistered) || !mainPage.hceSupported)
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.startEmulating();
                }
            }
            Button {
                id: stopEmulationButton
                text: "Stop HCE"
                enabled: mainPage.nfcSupported && mainPage.emulationStarted  && (!(mainPage.ppseRegistered || mainPage.aidRegistered) || !mainPage.hceSupported)
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                onClicked: {
                    mainPage.stopEmulating();
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