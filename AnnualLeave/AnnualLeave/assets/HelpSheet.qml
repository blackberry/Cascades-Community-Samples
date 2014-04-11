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

import bb.cascades 1.0

Sheet {
    id: root
    onOpened: {
        animation.play();
    }
    Page {
        actionBarVisibility: ChromeVisibility.Visible

        titleBar: TitleBar {
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
//                        background: Color.Green
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
//                        background: Color.Red
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 60
                        }
                        Label {
                            text: qsTr("Help")
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontWeight: style.heading_weight
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
//                        background: Color.Green
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                        CustomButton {
                            id: title_btn2
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            text: "Icons"
                            onClicked: {
                                icons.open()
                            }
                            rightPadding: 10
                            topPadding: 15
                        }
                    }
                }
            }
        }

        Container {
            layout: StackLayout {
            }
            leftPadding: 20
            rightPadding: 20
            verticalAlignment: VerticalAlignment.Fill
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
                                fromAngleZ: 0
                                toAngleZ: 360
                                duration: 1000
                            }
                        ]
                    }
                ]
            }

            ScrollView {

                Container {

                    Style {
                        id: style
                    }

                    Label {
                        id: lbl_help0
                        text: qsTr("Introduction")
                        multiline: true
                        textStyle {
                            fontWeight: style.help_label_style
                        }
                    }

                    Label {
                        id: lbl_help1
                        text: qsTr("The AnnualLeave application allows employees of a company to request time off from work. Requests are expected to be processed by a backend system with which this application has been integrated. For further information on how to approach integration, discuss with BlackBerry.")
                        multiline: true
                    }

                    Label {
                        id: lbl_help2
                        text: qsTr("There are three tabs available from the main screen:")
                        multiline: true
                    }

                    Label {
                        id: lbl_help3
                        text: qsTr("1. Requests")
                        multiline: true
                        textStyle {
                            fontWeight: style.help_label_style
                        }
                    }
                    Label {
                        id: lbl_help4
                        text: qsTr("This tab lists requests for time off that you have previously submitted, together with their current status.")
                        multiline: true
                    }

                    Label {
                        id: lbl_help5
                        text: qsTr("2. New")
                        multiline: true
                        textStyle {
                            fontWeight: style.help_label_style
                        }
                    }
                    Label {
                        id: lbl_help6
                        text: qsTr("From the New tab, you can formulate and submit a request for time off work. You need to indicate a start date and an end date and whether or not half days are involved (and if so, how). Select Submit and the request will find its way to the appropriate manager for approval.")
                        multiline: true
                    }

                    Label {
                        id: lbl_help7
                        text: qsTr("3. Inbox")
                        multiline: true
                        textStyle {
                            fontWeight: style.help_label_style
                        }
                    }
                    Label {
                        id: lbl_help8
                        text: qsTr("If you're a manager, you will occassionally receive 'Approval Tasks' in your Inbox. These are requests for time off work from employees you are responsioble for. To deal with an Approval Task, simply open it, select Approved or Rejected and submit your decision. Your decision will be communicated back to the employee who made the request.")
                        multiline: true
                    }
                }
            }
        }

        attachedObjects: [
            IconsSheet {
                id: icons
            }
        ]
    }
}
