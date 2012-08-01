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

Container {
    id: container
    property alias label: labelPart.text
    property alias text: textPart.text    
    
    layout: StackLayout {
        layoutDirection: LayoutDirection.LeftToRight
    }
    Label {
        id: labelPart
        textStyle {
            base: SystemDefaults.TextStyles.BodyText
            fontWeight: FontWeight.Bold
            color: Color.Gray
        }
    }
    Label {
        id: textPart
        textStyle {
            base: SystemDefaults.TextStyles.BodyText            
            color: Color.White
        }
    }
}
