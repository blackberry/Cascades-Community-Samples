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

Container {
    //Here we define properties and aliases to allow external access of these values
    property bool selected: false
    property bool active: false
    property alias titleText: title.text
    property alias descriptionText: description.text
    property alias imageSource: ballotIcon.imageSource
    
    //Define a text defintion here for reference throughout the control
    attachedObjects: [
        TextStyleDefinition {
            id: blackBoldStyle
            base: SystemDefaults.TextStyles.BodyText
            color: Color.Black
            fontWeight: FontWeight.Bold
        }
    ]
    
    //The background and opacity values will change with the selected/active properties
    background: selected ? Color.LightGray : Color.create("#BBFFFFFF")
    opacity: active ? 0.75 : 1.0
    preferredHeight: 200
    layout: StackLayout {
        layoutDirection: LayoutDirection.LeftToRight
    }
    Container {
        layoutProperties: StackLayoutProperties {
            spaceQuota: 2.0
        }
        Label {
            id: title
            enabled: false
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1.25
            }
            textStyle {
                base: blackBoldStyle.style
                size: 45
            }
        }
        TextArea {
            id: description
            enabled: false // We want this to act as a non-editable text area
            backgroundVisible: false // so we diable it and hide the default background.
            editable: false
            hintText: ""
            layoutProperties: StackLayoutProperties {
                spaceQuota: 3.25
            }
            textStyle {
                base: SystemDefaults.TextStyles.BodyText
                size: 30
            }
        }
    }
    
    //This Container will house the icon image and border
    Container {
        background: selected ? Color.White : Color.Black
        layout: StackLayout {
            //These padding attributes provide a border for the icons
            leftPadding: 10.0
            rightPadding: 10.0
            bottomPadding: 10.0
            topPadding: 10.0
        }
        layoutProperties: StackLayoutProperties {
            spaceQuota: 0.8
        }
        Container {
            layout: DockLayout {
            }
            ImageView {
                id: ballotIcon
            }
            Container {
                visible: selected
                background: Color.White
                opacity: 0.9
                layoutProperties: DockLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
                //This label will overlay the image when the ballot is selected
                Label {
                    text: "VOTE"
                    textStyle {
                        base: blackBoldStyle.style
                    }
                }
            }
        }
    }
}
