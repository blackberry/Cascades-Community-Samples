/* Copyright (c) 2013 Research In Motion Limited.
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
    
    titleBar: TitleBar {
        title: "Settings"
    }
    
    Container {
        layout: DockLayout {
        }
        background: Color.Black
        Container {
            layout: StackLayout {
            }
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            
            Label {
                text: "Demonstrating this Settings Page from application Menu, using NavigationPane.\n More NavigationPane Page objects can be pushed on top of this Page to allow more drill down navigation."
                multiline: true
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.fontSize: FontSize.Medium
                textStyle.color: Color.White
                textStyle.fontStyle: FontStyle.Italic
            }

        } // Container StackLyaout
    } // Container DockLayout
}
