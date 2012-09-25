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
        Container {
            Button {
                id: rockbutton
                objectName: "rockbutton"
                text: "ROCK!"
                onClicked: {
                    var winning = win.text;
                    var losing = lose.text;
                    var ties = tie.text;
                    var number = Math.floor((Math.random() * 3) + 1);
                    var gameoutcome = app.clickedRock(1, number);
                    if (number == 1) {
                        computerpick.text = "BlackBerry picks: Rock";
                    } else if (number == 2) {
                        computerpick.text = "BlackBerry picks: Paper";
                    } else {
                        computerpick.text = "BlackBerry picks: Scissors";
                    }
                    if (gameoutcome == 1) {
                        winnerlabel.text = "You WIN!";
                    } else if (gameoutcome == 2) {
                        winnerlabel.text = "BlackBerry WINS!";
                    } else {
                        winnerlabel.text = "Draw";
                    }
                    if (gameoutcome == 1) {
                        win.text = app.codeWinning(winning);
                    } else if (gameoutcome == 2) {
                        lose.text = app.codeWinning(losing);
                    } else {
                        tie.text = app.codeWinning(ties);
                    }
                }
            }
            Button {
                id: paperbutton
                objectName: "paperbutton"
                text: "PAPER!"
                onClicked: {
                    var winning = win.text;
                    var losing = lose.text;
                    var ties = tie.text;
                    var number = Math.floor((Math.random() * 3) + 1);
                    var gameoutcome = app.clickedPaper(2, number);
                    if (number == 1) {
                        computerpick.text = "BlackBerry picks: Rock";
                    } else if (number == 2) {
                        computerpick.text = "BlackBerry picks: Paper";
                    } else {
                        computerpick.text = "BlackBerry picks: Scissors";
                    }
                    if (gameoutcome == 1) {
                        winnerlabel.text = "You WIN!";
                    } else if (gameoutcome == 2) {
                        winnerlabel.text = "BlackBerry WINS!";
                    } else {
                        winnerlabel.text = "Draw";
                    }
                    if (gameoutcome == 1) {
                        win.text = app.codeWinning(winning);
                    } else if (gameoutcome == 2) {
                        lose.text = app.codeWinning(losing);
                    } else {
                        tie.text = app.codeWinning(ties);
                    }
                }
            }
            Button {
                id: scissorsbutton
                objectName: "scissorsbutton"
                text: "SCISSORS!"
                onClicked: {
                    var winning = win.text;
                    var losing = lose.text;
                    var ties = tie.text;
                    var number = Math.floor((Math.random() * 3) + 1);
                    var gameoutcome = app.clickedScissors(3, number);
                    if (number == 1) {
                        computerpick.text = "BlackBerry picks: Rock";
                    } else if (number == 2) {
                        computerpick.text = "BlackBerry picks: Paper";
                    } else {
                        computerpick.text = "BlackBerry picks: Scissors";
                    }
                    if (gameoutcome == 1) {
                        winnerlabel.text = "You WIN!";
                    } else if (gameoutcome == 2) {
                        winnerlabel.text = "BlackBerry WINS!";
                    } else {
                        winnerlabel.text = "Draw";
                    }
                    if (gameoutcome == 1) {
                        win.text = app.codeWinning(winning);
                    } else if (gameoutcome == 2) {
                        lose.text = app.codeWinning(losing);
                    } else {
                        tie.text = app.codeWinning(ties);
                    }
                }
            }
        }
        Container {
            topMargin: 50.0
            Label {
                id: computerpick
                text: "BlackBerry picks: ..."
            }
            Label {
                id: winnerlabel
                text: "Winner"
            }
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.LeftToRight
            }
            Label {
                text: "Record: "
            }
            Label {
                id: win
                text: "0"
            }
            Label {
                text: "-"
            }
            Label {
                id: lose
                text: "0"
            }
            Label {
                text: "-"
            }
            Label {
                id: tie
                text: "0"
            }
        }
    }
}
