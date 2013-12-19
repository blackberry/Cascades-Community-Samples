/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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
import com.robwilliamsjnr.holidaytrain 1.0

Page {
    property BluetoothManager bluetoothManager
    
    titleBar: TitleBar {
        title: "Bluetooth Devices"
    }
    actions: [
        ActionItem {
            title: "Refresh"
            onTriggered: {
                bluetoothManager.startInquiry();
            }
            enabled: ! bluetoothManager.inquiryInProgress
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/refresh.png"
        }
    ]
    Container {
        ListView {
            function itemType(data, indexPath) {
                return data["type"];
            }

            function pair(data) {
                bluetoothManager.pair(data.address);
            }
            function connectDevice(data) {
                bluetoothManager.connectDevice(data.address);
            }
            function closeDevice() {
                if (bluetoothManager.connection != null) {
                    bluetoothManager.connection.close();
                }
            }

            dataModel: bluetoothManager.devices

            listItemComponents: [
                ListItemComponent {
                    type: "device"
                    content: Container {
                        id: listItemContainer
                        Label {
                            text: ListItemData.name
                            textStyle.fontSize: FontSize.XLarge

                        }

                        Container {
                            property bool connected: ListItemData.connected
                            property bool paired: ! ListItemData.connected && ListItemData.paired
                            property bool nothing: ! connected && ! paired
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 1
                                }
                                text: ListItemData.address
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Left
                            }
                            Label {
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 1
                                }
                                text: ListItemData.connected ? "Connected" : (ListItemData.paired ? "Paired" : "")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Left
                                textStyle.textAlign: TextAlign.Right
                            }
                        }
                        Divider {
                        }
                        contextActions: [
                            ActionSet {
                                title: "Bluetooth"
                                ActionItem {
                                    title: "Pair"
                                    enabled: ! ListItemData.paired
                                    onTriggered: {
                                        listItemContainer.ListItem.view.pair(ListItemData);
                                    }
                                    imageSource: "asset:///images/pair.png"
                                }
                                ActionItem {
                                    title: "Connect"
                                    enabled: ListItemData.paired && ! ListItemData.connected
                                    onTriggered: {
                                        listItemContainer.ListItem.view.connectDevice(ListItemData);
                                    }
                                    imageSource: "asset:///images/connect.png"
                                }
                                ActionItem {
                                    title: "Close"
                                    enabled: ListItemData.connected
                                    onTriggered: {
                                        listItemContainer.ListItem.view.closeDevice();
                                    }
                                    imageSource: "asset:///images/disconnect.png"
                                }
                            }
                        ]
                    }
                }
            ]

        }
    }
}
