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

Container {
    id: container
    signal command(string command)
    
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {

        }
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        ImageView {
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Center
            imageSource: "asset:///images/left.png"
        }
        rightMargin: 5
        leftMargin: 5
        gestureHandlers: [
            TapHandler {
                onTapped: {
                    container.command(1);
                }
            }
        ]
    }
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {
        
        }
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        background: Color.Red
        Label {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            text: "Stop"
        }
        rightMargin: 5
        leftMargin: 5
        gestureHandlers: [
            TapHandler {
                onTapped: {
                    container.command(0);
                }
            }
        ]
    }
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {
        
        }
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        ImageView {
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Center
            imageSource: "asset:///images/right.png"
        }
        rightMargin: 5
        leftMargin: 5
        gestureHandlers: [
            TapHandler {
                onTapped: {
                    container.command(-1);
                }
            }
        ]
    }
    attachedObjects: [
        LayoutUpdateHandler {
            onLayoutFrameChanged: {
                container.preferredHeight = layoutFrame.width/3*1.25;
            }
        }
    ]
}
