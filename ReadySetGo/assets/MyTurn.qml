/*
 * Copyright (c) 2014 BlackBerry Limited.
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

import bb.cascades 1.4

Page {
    property alias code: cont.code
    shortcuts: [
        DeviceShortcut {
            //todo: change this to whatever the hardkey will be
            type: DeviceShortcuts.SendTap
            onTriggered: {
                word.text = "SEND"
                cont.background = Color.Green
                cont.entry = cont.entry + "1";
                cont.checkForWin(); 
                
            }
        },
        DeviceShortcut {
            //todo: change this to whatever the hardkey will be
            type: DeviceShortcuts.MenuTap
            onTriggered: {
                word.text = "MENU"
                cont.background = Color.White
                cont.entry = cont.entry + "2";
                cont.checkForWin(); 
            }
        },
        DeviceShortcut {
            //todo: change this to whatever the hardkey will be
            type: DeviceShortcuts.BackTap
            onTriggered: {
                word.text = "BACK"
                cont.background = Color.Gray
                cont.entry = cont.entry + "3";
                cont.checkForWin(); 
            }
        },
        DeviceShortcut {
            //todo: change this to whatever the hardkey will be
            type: DeviceShortcuts.EndTap
            onTriggered: {
                word.text = "END"
                cont.background = Color.Red
                cont.entry = cont.entry + "4";
                cont.checkForWin(); 
            }
        }
    ]

    Container {
        id: cont
        background: Color.Yellow
        property string code
        property string entry: ""

        layout: DockLayout {
        }
        
        function checkForWin() {
            if (code == entry) {
                background = Color.Yellow
                word.text = "YOU WIN!!"
            } else if (code.length == entry.length) {
                background = Color.Yellow
                word.text = "YOU LOSE!!"                
            }    
        }
        Label {
            id: word
            text: "GO FOR IT!"

            textStyle.fontSize: FontSize.XXLarge
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

        }
        
        bottomPadding: 50
        Button {
            text: "Play Again"
            onClicked: {
                nav.pop();
                nav.pop();
            }
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Bottom
        }
        

    }
    actionBarVisibility: ChromeVisibility.Hidden
     
}
