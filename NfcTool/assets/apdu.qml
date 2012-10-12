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
    content: MenuContainer {
        id: apdu
        objectName: "apdu_details"
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
                }
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
