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
    id: device_sheet
    property variant beacon_type
    property alias mac: lbl_mac_value.text
    property alias name: tf_name.text

	function setBeaconType(btype) {
	    beacon_type = btype;
	    var index=2;
	    if (btype == "Estimote") {
	        index = 0;
	    } else {
            if (btype == "SensorTag") {
                index = 1;
            }
	    }
	    dd_beacon_type.setSelectedIndex(index);
	}    

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
                            text: qsTr("Beacon Identity")
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
                        CustomButton {
                            id: title_btn2
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            text: "Save"
                            onClicked: {
                                console.log("Save clicked: mac="+mac+" name="+name+" beacon_type="+beacon_type);
                                app.save(mac, name, beacon_type);
                                close();
                            }
                            rightPadding: 10
                            topPadding: 15
                        }
                    }
                }
            }
        }

        Container {
            id: device
            leftPadding: 20
            rightPadding: 20
            Label {
                id: lbl_mac
                text: "MAC"
                textStyle.fontWeight: FontWeight.Bold
            }
            Label {
                id: lbl_mac_value
            }
            Label {
                id: lbl_name
                text: "Name"
                textStyle.fontWeight: FontWeight.Bold
            }
            TextField {
                id: tf_name
            }
            DropDown {
                id: dd_beacon_type
                title: "Beacon Type"
                Option {
                    text: "Estimote"
                    value: "Estimote"
                }
                Option {
                    text: "SensorTag"
                    value: "SensorTag"
                }
                Option {
                    text: "Other"
                    value: "Other"
                }
                onSelectedValueChanged: {
                    beacon_type = selectedValue;
                }
            }
        }

    }

}
