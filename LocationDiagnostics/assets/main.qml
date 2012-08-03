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

NavigationPane {
    id: navPane
    Page {
        actions: [
            ActionItem {
                title: "Start Updates"
                imageSource: "asset:///images/track.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    _locationDiagnostics.startLocationSession(true)
                }
            },
            ActionItem {
                title: "Request Update"
                imageSource: "asset:///images/pin.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    _locationDiagnostics.startLocationSession(false)
                }
            }
        ]
        Container {
            layout: DockLayout {
            }
            ImageView {
                imageSource: "asset:///images/Background.png"
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
                    layoutProperties: StackLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Fill
                    }
                    DropDown {
                        id: qddPositionMethods
                        objectName: "qddPositionMethods"
                        title: "Positioning Method:"
                        Option {
                            id: qoAll
                            objectName: "qoAll"
                            text: "All"
                            description: "GPS, MS-Based, MS-Assisted, Cell or Wifi"
                            selected: _locationDiagnostics.getValueFor(qoAll.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoAll.objectName, selected)
                            }
                        }
                        Option {
                            id: qoNonSatellite
                            objectName: "qoNonSatellite"
                            text: "Non-Satellite"
                            description: "Cell or WiFi"
                            selected: _locationDiagnostics.getValueFor(qoNonSatellite.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoNonSatellite.objectName, selected)
                            }
                        }
                        Option {
                            id: qoSatellite
                            objectName: "qoSatellite"
                            text: "Satellite"
                            description: "GPS, MS-Based, MS-Assisted"
                            selected: _locationDiagnostics.getValueFor(qoSatellite.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoSatellite.objectName, selected)
                            }
                        }
                        Option {
                            id: qoGPS
                            objectName: "qoGPS"
                            text: "GPS"
                            description: "GPS Only"
                            selected: _locationDiagnostics.getValueFor(qoGPS.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoGPS.objectName, selected)
                            }
                        }
                        Option {
                            id: qoCell
                            objectName: "qoCell"
                            text: "Cellular"
                            description: "Cellular only"
                            selected: _locationDiagnostics.getValueFor(qoCell.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoCell.objectName, selected)
                            }
                        }
                        Option {
                            id: qoWiFi
                            objectName: "qoWiFi"
                            text: "WiFi"
                            description: "WiFi only"
                            selected: _locationDiagnostics.getValueFor(qoWiFi.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoWiFi.objectName, selected)
                            }
                        }
                        Option {
                            id: qoMSB
                            objectName: "qoMSB"
                            text: "MS-Based"
                            description: "MS-Based only"
                            selected: _locationDiagnostics.getValueFor(qoWiFi.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoWiFi.objectName, selected)
                            }
                        }
                        Option {
                            id: qoMSA
                            objectName: "qoMSA"
                            text: "MS-Assisted"
                            description: "MS-Assisted only"
                            selected: _locationDiagnostics.getValueFor(qoWiFi.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoWiFi.objectName, selected)
                            }
                        }
                    }
                    DropDown {
                        id: qddAssistance
                        objectName: "qddAssistance"
                        title: "Assistance:"
                        Option {
                            id: qoAssistNone
                            objectName: "qoAssistNone"
                            text: "None"
                            description: "No carrier assistance"
                            selected: _locationDiagnostics.getValueFor(qoAssistNone.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoAssistNone.objectName, selected)
                                if (selected) {
                                    qtaPDEURL.visible = false
                                    qtaSLPURL.visible = false
                                    qtaAppID.visible = false
                                    qtaAppPassword.visible = false
                                }
                            }
                        }
                        Option {
                            id: qoAssistPDE
                            objectName: "qoAssistPDE"
                            text: "PDE"
                            description: "PDE Server"
                            selected: _locationDiagnostics.getValueFor(qoAssistPDE.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoAssistPDE.objectName, selected)
                                if (selected) {
                                    qtaPDEURL.visible = true
                                    qtaSLPURL.visible = false
                                    qtaAppID.visible = false
                                    qtaAppPassword.visible = false
                                }
                            }
                        }
                        Option {
                            id: qoAssistSUPL
                            objectName: "qoAssistSUPL"
                            text: "SLP"
                            description: "SLP Server"
                            selected: _locationDiagnostics.getValueFor(qoAssistSUPL.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoAssistSUPL.objectName, selected)
                                if (selected) {
                                    qtaPDEURL.visible = false
                                    qtaSLPURL.visible = true
                                    qtaAppID.visible = false
                                    qtaAppPassword.visible = false
                                }
                            }
                        }
                        Option {
                            id: qoAssistPassword
                            objectName: "qoAssistPassword"
                            text: "App ID/Password"
                            description: "Assistance requiring ID/Password"
                            selected: _locationDiagnostics.getValueFor(qoAssistPassword.objectName, "yes")
                            onSelectedChanged: {
                                _locationDiagnostics.saveValueFor(qoAssistPassword.objectName, selected)
                                if (selected) {
                                    qtaPDEURL.visible = false
                                    qtaSLPURL.visible = false
                                    qtaAppID.visible = true
                                    qtaAppPassword.visible = true
                                }
                            }
                        }
                    }
                    TextArea {
                        id: qtaPDEURL
                        objectName: "qtaPDEURL"
                        visible: false
                        hintText: "PDE URL"
                    }
                    TextArea {
                        id: qtaSLPURL
                        objectName: "qtaSLPURL"
                        visible: false
                        hintText: "SUPL URL"
                    }
                    TextArea {
                        id: qtaAppID
                        objectName: "qtaAppID"
                        visible: false
                        hintText: "App ID"
                    }
                    TextArea {
                        id: qtaAppPassword
                        objectName: "qtaAppPassword"
                        visible: false
                        hintText: "Password"
                    }
                    Divider {
                    }
                    WarpDrive {
                        id: qwdfrequency
                        objectName: "qwdfrequency"
                        text: "Frequency"
                    }
                    ToggleLabelButton {
                        id: qtlbSound
                        text: "Sound"
                        objectName: "qtlbSound"
                        onCheckedChanged: {
                            _locationDiagnostics.saveValueFor(qtlbSound.objectName, checked)
                        }
                    }
                    ToggleLabelButton {
                        id: qtlbBGMode
                        text: "Background Mode"
                        objectName: "qtlbBGMode"
                        onCheckedChanged: {
                            _locationDiagnostics.saveValueFor(qtlbBGMode.objectName, checked)
                        }
                    }
                }
            }
        }
    }
}
