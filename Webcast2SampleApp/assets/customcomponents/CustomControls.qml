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

//This class lets us push each of the CustomControl sample pages to the stack

NavigationPane {
    id: animationsPane
    TaskList {
        actions: [
            ActionItem {
                title: "Sample 2"
                imageSource: "asset:///images/icons/customcontrol2.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    animationsPane.push(ballotPage);
                }
            }
        ]
    }
    attachedObjects: [
        Ballot {
            id: ballotPage
            paneProperties: NavigationPaneProperties {
                //This overrides the back button letting us add our own icon and functionality
                backButton: ActionItem {
                    title: "First page"
                    imageSource: "asset:///images/icons/customcontrol1.png"
                    onTriggered: {
                        animationsPane.pop();
                    }
                }
            }
        }
    ]
}
