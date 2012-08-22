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
        background: Color.create("#87CEFA")
        ImageLabel {
            property string originalCaption: "Hold Me!"
            id: bubbleCaption
            caption: originalCaption
            //We define a set of context-sensitive actions for this element. These actions
            //will appear in a sidebar when the element is held for a short period of time.
            contextActions: [
                ActionSet {
                    ActionItem {
                        title: "Click Me"
                        imageSource: "asset:///images/icons/bubbleicon.png"
                        onTriggered: {
                            bubbleCaption.caption = "Thanks :)";
                            goodClick.play();
                        }
                    }
                    ActionItem {
                        title: "Don't Click Me"
                        onTriggered: {
                            bubbleCaption.caption = "Wrong choice!";
                            badClick.play();
                        }
                    }
                }
            ]
            animations: [
                SequentialAnimation {
                    id: goodClick
                    ScaleTransition {
                        duration: 500
                        fromX: 1.0
                        toX: 1.2
                        fromY: 1.0
                        toY: 1.2
                    }
                    ScaleTransition {
                        duration: 500
                        toX: 1.0
                        toY: 1.0
                    }
                },
                SequentialAnimation {
                    id: badClick
                    RotateTransition {
                        duration: 50
                        toAngleZ: 10
                    }
                    RotateTransition {
                        duration: 100
                        toAngleZ: -10
                    }
                    RotateTransition {
                        duration: 100
                        toAngleZ: 10
                    }
                    RotateTransition {
                        duration: 100
                        toAngleZ: -10
                    }
                    RotateTransition {
                        duration: 50
                        toAngleZ: 0
                    }
                }
            ]
        }
    }
    
    //The below actions are defined for the Page. These actions will appear in the Action Bar
    //until it fills after which additional items will show in the overflow menu
    actions: [
        ActionItem {
            title: "Opacity"
            imageSource: "asset:///images/icons/bubbleicon.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                if (bubbleCaption.opacity == 0) {
                    bubbleCaption.opacity = 1.0;
                } else {
                    bubbleCaption.opacity = 0.0;
                }
            }
        },
        ActionItem {
            title: "Spin!"
            ActionBar.placement: ActionBarPlacement.OnBar
            imageSource: "asset:///images/icons/bubbleicon.png"
            onTriggered: {
                bubbleCaption.rotationZ = (bubbleCaption.rotationZ + 180) % 360;
            }
        },
        ActionItem {
            title: "Caption"
            imageSource: "asset:///images/icons/bubbleicon.png"
            onTriggered: {
                bubbleCaption.caption = "Thanks! Change is good";
            }
        },
        ActionItem {
            title: "Move"
            imageSource: "asset:///images/icons/bubbleicon.png"
            onTriggered: {
                bubbleCaption.translationY = (bubbleCaption.translationY == 0 ? 200 : 0);
            }
        },
        ActionItem {
            title: "Reset"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                bubbleCaption.translationY = 0;
                bubbleCaption.opacity = 1.0;
                bubbleCaption.rotationZ = 0;
                bubbleCaption.caption = bubbleCaption.originalCaption;
            }
        }
    ]
}
