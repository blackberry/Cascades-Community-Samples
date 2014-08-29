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
        
        id: about
        property variant build_id: "0.0.3.4"
        
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
                            text: qsTr("About AnnualLeave")
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
            Style {
                id: style
            }
            Label {
                id: lblHeading_version
                text: qsTr("Beta 0.0.3 Build "+about.build_id)
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
                        text: qsTr("The AnnualLeave application is a customisable annual leave / vacation request application, designed to be adaptable to any back end enterprise system with which it needs to be integrated.")
                        multiline: true
                    }

                    Label {
                        id: lblAuthors
                        textStyle.fontWeight: style.about_label_style
                        text: qsTr("Authors")
                    }
                    Label {
                        id: lblAuthor_name1
                        text: qsTr("Martin Woolley (@mdwrim)")

                    }
                    Label {
                        id: lblAuthor_name2
                        text: qsTr("John Murray (@jcmrim)")

                    }
                    Label {
                        id: lbl_adapter_name
                        text: qsTr("Adapter Type")
                        textStyle.fontWeight: style.about_label_style
                    }
                    Label {
                        id: lbl_adapter_name_value
                        text: app.adapter_name
                    }
                    Label {
                        id: lbl_adapter_version
                        text: qsTr("Adapter Version")
                        textStyle.fontWeight: style.about_label_style
                    }
                    Label {
                        id: lbl_adapter_version_value
                        text: app.adapter_version
                    }
                    Label {
                        id: lbl_adapter_desc
                        text: qsTr("Adapter Description")
                        textStyle.fontWeight: style.about_label_style
                    }
                    Label {
                        id: lbl_adapter_desc_value
                        text: app.adapter_description
                        multiline: true
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
                        shareMe.data = "Check out the BlackBerry 10 'AnnualLeave' " + "application written by @jcmrim and @mdwrim " + "here http://bit.ly/SJBBKl";
                        shareHandler.confirm();
                    }
                }
            }
        ]

    }
}
