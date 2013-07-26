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
import bb.system 1.0

Page {
    Container {
        id: mainPage
        objectName: "mainPage"
        signal sendMessage(string message, string address)
        layout: StackLayout {
        }
        Label {
            id: lblHeading_log
            text: qsTr("SMS Messages")
            textStyle.base: SystemDefaults.TextStyles.BigText
            verticalAlignment: VerticalAlignment.Top
            horizontalAlignment: HorizontalAlignment.Center
        }
        Button {
            id: sendButton
            text: qsTr("Send Message")
            horizontalAlignment: HorizontalAlignment.Fill
            onClicked: {
                console.log("XXXX send message button clicked");
                if (destAddr.text.length > 2) {
                    mainPage.sendMessage(sendText.text, destAddr.text);
                    console.log("Playing envelope animation");
                    message.play();
                } else {
                    validationToast.show();
                }
            }
            attachedObjects: [
                SystemToast {
                    id: validationToast
                    body: "Please enter a mobile phone number"
                }
            ]
        }
        TextField {
            id: sendText
            text: "Hello from BlackBerry 10!"
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
        TextField {
            id: destAddr
            objectName: "destAddr"
            text: ""
            inputMode: TextFieldInputMode.PhoneNumber
            hintText: "Mobile phone number or short code"
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
        ImageView {
            id: envelope
            objectName: "envelope"
            imageSource: "images/envelope.png"
            visible: true
            attachedObjects: [
                SequentialAnimation {
                    id: message
                    animations: [
                        TranslateTransition {
                            toX: 0
                            duration: 0
                        },
                        TranslateTransition {
                            toX: 768
                            duration: 1500
                            easingCurve: StockCurve.SineIn
                        }
                    ]
                }
            ]
        }
        ScrollView {
            id: logScroller
            TextArea {
                id: log
                objectName: "logArea"
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                textStyle.fontWeight: FontWeight.Normal
                textStyle.fontSize: FontSize.Small
                textStyle.color: Color.Green
                text: ""
                editable: false
                onTextChanged: {
                    logScroller.scrollToPoint(0, Infinity);
                }
            }
        }
    }
    actions: [
        ActionItem {
            title: qsTr("About")
            imageSource: "asset:///images/about.png"
            onTriggered: {
                aboutInfo.open();
            }
        }
    ]
    attachedObjects: [
        AboutSheet {
            id: aboutInfo
        }
    ]
}
