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
                            text: qsTr("Icons")
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontWeight: style.heading_weight
                        }
                    }
                    Container {
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
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

            ScrollView {

                Container {

                    Container {
                        leftPadding: 22
                        Label {
                            id: lbl1
                            text: qsTr("Holiday Booking Requests")
                            multiline: true
                            textStyle {
                                fontWeight: style.help_label_style
                            }
                        }
                    }

                    IconHelp {
                        image_file: "asset:///images/waiting.png"
                        icon_text: "Your holiday booking request was created and is on your device waiting to be transmitted"
                    }

                    IconHelp {
                        image_file: "asset:///images/manager.png"
                        icon_text: "Your holiday booking request has been transmitted and is awaiting manager approval"
                    }

                    IconHelp {
                        image_file: "asset:///images/approved.png"
                        icon_text: "Your holiday booking request has been approved by your manager"
                    }

                    IconHelp {
                        image_file: "asset:///images/denied.png"
                        icon_text: "Your holiday booking request has been rejected by your manager"
                    }

                    Container {
                        leftPadding: 22
                        Label {
                            id: lbl2
                            text: qsTr("Cancellations")
                            multiline: true
                            textStyle {
                                fontWeight: style.help_label_style
                            }
                        }
                    }

                    IconHelp {
                        image_file: "asset:///images/cancellation_pending.png"
                        icon_text: "Your cancellation request was created and is on your device waiting to be transmitted"
                    }

                    IconHelp {
                        image_file: "asset:///images/cancellation_submitted.png"
                        icon_text: "Your cancellation request has been transmitted and is awaiting manager approval"
                    }

                    IconHelp {
                        image_file: "asset:///images/cancelled.png"
                        icon_text: "Your cancellation request has been approved by your manager"
                    }

                    IconHelp {
                        image_file: "asset:///images/cancellation_rejected.png"
                        icon_text: "Your cancellation request has been rejected by your manager"
                    }

                    Container {
                        leftPadding: 22
                        Label {
                            id: lbl3
                            text: qsTr("Updates")
                            multiline: true
                            textStyle {
                                fontWeight: style.help_label_style
                            }
                        }
                    }

                    IconHelp {
                        image_file: "asset:///images/update_pending.png"
                        icon_text: "Your update request was created and is on your device waiting to be transmitted"
                    }

                    IconHelp {
                        image_file: "asset:///images/update_submitted.png"
                        icon_text: "Your update request has been transmitted and is awaiting manager approval"
                    }

                    IconHelp {
                        image_file: "asset:///images/updated.png"
                        icon_text: "Your update request has been approved by your manager"
                    }

                    IconHelp {
                        image_file: "asset:///images/update_rejected.png"
                        icon_text: "Your update request has been rejected by your manager"
                    }

                }
            }
        }
    }
}
