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
