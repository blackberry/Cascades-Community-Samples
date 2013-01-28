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

NavigationPane {
    id: navigationPane
    backButtonsVisible: false
    Menu.definition: MenuDefinition {

        // Specify the actions that should be included in the menu
        actions: [
            ActionItem {
                title: "Help"
                //imageSource: "images/actionOneIcon.png"
                onTriggered: {
                    helpdialog.open();
                }
            },
            ActionItem {
                title: "Reset"
                onTriggered: {
                    pageDefinition.win.text = "0";
                    pageDefinition.lose.text = "0";
                    pageDefinition.tie.text = "0";
                    pageDefinition.backgroundPaint.imageSource = "asset:///images/RPSbkgrdD.png";
                    pageDefinition.winnerlabel.text = "";
                    if (pageDefinition.playerGroup.selectedIndex == 0) {
                        pageDefinition.player.imageSource = "asset:///images/KRock.png";
                    } else if (pageDefinition.playerGroup.selectedIndex == 1) {
                        pageDefinition.player.imageSource = "asset:///images/NRock.png";
                    } else if (pageDefinition.playerGroup.selectedIndex == 2) {
                        pageDefinition.player.imageSource = "asset:///images/KiRock.png";
                    }
                    pageDefinition.computer.imageSource = "asset:///images/DRock.png";
                }
            }
        ] // end of actions list
    }
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

To change characters during the battle, double tap your character to bring up the character list.


This code has been made open source available at: https://github.com/blackberry/Cascades-Community-Samples/tree/master/RockPaperScissors

Copyright (c) 2012 Brian Scheirer

Licensed under the Apache License, Version 2.0 (the \"License\"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an \"AS IS\" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License."
                                    editable: false
                                    inputMode: TextAreaInputMode.Text
                                    textStyle {
                                        fontWeight: FontWeight.Bold
                                        color: Color.Black
                                    }
                                }
                                Button {
                                    text: "Menu"
                                    onClicked: gameInfo.close()
                                    horizontalAlignment: HorizontalAlignment.Center
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
                      //  var page = pageDefinition.createObject();
                        navigationPane.push(pageDefinition);
                    }
                 

                    horizontalAlignment: HorizontalAlignment.Center
                }
                Button {
                    id: zero
                    text: "Game Info"
                    onClicked: {
                        gameInfo.open();
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                }
            }
        }
    }
                          attachedObjects: [
                            Gamescreen {
                            id: pageDefinition
                          //  source: "Gamescreen.qml"
                        },
                                    Dialog {
                                        id: helpdialog
                                        Container {
                                            preferredWidth: 768
                                            preferredHeight: 1280
                                            background: Color.create(0.0, 0.0, 0.0, 0.5)
                                            Container {
                                                maxHeight: 875.0
                                                maxWidth: 700.0
                                                background: Color.create("#DEB887")
                                                horizontalAlignment: HorizontalAlignment.Center
                                                TextArea {
                                                    text: "The evil dragon has taken the princess hostage. The only way to save her is to defeat him in an epic battle of ROCK, PAPER, SCISSORS! 
                                    
The Rules are simple:  
Rock defeats Scissors. 
Paper defeats Rock. 
Scissors defeats Paper.
                                    
To change characters during the battle, double tap your character to bring up the character list."
                                                    editable: false
                                                    textStyle.fontWeight: FontWeight.Bold
                                                }
                                                Container {
                                                    horizontalAlignment: HorizontalAlignment.Center
                                                    Button {
                                                        text: "Ok, Let's Battle!"
                                                        onClicked: {
                                                            helpdialog.close();
                                                        }
                                                    }

                                                }
                                            }
                                        }
                                    }
                    ]
}
