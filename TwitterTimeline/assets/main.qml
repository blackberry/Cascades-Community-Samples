/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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
    content: Container {
        layout : DockLayout {   
        }
        Container {
            id : screenInfo
            visible : true
            layout : StackLayout {
                layoutDirection : LayoutDirection.TopToBottom
            }
            layoutProperties : DockLayoutProperties {
                verticalAlignment : VerticalAlignment.Top
            }
            Label {
                text : "Enter a twitter screen name: "
            }
            TextArea {
                id : screenName
                text : "BlackBerryDev"
            }
            Button {
                objectName : "timelineBtn"
                id : timelineBtn
                layoutProperties : StackLayoutProperties {
                    horizontalAlignment : HorizontalAlignment.Center
                }
                text : "Timeline"
                onClicked: {
                    timelineBtn.enabled = false;
                    errorLabel.text = "";
                    app.reset();
                    app.getTimeline(screenName.text);
                }
            }
        }
        Container {
            objectName : "failureView"
            id : failureView
            layoutProperties : DockLayoutProperties {
                verticalAlignment : VerticalAlignment.Center
            }
            TextArea {
                objectName : "errorLabel"
                id : errorLabel
                editable : false
                backgroundVisible : false
                textStyle { base: SystemDefaults.TextStyles.BigText; color: Color.White }           
            }	                
        }
        Container {
            layoutProperties : DockLayoutProperties {
                horizontalAlignment : HorizontalAlignment.Center
                verticalAlignment : VerticalAlignment.Bottom
            }
            Label { text : "Select a ListView type for displaying the twitter feed" }
            SegmentedControl {
                id : viewTypes
                objectName : "viewTypes"
                Option {
                    id: basicView
                    text : "Standard"
                    value : "basic"
                    selected: true
                }
                Option {
                    id: richView
                    text : "Custom"
                    value : "rich"
                }
                onSelectedIndexChanged: {
                     app.changeView(viewTypes.selectedValue());
                }
            }
        }
    }
}
