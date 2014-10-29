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

import bb.cascades 1.4

/*
 * Ready Set Go
 * 
 * A simple memory game to demonstrate how to capture and use the belt buttons (send, menu, back, end) in your app
 * 
 */
NavigationPane {
    id: nav
    Page {
        
        Container {
            id: con
            background: Color.Yellow 
            layout: DockLayout {
            }      
            leftPadding: 50
            rightPadding: 50
            
            DropDown {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                id: leveldropdown
                title: "Level"
                Option {
                    text: "3"
                    description: "easy"
                }
                Option {
                    text: "4"
                }
                Option {
                    text: "5"
                    description: "normal"
                    selected: true
                }
                Option {
                    text: "6"
                }
                Option {
                    text: "7"
                }
                Option {
                    text: "8"
                }
                Option {
                    text: "9"
                }
                Option {
                    text: "10"
                    description: "hard"
                }
                Option {
                    text: "11"
                }
                Option {
                    text: "12"
                }
                Option {
                    text: "13"
                }
                Option {
                    text: "14"
                }

                Option {
                    text: "15"
                    description: "super hard"
                }
            }
            Label {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Bottom
                multiline: true
                text: "Remember the sequence of keys shown and then input the same sequence to win!"
            }
        }
        
        attachedObjects: [
            ComponentDefinition {
                id: gamepage
                source: "GamePage.qml"
            
            }
        ]
        actions: [
            ActionItem {
                ActionBar.placement: ActionBarPlacement.OnBar
                title: "Let's Play!"
                onTriggered: {
                    var gp = gamepage.createObject()
                    gp.level = leveldropdown.selectedOption.text
                    nav.push(gp);
                }
            }
        ]
    }
}