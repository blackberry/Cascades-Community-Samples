/*
 * Copyright (c) 2014 BlackBerry Limited.
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

Container {
    property alias text: label.text
    
    signal clicked()
    
    gestureHandlers: [
        TapHandler {
            onTapped: {
                console.log("Tap at position " + event.x + ", " + event.y)
                clicked();
            }
        }
    ]
    
    layout: DockLayout {
    }
    
    Container {
        id: bg
        background: Color.create("#5f09e0")
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        Container {
            layout: DockLayout {}
            topPadding: 10
            bottomPadding: 15
            leftPadding: 10
            rightPadding: 10
            horizontalAlignment: HorizontalAlignment.Center
            Label {
                id: label
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                text: ""
                textStyle {
                    base: tsLabel.style
                }
            }
        }
    }
    
    attachedObjects: [
        TextStyleDefinition {
            id: tsLabel
            base: SystemDefaults.TextStyles.BodyText
            fontSize: FontSize.Small
            color: Color.White
            fontWeight: FontWeight.W100
        }
    ]

}
