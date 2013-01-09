/*
 * Copyright (c) 2012, 2013 Brian Scheirer
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
 import bb.cascades.advertisement 1.0

Page {
    //create aliases to expose item id to the main.qml file
    property alias win: win
    property alias lose: lose
    property alias tie: tie
    property alias winnerlabel: winnerlabel
    property alias backgroundPaint: backgroundPaint
    property alias player: player
    property alias computer: computer
    property alias playerGroup: playerGroup
    //main content container
    content: Container {
        background: backgroundPaint.imagePaint
        attachedObjects: [
            ImagePaintDefinition {
                id: backgroundPaint
                imageSource: "asset:///images/RPSbkgrdD.png"
            },
      
            Sheet {
                id: charSelectSheet
                Page {
                    Container {
                        background: Color.create("#DEB887")
                        Container {
                            horizontalAlignment: HorizontalAlignment.Center
                            Label {
                                text: "Preview"
                                textStyle.fontSize: FontSize.XLarge
                            }
                        }
                        Container {
                            background: Color.create(1, 1, 1)
                            minHeight: 550.0
                            minWidth: 700.0
                            horizontalAlignment: HorizontalAlignment.Center
                            ImageView {
                                id: previewChar
                                imageSource: "asset:///images/Knight.png"
                                horizontalAlignment: HorizontalAlignment.Center
                            }
                        }
                        Container {
                            horizontalAlignment: HorizontalAlignment.Center
                            Label {
                                text: "Character Selector"
                                verticalAlignment: VerticalAlignment.Center
                                textStyle.fontSize: FontSize.XLarge
                            }
                        }
                        Container {
                            background: Color.create(1, 1, 1)
                            maxHeight: 400.0
                            maxWidth: 700.0
                            horizontalAlignment: HorizontalAlignment.Center
                            ScrollView {
                                RadioGroup {
                                    id: playerGroup
                                    dividersVisible: false
                                    Option {
                                        text: "Knight"
                                        selected: true
                                    }
                                    Option {
                                        text: "Ninja"
                                    }
                                    Option {
                                        text: "King"
                                    }
                                    onSelectedIndexChanged: {
                                        if (playerGroup.selectedIndex == 0) {
                                            playerbody.imageSource = "asset:///images/Knight.png";
                                            previewChar.imageSource = "asset:///images/Knight.png";
                                            player.imageSource = "asset:///images/KRock.png";
                                        } else if (playerGroup.selectedIndex == 1) {
                                            playerbody.imageSource = "asset:///images/Ninja.png";
                                            previewChar.imageSource = "asset:///images/Ninja.png";
                                            player.imageSource = "asset:///images/NRock.png";
                                        } else if (playerGroup.selectedIndex == 2) {
                                            playerbody.imageSource = "asset:///images/King.png";
                                            previewChar.imageSource = "asset:///images/King.png";
                                            player.imageSource = "asset:///images/KiRock.png";
                                        }
                                        backgroundPaint.imageSource = "asset:///images/RPSbkgrdD.png";
                                        winnerlabel.text = "";
                                        computer.imageSource = "asset:///images/DRock.png";
                                    }
                                }
                            }
                        }
                        Container {
                            horizontalAlignment: HorizontalAlignment.Center
                            topPadding: 20.0
                            Button {
                                text: "Back to the Battle"
                                onClicked: charSelectSheet.close()
                            }
                        }
                    }
                }
            }
        ]
        preferredHeight: 1280.0
        preferredWidth: 768.0
        //main container for Scoreboard
        layout: StackLayout {
        }
        minHeight: 1280.0
                    Container {
                        background: Color.create("#DEB887")
                        preferredWidth: 768.0
                        horizontalAlignment: HorizontalAlignment.Center
                        minHeight: 110.0
                        topPadding: 20.0
                        Banner {
						  //  replace zoneId with your own from your Ad Services account
                            zoneId: 117145
                            refreshRate: 60
                            preferredWidth: 320
                            preferredHeight: 50
                            transitionsEnabled: true
                          //  placeHolderURL: "asset:///placeholder_728x90.png"
                            backgroundColor: Color.White
                            borderColor: Color.White
                            borderWidth: 2
                            horizontalAlignment: HorizontalAlignment.Center
                            scaleX: 2.0
                            scaleY: 2.0
                            
                        }
                    }
        Container {
            background: Color.create("#DEB887")
            //subcontainer of scoreboard for top row
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                minWidth: 768.0
                Container {
                    Label {
                        text: "  Scoreboard"
                        textStyle.fontSizeValue: 10.0
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        minWidth: 590.0
                        Label {
                            text: "  Win: "
                            textStyle.fontSizeValue: 10.0
                        }
                        Label {
                            id: win
                            text: "0"
                            textStyle.fontSizeValue: 10.0
                        }
                        Label {
                            text: " Loss: "
                            textStyle.fontSizeValue: 10.0
                        }
                        Label {
                            id: lose
                            text: "0"
                            textStyle.fontSizeValue: 10.0
                        }
                        Label {
                            text: " Tie: "
                            textStyle.fontSizeValue: 10.0
                        }
                        Label {
                            id: tie
                            text: "0"
                            textStyle.fontSizeValue: 10.0
                        }
                    }
                }

            }
        }
        //winner label container
        Container {
            layoutProperties: StackLayoutProperties {
            }
            topMargin: 75.0
            preferredHeight: 225.0
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            Label {
                id: winnerlabel
                text: ""
                textStyle.fontSize: FontSize.XXLarge
            }
        }
        //character image container **2**
        Container {
            layout: DockLayout {
            }
            gestureHandlers: [
                DoubleTapHandler {
                    onDoubleTapped: {
                        charSelectSheet.open();
                    }
                }
            ]
            //container with hand images of the characters **3**
            Container {
                preferredWidth: 720.0
                preferredHeight: 550.0
                layout: AbsoluteLayout {
                }
                ImageView {
                    id: player
                    imageSource: "asset:///images/KRock.png"
                    animations: [
                        SequentialAnimation {
                            id: playerrock
                            animations: [
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                }
                            ]
                            onEnded: {
                                if (playerGroup.selectedIndex == 0) {
                                    player.imageSource = "asset:///images/KRock.png";
                                } else if (playerGroup.selectedIndex == 1) {
                                    player.imageSource = "asset:///images/NRock.png";
                                } else if (playerGroup.selectedIndex == 2) {
                                    player.imageSource = "asset:///images/KiRock.png";
                                }
                                var winning = win.text;
                                var losing = lose.text;
                                var ties = tie.text;
                                var number = Math.floor((Math.random() * 3) + 1);
                                var gameoutcome = rpscp.clickedRock(1, number);
                                if (number == 1) {
                                    computer.imageSource = "asset:///images/DRock.png";
                                } else if (number == 2) {
                                    computer.imageSource = "asset:///images/DPaper.png";
                                } else {
                                    computer.imageSource = "asset:///images/DScissors.png";
                                }
                                if (gameoutcome == 1) {
                                    winnerlabel.text = "You WIN!";
                                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdW.png";
                                } else if (gameoutcome == 2) {
                                    winnerlabel.text = "Dragon WINS!";
                                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdL.png";
                                } else {
                                    winnerlabel.text = "Draw";
                                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdD.png";
                                }
                                if (gameoutcome == 1) {
                                    win.text = rpscp.codeWinning(winning);
                                } else if (gameoutcome == 2) {
                                    lose.text = rpscp.codeWinning(losing);
                                } else {
                                    tie.text = rpscp.codeWinning(ties);
                                }
                                rockbutton.enabled = true;
                                paperbutton.enabled = true;
                                scissorsbutton.enabled = true;
                            }
                        },
                        SequentialAnimation {
                            id: playerpaper
                            animations: [
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                }
                            ]
                            onEnded: {
                                if (playerGroup.selectedIndex == 0) {
                                    player.imageSource = "asset:///images/KPaper.png";
                                } else if (playerGroup.selectedIndex == 1) {
                                    player.imageSource = "asset:///images/NPaper.png";
                                } else if (playerGroup.selectedIndex == 2) {
                                    player.imageSource = "asset:///images/KiPaper.png";
                                }
                                var winning = win.text;
                                var losing = lose.text;
                                var ties = tie.text;
                                var number = Math.floor((Math.random() * 3) + 1);
                                var gameoutcome = rpscp.clickedPaper(2, number);
                                if (number == 1) {
                                    computer.imageSource = "asset:///images/DRock.png";
                                } else if (number == 2) {
                                    computer.imageSource = "asset:///images/DPaper.png";
                                } else {
                                    computer.imageSource = "asset:///images/DScissors.png";
                                }
                                if (gameoutcome == 1) {
                                    winnerlabel.text = "You WIN!";
                                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdW.png";
                                } else if (gameoutcome == 2) {
                                    winnerlabel.text = "Dragon WINS!";
                                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdL.png";
                                } else {
                                    winnerlabel.text = "Draw";
                                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdD.png";
                                }
                                if (gameoutcome == 1) {
                                    win.text = rpscp.codeWinning(winning);
                                } else if (gameoutcome == 2) {
                                    lose.text = rpscp.codeWinning(losing);
                                } else {
                                    tie.text = rpscp.codeWinning(ties);
                                }
                                rockbutton.enabled = true;
                                paperbutton.enabled = true;
                                scissorsbutton.enabled = true;
                            }
                        },
                        SequentialAnimation {
                            id: playerscissors
                            animations: [
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                }
                            ]
                            onEnded: {
                                if (playerGroup.selectedIndex == 0) {
                                    player.imageSource = "asset:///images/KScissors.png";
                                } else if (playerGroup.selectedIndex == 1) {
                                    player.imageSource = "asset:///images/NScissors.png";
                                } else if (playerGroup.selectedIndex == 2) {
                                    player.imageSource = "asset:///images/KiScissors.png";
                                }

                                // player.imageSource = "asset:///images/KScissors.png";
                                var winning = win.text;
                                var losing = lose.text;
                                var ties = tie.text;
                                var number = Math.floor((Math.random() * 3) + 1);
                                var gameoutcome = rpscp.clickedScissors(3, number);
                                if (number == 1) {
                                    computer.imageSource = "asset:///images/DRock.png";
                                } else if (number == 2) {
                                    computer.imageSource = "asset:///images/DPaper.png";
                                } else {
                                    computer.imageSource = "asset:///images/DScissors.png";
                                }
                                if (gameoutcome == 1) {
                                    winnerlabel.text = "You WIN!";
                                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdW.png";
                                } else if (gameoutcome == 2) {
                                    winnerlabel.text = "Dragon WINS!";
                                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdL.png";
                                } else {
                                    winnerlabel.text = "Draw";
                                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdD.png";
                                }
                                if (gameoutcome == 1) {
                                    win.text = rpscp.codeWinning(winning);
                                } else if (gameoutcome == 2) {
                                    lose.text = rpscp.codeWinning(losing);
                                } else {
                                    tie.text = rpscp.codeWinning(ties);
                                }
                                rockbutton.enabled = true;
                                paperbutton.enabled = true;
                                scissorsbutton.enabled = true;
                            }
                        }
                    ]
                    translationX: 150.0
                    translationY: 200.0
                }
                ImageView {
                    id: computer
                    imageSource: "asset:///images/DRock.png"
                    animations: [
                        SequentialAnimation {
                            id: computermove
                            animations: [
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 200
                                    duration: 300
                                },
                                TranslateTransition {
                                    toY: 250
                                    duration: 300
                                }
                            ]
                        }
                    ]
                    leftMargin: 0.0
                    translationY: 200.0
                    translationX: 350.0
                }
            }
            //container with charater bodies
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                preferredWidth: 720.0
                layoutProperties: StackLayoutProperties {
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                ImageView {
                    id: playerbody
                    imageSource: "asset:///images/Knight.png"
                    rightMargin: 120.0
                }
                ImageView {
                    id: computerbody
                    imageSource: "asset:///images/Dragon.png"
                }
            }
        }
        //extra container for spacing purposes
        Container {
            preferredHeight: 50.0
        }
        //button container
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: rockbutton
                objectName: "rockbutton"
                text: "ROCK!"
                onClicked: {
                    rockbutton.enabled = false;
                    paperbutton.enabled = false;
                    scissorsbutton.enabled = false;
                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdD.png";
                    winnerlabel.text = "";
                    if (playerGroup.selectedIndex == 0) {
                        player.imageSource = "asset:///images/KRock.png";
                    } else if (playerGroup.selectedIndex == 1) {
                        player.imageSource = "asset:///images/NRock.png";
                    } else if (playerGroup.selectedIndex == 2) {
                        player.imageSource = "asset:///images/KiRock.png";
                    }
                    computer.imageSource = "asset:///images/DRock.png";
                    playerrock.play();
                    computermove.play();
                }
            }
            Button {
                id: paperbutton
                objectName: "paperbutton"
                text: "PAPER!"
                onClicked: {
                    rockbutton.enabled = false;
                    paperbutton.enabled = false;
                    scissorsbutton.enabled = false;
                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdD.png";
                    winnerlabel.text = "";
                    if (playerGroup.selectedIndex == 0) {
                        player.imageSource = "asset:///images/KRock.png";
                    } else if (playerGroup.selectedIndex == 1) {
                        player.imageSource = "asset:///images/NRock.png";
                    } else if (playerGroup.selectedIndex == 2) {
                        player.imageSource = "asset:///images/KiRock.png";
                    }
                    computer.imageSource = "asset:///images/DRock.png";
                    playerpaper.play();
                    computermove.play();
                }
            }
            Button {
                id: scissorsbutton
                objectName: "scissorsbutton"
                text: "SCISSORS!"
                onClicked: {
                    rockbutton.enabled = false;
                    paperbutton.enabled = false;
                    scissorsbutton.enabled = false;
                    backgroundPaint.imageSource = "asset:///images/RPSbkgrdD.png";
                    winnerlabel.text = "";
                    if (playerGroup.selectedIndex == 0) {
                        player.imageSource = "asset:///images/KRock.png";
                    } else if (playerGroup.selectedIndex == 1) {
                        player.imageSource = "asset:///images/NRock.png";
                    } else if (playerGroup.selectedIndex == 2) {
                        player.imageSource = "asset:///images/KiRock.png";
                    }
                    computer.imageSource = "asset:///images/DRock.png";
                    playerscissors.play();
                    computermove.play();
                }
            }
        }
    }
}
