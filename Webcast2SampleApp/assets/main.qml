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

//Import each of the directories containing the samples
import "animations"
import "customcomponents"
import "lists"

//Display each pane of samples in a different Tab of our TabbedPane

//We have only 3 panes to display, so the TabbedPane is a good 

//navigation mechanism

TabbedPane {
    activeTab: animationsTab
    showTabsOnActionBar: true
    Tab {
        id: animationsTab
        title: "Animations"
        imageSource: "asset:///images/icons/animationicon2.png"
        Animations {
        }
    }
    Tab {
        id: customComponentsTab
        title: "Custom Components"
        imageSource: "asset:///images/icons/customcontrol1.png"
        CustomControls {
        }
    }
    Tab {
        id: listsTab
        title: "Lists"
        imageSource: "asset:///images/icons/listsicon2.png"
        Lists {
        }
    }
}
