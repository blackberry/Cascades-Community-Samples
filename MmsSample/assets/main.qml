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
import bb.cascades.pickers 1.0

Page {
    property int p_file_attached: 0
    property string p_file_path: ""

    Container {
        id: mainPage
        objectName: "mainPage"
        signal sendMessage(string message1, string message2, string address, string attachment_url)
        layout: StackLayout {
        }
        Label {
            id: lblHeading_log
            text: qsTr("MMS Sample")
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
                if (destAddr.text.length > 2 && p_file_attached == 1) {
                    mainPage.sendMessage(sendText.text, sendText2.text, destAddr.text, p_file_path);
                    sendingToast.show();
                } else {
                    validationToast.show();
                }
            }
        }
        TextField {
            id: sendText
            text: "Here's a photo for you..."
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
        TextField {
            id: sendText2
            text: "...and a friendly message"
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
            id: attachment
            objectName: "attachment"
            visible: true
            preferredHeight: 200
            preferredWidth: 150
            maxWidth: 200
            scalingMethod: ScalingMethod.AspectFit
            imageSource: "images/paper_clip.png"
            onTouch: {
                if (event.isUp()) {
                    filePicker.open()
                }
            }
            onImageSourceChanged: {
                console.log("XXXX imageSourceChanged to "+imageSource);
            }
            horizontalAlignment: HorizontalAlignment.Center
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
                text: ""
                editable: false
                onTextChanged: {
                    logScroller.scrollToPoint(0, Infinity);
                }
            }
        }
    }
    attachedObjects: [
        SystemToast {
            id: validationToast
            body: "Enter a phone number and attach a photo"
        },
        SystemToast {
            id: sendingToast
            body: "Sending...."
        },
        AboutSheet {
            id: aboutInfo
        },
        FilePicker {
            id: filePicker
            type: FileType.Picture
            title: "Select Picture"
            directories: [ "/accounts/1000/shared/camera" ]
            onFileSelected: {
                console.log("fileSelected signal received : " + selectedFiles);
                //make sure to prepend "file://" when using as a source for an ImageView or MediaPlayer
                // call C++ method here (signal)
                p_file_attached = 1;
                console.log("XXXX setting imageSource to ["+"file://" + selectedFiles[0]+"]")
                attachment.imageSource = "file://" + selectedFiles[0];
                p_file_path = selectedFiles[0];
            }
            onCanceled: {
                console.log("canceled signal received : " + selectedFiles);
                p_file_attached = 0;
            }
        }
    ]
    actions: [
        ActionItem {
            title: qsTr("About")
            imageSource: "asset:///images/about.png"
            onTriggered: {
                aboutInfo.open();
            }
        }
    ]
}