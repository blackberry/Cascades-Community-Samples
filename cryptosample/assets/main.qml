/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
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

    // ======== Attached Objects ========
    
    attachedObjects: [
        AboutSheet {
            id: aboutInfo
        }
    ]
    
    Menu.definition: MenuDefinition {
        actions: [
            ActionItem {
                title: "About"
                
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
        
        property bool doingRandom: false
        property bool doingHash: false
        property bool doingRsa: false
        property bool doingKdf: false
        property bool doingStream: false
        property int hashType: 0
        
        // ======== SIGNAL()s ==============

        signal initRandomNumber()
        signal getRandomNumber()
        signal endRandomNumber()

        signal initHash()
        signal endHash()
        signal makeHash(int type)

        signal initRsa()
        signal endRsa()
        signal doRsa()
        
        signal initKdf()
        signal endKdf()
        signal doKdf()

        signal initStream()
        signal endStream()
        signal doStream()

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
            text: qsTr("Simple Crypto Stuff")
            textStyle {
                base: SystemDefaults.TextStyles.BigText
                fontWeight: FontWeight.Bold
            }
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: initRandomNumber
                enabled: !mainPage.doingRandom && !mainPage.doingHash && !mainPage.doingRsa && !mainPage.doingKdf && !mainPage.doingStream
                text: "Init Random"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    mainPage.doingRandom = true;
                    mainPage.initRandomNumber();
                }
            }
            Button {
                id: randomNumber
                enabled: mainPage.doingRandom
                text: "Get Number"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    mainPage.getRandomNumber();
                }
            }
            Button {
                id: endRandomNumber
                enabled: mainPage.doingRandom
                text: "End Random"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    mainPage.doingRandom = false;
                    mainPage.endRandomNumber();
                }
            }
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: initSecurity
                enabled: !mainPage.doingRandom && !mainPage.doingHash && !mainPage.doingRsa && !mainPage.doingKdf && !mainPage.doingStream
                text: "Init Hash"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    enabled: mainPage.doingHash = true;
                    mainPage.initHash();
                }
            }
            Button {
                id: makeHash
                enabled: mainPage.doingHash
                text: "Make Hash"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    mainPage.makeHash(mainPage.hashType);
                }
            }
            Button {
                id: endSecurity
                enabled: mainPage.doingHash
                text: "End Hash"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    enabled: mainPage.doingHash = false;
                    mainPage.endHash();
                }
            }
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            visible: mainPage.doingHash
            RadioGroup {
                Option {
                    id: optSHA1
                    text: "SHA1"
                    selected: (mainPage.hashType == 0)
                }
                onSelectedOptionChanged: {
                    if (optSHA1.selected == true) {
                        mainPage.hashType = 0;
                    }
                }
            }
            RadioGroup {
                Option {
                    id: optSHA256
                    text: "SHA256"
                    selected: (mainPage.hashType == 1)
                }
                onSelectedOptionChanged: {
                    if (optSHA256.selected == true) {
                        mainPage.hashType = 1;
                    }
                }
            }
            RadioGroup {
                Option {
                    id: optHMAC
                    text: "HMAC"
                    selected: (mainPage.hashType == 2)
                }
                onSelectedOptionChanged: {
                    if (optHMAC.selected == true) {
                        mainPage.hashType = 2;
                    }
                }
            }
            RadioGroup {
                Option {
                    id: optSHA1KDF2
                    text: "KDF2"
                    selected: (mainPage.hashType == 3)
                }
                onSelectedOptionChanged: {
                    if (optSHA1KDF2.selected == true) {
                        mainPage.hashType = 3;
                    }
                }
            }
        }
        
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: initRsa
                enabled: !mainPage.doingRandom && !mainPage.doingHash && !mainPage.doingRsa && !mainPage.doingKdf && !mainPage.doingStream
                text: "Init RSA"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    enabled: mainPage.doingRsa = true;
                    mainPage.initRsa();
                }
            }
            Button {
                id: makeRsa
                enabled: mainPage.doingRsa
                text: "Do RSA"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    mainPage.doRsa();
                }
            }
            Button {
                id: endRsa
                enabled: mainPage.doingRsa
                text: "End RSA"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    enabled: mainPage.doingRsa = false;
                    mainPage.endRsa();
                }
            }
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: initKdf
                enabled: !mainPage.doingRandom && !mainPage.doingHash && !mainPage.doingRsa && !mainPage.doingKdf && !mainPage.doingStream
                text: "Init KDF"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    enabled: mainPage.doingKdf = true;
                    mainPage.initKdf();
                }
            }
            Button {
                id: makeKdf
                enabled: mainPage.doingKdf
                text: "Gen Key"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    mainPage.doKdf();
                }
            }
            Button {
                id: endKdf
                enabled: mainPage.doingKdf
                text: "End Kdf"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    enabled: mainPage.doingKdf = false;
                    mainPage.endKdf();
                }
            }
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: initStream
                enabled: !mainPage.doingRandom && !mainPage.doingHash && !mainPage.doingRsa && !mainPage.doingKdf && !mainPage.doingStream
                text: "Init Stream"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    enabled: mainPage.doingStream = true;
                    mainPage.initStream();
                }
            }
            Button {
                id: doStream
                enabled: mainPage.doingStream
                text: "Do Stream"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    mainPage.doStream();
                }
            }
            Button {
                id: endStream
                enabled: mainPage.doingStream
                text: "End Stream"
                horizontalAlignment: HorizontalAlignment.Center
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 33
                }
                onClicked: {
                    enabled: mainPage.doingStream = false;
                    mainPage.endStream();
                }
            }
        }

        Logger {
            id: log
        }
    }
}
