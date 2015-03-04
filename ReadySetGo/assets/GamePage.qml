/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

import bb.cascades 1.2
import app.neededstuff 1.0
Page {
    id: gamepage
    property alias level: cont.level
    actionBarVisibility: ChromeVisibility.Hidden
    Container {
        id: cont

        property int level
        property int round: 0
        property string code: ""

        layout: DockLayout {
        }

        background: Color.Yellow

        Label {
            id: word
            text: "GET READY..."
            
            textStyle.fontSize: FontSize.XXLarge
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

        }

        onLevelChanged: {
            var s = "";
            var i = 0;

            for (i = 0; i < level; i ++) {
                //// Choose a random number between 1 and 4.
                var c = Math.ceil(Math.random() * 4);
                
                //...and make sure its not the same as previous one.
                while ((i > 0) && (c.toString() == s.charAt(i-1))) {
                    
                    c = Math.ceil(Math.random() * 4);
                }
                s = s + c;
            }
            console.log("len " + level);
            console.log("s  " + s);
            code = s;
            timer.start();
        }

        function doTheShow() {
            if (code.charAt(round) == '1') {
                background = Color.Green;
                word.text = "SEND";
            } else if (code.charAt(round) == '2') {
                background = Color.White;
                word.text = "MENU";
            } else if (code.charAt(round) == '3') {
                background = Color.Gray;
                word.text = "BACK";
            }else if (code.charAt(round) == '4') {
                background = Color.Red;
                word.text = "END";
            }
            
            round ++;
            if (round == level) {
                timer.stop();
                
                gamepage.addAction(myTurnAction);
                gamepage.actionBarVisibility = ChromeVisibility.Visible
            }
        }
        attachedObjects: [
            ActionItem {
                id: myTurnAction
                title: "My Turn"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    var mt = myturn.createObject();
                    mt.code = cont.code;
                    nav.push(mt);
                }
            },
            QTimer {
                id: timer
                interval: 1500
                onTimeout: {
                    console.log("Timer triggered ");
                    cont.doTheShow();
                }
            }, 
            ComponentDefinition {
                id: myturn
                source: "MyTurn.qml"
            }
        ]
    }
}
