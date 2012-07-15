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

// The application page with a Container as content, all UI components are children to that Container.
Page {
    property bool trigLeft: false
    property bool trigRight: false
    
    content: Container {
        layout: AbsoluteLayout {
        }
    
        // Background ImageView.
        ImageView {
            imageSource: "asset:///images/background.png"
        
            // If we touch the background we want a moo to sound.
            onTouch: {
                if (event.isDown ()) {
                    cowbellApp.playSound ("moo.ogg");
                }
            }
        }
    
        // The bell.
        ImageView {
            id: bell
            imageSource: "asset:///images/bell.png"
            preferredWidth: 503
            preferredHeight: 768
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 582
                positionY: -50
            }

            // The bell should rotate around a point located in the middle of
            // the image in x-direction and located somewhere above the screen
            // in y (it hangs around the neck of the cow which is not visible).
            // If the center is not altered all rotation animations will be around
            // the middle of the image.
            pivotY: -600
        
            // We need two animations, one to swing to the left and one to swing to the right.
            animations: [
                SequentialAnimation {
                    id: "animLeft"
                    animations: [
                        RotateTransition {
                            toAngleZ: -1.5
                            duration: 200
                        },
                        RotateTransition {
                            toAngleZ: 0
                            duration: 200
                        }
                    ]
                    onStopped: {
                        // Play stopped animations if any.
                        playStoppedAnimations ()
                    }
                },
                SequentialAnimation {
                    id: "animRight"
                    animations: [
                        RotateTransition {
                            toAngleZ: 1.5
                            duration: 200
                        },
                        RotateTransition {
                            toAngleZ: 0
                            duration: 200
                        }
                    ]
                    onStopped: {
                        playStoppedAnimations ();
                    }
                }
            ]
        
            // Touch handler for the bell image.
            onTouch: {
                if (event.isDown ()) {
                
                    // Do not trigger the bell sound if touches occurred on the image, but outside the 
                    // actual bell (above or below).
                    if (event.screenY > 140 && event.screenY < (preferredHeight - 60)) {
                    
                        // Play the sound.
                        cowbellApp.playSound ("cowbell.wav");
                    
                        // If the image is hit on the left side swing it to the right.
                        if (event.screenX < bell.layoutProperties.positionX + preferredWidth / 2) {                        
                        
                            // First check if another animation is already running on the bell.
                            trigLeft = checkPlayingAnimations ();
                        
                            // If no animation was running, play it. Otherwise we have to wait until
                            // onStopped to avoid conflicting animations. 
                            if (trigLeft == false) {
                                animLeft.play ();
                            }
                        } else {
                            // And vice versa for the right side.
                            trigRight = checkPlayingAnimations ();
                            if (trigRight == false) {
                                animRight.play ();
                            }
                        }
                    }
                }
            }
        }
    }
    
    // This function checks if any of the animations are running on the bell. If that 
    // is the case the animation is stopped and true is returned to notify the caller.
    function checkPlayingAnimations () {
        var animationWasStopped = false;
        if (animLeft.isPlaying ()) {
            animLeft.stop ();
            animationWasStopped = true;
        }
        if (animRight.isPlaying ()) {
            animRight.stop ();
            animationWasStopped = true;
        }
        return animationWasStopped;
    }

    // This function is called from the animations onStopped function. If an animation  
    // was stopped we can not start it immediately. trigRight/trigLeft is true if the animation
    // was stopped and has to be started in onStopped.     
    function playStoppedAnimations () {
        if (trigRight) {
            animRight.play ();
        } else if (trigLeft) {
            animLeft.play ();
        }
        
        trigRight = false;
        trigLeft = false;
    }
}
