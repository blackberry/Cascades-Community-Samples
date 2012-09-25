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
*/

import bb.cascades 1.0

Page {
    content: Container {
        layoutProperties: DockLayoutProperties {
            verticalAlignment: VerticalAlignment.Fill
        }
        preferredHeight: 1280.0
        layout: StackLayout {
        }
        Container {
            background: Color.create(0.9,0.9,0.9)
            Label {
                text: "Scoreboard:"
                textStyle.size: 50.0
            }
            Container {
                layout: StackLayout {
                    layoutDirection: LayoutDirection.LeftToRight
                }
                Label {
                    id: win
                    text: "0"
                    textStyle.size: 50.0
                }
                Label {
                    text: "-"
                    textStyle.size: 50.0
                }
                Label {
                    id: lose
                    text: "0"
                    textStyle.size: 50.0
                }
                Label {
                    text: "-"
                    textStyle.size: 50.0
                }
                Label {
                    id: tie
                    text: "0"
                    textStyle.size: 50.0
                }
            }
        }
        //winner label container
        Container {
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
           
            topMargin: 50.0
            preferredHeight: 150.0
            Label {
                            id: winnerlabel
                            text: "BlackBerry WINS!"
                            textStyle.size: 75.0
                        }
        }
        //image container
        Container {
                        layout: StackLayout {
                            layoutDirection: LayoutDirection.LeftToRight
                        }
                        preferredWidth: 720.0
                        layoutProperties: StackLayoutProperties {
                            horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Top
            }
                        preferredHeight: 400.0
                        ImageView {
                            id: player
                            imageSource: "asset:///images/rockimg.png"
                            animations: [
                                SequentialAnimation {
                                    id: playermove
                                    animations: [
                                        TranslateTransition {
                                            toY: 100
                                            duration: 500
                                        },
                                        TranslateTransition {
                                            toY: 0
                                            duration: 500
                                        },
                                        TranslateTransition {
                                            toY: 100
                                            duration: 500
                                        },
                                        TranslateTransition {
                                            toY: 0
                                            duration: 500
                                        },
                                        TranslateTransition {
                                            toY: 100
                                            duration: 500
                                        }
                                    ]
                                    onEnded: {
                                        player.imageSource = "asset:///images/paperimg.png";
                                    }
                                }
                            ]
                rightMargin: 120.0
            }
                        ImageView {
                            id: computer
                            imageSource: "asset:///images/rockimg.png"
                            animations: [
                                SequentialAnimation {
                                    id: computermove
                                    animations: [
                                        TranslateTransition {
                                            toY: 100
                                            duration: 500
                                        },
                                        TranslateTransition {
                                            toY: 0
                                            duration: 500
                                        },
                                        TranslateTransition {
                                            toY: 100
                                            duration: 500
                                        },
                                        TranslateTransition {
                                            toY: 0
                                            duration: 500
                                        },
                                        TranslateTransition {
                                            toY: 100
                                            duration: 500
                                        }
                                    ]
                                    onEnded: {
                                        computer.imageSource = "asset:///images/scissorsimg.png";
                                    }
                                }
                            ]
                leftMargin: 0.0
            }
        }

        Container{
            background: Color.create(0.8,0.8,0.8)
                        preferredHeight: 400.0
            }
        //button container
        Container {

            
            layout: StackLayout {
                layoutDirection: LayoutDirection.LeftToRight
            }


            Button {
                id: rockbutton
                objectName: "rockbutton"
                text: "ROCK!"
            onClicked: {
                player.imageSource = "asset:///images/rockimg.png";
                computer.imageSource = "asset:///images/rockimg.png";
                playermove.play();
                computermove.play();
            }

            }
            Button {
                id: paperbutton
                objectName: "paperbutton"
                text: "PAPER!"
                onClicked: {
                }
            }
            Button {
                id: scissorsbutton
                objectName: "scissorsbutton"
                text: "SCISSORS!"
                onClicked: {
                }
            }
        }
    }
}
