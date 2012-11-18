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

ImageView {
    id: sign
    property bool activated: false

    imageSource: "asset:///images/Sign.png"
    translationX: 640
    translationY: 500
    pivotY: 110
    pivotX: -50
    
    attachedObjects: [
        ImplicitAnimationController {
            propertyName: "rotationZ"
            enabled: false
        }
    ]
    
    animations: [
        SequentialAnimation {
            id: swingAnimation
            animations: [
		        RotateTransition {
		            toAngleZ: 10
		            duration: 100
		        },
		        RotateTransition {
		            toAngleZ: -10
		            duration: 200
		        },
		        RotateTransition {
		            toAngleZ: 5
		            duration: 100
		        },
		        RotateTransition {
		            toAngleZ: -5
		            duration: 100
		        },
		        RotateTransition {
		            toAngleZ: 2
		            duration: 100
		        },
		        RotateTransition {
		            toAngleZ: -2
		            duration: 100
		        },
		        RotateTransition {
		            toAngleZ: 0
		            duration: 100
		        }
            ]
        }
    ]

    function activate() {
        sign.activated = true
        if (swingAnimation.isPlaying()) {
            swingAnimation.stop()
        }
        if (!swingAnimation.isPlaying()) {
            swingAnimation.play()
        }
        sign.translationY = 320
    }
    
    function deactivate() {
        sign.activated = false
        sign.translationY = 500
    }
}
