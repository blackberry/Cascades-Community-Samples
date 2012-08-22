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
        //Define our DockLayout to allow  placement of elements by alignment constants
        layout: DockLayout {
        }
        background: Color.create ("#87CEFA")
        Container {
            layout: DockLayout {
            }
            preferredHeight: maxHeight
            preferredWidth: maxWidth
            //LayoutBlock is the block we will be moving about the screen
            LayoutBlock {
                id: block
                layoutProperties: DockLayoutProperties {
                }
            }
        }
        Container {
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Bottom
                horizontalAlignment: HorizontalAlignment.Center
            }
            //The SegmentedControl allows several options to be displayed to the user at once
            //This control will handle the Vertical alignment of our block
            SegmentedControl {
                id: verticalAlignControl
                Option {
                    text: "Top"
                    selected: true
                }
                Option {
                    text: "Center"
                }
                Option {
                    text: "Bottom"
                }
                //When the user changes their selected option we will apply the selection
                //to the alignment of our block ImageView element
                onSelectedIndexChanged: {
                    var option = verticalAlignControl.at (selectedIndex);
                    if (option.text == "Top") {
                        block.layoutProperties.verticalAlignment = VerticalAlignment.Top;
                    } else if (option.text == "Center") {
                        block.layoutProperties.verticalAlignment = VerticalAlignment.Center;
                    } else if (option.text == "Bottom") {
                        block.layoutProperties.verticalAlignment = VerticalAlignment.Bottom;
                    }
                }
            }
            //The SegmentedControl allows several options to be displayed to the user at once
            //This control will handle the Horiontal alignment of our block
            SegmentedControl {
                id: horizontalAlignControl
                Option {
                    text: "Left"
                    selected: true
                }
                Option {
                    text: "Center"
                }
                Option {
                    text: "Right"
                }
                //When the user changes their selected option we will apply the selection
                //to the alignment of our block ImageView element
                onSelectedIndexChanged: {
                    var option = horizontalAlignControl.at (selectedIndex);
                    if (option.text == "Left") {
                        block.layoutProperties.horizontalAlignment = HorizontalAlignment.Left;
                    } else if (option.text == "Center") {
                        block.layoutProperties.horizontalAlignment = HorizontalAlignment.Center;
                    } else if (option.text == "Right") {
                        block.layoutProperties.horizontalAlignment = HorizontalAlignment.Right;
                    }
                }
            }
        }
    }
}
