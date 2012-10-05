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
    id: root
    property int angle: 0
    property int baseAngle: 0
    property double offsetValue: 0
    property variant centeredControl: 0
    property double releaseSpeed: 0
    property double momentumSpeed: 0
    layout: DockLayout {
    }
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        id: carouselContainer
        layout: AbsoluteLayout {
        }
        CarouselItem {
            centered: true
            angle: root.angle + root.baseAngle
            imgFileName: "images/bg1.png"
            // use one item to trigger sort
            onDepthChanged: {
                root.sort()
            }
        }
        CarouselItem {
            angle: root.angle + root.baseAngle
            imgFileName: "images/bg2.png"
        }
        CarouselItem {
            angle: root.angle + root.baseAngle
            imgFileName: "images/johan.png"
        }
        CarouselItem {
            angle: root.angle + root.baseAngle
            imgFileName: "images/bg4.png"
        }
        CarouselItem {
            angle: root.angle + root.baseAngle
            imgFileName: "images/bg5.png"
        }
    }
    onCreationCompleted: {
        root.offsetValue = 360 / (carouselContainer.controls.length)
        var currentOffset = 0
        for (var i = 0; i < carouselContainer.controls.length; i ++) {
            carouselContainer.controls[i].angleOffset = currentOffset
            currentOffset += root.offsetValue;
        }
        root.centeredControl = carouselContainer.controls[0]
        sort();
    }
    function sort() {
        //Bubblesort the carousel items
        var n = carouselContainer.controls.length
        var swopped
        do {
            swopped = false
            for (var i = 0; i < n - 1; ++ i) {
                // a larger depth value means the pic should be on top, i.e. move towards the end of the list of items in the container.
                if (carouselContainer.controls[i].depth > carouselContainer.controls[i + 1].depth) {
                    carouselContainer.swap(i, i + 1)
                    swopped = true
                }
            }
        } while (swopped)
    }
    onAngleChanged: {
        sort();
        root.centeredControl.centered = false;
    }
    onBaseAngleChanged: {
        sort();
    }
    function snapToItem() {
        if (root.baseAngle % root.offsetValue != 0) {
            snapToItemAnimation.startValue = root.baseAngle
            snapToItemAnimation.duration = 300

            // snap in the directon the user is scrolling to avoid unsmooth snapping
            if (root.releaseSpeed < 0) {
                snapToItemAnimation.endValue = Math.floor(root.baseAngle / root.offsetValue) * root.offsetValue
            } else if (root.releaseSpeed > 0) {
                snapToItemAnimation.endValue = Math.ceil(root.baseAngle / root.offsetValue) * root.offsetValue
            } else {
                snapToItemAnimation.endValue = Math.round(root.baseAngle / root.offsetValue) * root.offsetValue
            }

            // Calculate the duration of the snap if we are snapping from a momentum scroll
            if (root.momentumSpeed != 0) {
                var dist = Math.abs(snapToItemAnimation.endValue - snapToItemAnimation.startValue)
                snapToItemAnimation.duration = Math.abs(dist / root.momentumSpeed)
            }
            snapToItemAnimation.start()

            // Set the new centered object
            for (var i = 0; i < carouselContainer.controls.length; i ++) {
                //if (carouselContainer.controls[i].angleOffset == Math.abs(360 - (snapToItemAnimation.endValue)) % 360) {
                if (carouselContainer.controls[i].angleOffset == Math.abs(360 - snapToItemAnimation.endValue) % 360) {
                    root.centeredControl.centered = false;
                    root.centeredControl = carouselContainer.controls[i];
                    root.centeredControl.centered = true;
                }
            }
        }
    }
    attachedObjects: [
        // QPropertyAnimation should be used with care since it is a client side animation that will
        // spam the server with messages for every update.
        QPropertyAnimation {
            id: snapToItemAnimation
            startValue: 0
            endValue: 360
            duration: 300
            easingCurve: app.getEase() // TODO:: expose this correctly
            targetObject: root
            propertyName: app.getBytes("baseAngle") // TODO:: workaround since we need the QByteArray of the property name
            onFinished: {
                root.baseAngle = root.baseAngle % 360
            }
        }
    ]
}
