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
import utils 1.0

Container {
    id: swipeHandler
    property double dragOffset: 0
    property double momentumOffset: 0
    property double speed
    property double endSpeed
    property double touchDownX
    property variant startTime
    property double timeConstant
    property bool decelerationStarted
    signal scrollingStart()
    signal momentumScrollingEnd()
    signal dragScrollingEnd()
    onTouch: {
        if (event.isDown()) {
            scrollingStart();
            touchDownX = event.windowX;
            startTime = Date.now()
            dragOffset = 0
            if (decelerationStarted == true) {
                timer.stop();
                decelerationStarted = false
            }
        } else if (event.isMove()) {
            dragOffset = touchDownX - event.windowX
        } else if (event.isUp()) {
            dragOffset = touchDownX - event.windowX
            var diffTime = Date.now() - startTime
            speed = dragOffset / diffTime;
            if (Math.abs(dragOffset) > 50 && Math.abs(speed) > 1) {
                decelerationStarted = true
                timeConstant = 3000 / 6;
                timer.t = 0;
                timer.start()
                scrollingStart()
                momentumOffset = 0
            } else {
                dragScrollingEnd();
            }
        }
    }
    attachedObjects: [
        QTimer {
            id: timer
            property double t: 0
            property double prevOffset: 0
            interval: 16
            onTimeout: {
                var delta = speed * Math.exp(- t / timeConstant);
                prevOffset = momentumOffset
                if (t < 1000) {
                    momentumOffset = momentumOffset + 2 * delta;
                } else {
                    momentumOffset = momentumOffset - delta;
                }
                t = t + 16;
                if (Math.abs(momentumOffset - prevOffset) < 3) {
                    endSpeed = delta / 16
                    speed = 0;
                    timer.stop();
                    decelerationStarted = false
                    momentumScrollingEnd()
                }
            }
        }
    ]
}
