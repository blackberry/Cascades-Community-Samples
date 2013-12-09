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
    property double touchDownX
    property double xMove: 0
    property bool dragged: false
    property double touchXCentreOffset: 0
    property variant startTime
    property int minimum_movement_for_drag: 20
    
    IconDimensions {
        id: icon_dimensions
    }
    
    onTouch: {
        if (event.isDown()) {
            touchDownX = event.windowX;
            console.log("QQQQ touch event isDown: touchDownX="+touchDownX);
            startTime = Date.now();
            xMove = 0;
            dragged = false;
        } else if (event.isMove()) {
            xMove = touchDownX - event.windowX
            console.log("QQQQ isMove: xMove:" + xMove);
        } else if (event.isUp()) {
            xMove = touchDownX - event.windowX
            console.log("QQQQ touch event isDown: touchDownX="+touchDownX+",xMove="+xMove);
            if (Math.abs(xMove) > minimum_movement_for_drag) {
                dragged = true
            }
            touchXCentreOffset = mainPage.centre_x - event.windowX
            var diffTime = Date.now() - startTime

            if (dragged) {
                if (xMove < 0) {
                    mainPage.slideItemsRight();
                } else {
                    mainPage.slideItemsLeft();
                }
                dragged = false;
                return;
            }

            if (touchXCentreOffset > icon_dimensions.icon_width) {
                mainPage.slideItemsLeft();
            } else {
                if (touchXCentreOffset < (-1 * icon_dimensions.icon_width)) {
                    mainPage.slideItemsRight();
                } else {
                    mainPage.centralItemSelected()
                }
            }
        }
    }
}
