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
    
    // Define the ActionItems to register and unregister our push service
    actions: [
        ActionItem {
            objectName: "registerActionItem"
            title: "Register"
            imageSource: "asset:///images/actionbar/registericon.png"
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            objectName: "unregisterActionItem"
            title: "Unregister"
            imageSource: "asset:///images/actionbar/unregistericon.png"
            ActionBar.placement: ActionBarPlacement.OnBar
        }
    ]
    Container {
        leftPadding: 20
        rightPadding: 20
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Center
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            
            // Display the UrbanAirship logo. Used with permission from UrbanAirship.
            ImageView {
                imageSource: "asset:///images/UrbanAirship_header.png"
            }
            
            // The following 3 StatusField components display graphical updates
            // of the registration process. See @StatusField.qml for more info.
            StatusField {
                objectName: "createPushSessionStatus"
                processLabel: "Create Push Session: "
            }
            StatusField {
                objectName: "createPushChannelStatus"
                processLabel: "Push Channel: "
            }
            StatusField {
                objectName: "registertoLaunchStatus"
                processLabel: "Register to Launch: "
            }
            StatusField {
                objectName: "registerWithUrbanAirshipStatus"
                processLabel: "Register to With Urban Airship: "
            }
            
            // Display a Legend to the user to explain the various
            // status icon meanings.
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                LegendField {
                    imageSource: "asset:///images/goodStatus.png"
                    text: "Created/Registered"
                }
                LegendField {
                    imageSource: "asset:///images/badStatus.png"
                    text: "Deleted/Unregistered"
                }
                LegendField {
                    imageSource: "asset:///images/disableStatus.png"
                    text: "Unknown"
                }
            }
        }
        Divider {
        }
        Container {
            id: container
            preferredWidth: 700
            horizontalAlignment: HorizontalAlignment.Center
            Label {
                text: "Log:"
            }
            
            // The following Container serves 2 purposes:
            //  1) Display an ActivityIndicator when calls are in progress
            //  2) Display log information for when pushes are received or errors occur
            Container {
                background: Color.LightGray
                preferredHeight: 330
                horizontalAlignment: HorizontalAlignment.Fill
                layout: DockLayout {
                }
                ActivityIndicator {
                    objectName: "activityIndicator"
                    verticalAlignment: VerticalAlignment.Fill
                    horizontalAlignment: HorizontalAlignment.Fill
                    touchPropagationMode: TouchPropagationMode.PassThrough
                }
                ScrollView {
                    scrollViewProperties.scrollMode: ScrollMode.Vertical
                    TextArea {
                        preferredWidth: container.preferredWidth
                        objectName: "logger"
                        hintText: ""
                        editable: false
                    }
                }
            }
        }
    }
}
