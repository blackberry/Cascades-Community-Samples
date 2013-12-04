/*
 * Copyright (c) 2013 BlackBerry Limited.
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
// Default empty project template
import bb.cascades 1.0

// creates one page with a label
Page {
    Container {
        topPadding: 50
        
        layout: StackLayout {   
        }
        
        
        Label {
            text: qsTr("When I shake the device first:")
            horizontalAlignment: HorizontalAlignment.Center
        }

        Container {
            leftPadding: 10
            rightPadding: 10
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            DropDown {
                
                onSelectedIndexChanged: {
                    _color.step1Color = selectedIndex
                }

                Option {
                    text: "Blink Red"
                    value: "red"
                }
                Option {
                    text: "Blink Green"
                    value: "green"
                }
                Option {
                    text: "Blink Blue"
                    value: "blue"
                }
            }
            DropDown {
                
                onSelectedValueChanged: {
                    _color.step1Seconds = selectedValue
                }

                Option {
                    text: "for 1 Second"
                    value: 1
                }
                Option {
                    text: "for 2 Second"
                    value: 2
                }
                Option {
                    text: "for 3 Second"
                    value: 3
                }
            }
        }
        Label {
            text: qsTr("then:")
            horizontalAlignment: HorizontalAlignment.Center
        }
        Container {
            leftPadding: 10
            rightPadding: 10
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            DropDown {
                onSelectedIndexChanged: {
                    _color.step2Color = selectedIndex
                }
                
                Option {
                    text: "Blink Red"
                    value: "Red"
                }
                Option {
                    text: "Blink Green"
                    value: "Green"
                }
                Option {
                    text: "Blink Blue"
                    value: "Blue"
                }
            }
            DropDown {
                onSelectedValueChanged: {
                    _color.step2Seconds = selectedValue
                }
                
                Option {
                    text: "for 1 Second"
                    value: 1
                }
                Option {
                    text: "for 2 Second"
                    value: 2
                }
                Option {
                    text: "for 3 Second"
                    value: 3
                }
            }
        }
        Label {
            text: qsTr("and finally:")
            horizontalAlignment: HorizontalAlignment.Center
        }
        Container {
            leftPadding: 10
            rightPadding: 10
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            DropDown {
                onSelectedIndexChanged: {
                    _color.step3Color = selectedIndex
                }
                
                Option {
                    text: "Blink Red"
                    value: "Red"
                }
                Option {
                    text: "Blink Green"
                    value: "Green"
                }
                Option {
                    text: "Blink Blue"
                    value: "Blue"
                }
            }
            DropDown {
                onSelectedValueChanged: {
                    _color.step3Seconds = selectedValue
                }
                
                Option {
                    text: "for 1 Second"
                    value: 1
                }
                Option {
                    text: "for 2 Second"
                    value: 2
                }
                Option {
                    text: "for 3 Second"
                    value: 3
                }
            }
        }

        Container {

            horizontalAlignment: HorizontalAlignment.Center
            topPadding: 50
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                text: qsTr("Stop the Blinking!")
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    // Submit
                    _color.clear()
                }
            }

            Button {
                text: qsTr("Submit")
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    // Submit
                    _color.done()
                }
            }
        }
    }
}

