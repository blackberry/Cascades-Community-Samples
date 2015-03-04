/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

import bb.cascades 1.2

Sheet {

    id: beacons

    property int scan_state_stopped: 1
    property int scan_state_started: 2

    signal startScanning()
    
    function openConfig(mac) {
        console.log("QQQQ BeaconList.openConfig:"+mac)
        beacon_config.mac = mac
        beacon_config.setBeaconType(app.getBeaconType(mac));
        beacon_config.name = app.getBeaconName(mac);
        beacon_config.uuid = app.getBeaconUuid().substring(0,8)+"-"+app.getBeaconUuid().substring(8,12)+"-"+app.getBeaconUuid().substring(12,16)+"-"+app.getBeaconUuid().substring(16,20)+"-"+app.getBeaconUuid().substring(20,32);
        console.log("QQQQ calling getBeaconMajor");
        beacon_config.major = app.getBeaconMajor();
        console.log("QQQQ calling getBeaconMinor");
        beacon_config.minor = app.getBeaconMinor();
        console.log("QQQQ done calling getBeaconMinor");
        beacon_config.power = app.getBeaconPower();
        beacon_config.interval = app.getBeaconInterval();
        beacon_config.pin = app.getBeaconPin();
        beacon_config.open();
    }

    attachedObjects: [
        BeaconConfiguration {
            id: beacon_config
        }
    ]


    Page {
        
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
                            text: qsTr("Configurable Beacons")
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

        Container {

            layout: StackLayout {
            }

            topPadding: 10
            leftPadding: 30
            rightPadding: 30

            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Button {
                    id: startScanningButton
                    text: "Scan"
                    enabled: app.scan_state == scan_state_stopped
                    horizontalAlignment: HorizontalAlignment.Center
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 50
                    }
                    onClicked: {
                        beacons.startScanning();
                    }
                }

            }

            Container {
                topPadding: 20
                leftPadding: 20
                rightPadding: 20
                bottomPadding: 20

                ListView {
                    id: beacon_list
                    objectName: "beacon_list"

                    dataModel: app.config_model

                    listItemComponents: [
                        ListItemComponent {
                            type: "listItem"
                            CustomListItem {
                                dividerVisible: true
                                highlightAppearance: HighlightAppearance.Frame
                                Container {
                                    Label {
                                        text: "NAME: " + app.getBeaconName(ListItemData.MAC)
                                        textStyle.fontWeight: FontWeight.Bold
                                        visible: (app.getBeaconName(ListItemData.MAC) != "" ? true : false)
                                    }
                                    Label {
                                        text: "MAC: " + ListItemData.MAC
                                        textStyle.fontWeight: FontWeight.Bold
                                    }
                                    Label {
                                        text: "Type: " + app.getBeaconType(ListItemData.MAC)
                                        visible: (app.getBeaconType(ListItemData.MAC) != "" ? true : false)
                                    }
                                    Divider {

                                    }
                                    onTouch: {
                                        if (event.isUp()) {
                                            app.readBeaconCharacteristics(ListItemData.MAC);
                                        }
                                    }
                                }

                            }
                        }
                    ]

                    function itemType(data, indexPath) {
                        if (indexPath.length == 1) {
                            return "header";
                        } else {
                            return "listItem";
                        }
                    }
                    accessibility.name: "TODO: Add property content"
                }
            }
        }
    }
}