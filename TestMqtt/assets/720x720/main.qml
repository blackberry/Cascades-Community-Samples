/* Copyright (c) 2013 BlackBerry Limited.
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

            property bool clientConnected: false
            property bool clientSubscribed: false

            // ======== SIGNAL()s ========

            signal mqttConnect(string hostname)
            signal mqttDisconnect()
            signal mqttPublish(string topic, string message)
            signal mqttSubscribe(string topic)
            signal mqttUnSubscribe(string topic)

            // ======== SLOT()s ========

            function mqttConnected() {
                clientConnected = true;
                mainPage.logMessage("Client Started");
            }

            function mqttDisconnected() {
                clientConnected = false;
                mainPage.logMessage("Client Stopped");
            }

            function message(data) {
                logMessage(data);
            }

            function clientConnectionStatus() {
                if (clientConnected) {
                    return "started";
                } else {
                    return "not started";
                }
            }
            // ======== Local functions ========

            function logMessage(message) {
                log.text += (qsTr("\n") + message );
            }

            // ======== Components ========

            layout: StackLayout {
            }

            topPadding: 10
            leftPadding: 30
            rightPadding: 30

            Container {
                layout: DockLayout {
                }

                leftPadding: 20
                bottomPadding: 20
                horizontalAlignment: HorizontalAlignment.Center

                Container {
                    layout: StackLayout {
                    }
                    Label {
                        text: qsTr("MQTT Test Client")
                        horizontalAlignment: HorizontalAlignment.Center
                        textStyle {
                            color: Color.LightGray
                            fontWeight: FontWeight.Bold
                            textAlign: TextAlign.Center
                        }
                    }

                    SegmentedControl {
                        id: segments
                        Option {
                            id: seg_control
                            text: "Control"
                            value: "control"
                            selected: true
                        }
                        Option {
                            id: seg_log
                            text: "Activity"
                            value: "activity"
                        }
                        onSelectedIndexChanged: {
                            var selected_seg = segments.selectedValue
                            console.log("Selected segment: " + selected_seg);
                            if (selected_seg == "control") {
                                container_control.visible = true
                                container_activity.visible = false
                            } else {
                                container_control.visible = false
                                container_activity.visible = true
                            }
                        }
                    }
                    Divider {
                    }
                    Container {
                        id: container_control

                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Button {
                                id: startButton
                                text: mainPage.clientConnected ? qsTr("Stop") : qsTr("Start")
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                                onClicked: {
                                    if (mainPage.clientConnected) {
                                        mainPage.mqttDisconnect();
                                        mainPage.clientSubscribed = false;
                                    } else {
                                        mainPage.mqttConnect(hostname.text);
                                    }

                                }
                            }
                            TextField {
                                id: hostname
                                text: "test.mosquitto.org"
                                enabled: ! mainPage.clientConnected
                                verticalAlignment: VerticalAlignment.Center
                                inputMode: TextFieldInputMode.Text
                                input.flags: TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoPeriodOff | TextInputFlag.PredictionOff | TextInputFlag.SpellCheckOff
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }

                            Button {
                                id: startSubscribeButton
                                text: (mainPage.clientConnected & mainPage.clientSubscribed) ? qsTr("Unsubscribe") : qsTr("Subscribe")
                                enabled: mainPage.clientConnected
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                                onClicked: {
                                    if (mainPage.clientSubscribed) {
                                        mainPage.clientSubscribed = false;
                                        mainPage.mqttUnSubscribe(topicText.text);
                                    } else {
                                        mainPage.clientSubscribed = true;
                                        mainPage.mqttSubscribe(topicText.text);
                                    }
                                }
                            }
                            TextField {
                                id: topicText
                                text: "bbc/#"
                                //text: "foo/bar"
                                inputMode: TextFieldInputMode.Text
                                input.flags: TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoPeriodOff | TextInputFlag.PredictionOff | TextInputFlag.SpellCheckOff
                                enabled: mainPage.clientConnected & ! mainPage.clientSubscribed
                                verticalAlignment: VerticalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {

                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Container {

								leftPadding: 15

                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }

                                Label {
                                    text: "    Publish to"
                                }
                            }
                            TextField {
                                id: publishTopicText
                                text: "foo/bar"
                                inputMode: TextFieldInputMode.Text
                                input.flags: TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoPeriodOff | TextInputFlag.PredictionOff | TextInputFlag.SpellCheckOff
                                enabled: mainPage.clientConnected
                                verticalAlignment: VerticalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {

                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }

                            Button {
                                id: sendButton
                                text: qsTr("Publish")
                                enabled: mainPage.clientConnected
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                                onClicked: {
                                    mainPage.mqttPublish(publishTopicText.text, sendText.text);
                                }
                            }
                            TextField {
                                id: sendText
                                text: "Hello, MQTT!"
                                inputMode: TextFieldInputMode.Text
                                enabled: mainPage.clientConnected
                                verticalAlignment: VerticalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Divider {
                        }
                        ImageLabel {
                            label: qsTr("MQTT client " + mainPage.clientConnectionStatus())
                            image: mainPage.clientConnected ? "asset:///images/led_green_black.png" : "asset:///images/led_red_black.png"
                            horizontalAlignment: HorizontalAlignment.Center
                        }

                    }
                    Container {
                        id: container_activity
                        visible: false
                        Logger {
                            id: log
                        }
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
                    //mimeType: "message/rfc822"
                    //invokeActionId: "bb.action.COMPOSE"
                    onQueryChanged: {
                        shareMe.query.updateQuery()
                    }
                }
                handler: InvokeHandler {
                    id: shareHandler
                    onInvoking: {
                        shareMe.data = "Check out the #BlackBerry10 'TestMQTT' " + "Sample App on Git Hub written by @jcmrim and @mdwrim " + "here http://bit.ly/16Z4oPM";
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
