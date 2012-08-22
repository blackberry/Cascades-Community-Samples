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

Page {
    content: Container {
        background: Color.create ("#87CEFA")
        
        //The StackLayout has a few other attributes we will not be covering in detail, but the
        //Padding attributes used here basically allow a minimum space between the edge of the 
        //Container and its inner elements.
        layout: StackLayout {
            topPadding: 10
            leftPadding: 10
            rightPadding: 10
        }
        
        //This DropDown lists the various options for a StackLayout's LayoutDirection. When
        //the user changes their selection we simply apply the new LayoutDirection to the 
        //layoutBlocks container which will animate the blocks in to place, implicitely
        DropDown {
            id: stackLayoutChoices
            title: "Layout Options"
            Option {
                text: "Left to Right"
                value: "LeftToRight"
            }
            Option {
                text: "Right to Left"
                value: "RightToLeft"
            }
            Option {
                text: "Top to Bottom"
                value: "TopToBottom"
            }
            Option {
                text: "Bottom to Top"
                value: "BottomToTop"
            }
            onSelectedIndexChanged: {
                var selectedOption = stackLayoutChoices.at (selectedIndex);
                currentLayoutTitle.text = selectedOption.text;
                if (selectedOption.value == "LeftToRight") {
                    layoutBlocks.layout.layoutDirection = LayoutDirection.LeftToRight;
                } else if (selectedOption.value == "RightToLeft") {
                    layoutBlocks.layout.layoutDirection = LayoutDirection.RightToLeft;
                } else if (selectedOption.value == "TopToBottom") {
                    layoutBlocks.layout.layoutDirection = LayoutDirection.TopToBottom;
                } else if (selectedOption.value == "BottomToTop") {
                    layoutBlocks.layout.layoutDirection = LayoutDirection.BottomToTop;
                }
            }
        }
        Container {
            layout: DockLayout {
            }
            preferredWidth: maxWidth
            preferredHeight: 700
            Container {
                layoutProperties: DockLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
                Label {
                    id: currentLayoutTitle
                    text: "Top to Bottom"
                    textStyle {
                        color: Color.White
                    }
                }
                
                //This is the Container which will be manipulated by the user selection from the above
                //DropDown. By default this will begin with a TopToBottom LayourDirection.
                Container {
                    id: layoutBlocks
                    layout: StackLayout {
                    }
                    LayoutBlock {
                        text: "1"
                    }
                    LayoutBlock {
                        text: "2"
                    }
                    LayoutBlock {
                        text: "3"
                    }
                }
            }
        }
    }
}
