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

import bb.cascades 1.0

Container {
    property int final_x: 200
    property int enter_duration: 5000

    function enterChicken(image_url, end_at_X, duration) {
        // Implicit animations are turned off for the translationX property,
        // so the movement of a chicken to align it will happen immediately.
        console.log("XXXX enterChicken(" + image_url + "," + end_at_X  + "," + duration + ")");
        chicken_image.imageSource = image_url;
        final_x = end_at_X;
        enter_duration = duration;
        enter_chicken.play();
    }
    
    id: chickenContainer
    layout: DockLayout {
    }
    animations: [
        SequentialAnimation {
            id: enter_chicken
            animations: [
                TranslateTransition {
                    fromX: 800
                    toX: final_x
                    toY: 0
                    duration: enter_duration
                    easingCurve: StockCurve.SineIn
                }
            ]
        }
    ]
    ImageView {
        id: chicken_image
    }

    // This is the implicit-animation controller for the chicken.
    // We need this to set the chicken to a new position before showing it without
    // triggering implicit animations.
    attachedObjects: [
        ImplicitAnimationController {
            id: offScreenController
            propertyName: "translationX"
            enabled: false
        }
    ]

}
