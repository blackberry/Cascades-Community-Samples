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

/* StatusField is used to provide graphical feedback to the user
 * on the status of various calls/registrations made by the 
 * application.
 */

Container {
    property alias processLabel: process.text
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    topMargin: 10
    bottomMargin: 10
    Label {
        id: process
        preferredWidth: 600
        verticalAlignment: VerticalAlignment.Center
    }
    
    // By default we use a generic disabled image as we don't know
    // the status before the application starts
    ImageView {
        id: pic
        imageSource: "asset:///images/disableStatus.png"
        verticalAlignment: VerticalAlignment.Center
    }
    
    // Exposed slot able to be updated to change the status icon
    function setState(state) {
        if (state) {
            pic.imageSource = "asset:///images/goodStatus.png"
        } else {
            pic.imageSource = "asset:///images/badStatus.png";
        }
    }
}
