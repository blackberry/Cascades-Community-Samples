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
import bb.system.phone 1.0
import bb.multimedia 1.0
import bb.system 1.0

// This is the application Page with a Container as content, all UI components are children to that Container.

Page {
    actionBarVisibility: ChromeVisibility.Overlay
    onCreationCompleted: { 
        Qt.phone = phone; 
        phone.callUpdated.connect(communicatorApp.onCallUpdated);
    }
    actions: [
        ActionItem {
            title: "Log"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/CallsLog.png"
            onTriggered: {
                phone.initiateCellularCall("")
            }
        },
        ActionItem {
            title: "SMS"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/031.Chat.png"
            onTriggered: {
                tSMSDialog.show()
            }
        },
        ActionItem {
            title: "Dial Pad"
            ActionBar.placement: ActionBarPlacement.InOverflow
            imageSource: "asset:///images/icon_127.png"
            onTriggered: {
                phone.requestDialpad(dialPadCtn.phoneNumber)
            }
        }
    ]
    attachedObjects: [
        Phone {
            id: phone
        },
        ComponentDefinition {
            id: customSMSSheet
            source: "CustomSMSComposer.qml" 
        },
        SystemDialog {
            id: tSMSDialog
            title: "How you want to Compose SMS"
            confirmButton.label: "Invoke System Composer"
            customButton.label: "In App Composer"
            buttonAreaLimit:1     
            onFinished: {
                var x = result;
                if (x == SystemUiResult.ConfirmButtonSelection) {
                    communicatorApp.InvokeSmsComposer(dialPadCtn.phoneNumber)
                } else if (x == SystemUiResult.CustomButtonSelection) {
                    var sheet = customSMSSheet.createObject()
                    sheet.syncPhoneNumber(dialPadCtn.phoneNumber)
                    sheet.open();
                }
            }   
        }
    ]
    ScrollView {
        Container {
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            layout: DockLayout {}
            background: Color.Black
            ImageView {
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                imageSource: "asset:///images/communicatorBG2.png"
            }
            Container {
                CustomDialpadView {
                    id: dialPadCtn
                    topPadding: 70
                    //Custom Properties
                    topMarginVal: 40
                    leftMarginVal: 100
                    callBtnLeftMarginVal: 140
                }
                Container {
                    verticalAlignment: VerticalAlignment.Fill
                    horizontalAlignment: HorizontalAlignment.Fill
                    leftPadding: 70
                    Divider { }
                    Label {
                        //horizontalAlignment: HorizontalAlignment.Center
                        text: "Call Updated Log:"
                        textStyle.base: SystemDefaults.TextStyles.SubtitleText
                        textStyle.color: Color.White
                        textStyle.textAlign: TextAlign.Left
                    }
                    TextArea {
                        topPadding: 0
                        objectName: "CallInfoTxt"
                        hintText: ""
                        backgroundVisible: false;
                        text: ""
                        scrollMode: TextAreaScrollMode.Elastic
                        textStyle.base: SystemDefaults.TextStyles.SmallText
                        textStyle.color: Color.White
                        focusPolicy: FocusPolicy.None
                        editable: false
                    }   
                }
            }
        }
    }
}
