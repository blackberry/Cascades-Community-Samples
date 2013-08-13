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

Container{
    id: adRoot
    // The source for the image file, assigned in the ListView
    property alias imageSource: adImage.imageSource
    property string uri: "" //The app's content ID
    property string name: "" //The name for the ad/app/site (not used here)
    property bool animated: false //True if using an animated GIF
    
    //The image displayed as the ad face
    ImageView {
        id: adImage
        scalingMethod: ScalingMethod.AspectFill
        attachedObjects: [
            ImageAnimator {
                //The animator is only started if the "animated" property is true
                id: adAnimator
                animatedImage: adImage
                started: adRoot.animated
            }
        ]
    } 
    
    //When the user taps this Container we can fire off our invocation
    gestureHandlers: [
        TapHandler {
            onTapped: {
                triggerAdFunction.trigger("bb.action.OPEN");
            }
        }
    ]
    
    attachedObjects: [
        Invocation {
            id: triggerAdFunction
            query {
                //Launch BlackBerry World with the URI passed to this control
                invokeTargetId: "sys.appworld"
                mimeType: "application/x-bb-appworld"
                uri: "appworld://content/" + adRoot.uri
                onQueryChanged: {
                    triggerAdFunction.query.updateQuery();
                }
            }
        }
    ]

}