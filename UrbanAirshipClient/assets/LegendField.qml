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

/* This control was created to clean up main.qml a bit. The result of this file
 * is simply an image followed by some text to create the "Legend" for the status
 * updates displayed to the user.
 */

Container {
    property alias imageSource: image.imageSource
    property alias text: label.text
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    ImageView {
        id: image
    }
    Label {
        id: label
        verticalAlignment: VerticalAlignment.Center
    }
}
