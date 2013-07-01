/* Copyright (c) 2013 Research In Motion Limited.
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

NavigationPane {
    id: navigationPane
    Page {
        Container {
            id: mainPage
            objectName: "mainPage"

            // ======== Properties ========
            property bool audioIsStarted: false
            property int maxLevel: 32767
            property int levelScale: 100
            property int redThreshold: 10000
            property int signalLevelThreshold: 200

            // ======== SIGNAL()s ========
            signal startAudio()
            signal stopAudio()
//            signal muteInput()
            signal sendMessage(string message);

            // ======== SLOT()s ========
            function audioStarted() {
                audioIsStarted = true;
                mainPage.logMessage("Audio Started");
            }
            function audioStopped() {
                audioIsStarted = false;
                levelText.text = 0;
                levelIndicator.value = 0;
                mainPage.logMessage("Audio Stopped");
            }
            function message(data) {
                logMessage(data);
            }
            function signalLevel(level) {
                levelText.text = level;
                levelIndicator.value = levelToProgressScale(level);
                if (level < mainPage.redThreshold) {
                    levelIndicator.state = ProgressIndicatorState.Progress;
                } else {
                    levelIndicator.state = ProgressIndicatorState.Error;
                }
            }

            // ======== Local functions ========
            function logMessage(message) {
                log.text += (qsTr("\n") + message );
            }
            function levelToProgressScale(level) {
                var scale;
                if (level >= 0) {
                    scale = mainPage.levelScale * Math.log(level + 1);
                } else {
                    scale = 0;
                }
                return scale;
            }

            // ======== Components ========
            layout: StackLayout {
            }
            topPadding: 10
            leftPadding: 30
            rightPadding: 30
            preferredWidth: 768
            preferredHeight: 1280
            Container {
                layout: DockLayout {
                }
                Container {
                    layout: StackLayout {
                    }
                    horizontalAlignment: HorizontalAlignment.Fill
                    Label {
                        text: qsTr("Audio Accessory")
                        horizontalAlignment: HorizontalAlignment.Center
                        textStyle {
                            base: SystemDefaults.TextStyles.BigText
                            color: Color.LightGray
                            textAlign: TextAlign.Center
                        }
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                        ImageLabel {
                            label: qsTr("Audio")
                            image: mainPage.audioIsStarted ? "asset:///images/led_green_black.png" : "asset:///images/led_red_black.png"
                            horizontalAlignment: HorizontalAlignment.Left
                        }
                        ImageLabel {
                            label: qsTr("Signal")
                            image: (levelText.text > mainPage.signalLevelThreshold) ? "asset:///images/led_green_black.png" : "asset:///images/led_red_black.png"
                            horizontalAlignment: HorizontalAlignment.Right
                        }
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
	                    Button {
	                        id: startButton
	                        text: qsTr("Start Audio")
	                        enabled: ! mainPage.audioIsStarted
	                        horizontalAlignment: HorizontalAlignment.Fill
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 10
                            }
	                        onClicked: {
	                            mainPage.startAudio();
	                            mainPage.logMessage("Starting Audio");
	                        }
	                    }
	                    Button {
	                        id: stopButton
	                        text: qsTr("Stop Audio")
	                        enabled: mainPage.audioIsStarted
	                        horizontalAlignment: HorizontalAlignment.Fill
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 10
                            }
	                        onClicked: {
	                            mainPage.stopAudio();
	                            mainPage.logMessage("Stopping Audio");
	                        }
	                    }
	                }
//                    Button {
//                        id: muteButton
//                        text: qsTr("Mute Input")
//                        enabled: mainPage.audioIsStarted
//                        horizontalAlignment: HorizontalAlignment.Fill
//                        onClicked: {
//                            mainPage.muteInput();
//                            mainPage.logMessage("Muting input");
//                        }
//                    }
					Container {
					    layout: StackLayout {
					        orientation: LayoutOrientation.LeftToRight
					    }
	                    Button {
	                        id: sendButton
	                        text: qsTr("Send Message")
	                        enabled: mainPage.audioIsStarted
	                        horizontalAlignment: HorizontalAlignment.Fill
	                        topPadding: 20
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 10
                            }
	                        onClicked: {
	                            mainPage.sendMessage(sendText.text);
	                            mainPage.logMessage("Sending Message " + sendText.text);
	                        }
	                    }
	                    TextField {
	                        id: sendText
	                        enabled: mainPage.audioIsStarted
	                        verticalAlignment: VerticalAlignment.Center
	                        horizontalAlignment: HorizontalAlignment.Center
	                        hintText: ""
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 10
                            }
	                    }
	                }
	                Container {
                        horizontalAlignment: HorizontalAlignment.Left
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        Label {
                            text: qsTr("Level:")
                        }
                        Label {
                            id: levelText
                            text: qsTr("0")
                        }
                    }
                    ProgressIndicator {
                        id: levelIndicator
                        visible: true
                        fromValue: 0
                        toValue: mainPage.levelToProgressScale(mainPage.maxLevel)
                        value: 0
                        state: ProgressIndicatorState.Progress
                    }
                    Logger {
                        id: log
                    }
                }
            }
            onCreationCompleted: {
            }
        }
        actions: [
            ActionItem {
                title: "About"
                imageSource: "asset:///images/about.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    aboutInfo.open();
                }
            },
            InvokeActionItem {
                id: shareMe
                ActionBar.placement: ActionBarPlacement.OnBar
                query {
                    mimeType: "text/plain"
                    invokeActionId: "bb.action.SHARE"
                    onQueryChanged: {
                        shareMe.query.updateQuery()
                    }
                }
                handler: InvokeHandler {
                    id: shareHandler
                    onInvoking: {
                        shareMe.data = "Check out the #BlackBerry10 'AudioAccessory' " + "Sample App on Git Hub written by @jcmrim and @mdwrim " + "here http://bit.ly/SJBBKl";
                        shareHandler.confirm();
                    }
                }
            }
        ]
        attachedObjects: [
            AboutSheet {
                id: aboutInfo
            }
        ]
    }
    onCreationCompleted: {
        console.log("XXXX NavigationPane - onCreationCompleted()");
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.All;
    }
    onPopTransitionEnded: {
        console.log("XXXX NavigationPane - onPopTransitionEnded()");
        page.destroy();
    }
}
