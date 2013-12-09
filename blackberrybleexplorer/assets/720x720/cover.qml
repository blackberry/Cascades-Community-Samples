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

import bb.cascades 1.0

Container {

    id: cover_container
    objectName: "cover"

    property int device_count: 0

    function setDeviceCount(count) {
        console.log("QQQQ cover setDeviceCount(" + count + ")");
        device_count = count;
    }

    layout: DockLayout {
    }

    background: Color.Black
    ImageView {
        imageSource: "asset:///images/cover_bg.png"
        scalingMethod: ScalingMethod.AspectFit
    }

    horizontalAlignment: HorizontalAlignment.Fill

    Container {

        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Center
        
        layout: StackLayout {
        }

        Label {
            id: info1
            text: cover_container.device_count
            horizontalAlignment: HorizontalAlignment.Center
        }

        ImageView {
            id: bt_image
            horizontalAlignment: HorizontalAlignment.Center
            imageSource: "asset:///images/bluetooth.png"
        }
        
        Label {
            id: info2
            horizontalAlignment: HorizontalAlignment.Center
            text: "devices"
        }
    }
}
