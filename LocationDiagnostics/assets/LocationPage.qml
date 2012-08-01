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
*/

import bb.cascades 1.0

Page {    
    Container {
        layout: DockLayout {
        }
        ImageView {
            imageSource: "asset:///images/Background.png"
        }
        Container {
            layout: DockLayout {
                topPadding: 100
            }
            ScrollView {
                translationY: 0
                id: qsvLocation
                scrollViewProperties {
                    scrollMode: ScrollMode.Vertical
                }
                Container {
                    layout: DockLayout {
                    }
                    Container {
                        layout: StackLayout {
                            topPadding: 50
                            leftPadding: 50
                            rightPadding: 50
                        }
                        LabelLabel {
                            objectName: "qllTime"
                            label: "Timestamp:"
                            text: "-/-/- -:-"
                        }
                        LabelLabel {
                            objectName: "qllMethod"
                            label: "Method:"
                            text: "--"
                        }
                        Divider {
                        }
                        LabelLabel {
                            objectName: "qllLatitude"
                            label: "Latitude:"
                            text: "--"
                        }
                        LabelLabel {
                            objectName: "qllLongitude"
                            label: "Longitude:"
                            text: "--"
                        }
                        LabelLabel {
                            objectName: "qllAltitude"
                            label: "Altitude:"
                            text: "--"
                        }
                        Divider {
                        }
                        LabelLabel {
                            objectName: "qllDirection"
                            label: "Direction:"
                            text: "--"
                        }
                        LabelLabel {
                            objectName: "qllGroundSpeed"
                            label: "Ground Speed:"
                            text: "-- km/h"
                        }
                        LabelLabel {
                            objectName: "qllVerticalSpeed"
                            label: "Vertical Speed:"
                            text: "-- km/h"
                        }
                        Divider {
                        }
                        LabelLabel {
                            objectName: "qllMagneticVariation"
                            label: "Magnetic Variation:"
                            text: "--"
                        }
                        LabelLabel {
                            objectName: "qllHorizontalAccuracy"
                            label: "Horizontal Accuracy:"
                            text: "--"
                        }
                        LabelLabel {
                            objectName: "qllVerticalAccuracy"
                            label: "Vertical Accuracy:"
                            text: "--"
                        }
                        Divider {
                        }
                        DropDown {
                            id: qddResetType
                            selectedIndex: 0
                            objectName: "qddResetType"
                            title: "Reset Mode:"
                            Option {                                
                                text: "Cold"
                            }
                            Option {                                
                                text: "Warm"
                            }
                            Option {
                                text: "Hot"
                            }
                        }
                        Button {
                            layoutProperties: StackLayoutProperties {
                                horizontalAlignment: HorizontalAlignment.Center
                            }                            
                            text: "Reset"
                            onClicked: {
                                if (qddResetType.selectedIndex == 0) {
                                    _engine.resetEngine("cold")
                                } else if (qddResetType.selectedIndex == 1) {
                                    _engine.resetEngine("warm")
                                } else if (qddResetType.selectedIndex == 2) {
                                    _engine.resetEngine("hot")
                                }
                            }
                        }
                    }
                }
            }
            ScrollView {
                translationY: 1280
                id: qsvSatellite
                scrollViewProperties {
                    scrollMode: ScrollMode.Vertical
                }
                Container {
                    layout: StackLayout {
                    }
                    TextArea {
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            fontWeight: FontWeight.Normal
                            fontFamily: "courier"
                        }
                        editable: false
                        backgroundVisible: true
                        objectName: "sats"
                        hintText: "Satellite Information"                        
                    }
                }
            }
            ScrollView {                                
                translationY: 1280
                id: qsvLog
                scrollViewProperties {                                       
                    scrollMode: ScrollMode.Vertical                    
                }
                Container {
                    layout: StackLayout {
                    }
                    TextArea {
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            fontWeight: FontWeight.Normal
                            fontFamily: "courier"
                        }
                        editable: false
                        backgroundVisible: true
                        objectName: "logs"
                        hintText: "Logs"
                    }
                }
            }
        }
        Container {
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Center
            }
            visible: true
            SegmentedControl {
                id: segments
                Option {
                    text: "Location"
                    selected: true
                }
                Option {
                    text: "Satellites"
                }
                Option {
                    text: "Logs"
                }
                onSelectedIndexChanged: {
                    var option = segments.at(selectedIndex);
                    if (option.text == "Location") {
                        qsvLocation.translationY = 0
                        qsvSatellite.translationY = 1280
                        qsvLog.translationY = 1280
                    } else if (option.text == "Satellites") {
                        qsvLocation.translationY = 1280
                        qsvSatellite.translationY = 0
                        qsvLog.translationY = 1280
                    } else if (option.text == "Logs") {
                        qsvLocation.translationY = 1280
                        qsvSatellite.translationY = 1280
                        qsvLog.translationY = 0
                    }
                }
            }
        }
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            title: "Back"
            onTriggered: {
                _engine.stopUpdates()
                appPage.popAndDelete()
            }
        }
    }
}
