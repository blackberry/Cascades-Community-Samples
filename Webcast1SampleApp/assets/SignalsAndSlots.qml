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
        background: Color.create (red.liveValue, green.liveValue, blue.liveValue)
        Label {
            text: "Red: " + red.value.toFixed (3)
            textStyle.color: Color.White
        }
        
        //For each of the Sliders defined in this sample we will be using their
        //Signal Handler onValueChanging to listen for any time that the 
        //valueChanging() signal is fired
        Slider {
            id: red
            property real liveValue: value
            toValue: 1.0
            value: 0.529
            onValueChanging: {
                liveValue = value
            }
        }
        Label {
            text: "Green: " + green.value.toFixed (3)
            textStyle.color: Color.White
        }
        Slider {
            id: green
            property real liveValue: value
            toValue: 1.0
            value: 0.807
            onValueChanging: {
                liveValue = value
            }
        }
        Label {
            text: "Blue: " + blue.value.toFixed (3)
            textStyle.color: Color.White
        }
        Slider {
            id: blue
            property real liveValue: value
            toValue: 1.0
            value: 0.980
            onValueChanging: {
                liveValue = value
            }
        }
        Container {
            layout: DockLayout {
            }
            preferredWidth: maxWidth
            
            //When this button is clickd we will call the resetValue() clot on each
            //of the Sliders defined
            Button {
                layoutProperties: DockLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                text: "Reset Sliders"
                
                //onClicked is the Signal handler that gets called any time the Button's
                //clicked() Signal is fired
                onClicked: {
                    red.resetValue ();
                    green.resetValue ();
                    blue.resetValue ();
                }
            }
        }
    }
}
