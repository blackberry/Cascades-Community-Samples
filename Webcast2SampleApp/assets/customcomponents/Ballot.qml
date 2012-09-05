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

// This class is a simple test case for the custom ImageBallotComponent class

Page {
    content: Container {
        background: Color.create ("#FF8C00")
        ImageBallotComponent {
            titleText: "Vote for Garett"
            descriptionText: "He will make a great mayor and promises more Cascades webcasts if elected!"
            imageSource: "asset:///images/ballotimages/garettscaled.png"
            //Set the selected and active elements to be controlled by the ToggleButtons below 
            selected: selected.checked
            active: active.checked
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.LeftToRight
            }
            Label {
                text: "Ballot Selected: "
            }
            ToggleButton {
                id: selected
            }
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.LeftToRight
            }
            Label {
                text: "Ballot Active: "
            }
            ToggleButton {
                id: active
            }
        }
    }
}
