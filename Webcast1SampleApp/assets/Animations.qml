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

Page {
    content: Container {
        layout: DockLayout {
        }
        ImageView {
            /**
            By Wknight94 (Own work) [GFDL (www.gnu.org/copyleft/fdl.html) or CC-BY-SA-3.0 (www.creativecommons.org/licenses/by-sa/3.0)], via Wikimedia Commons
            **/
            imageSource: "asset:///images/water.jpg"
        }
        Container {
            layout: AbsoluteLayout {
            }
            preferredHeight: maxHeight
            preferredWidth: maxWidth
            ImageView {
                id: fish
                /**
                By ArielGold (Own work) [GFDL (http://www.gnu.org/copyleft/fdl.html) or CC-BY-SA-3.0-2.5-2.0-1.0 (http://creativecommons.org/licenses/by-sa/3.0)], via Wikimedia Commons
                **/
                imageSource: "images/cartoonfish.png"
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 0
                    positionY: 0
                }
                
                //This section allows us to enable or disable the Implicit Animations of the ImageView element
                attachedObjects: [
                    ImplicitAnimationController {
                        enabled: implicitToggle.checked
                    }
                ]
                //The animations block defines the movement of our ImageView element that will
                //be triggered upon calling fishSwimAnimation.play()
                animations: [
                    ParallelAnimation {
                        id: fishSwimAnimation
                        TranslateTransition {
                            duration: 3000
                            fromX: 0
                            fromY: 0
                            toX: 350
                            toY: 600
                            easingCurve: StockCurve.SineInOut
                        }
                        ScaleTransition {
                            duration: 3000
                            fromX: 1.0
                            toX: -2.0
                            fromY: 1.0
                            toY: 2.0
                        }
                        SequentialAnimation {
                            RotateTransition {
                                duration: 750
                                fromAngleZ: 0
                                toAngleZ: 25
                            }
                            RotateTransition {
                                duration: 1500
                                toAngleZ: -25
                            }
                            RotateTransition {
                                duration: 750
                                toAngleZ: 0
                            }
                        }
                    }
                ]
            }
        }
        Container {
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Bottom
                horizontalAlignment: HorizontalAlignment.Center
            }
            Container {
                layout: StackLayout {
                    layoutDirection: LayoutDirection.LeftToRight
                }
                
                //Move our fish ImageView by setting some properties directly
                //Demonstrates implicite animations
                Button {
                    text: "Implicit Move"
                    onClicked: {
                        if (fish.translationX == 0) {
                            fish.translationX = 350;
                            fish.translationY = 600;
                            fish.scaleX = -2.0;
                            fish.scaleY = 2.0;
                        } else {
                            fish.translationX = 0;
                            fish.translationY = 0;
                            fish.scaleX = 1.0;
                            fish.scaleY = 1.0;
                        }
                    }
                }
                
                //Move our fish ImageView by calling play() on the defined animation
                //Demonstrates explicite animations
                Button {
                    text: "Explicit Move"
                    onClicked: {
                        fishSwimAnimation.play ();
                    }
                }
            }
            Container {
                layout: DockLayout {
                }
                preferredWidth: 600
                Label {
                    layoutProperties: DockLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Center
                    }
                    text: "Implicit Animations: "
                    textStyle {
                        color: Color.White
                    }
                }
                //This ToggleButton will enable/disable the implicit animation of our fish ImaeView
                ToggleButton {
                    layoutProperties: DockLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Right
                    }
                    id: implicitToggle
                    checked: true
                }
            }
        }
    }
}
