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

Sheet {
    id: root
    onOpened: {
        animation.play();
    }
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
                text: qsTr("About BlackBerryBleExplorer")
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            Label {
                id: lblHeading_version
                text: qsTr("V1.0.0")
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            ImageView {
                horizontalAlignment: HorizontalAlignment.Center
                imageSource: "asset:///images/icon.png"
                opacity: 0
                onTouch: {
                    if (event.isDown()) {
                        animation.play()
                    }
                }
                attachedObjects: [
                    SequentialAnimation {
                        id: animation
                        animations: [
                            FadeTransition {
                                duration: 1000
                                fromOpacity: 0.0
                                toOpacity: 1.0
                            },
                            RotateTransition {
                                toAngleZ: -10
                                duration: 125
                            },
                            RotateTransition {
                                toAngleZ: 10
                                duration: 250
                            },
                            RotateTransition {
                                toAngleZ: -10
                                duration: 250
                            },
                            RotateTransition {
                                toAngleZ: 10
                                duration: 250
                            },
                            RotateTransition {
                                toAngleZ: 0
                                duration: 125
                            }
                        ]
                    }
                ]
            }

            Label {
                id: taBody_about1
                text: qsTr("The BlackBerryBleExplorer application demonstrates the BlackBerry 10 Bluetooth® Smart APIs")
                multiline: true
            }
            Label {
                id: lblAuthors
                text: qsTr("Authors: Martin Woolley (@mdwrim) and John Murray (@jcmrim)")
            }
        }
        actions: [
            ActionItem {
                title: "Back"
                imageSource: "asset:///images/previous.png"
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
                        shareMe.data = "Check out the #BlackBerry10 'BlackBerryBleExplorer' " + "Sample App on Git Hub written by @jcmrim and @mdwrim " + "here http://bit.ly/SJBBKl";
                        shareHandler.confirm();
                    }
                }
            }
        ]

    }
}
