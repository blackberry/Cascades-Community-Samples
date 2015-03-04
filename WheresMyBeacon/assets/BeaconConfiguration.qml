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

import bb.cascades 1.2

Sheet {
    id: device_sheet
    property variant beacon_type
    property alias name: tf_name.text
    property alias mac: lbl_mac_value.text
    property variant uuid: tf_uuid.text
    property int major: 0
    property int minor: 0
    property int power: 0
    property int interval: 0
    property int pin: 0

    function setBeaconType(btype) {
        beacon_type = btype;
        var index = 2;
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
                            text: qsTr("Beacon Settings")
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
                                app.updateBeaconAttributes(tf_uuid.text, tf_major.text, tf_minor.text, tf_power.text, tf_interval.text, tf_pin.text);
                                close();
                            }
                            rightPadding: 10
                            topPadding: 15
                        }
                    }
                }
            }
        }

        ScrollView {
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
                Label {
                    id: lbl_uuid
                    text: "UUID"
                    textStyle.fontWeight: FontWeight.Bold
                }
                TextField {
                    id: tf_uuid
                    text: uuid
                    maximumLength: 36
                    validator: Validator {
                        id: tf_uuid_validator
                        mode: ValidationMode.Immediate
                        errorMessage: "UUID may contain hex characters, space and dash only"
                        onValidate: {
                            if (tf_uuid.text.match('^[\\-\\ 0-9.* A.* B.* C.* D.* E.* F.* ]+$')) {
                                tf_uuid_validator.state = ValidationState.Valid
                            } else {
                                tf_uuid_validator.state = ValidationState.Invalid
                            }
                        }
                    }
                }
                Label {
                    id: lbl_major
                    text: "Major"
                    textStyle.fontWeight: FontWeight.Bold
                }
                TextField {
                    id: tf_major
                    text: major
                    maximumLength: 5
                }
                Label {
                    id: lbl_minor
                    text: "Minor"
                    textStyle.fontWeight: FontWeight.Bold
                }
                TextField {
                    id: tf_minor
                    text: minor
                    maximumLength: 5
                }
                Label {
                    id: lbl_power
                    text: "Power"
                    textStyle.fontWeight: FontWeight.Bold
                }
                TextField {
                    id: tf_power
                    text: power
                    maximumLength: 3
                }
                Label {
                    id: lbl_interval
                    text: "Interval"
                    textStyle.fontWeight: FontWeight.Bold
                }
                TextField {
                    id: tf_interval
                    text: interval
                }
                Label {
                    id: lbl_pin
                    text: "PIN"
                    textStyle.fontWeight: FontWeight.Bold
                }
                TextField {
                    id: tf_pin
                    text: pin
                }
            }

        }
    }
}