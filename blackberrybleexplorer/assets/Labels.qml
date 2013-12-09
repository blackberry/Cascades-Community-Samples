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
    property string attr1_text: ""
    property string attr2_text: ""
    property string attr3_text: ""
    property bool attr3_visible: false
    property variant text_color: Color.DarkBlue
    
    id: label_area
    layout: StackLayout {
    }
    horizontalAlignment: HorizontalAlignment.Center
    verticalAlignment: VerticalAlignment.Bottom
    Label {
        id: lbl_attr1
        text: attr1_text
        visible: true
        horizontalAlignment: HorizontalAlignment.Center
        textStyle {
            base: SystemDefaults.TextStyles.TitleText
            color: text_color
        }
    }
    Label {
        id: lbl_attr2
        visible: true
        text: attr2_text
        horizontalAlignment: HorizontalAlignment.Center
        textStyle {
            base: SystemDefaults.TextStyles.TitleText
            color: text_color
        }
    }
    Label {
        id: lbl_attr3
        visible: attr3_visible
        text: attr3_text
        horizontalAlignment: HorizontalAlignment.Center
        textStyle {
            base: SystemDefaults.TextStyles.TitleText
            color: text_color
        }
    }
}
