/* Copyright (c) 2012 Research In Motion Limited.
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
*/import bb.cascades 1.0

Page {
    actions: [
        ActionItem {
            title: "Invoke (best-fit)"
            imageSource: "asset:///images/icon.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                if (qddAction.selectedIndex != 8) {
                    _app.invoke(qddInvokeType.selectedIndex, qddAction.at(qddAction.selectedIndex).text, qtaMimeType.text, qtaURI.text, qtaData.text, qtaTargetId.text)
                } else {
                    _app.invoke(qddInvokeType.selectedIndex, qtaAction.text, qtaMimeType.text, qtaURI.text, qtaData.text, qtaTargetId.text)
                }
            }
        },
        ActionItem {
            title: "Query"
            imageSource: "asset:///images/query.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                if (qddAction.selectedIndex != 5) {
                    _app.query(qddInvokeType.selectedIndex, qddAction.at(qddAction.selectedIndex).text, qtaMimeType.text, qtaURI.text)
                } else {
                    _app.query(qddInvokeType.selectedIndex, qtaAction.text, qtaMimeType.text, qtaURI.text)
                }
                qsQuery.visible = true
            }
        },
        InvokeActionItem {
            query {
                mimeType: "image/png"
                invokeActionId: "bb.action.OPEN"
            }
        },
        InvokeActionItem {
            title: "InvokeTarget1"
            query {
                mimeType: "image/png"
                invokeTargetId: "com.example.InvokeTarget1"
            }
        },
        InvokeActionItem {
            title: "InvokeTarget2"
            query {
                mimeType: "image/png"
                invokeTargetId: "com.example.InvokeTarget2"
            }
        }
    ]
    Container {
        layout: DockLayout {
        }
        ImageView {
            imageSource: "asset:///images/background.png"
        }
        ScrollView {
            scrollViewProperties {
                scrollMode: ScrollMode.Vertical
            }
            Container {
                layout: StackLayout {
                    topPadding: 50
                    leftPadding: 50
                    rightPadding: 50
                }
                id: qcSetup
                DropDown {
                    id: qddInvokeType
                    objectName: "qddInvokeType"
                    title: "Invocation Type:"
                    Option {
                        selected: true
                        text: "All"
                        description: "All types of invocation targets."
                    }
                    Option {
                        text: "Application"
                        description: "Targets that launch in a new window."
                    }
                    Option {
                        text: "Viewer"
                        description: "Targets that launch embedded."
                    }
                    Option {
                        text: "Service"
                        description: "Targets that launch in background."
                    }
                }
                Container {
                    clipContentToBounds: false
                    preferredWidth: maxWidth
                    layout: DockLayout {
                    }
                    ImageView {
                        layoutProperties: DockLayoutProperties {
                            verticalAlignment: VerticalAlignment.Top
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        imageSource: "asset:///images/nolight.png"
                    }
                    ImageView {
                        layoutProperties: DockLayoutProperties {
                            verticalAlignment: VerticalAlignment.Top
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        imageSource: "asset:///images/light.png"
                        animations: [
                            FadeTransition {
                                id: fadeout
                                fromOpacity: 1
                                toOpacity: 0
                                duration: 300
                                onEnded: {
                                    fadein.play();
                                }
                            },
                            FadeTransition {
                                id: fadein
                                duration: fadeout.duration
                                fromOpacity: 0
                                toOpacity: 1
                                easingCurve: StockCurve.BounceInOut
                                onEnded: {
                                    fadeout.play();
                                }
                            }
                        ]
                        onCreationCompleted: {
                            fadeout.play();
                        }
                    }
                }
                DropDown {
                    id: qddAction
                    objectName: "qddAction"
                    title: "Action:"
                    Option {
                        selected: true
                        text: "All"
                        description: "Valid for queries only."
                    }
                    Option {
                        selected: true
                        text: "Menu Actions"
                        description: "Valid for queries only."
                    }
                    Option {
                        selected: true
                        text: "bb.action.OPEN"
                        description: "A menu action for opening content."
                    }
                    Option {
                        text: "bb.action.SET"
                        description: "A menu action for setting content as"
                    }
                    Option {
                        text: "bb.action.SHARE"
                        description: "A menu action for sharing content."
                    }
                    Option {
                        text: "Custom"
                        description: "Specify a custom action."
                        onSelectedChanged: {
                            if (selected) {
                                qtaAction.visible = true
                            } else {
                                qtaAction.visible = false
                            }
                        }
                    }
                }
                TextArea {
                    id: qtaAction
                    objectName: "qtaAction"
                    visible: false
                    hintText: "[Custom Action]"
                }
                TextArea {
                    id: qtaMimeType
                    objectName: "qtaMimeType"
                    hintText: "[MIME Type]"
                    text: "image/png"
                }
                TextArea {
                    id: qtaURI
                    objectName: "qtaURI"
                    hintText: "[URI]"
                }
                TextArea {
                    id: qtaData
                    objectName: "qtaData"
                    hintText: "[Data]"
                    text: "Test data.."
                }
                TextArea {
                    id: qtaTargetId
                    objectName: "qtaTargetId"
                    hintText: "[Target ID - for Bound Invocation]"
                }
            }
        }
        attachedObjects: [
            Sheet {
                id: qsQuery
                content: Page {
                    Container {
                        layout: DockLayout {
                        }
                        ImageView {
                            imageSource: "asset:///images/background.png"
                        }
                        Container {
                            Container {
                                layout: StackLayout {
                                    topPadding: 50
                                    leftPadding: 50
                                    rightPadding: 50
                                }
                                Button {
                                    layoutProperties: StackLayoutProperties {
                                        horizontalAlignment: HorizontalAlignment.Center
                                    }
                                    text: "Close"
                                    onClicked: qsQuery.visible = false
                                }
                                Divider {
                                }
                            }
                            Container {
                                preferredWidth: maxWidth
                                layout: StackLayout {
                                    topPadding: 50
                                    leftPadding: 50
                                    rightPadding: 50
                                }
                                objectName: "qcQueryResults"
                            }
                        }
                    }
                }
            },
            CustomDialog {
                id: qcdDialog
                objectName: "qcdDialog"
                Container {
                    background: Color.Red
                    preferredWidth: maxWidth
                    clipContentToBounds: false
                    layout: StackLayout {
                    }
                    Label {
                        layoutProperties: StackLayoutProperties {
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        id: qlDialogMessage
                        objectName: "qlDialogMessage"
                    }
                    Button {
                        layoutProperties: StackLayoutProperties {
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        text: "OK"
                        onClicked: qcdDialog.visible = false
                    }
                }
            }
        ]
    }
}
