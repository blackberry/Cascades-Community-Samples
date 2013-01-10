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
 */

import bb.cascades 1.0

Page {
    property bool havePinsBeenReceived: false
    actions: [
        ActionItem {
            title: "Single PIN"
            ActionBar.placement: ActionBarPlacement.OnBar
            enabled: ! networkCallIndicator.running
            onTriggered: {
                //simplePushServer is a C++ object
                simplePushServer.pushMessageToUser(targetPinTextField.text, priorityLevel.selectedValue, titleTextField.text, bodyTextArea.text);
                networkCallIndicator.start();
            }
        },
        ActionItem {
            title: "Multi PIN"
            ActionBar.placement: ActionBarPlacement.OnBar
            enabled: havePinsBeenReceived && ! networkCallIndicator.running
            onTriggered: confirmationPrompt.open()
        },
        ActionItem {
            title: "Get PINs"
            ActionBar.placement: ActionBarPlacement.OnBar
            enabled: ! networkCallIndicator.running
            onTriggered: {
                //simplePushServer is a C++ object
                simplePushServer.requestSubscribedUserList();
                networkCallIndicator.start();
            }
        }
    ]
    attachedObjects: [
        CustomPrompt {
            id: confirmationPrompt
            text: "<html>Are you sure you want to send this message to <em>all</em> registered users?\n</html>"
            confirmButtonText: "I'm sure"
            cancelButtonText: "Not quite"
            onConfirmed: {
                //simplePushServer is a C++ object
                simplePushServer.pushMessageToUserList(priorityLevel.selectedValue, titleTextField.text, bodyTextArea.text);
                networkCallIndicator.start();
                close();
            }
            onCancelled: {
                close();
            }
        }
    ]
    Container {
        leftPadding: 20
        rightPadding: 20
        topPadding: 20
        Label {
            text: "Priority"
        }
        SegmentedControl {
            id: priorityLevel
            Option {
                text: "Low"
                value: 0
                selected: true
            }
            Option {
                text: "Medium"
                value: 1
            }
            Option {
                text: "High"
                value: 2
            }
        }
        TextField {
            id: titleTextField
            hintText: "Push message title"
        }
        TextArea {
            id: bodyTextArea
            hintText: "Push message body"
            preferredHeight: 200
        }
        TextField {
            id: targetPinTextField
            hintText: "Device PIN (if pushing to single device)"
            inputMode: TextFieldInputMode.Pin
        }
        Container {
            layout: DockLayout {
            }
            ScrollView {
                scrollViewProperties.scrollMode: ScrollMode.Vertical
                TextArea {
                    id: logTextArea
                    objectName: "logTextArea"
                    hintText: ""
                    editable: false
                    preferredHeight: 400
                    onTextChanged: {
                        networkCallIndicator.stop();
                    }
                }
            }
            ActivityIndicator {
                id: networkCallIndicator
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
                touchPropagationMode: TouchPropagationMode.None
                overlapTouchPolicy: OverlapTouchPolicy.Allow
            }
        }
    }
}
