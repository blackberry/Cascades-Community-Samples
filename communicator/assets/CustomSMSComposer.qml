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
import bb.cascades 1.2

Sheet {
    id: customSMSSheet
    function syncPhoneNumber(toPhoneNumber) {
        toLabel.text = toPhoneNumber
    }
    Page {
        titleBar: TitleBar {
            acceptAction: ActionItem {
                id: sendBtn
                title: "send"
                enabled: false
                onTriggered: {
                    communicatorApp.SendSMSMessage(toLabel.text,userSMSBody.text)
                    customSMSSheet.close();
                    customSMSSheet.destroy();
                }
            }
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    customSMSSheet.close()
                    customSMSSheet.destroy();
                }
            }
            visibility: ChromeVisibility.Visible
            title: "Compose"
        } 
        Container {
           Label {
               id: toLabel
           }
            Divider { }
            TextArea {
               id: userSMSBody
               backgroundVisible: false	
               inputMode: TextAreaInputMode.Chat
               onTextChanging: {
                   if(userSMSBody.text.length >0){
                       sendBtn.enabled = true
                   } else {
                       sendBtn.enabled = false
                   }
               }
           }
        }
    }
}