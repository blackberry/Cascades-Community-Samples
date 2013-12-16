/* Copyright (c) 2013 BlackBerry Limited.
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

Container {

    function getActivityText() {
        return "Connecting to .. " + charPage.selectedServiceName + " on .. " + charPage.selectedDeviceName;
    }

    horizontalAlignment: HorizontalAlignment.Fill

    layout: DockLayout {
    }

    Label {
        horizontalAlignment: HorizontalAlignment.Left
        verticalAlignment: VerticalAlignment.Top
        multiline: false
        textStyle {
            fontWeight: FontWeight.Bold
            fontSize: FontSize.Large
            color: Color.create("#ffff00"); // yellow
        }
        text: charPage.statusMessage
    }
}