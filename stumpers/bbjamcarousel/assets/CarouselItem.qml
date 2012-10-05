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
    id: itemRoot
    property int angle: 0
    property int angleOffset: 0
    property bool centered: false
    property alias image: theImage.imageSource
    property string imgFileName: ""
    onImgFileNameChanged: {
        theImage.image = app.createMirrorImage(imgFileName)
    }
    preferredWidth: 426
    preferredHeight: 280 + 280 * 0.25 + 2
    translationX: 427 + 410 * Math.cos((itemRoot.angle + itemRoot.angleOffset + 90) * (Math.PI / 180))
    translationY: 130 + 220 * Math.sin((itemRoot.angle + itemRoot.angleOffset + 90) * (Math.PI / 180))
    attachedObjects: [
        ImplicitAnimationController {
            propertyName: "translationX"
            enabled: false
        },
        ImplicitAnimationController {
            propertyName: "translationY"
            enabled: false
        },
        ImplicitAnimationController {
            propertyName: "scaleX"
            enabled: false
        },
        ImplicitAnimationController {
            propertyName: "scaleY"
            enabled: false
        }
    ]
    property double scaleFactor: 1 - 0.55 * Math.sin((itemRoot.angle + itemRoot.angleOffset + 180 + 90) * (Math.PI / 180))
    // value pening between 0.0 (background) and 2.0 (foreground)
    property double depth: 1 + Math.cos((itemRoot.angle + itemRoot.angleOffset) * (Math.PI / 180))
    scaleX: scaleFactor
    scaleY: scaleFactor
    layout: DockLayout {
    }
    ImageView {
        id: theImage
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        scalingMethod: ScalingMethod.AspectFill
        opacity: (itemRoot.depth * 0.375) + 0.25
        attachedObjects: [
            ImplicitAnimationController {
                propertyName: "opacity"
                enabled: false
            }
        ]
    }
}
