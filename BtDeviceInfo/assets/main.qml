/* Copyright (c) 2013 BlackBerry Limited.
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
import bb.multimedia 1.0

TabbedPane {
    id: tabbedPane
    objectName: "tabbed_pane"
    showTabsOnActionBar: true
    property string p_dev_addr
    property string p_dev_name

    function getInfo(device_addr, device_name) {
        console.log("XXXX getInfo called");
        _bt.setRemoteDevice(device_addr);
        _bdi.getInfo(device_addr, device_name);
        p_dev_addr = device_addr;
        p_dev_name = device_name;
        console.log("XXXX p_dev_addr=" + p_dev_addr + ",p_dev_name=" + p_dev_name);
    }

    function logMessage(message) {
        console.log("XXXX logMessage:"+message);
        log.text += (qsTr("\n") + message );
    }
    
    function infoMessage(message) {
        console.log("XXXX infoMessage:"+message);
        lbl_message.text = message;
    }

    Tab {
        id: info_tab
        imageSource: "asset:///images/bluetooth.png"
        title: "Device Info"
        Page {
            Container {
                layout: StackLayout {
                }
                Label {
                    text: qsTr("Bluetooth Device Info")
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: lbl_message
                    text: qsTr("")
                    textStyle.color: Color.Red
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                ScrollView {
                    id: logScroller
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    TextArea {
                        id: log
                        text: ""
                        editable: false
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                        }
                        content.flags: TextContentFlag.ActiveTextOff
                        onTextChanged: {
                            logScroller.scrollToPoint(0, Infinity);
                        }
                    }
                }
            }
        }
    }
    Tab {
        id: scan_tab
        title: "Scan"
        imageSource: "asset:///images/scan.png"
        onTriggered: {
            console.log("XXXX calling discover()");
            activityIndicator.start();
            _bt.deviceListing.discover();
            _bt.deviceListing.update();
            activityIndicator.stop();
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    id: lblHeading_scan
                    textStyle {
                        base: SystemDefaults.TextStyles.TitleText
                    }
                    text: qsTr("Bluetooth Devices")
                    verticalAlignment: VerticalAlignment.Top
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: lblSelect
                    text: "Please select a device"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        fontFamily: "Courier"
                        lineHeight: 1.1
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Container {
                    layout: DockLayout {
                    }
                    topPadding: 20
                    leftPadding: 20
                    rightPadding: 20
                    bottomPadding: 20
                    ListView {
                        dataModel: _bt.deviceListing.model
                        listItemComponents: [
                            ListItemComponent {
                                type: "listItem"
                                StandardListItem {
                                    title: ListItemData.deviceName
                                }
                            }
                        ]
                        onTriggered: {
                            var selectedItem = dataModel.data(indexPath);
                            getInfo(selectedItem.deviceAddress, selectedItem.deviceName);
                            tabbedPane.activeTab = info_tab;
                        }
                        function itemType(data, indexPath) {
                            if (indexPath.length == 1) {
                                return "header";
                            } else {
                                return "listItem";
                            }
                        }
                    }
                    ActivityIndicator {
                        id: activityIndicator
                        preferredWidth: 300
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }
    }
    Tab {
        id: about_tab
        title: "About"
        imageSource: "asset:///images/about.png"
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Center
                Container {
                    layout: StackLayout {
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    Label {
                        id: lblHeading_about
                        text: qsTr("About BtDeviceInfo")
                        textStyle.base: SystemDefaults.TextStyles.TitleText
                        textStyle.textAlign: TextAlign.Center
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblHeading_version
                        text: qsTr("V1.0.0")
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblBody_about1
                        text: qsTr("Works with the Bluetooth Device Info Profile")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblAuthors
                        text: qsTr("Authors:")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblAuthors1
                        text: qsTr("Martin Woolley (@mdwrim)")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblAuthors2
                        text: qsTr("John Murray (@jcmrim)")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                }
            }
        }
    }
}
