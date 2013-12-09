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

Container {
    id: slidable_item_container
    property int device_identifier: -1
    property int depth: -999 // -999 ensures that onDepthChanged is fired for all items on initial creation
    property int centre_offset: -999 // -1 means "1 place left of centre", +1 means "1 place right of centre". -2 is off screen left. +2 is off screen right.

    function setImage(image_path) {
        theImage.imageSource = image_path;
    }

    function setOffset(offset) {
        centre_offset = offset;
        console.log("QQQQ mainPage.centre_x=" + mainPage.centre_x + ",icon_width=" + icon_dimensions.icon_width + ",centre_offset=" + centre_offset + ",horizontal_displacement=" + icon_dimensions.horizontal_displacement);
        translationX = mainPage.centre_x - (icon_dimensions.icon_width / 2) + (centre_offset * icon_dimensions.horizontal_displacement);
        depth = Math.abs(centre_offset);
        console.log("QQQQ offset changed on ItemDevice " + device_identifier + " to " + centre_offset + " - translationX=" + translationX);
    }

    function disableTranslationAnimation() {
        console.log("QQQQ disabling translation animation for item " + device_identifier);
        x_controller.enabled = false;
    }

    function enableTranslationAnimation() {
        console.log("QQQQ enabling translation animation for item " + device_identifier);
        x_controller.enabled = true;
    }

    onDepthChanged: {
        scaleX = 2 - (depth * 0.25);
        scaleY = 2 - (depth * 0.25)
        console.log("QQQQ ItemDevice "+device_identifier+" depth changed to "+depth+", scale factors="+scaleX+","+scaleY);
    }

    onVisibleChanged: {
        console.log("CCCC visible changed in ItemDevice " + device_identifier + " to " + visible);
    }

    IconDimensions {
    	id: icon_dimensions
    }
    
    ImageView {
        id: theImage
        imageSource: "asset:///images/bluetooth.png"
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        scalingMethod: ScalingMethod.AspectFill
        opacity: 1 - (depth * 0.25)
        attachedObjects: [
            ImplicitAnimationController {
                propertyName: "opacity"
                enabled: false
            }        ]
    }

    translationY: mainPage.centre_y - (icon_dimensions.icon_height / 2) - (depth * icon_dimensions.vertical_displacement)

    attachedObjects: [
        ImplicitAnimationController {
            id: x_controller
            propertyName: "translationX"
            enabled: true
        }
    ]

}