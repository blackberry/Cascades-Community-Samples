/* Copyright (c) 2014 BlackBerry Ltd.
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
 * 
 * The Lithium REST APIs are owned by Lithium Technologies, Inc. 
 * (http://www.lithium.com) and are not part of the “Works” licensed 
 * hereunder pursuant to the Apache 2.0 license.
 */

import bb.cascades 1.2

Page {
    Container {
        leftPadding: 10.0
        topPadding: 10.0
        rightPadding: 10.0
        bottomPadding: 10.0
        
        layout: DockLayout {

        }
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill
        ImageView {
            imageSource: "asset:///images/bbmBackground.png"
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            opacity: 0.2
            enabled: false

        }
        ScrollView {

            Container {
                layoutProperties: StackLayoutProperties {

                }
                horizontalAlignment: HorizontalAlignment.Fill
                
                Label {
                    text: qsTr("Choose a BBM Channel to Open")
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.fontSize: FontSize.Large
                }
                Label {
                    horizontalAlignment: HorizontalAlignment.Center
                    text: qsTr("Your Channel Name")  //ToDo:  Enter your channel name
                }
                ImageButton {
                    horizontalAlignment: HorizontalAlignment.Center
                    defaultImageSource: "asset:///images/BlackBerryChannel.png"
                    onClicked: {
                        jamChannel.trigger("bb.action.OPENBBMCHANNEL")
                    }
                }
            }
        }
    }
    attachedObjects: [
        Invocation {
            id: jamChannel
            property bool auto_trigger: false
            query {
                invokeTargetId: "sys.bbm.channels.card.previewer"
                uri: "bbmc:C0001F622"  //ToDo: Your Channel ID here
            }
            onArmed: {
                // don't auto-trigger on initial setup
                if (auto_trigger) {
                    trigger("bb.action.OPEN");
                }
                auto_trigger = true; // allow re-arming to auto-trigger
            }
        }
    ]
}
