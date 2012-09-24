/*
* Copyright (c) 2012 Brian Scheirer
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
*/import bb.cascades 1.0

import bb.cascades 1.0

NavigationPane {
    id: navigationPane
    backButtonsVisible: false
    Page {
        content: Container {
            background: backgroundPaint.imagePaint
            attachedObjects: [
                ImagePaintDefinition {
                    id: backgroundPaint
                    imageSource: "asset:///images/RPSbkgrdD.png"
                },
                Sheet {
                    id: gameInfo
                    Page {
                        Container {
                            background: backgroundPaint.imagePaint
                            Container {
                                background: Color.create(0.9, 0.9, 0.9, 0.75)
                                TextArea {
                                    text: "The evil dragon has taken the princess hostage. The only way to save her is to defeat him in an epic battle of ROCK, PAPER, SCISSORS! 
            The Rules are simple:  
            Rock defeats Scissors. 
            Paper defeats Rock. 
            Scissors defeats Paper.

***Note*** To change characters during the battle, double tap your character to bring up the character list."
                                    editable: false
                                    textStyle {
                                        size: 50
                                        fontWeight: FontWeight.Bold
                                        color: Color.Black
                                    }
                                }
                                Button {
                                    text: "Menu"
                                    onClicked: gameInfo.visible = false
                                    layoutProperties: StackLayoutProperties {
                                        horizontalAlignment: HorizontalAlignment.Center
                                    }
                                }
                            }
                        }
                    }
                }
            ]
            minHeight: 1282.0
            Container {
                ImageView {
                    imageSource: "asset:///images/TitleScreen.png"
                }
            }
            Container {
                preferredHeight: 200.0
            }
            Container {
                minWidth: 500.0
                Button {
                    id: one
                    text: "Play"
                    onClicked: {
                        navigationPane.deprecatedPushQmlByString("gameScreen.qml");
                    }
                    layoutProperties: StackLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                }
                Button {
                    id: zero
                    text: "Game Info"
                    onClicked: {
                        gameInfo.visible = true;
                    }
                    layoutProperties: StackLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                }
            }
        }
    }
}
