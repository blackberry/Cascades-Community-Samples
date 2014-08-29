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
    id: main_pane
    objectName: "main"
    showTabsOnActionBar: true
    
    function simStateName(state) {
        switch (state) {
            case 0:
                return "Not Detected";
                break;
            case 1:
                return "Incompatible";
                break;
            case 2:
                return "Not Provisioned";
                break;
            case 3:
                return "Read Error";
                break;
            case 4:
                return "PIN Required";
                break;
            case 5:
                return "Ready";
                break;
            default:
                return "Invalid State Value";
        }
    }

    function hdmiConnectorType(hdmi) {
        switch (hdmi) {
            case 0:
                return "Unknown";
                break;
            case 1:
                return "None";
                break;
            case 2:
                return "Micro";
                break;
            default:
                return "Invalid HdmiConnector Value";
        }
    }

    function batteryChargingStateName(state) {
        switch (state) {
            case 0:
                return "Unknown";
                break;
            case 1:
                return "Not Charging";
                break;
            case 2:
                return "Charging";
                break;
            case 3:
                return "Discharging";
                break;
            case 4:
                return "Full";
                break;
            default:
                return "Invalid Value";
        }
    }

    function batteryConditionName(state) {
        switch (state) {
            case 0:
                return "Unknown";
                break;
            case 1:
                return "OK";
                break;
            case 2:
                return "Bad";
                break;
            default:
                return "Invalid Value";
        }
    }

    function aspectTypeName(state) {
        switch (state) {
            case 0:
                return "Landscape";
                break;
            case 1:
                return "Portrait";
                break;
            case 2:
                return "Square";
                break;
            default:
                return "Invalid Value";
        }
    }

    function displayTechName(tech) {
        switch (tech) {
            case 0:
                return "Unknown";
                break;
            case 1:
                return "LCD";
                break;
            case 2:
                return "OLED";
                break;
            case 3:
                return "CRT";
                break;
            case 4:
                return "Plasma";
                break;
            case 5:
                return "LED";
                break;
            default:
                return "Invalid Value";
        }
    }


    Tab {
        id: device_tab
        title: "Hardware"
        imageSource: "asset:///images/device.png"
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    id: lblHeading_device
                    textStyle {
                        base: SystemDefaults.TextStyles.TitleText
                    }
                    text: qsTr("Device Info")
                    verticalAlignment: VerticalAlignment.Top
                    horizontalAlignment: HorizontalAlignment.Center
                }
                ScrollView {
                    id: deviceScroller
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    Container {
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_device_name
                                text: qsTr("Device Name")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_device_name
                                text: hw.deviceName
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_hw_id
                                text: qsTr("Hardware ID")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_hw_id
                                text: hw.hardwareId
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_hdmi
                                text: qsTr("HDMI")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_hdmi
                                text: hdmiConnectorType(hw.hdmiConnector)
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_imei
                                text: qsTr("IMEI")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_imei
                                text: hw.imei
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_kb
                                text: qsTr("Physical Keyboard?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_kb
                                text: hw.isPhysicalKeyboardDevice
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_meid
                                text: qsTr("MEID")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_meid
                                text: hw.meid
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_model
                                text: qsTr("Model Name")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_model
                                text: hw.modelName
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_model_num
                                text: qsTr("Model No.")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_model_num
                                text: hw.modelNumber
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_pin
                                text: qsTr("PIN")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_pin
                                text: hw.pin
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_serial_num
                                text: qsTr("Serial Number")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_serial_num
                                text: hw.serialNumber
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                multiline: true
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_processors
                                text: qsTr("Processors")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_processors
                                text: app.getProcessorCount()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_processor_model
                                text: qsTr("Processor Model")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_processor_model
                                text: app.getProcessorModels()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                multiline: true
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_processor_speed
                                text: qsTr("Processor Speed")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_processor_speed
                                text: app.getProcessorSpeeds()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                multiline: true
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Tab {
        id: sim_tab
        imageSource: "asset:///images/sim_card.png"
        title: "SIM"
        Page {
            Container {
                layout: StackLayout {
                }
                Label {
                    text: qsTr("SIM Card Info")
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                ScrollView {
                    id: simScroller
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    Container {

                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_mcc
                                text: qsTr("MCC")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 40
                                }
                            }
                            Label {
                                id: val_mcc
                                text: sim.mobileCountryCode
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 60
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_mnc
                                text: qsTr("MNC")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 40
                                }
                            }
                            Label {
                                id: val_mnc
                                text: sim.mobileNetworkCode
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 60
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_sno
                                text: qsTr("Serial Number")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 40
                                }
                            }
                            Label {
                                id: val_sno
                                text: sim.serialNumber
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 60
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_state
                                text: qsTr("State")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 40
                                }
                            }
                            Label {
                                id: val_state
                                text: simStateName(sim.state)
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 60
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Tab {
        id: battery_tab
        imageSource: "asset:///images/battery.png"
        title: "Battery"
        Page {
            Container {
                layout: StackLayout {
                }
                Label {
                    text: qsTr("Battery Info")
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                ScrollView {
                    id: batteryScroller
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    Container {
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_charging_state
                                text: qsTr("Charging State")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_charging_state
                                text: batteryChargingStateName(bat.chargingState)
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_battery_condition
                                text: qsTr("Condition")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_battery_condition
                                text: batteryConditionName(bat.condition)
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_cycle_count
                                text: qsTr("Cycle Count")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_cycle_count
                                text: bat.cycleCount
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_full_charge_capacity
                                text: qsTr("Full Charge Capacity")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_full_charge_capacity
                                text: bat.fullChargeCapacity
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_level
                                text: qsTr("Level")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_level
                                text: bat.level
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_present
                                text: qsTr("Present?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_present
                                text: bat.present
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_temperature
                                text: qsTr("Temperature")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_temperature
                                text: bat.temperature
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Tab {
        id: display_tab
        imageSource: "asset:///images/display.png"
        title: "Display"
        Page {
            Container {
                layout: StackLayout {
                }
                Label {
                    text: qsTr("Display Info")
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                ScrollView {
                    id: displayScroller
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    Container {
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_aspect_type
                                text: qsTr("Aspect Type")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_aspect_type
                                text: aspectTypeName(disp.aspectType)
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_attached
                                text: qsTr("Attached")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_attached
                                text: disp.attached
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_detachable
                                text: qsTr("Detachable")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_detachable
                                text: disp.detachable
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_display_id
                                text: qsTr("Display ID")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_display_id
                                text: disp.displayId
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_display_name
                                text: qsTr("Display Name")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_display_name
                                text: disp.displayName
                                multiline: true
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_display_tech
                                text: qsTr("Display Tech")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_display_tech
                                text: displayTechName(disp.displayTechnology)
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_width_mm
                                text: qsTr("Width (mm)")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_width_mm
                                text: app.getDisplayPhysicalWidth()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_height_mm
                                text: qsTr("Height (mm)")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_height_mm
                                text: app.getDisplayPhysicalHeight()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_width_pixels
                                text: qsTr("Width (pixels)")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_width_pixels
                                text: app.getDisplayPixelWidth()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_height_pixels
                                text: qsTr("Height (pixels)")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_height_pixels
                                text: app.getDisplayPixelHeight()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_width_resolution
                                text: qsTr("Res. Width (pixels/m)")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_width_resolution
                                text: app.getDisplayResolutionWidth()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_height_resolution
                                text: qsTr("Res. Height (pixels/m)")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_height_resolution
                                text: app.getDisplayResolutionHeight()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_wireless
                                text: qsTr("Wireless connected?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_wireless
                                text: disp.wireless
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Tab {
        id: network_tab
        imageSource: "asset:///images/wireless.png"
        title: "Network"
        Page {
            Container {
                layout: StackLayout {
                }
                Label {
                    text: qsTr("Network Info")
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                ScrollView {
                    id: networkScroller
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    Container {
                        Container {
                            DropDown {
                                id: networkInterfaces
                                objectName: "networkInterfaces"
                                title: "Network Interfaces"
                                enabled: true
                                
                                onSelectedIndexChanged: {
                                    console.log("SelectedIndex was changed to " + selectedOption.text);
                                    console.log("QQQQ i/f type="+app.selected_if_type);
                                    app.selectNetworkInterfaceByName(selectedOption.text);
                                    val_net_type = app.selected_if_type
                                    val_hw_address.text = app.selected_link_addr
                                    val_net_addresses.text = app.selected_ip_addresses
                                    val_ip_status.text = app.selected_ip_status
                                    val_is_up.text = app.selected_if_is_up
                                    val_is_running.text = app.selected_if_is_running
                                    val_can_broadcast.text = app.selected_if_can_broadcast
                                    val_is_loopback.text = app.selected_if_is_loopback
                                    val_is_point_to_point.text = app.selected_if_is_p2p
                                    val_can_multicast = app.selected_if_can_multicast
                                    val_is_up.text = app.selected_if_is_up
                                }

                            }
                            background: Color.Gray
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_net_type
                                text: qsTr("Type")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_net_type
                                text: app.selected_if_type
                                multiline: true
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_hw_addr
                                text: qsTr("Hardware Address")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_hw_address
                                text: app.selected_link_addr
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_net_addresses
                                text: qsTr("IP Address(es)")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_net_addresses
                                text: app.selected_ip_addresses
                                multiline: true
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_ip_status
                                text: qsTr("IP Status")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_ip_status
                                text: app.selected_ip_status
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_is_up
                                text: qsTr("Is up?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_is_up
                                text: app.selected_if_is_up
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_is_running
                                text: qsTr("Is running?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_is_running
                                text: app.selected_if_is_running
                                multiline: true
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_can_broadcast
                                text: qsTr("Can broadcast?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_can_broadcast
                                text: app.selected_if_can_broadcast
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_is_loopback
                                text: qsTr("Is loopback?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_is_loopback
                                text: app.selected_if_is_loopback
                                multiline: true
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_is_point_to_point
                                text: qsTr("Point to point?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_is_point_to_point
                                text: app.selected_if_is_p2p
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_can_multicast
                                text: qsTr("Can multicast?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_can_multicast
                                text: app.selected_if_can_multicast
                                multiline: true
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Tab {
        id: phone_tab
        imageSource: "asset:///images/phone.png"
        title: "Phone"
        Page {
            Container {
                layout: StackLayout {
                }
                Label {
                    text: qsTr("Phone Info")
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                ScrollView {
                    id: phoneScroller
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    Container {
                        Container {
                            DropDown {
                                id: phoneLines
                                objectName: "phoneLines"
                                title: "Phone Lines"
                                enabled: true

                                onSelectedIndexChanged: {
                                    console.log("SelectedIndex was changed to " + selectedOption.text);
                                    app.selectLineById(selectedOption.text);
                                    val_phone_line_type.text = app.getLineType()
                                    val_phone_address.text = app.getLineAddress();
                                    val_phone_description.text = app.getLineDescription()
                                    val_phone_isvalid = app.isLineValid();
                                }

                            }
                            background: Color.Gray
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_phone_ine_type
                                text: qsTr("Line Type")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_phone_line_type
                                text: app.getLineType()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_phone_address
                                text: qsTr("Address")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_phone_address
                                text: app.getLineAddress()
                                multiline: true
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_phone_description
                                text: qsTr("Description")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_phone_description
                                text: app.getLineDescription()
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            background: Color.Gray
                            Label {
                                id: lbl_phone_isvalid
                                text: qsTr("Valid?")
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 45
                                }
                            }
                            Label {
                                id: val_phone_isvalid
                                text: app.isLineValid()
                                multiline: true
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 55
                                }
                            }
                        }
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
                        text: qsTr("About MyDevice")
                        textStyle.base: SystemDefaults.TextStyles.TitleText
                        textStyle.textAlign: TextAlign.Center
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblHeading_version
                        text: qsTr("V1.1.0")
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblBody_about1
                        text: qsTr("Demonstrates info available from various classes such as SimCardInfo, HardwareInfo and Phone and Line")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblAuthors
                        text: qsTr("Author:")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
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
