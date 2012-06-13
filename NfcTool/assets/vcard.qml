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
        id: sendVcard
        objectName: "sendVcard"
        signal sendVcardRequested ()
        Container {
            layout: StackLayout {
                topPadding: 10
                leftPadding: 30
                rightPadding: 30
            }
            TextArea {
                text: "SNEP: Send a vCard"
                editable: false
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                }
            }
            Container {
                layout: StackLayout {
                }
                topMargin: 70
                bottomMargin: 50
                TextArea {
                    editable: false
                    text: "First Name:"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.LightGray
                    }
                }
                TextArea {
                    preferredHeight: 65
                    id: txf_first_name
                    objectName: "txf_first_name"
                    hintText: "Enter your first name"
                    text: _sendVcard._first_name
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                    }
                }
                TextArea {
                    editable: false
                    text: "Last Name:"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.LightGray
                    }
                }
                TextArea {
                    preferredHeight: 65
                    id: txf_last_name
                    objectName: "txf_last_name"
                    hintText: "Enter your last name"
                    text: _sendVcard._last_name
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                    }
                }
                TextArea {
                    editable: false
                    text: "Address:"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.LightGray
                    }
                }
                TextArea {
                    preferredHeight: 65
                    id: txf_address
                    objectName: "txf_address"
                    hintText: "Enter your address"
                    text: _sendVcard._address
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                    }
                }
                TextArea {
                    editable: false
                    text: "Email:"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.LightGray
                    }
                }
                TextArea {
                    preferredHeight: 65
                    id: txf_email
                    objectName: "txf_email"
                    hintText: "Enter your email address"
                    text: _sendVcard._email
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                    }
                }
            }
            TextArea {
                editable: false
                text: "Mobile:"
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    color: Color.LightGray
                }
            }
            TextArea {
                preferredHeight: 65
                id: txf_mobile
                objectName: "txf_mobile"
                hintText: "Enter your mobile number"
                text: _sendVcard._mobile
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                }
            }
        }
    }
    actions: [
        ActionItem {
            id: send_vcard_action
            title: "Send vCard"
            imageSource: "images/snep.png"
            onTriggered: {
                console.log ("Send vCard Triggered");
                sendVcard.sendVcardRequested ();
            }
        }
    ]
}
