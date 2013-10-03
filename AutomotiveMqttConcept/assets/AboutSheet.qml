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

Sheet {
    id: root
    Page {
        actionBarVisibility: ChromeVisibility.Visible
        Container {
            layout: StackLayout {
            }
            leftPadding: 20
            rightPadding: 20
            verticalAlignment: VerticalAlignment.Fill
            Label {
                id: lblHeading_about
                text: qsTr("About AutomotiveMqttConcept")
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            Label {
                id: lblHeading_version
                text: qsTr("V1.0.0")
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            TextArea {
                id: taBody_about1
                text: qsTr("The AutomotiveMqttConcept application demonstrates acquisition of data using Bluetooth sensors and its transmission to the cloud using MQTT")
                textStyle {
                    base: SystemDefaults.TextStyles.SmallText
                }
                editable: false
            }
            TextArea {
                id: taBody_about2
                text: qsTr("All features are hard coded for concept visualisation purposes except for the heart rate measurement which works with any Bluetooth Smart device that implements the heart rate profile")
                textStyle {
                    base: SystemDefaults.TextStyles.SmallText
                }
                editable: false
            }
            Container {
            	leftPadding: 20
                Label {
                    id: lblAuthors
                    text: qsTr("Authors:")
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                    }
                }
                Label {
                    id: lblAuthors1
                    text: qsTr("Martin Woolley (@mdwrim)")
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                    }
                }
                Label {
                    id: lblAuthors2
                    text: qsTr("John Murray (@jcmrim)")
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                    }
                }
            }
        }
        actions: [
            ActionItem {
                title: "Back"
                imageSource: "asset:///images/ic_previous.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    root.close();
                }
            },
            InvokeActionItem {
                id: shareMe
                ActionBar.placement: ActionBarPlacement.OnBar
                query {
                    mimeType: "text/plain"
                    invokeActionId: "bb.action.SHARE"
                    onQueryChanged: {
                        shareMe.query.updateQuery();
                    }
                }
                handler: InvokeHandler {
                    id: shareHandler
                    onInvoking: {
                        shareMe.data = "Check out the #BlackBerry10 'AutomotiveMqttConcept' " + "Sample App on Git Hub written by @jcmrim and @mdwrim " + "here http://bit.ly/SJBBKl";
                        shareHandler.confirm();
                    }
                }
            }
        ]

    }
}
