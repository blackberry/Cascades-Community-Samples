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
    layout: StackLayout {
        layoutDirection: LayoutDirection.LeftToRight
    }
    //Here we define properties and aliases to allow external access of these values
    property alias taskName: label.text
    property alias completed: checkBox.checked
    
    //Define a custom signal that implementing classes can connect
    signal taskStatusChanged (bool completed)
    CheckBox {
        id: checkBox
        onCheckedChanged: {
            //Emit the signal defined above passing in the 'checked' bool
            taskStatusChanged (checked);
        }
    }
    Label {
        id: label
    }
}
