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
import "common"

Page {
    resizeBehavior: PageResizeBehavior.None
    content: MenuContainer {
        id: apdu
        objectName: "apdu_details"
        onCreationCompleted: {
            console.log("XXXX apdu.qml created: _apdu._target_inx=" + _apdu._target_inx);
        }
        signal apduRequested()
        Container {
            layout: StackLayout {
            }
            Label {
                text: "ISO7816-4 APDU"
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                }
            }
            Label {
                text: "Enter hex values only"
                textStyle {
                    base: SystemDefaults.TextStyles.SubtitleText
                    fontWeight: FontWeight.W100
                    color: Color.White
                }
            }
            DropDown {
                id: ddn_target
                objectName: "ddn_target"
                title: "Target"
                enabled: true
                selectedIndex: _apdu._target_inx
                Option {
                    text: "SIM SE"
                    description: "SIM Secure Element"
                    value: "SIM"
                    selected: true
                    onSelectedChanged: {
                    }
                }
                Option {
                    text: "Contactless Card"
                    description: "Physical, contactless card"
                    value: "CARD"
                }
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Label {
                    id: cbx_label1
                    text: "SELECT only"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.LightGray
                    }
                }
                CheckBox {
                    id: cbx_select_only
                    objectName: "cbx_select_only"
                    verticalAlignment: VerticalAlignment.Center
                    checked: _apdu._select_only
                    onCheckedChanged: {
                        txf_cla.enabled = ! checked;
                        txf_ins.enabled = ! checked;
                        txf_p1p2.enabled = ! checked;
                        txf_lc.enabled = ! checked;
                        txf_command.enabled = ! checked;
                        txf_le.enabled = ! checked;
                    }
                }
                Label {
                    id: cbx_label2
                    text: "PPSE"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.LightGray
                    }
                }
                CheckBox {
                    id: cbx_ppse
                    objectName: "cbx_ppse"
                    verticalAlignment: VerticalAlignment.Center
                    checked: _apdu._ppse
                    onCheckedChanged: {
                        if (checked) {
                            txf_aid.text = "325041592E5359532E444446303100"
                        } else {
                            txf_aid.text = ""
                        }
                        txf_aid.enabled = ! checked;
                    }
                }
            }
            Label {
                text: "AID:"
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    color: Color.LightGray
                }
            }
            TextArea {
                id: txf_aid
                objectName: "txf_aid"
                hintText: "Enter AID value"
                text: _apdu._aid
                enabled: ! cbx_ppse
                textStyle {
                    base: SystemDefaults.TextStyles.SmallText
                }
            }
            Label {
                text: "Header:"
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    color: Color.LightGray
                }
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Label {
                    text: "CLA:"
                    verticalAlignment: VerticalAlignment.Center
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        color: Color.LightGray
                    }
                }
                TextArea {
                    id: txf_cla
                    objectName: "txf_cla"
                    hintText: "Enter CLA value"
                    text: _apdu._cla
                    enabled: ! cbx_select_only
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                    }
                }
                Label {
                    verticalAlignment: VerticalAlignment.Center
                    text: "INS:"
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        color: Color.LightGray
                    }
                }
                TextArea {
                    id: txf_ins
                    objectName: "txf_ins"
                    hintText: "Enter INS value"
                    text: _apdu._ins
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                    }
                    enabled: ! cbx_select_only.checked
                }
                Label {
                    verticalAlignment: VerticalAlignment.Center
                    text: "P1 P2:"
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        color: Color.LightGray
                    }
                }
                TextArea {
                    id: txf_p1p2
                    objectName: "txf_p1p2"
                    hintText: "Enter P1 P2 values"
                    text: _apdu._p1p2
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                    }
                    enabled: ! cbx_select_only.checked
                }
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Label {
                    text: "Lc:"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.LightGray
                    }
                }
                TextArea {
                    id: txf_lc
                    objectName: "txf_lc"
                    hintText: "Enter Lc value or leave blank"
                    text: _apdu._lc
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                    }
                    enabled: ! cbx_select_only.checked
                }
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Label {
                    text: "Le:"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.LightGray
                    }
                }
                TextArea {
                    id: txf_le
                    objectName: "txf_le"
                    hintText: "Enter Le value or leave blank"
                    text: _apdu._le
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                    }
                    enabled: ! cbx_select_only.checked
                }
            }
            Label {
                text: "Command data:"
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    color: Color.LightGray
                }
            }
            TextArea {
                id: txf_command
                objectName: "txf_command"
                hintText: "Enter command byte values"
                text: _apdu._command
                textStyle {
                    base: SystemDefaults.TextStyles.SmallText
                }
                enabled: ! cbx_select_only.checked
            }
        }
    }
    actions: [
        ActionItem {
            id: send_apdu_action
            title: "Issue APDU"
            imageSource: "images/iso7816_small.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                console.log("Issue APDU Triggered");
                apdu.apduRequested();
            }
        }
    ]
}
