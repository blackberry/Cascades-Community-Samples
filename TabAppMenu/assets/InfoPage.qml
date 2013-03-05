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
import bb.cascades.pickers 1.0

Page {
    id: infoPage

    // Custom signal for notifying that this page needs to be closed
    signal done ()
    
    titleBar: TitleBar {
        title: "Info"
        dismissAction: ActionItem {
            title: "Close"
            onTriggered: {
                // Emit the custom signal here to indicate that this page needs to be closed
                // The signal would be handled by the page which invoked it
                infoPage.done();
            }
        }
    }
    
    Container {
        layout: DockLayout {
        }
        Container {
            layout: StackLayout {
            }
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            Label {
                text: "<html><a href=\"https://github.com/blackberry/Cascades-Community-Samples/tree/master/TabAppMenu\">Github Source Code In This Link</a></html>"
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.fontSize: FontSize.XXSmall
            }
            TextArea {
                text: "This is a simple Cascades sample app for developers only.\n\n" + "The purpose of this app is to demonstrate how to correctly use the following Cascades components together in an app:\n\n" +
                 "TabbedPane, Tab, application Menu, Sheet, NavigationPane\n\n" + 
                "  Author:\n\n" + 
                "  Shadab Rashid\n" + 
                "  Application Development Consultant, Developer Relations\n" + 
                "  BlackBerry\n"
                
                editable: false
                scrollMode: TextAreaScrollMode.Stiff
                textStyle.fontSize: FontSize.XXSmall
                textStyle.fontStyle: FontStyle.Italic
                textStyle.textAlign: TextAlign.Justify
                textStyle.color: Color.DarkRed
            }
            Label {
                text: "<html><a href=\"https://twitter.com/surashid\">@surashid</a></html>"
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.fontSize: FontSize.XXSmall
            }
        } // Container StackLyaout
    } // Container DockLayout
    
} // Page