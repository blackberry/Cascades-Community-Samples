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

//This sample shows an in-line animation on an ImageView component

Page {
    content: Container {
        background: Color.Gray
        Container {
            preferredHeight: 450
            preferredWidth: 450
            ImageView {
                imageSource: "asset:///images/ball.png"
                //Here we define a grouped animation to be applied to the ImageView
                animations: ParallelAnimation {
                    //This ID allows the animation to get referenced and started/stopped
                    id: bounce
                    animations: [
                        TranslateTransition {
                            fromY: 0
                            toY: 300
                            duration: 1000
                            easingCurve: StockCurve.DoubleBounceOut
                        },
                        TranslateTransition {
                            fromX: 0
                            toX: 300
                            duration: 1000
                            easingCurve: StockCurve.QuarticOut
                        }
                    ]
                }
            }
        }
        Button {
            text: "Bounce the Ball"
            onClicked: {
                //Trigger the animation to start playing
                bounce.play ();
            }
        }
    }
}
