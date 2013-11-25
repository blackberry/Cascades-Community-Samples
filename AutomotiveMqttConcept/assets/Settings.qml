/* Copyright (c) 2013 Research In Motion Limited.
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

Sheet {
    id: root
    Page {
        actionBarVisibility: ChromeVisibility.Visible
        Container {
            layout: StackLayout {
            }
            leftPadding: 20
            rightPadding: 20
            verticalAlignment: VerticalAlignment.Fill
            Label {
                id: lblHeading_about
                text: qsTr("Settings")
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            TextField {
                id: tfHost
                text: _hrm.mqtt_host
                inputMode: TextFieldInputMode.Text
                input {
                    flags: TextInputFlag.AutoCapitalizationOff
                }
            }
            TextField {
                id: tfPort
                text: _hrm.mqtt_port
                inputMode: TextFieldInputMode.NumbersAndPunctuation
            }
        }
        actions: [
            ActionItem {
                title: "Back"
                imageSource: "asset:///images/ic_previous.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    console.log("Calling setConnectionArgs with host="+tfHost.text+" and port="+tfPort.text);
                    _hrm.setConnectionArgs(tfHost.text,tfPort.text);
                    root.close();
                }
            }
        ]

    }
}
