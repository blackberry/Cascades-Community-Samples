/*
 * Copyright (c) 2014 BlackBerry Limited.
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

Sheet {
    id: root
    
    onOpened: {
        animation.play();
    }

    Page {
        
        id: about
        titleBar: TitleBar {
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                        CustomButton {
                            id: title_btn1
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            text: "Back"
                            onClicked: {
                                close();
                            }
                            leftPadding: 10
                            topPadding: 15
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Center
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 60
                        }
                        Label {
                            text: qsTr("About ")
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontWeight: FontWeight.W800
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                    }
                }
            }
        }

        actionBarVisibility: ChromeVisibility.Visible
        Container {
            layout: StackLayout {
            }
            leftPadding: 20
            rightPadding: 20
            verticalAlignment: VerticalAlignment.Fill
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

            ScrollView {

                Container {

                    Label {
                        id: taBody_about1
                        horizontalAlignment: HorizontalAlignment.Center
                        text: qsTr("The GattServerRandom sample application illustrates how to create a custom Bluetooth Low Energy GATT server for BlackBerry 10.")
                        multiline: true
                    }
                    
                    Label {
                        id: taBody_about2
                        horizontalAlignment: HorizontalAlignment.Center
                        text: qsTr("Write a value of 1 to service handle 3 and the GattServerRandom will start sending notifications containing a random byte value in the range 0-255 at 1 second intervals.")
                        multiline: true
                    }
                    
                    Label {
                        id: taBody_about3
                        horizontalAlignment: HorizontalAlignment.Center
                        text: qsTr("Write a value of 0 to service handle 3 and the GattServerRandom will stop sending notifications.")
                        multiline: true
                    }

                    Label {
                        id: lblAuthors
                        textStyle.fontWeight: FontWeight.Bold
                        text: qsTr("Author")
                    }
                    Label {
                        id: lblAuthor_name2
                        text: qsTr("John Murray (@jcmrim)")

                    }
                }
            }
        }
        actions: [
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
                        shareMe.data = "Check out the BlackBerry 10 'GattServerRandom' " + "application written by @jcmrim  " + "here http://bit.ly/SJBBKl";
                        shareHandler.confirm();
                    }
                }
            }
        ]

    }
}
