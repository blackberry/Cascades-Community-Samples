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

// This is a mock screen where gameplay could be added at a later time

Page {
    id: gamePlay
    actionBarVisibility: ChromeVisibility.Hidden
    Container {
        layout: DockLayout {
        }
        ImageView {
            scalingMethod: ScalingMethod.AspectFill
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            imageSource: "asset:///images/backgroundLandscape.png"
        }
        FreemiumBanner {
            scaleX: 2.0
            scaleY: 2.0
            hideAd: removeAdsPurchased
            horizontalAlignment: HorizontalAlignment.Center
        }
        ImageView {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            imageSource: "asset:///images/insertGamePlay.png"
        }

        //This button pops the GamePlay from the stack, returning to the previous page
        Container {
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Bottom
            leftPadding: 25
            bottomPadding: 25
            ImageButton {
                defaultImageSource: "asset:///images/ic_previous.png"
                onClicked: gamePlay.parent.pop()
            }
        }
    }
}
