/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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
import "common"

Page {

    function logMessage(message) {
        console.log("XXXX logMessage:"+message);
        log.text += (qsTr("\n") + message );
    }
    
    content: MenuContainer {
        id: llcp_menu
        objectName: "llcp"

        signal sendLlcpRequested()
        signal llcpBackButton()

        function getText() {
            return _llcp.getText();
        }

        Container {
            Label {
                text: "LLCP Test"
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                }
            }
            Label {
                text: "Message:"
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    color: Color.LightGray
                }
            }
            TextArea {
                id: txf_message
                objectName: "txf_message"
                hintText: "Enter some text to send over LLCP"
                text: llcp.getText()
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                }
            }
            Button {
                text: "Send"
                onClicked: {
                    _llcp.sendOverLlcp(txf_message.text);
                }
            }
            ScrollView {
                id: logScroller
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                TextArea {
                    id: log
                    text: ""
                    editable: false
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Green
                        fontFamily: "Courier"
                        lineHeight: 1.1
                    }
                    content.flags: TextContentFlag.ActiveTextOff
                    onTextChanged: {
                        logScroller.scrollToPoint(0, Infinity);
                    }
                }
            }
        }
    }
    
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: {
                console.log("LLCP Back Button Triggered");
                llcp.llcpBackButton();
            }
        }
    }
}