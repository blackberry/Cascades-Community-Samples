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

//This component will allow the user to touch for 1 second then it animates

// and becomes movable around the screen, following the user touch

Container {
    property alias imageSource: image.imageSource
    id: movableImage
    property int touchX: 0
    property int touchY: 0
    ImageView {
        id: image
    }
    animations: [
        TranslateTransition {
            //This delay will cause the user to hold for 1 second
            // before the image becomes movable
            id: touchPause
            delay: 1000
            onEnded: {
                //If this animation ends without being stopped, start the pulsing
                // animation to signify the image can be moved
                pulseAnimation.play();
            }
        },
        SequentialAnimation {
            //This animation scales the images larger than back to its original
            // size repeatedly until the touch is releases
            id: pulseAnimation
            ScaleTransition {
                duration: 350
                fromX: 1.0
                toX: 1.1
                fromY: 1.0
                toY: 1.1
                easingCurve: StockCurve.QuadraticInOut
            }
            ScaleTransition {
                duration: 350
                fromX: 1.2
                toX: 1.0
                fromY: 1.2
                toY: 1.0
                easingCurve: StockCurve.QuadraticInOut
            }
            onEnded: {
                //This loops the animation indefinitely
                pulseAnimation.play();
            }
            onStopped: {
                //If the animation is stopped by the application then call the 
                // animation to restore the image to its original scale
                restoreScaleAnimation.play();
            }
        },
        ScaleTransition {
            //This animation simply scales the image to its original scale
            id: restoreScaleAnimation
            duration: 200
            toX: 1.0
            toY: 1.0
        }
    ]
    onTouch: {
        if (event.isDown()) {
            //If the user touches this container we start the delay animation then
            // retrieve the X and Y coordinates local to the container 
            touchPause.play();
            movableImage.touchX = event.localX
            movableImage.touchY = event.localY
        } else if (event.isCancel() || event.isUp()) {
            //If the touch moves outside the container or the user raises their finger
            // then we need to stop the animations
            touchPause.stop();
            pulseAnimation.stop();
        } else if (event.isMove() && pulseAnimation.isPlaying()) {
            //If the pulse animation is playing and the touch point is moving we will 
            // update the translation points of the image allowing it to move about 
            // the screen following the touch point
            movableImage.translationX = event.windowX - movableImage.touchX;
            movableImage.translationY = event.windowY - movableImage.touchY;
        }
    }
}
