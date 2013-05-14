/* Copyright (c) 2013 BlackBerry.
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

/**
 * This is PageTwo, however if you think this means you actually found it
 * then you are incorrect and this is considered cheating.
 * 
 * ...just joking!
 * 
 * There is nothing overly complex in this "secret" page, just some
 * ImageViews and a button to pop the screen. The button of course
 * also logs the click event.
 */

Page {
    id: pageTwo

    // Hide the ActionBar as we will use out own control to pop the page
    actionBarVisibility: ChromeVisibility.Hidden

    Container {
        layout: DockLayout {

        }
        ImageView {
            imageSource: "asset:///images/crumpledPage.png"
        }
        Container {
            verticalAlignment: VerticalAlignment.Center

            horizontalAlignment: HorizontalAlignment.Fill
            ImageView {
                imageSource: "asset:///images/congratulations.png"
                horizontalAlignment: HorizontalAlignment.Right
                preferredWidth: 650
                scalingMethod: ScalingMethod.AspectFill
            }
            ImageView {
                imageSource: "asset:///images/youFound.png"
                horizontalAlignment: HorizontalAlignment.Left
                preferredWidth: 550
                scalingMethod: ScalingMethod.AspectFill
            }
            ImageView {
                imageSource: "asset:///images/page2Part2.png"
                horizontalAlignment: HorizontalAlignment.Center
                preferredWidth: 550
                scalingMethod: ScalingMethod.AspectFill
            }
        }
        
        // This Button will pop the page and log the event. This will let us
        // track how many users want to try the app a second time
        Button {
            id: tryAgainButton
            objectName: "tryAgainButton"
            text: "Try again?"
            onClicked: {
                cpp.logEvent(tryAgainButton.objectName + " clicked");
                navigationPane.pop();
            }
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Center
        }
    }
}
