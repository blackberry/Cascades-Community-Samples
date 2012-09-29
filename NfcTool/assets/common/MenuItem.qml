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

// Item component for the list presenting available recipes.

Container {
    layout: DockLayout {
    }
    Container {
        layout: DockLayout {
        }

        topPadding: 2
        bottomPadding: 2
        horizontalAlignment: HorizontalAlignment.Center
        
        // Item background image.
        ImageView {
            id: itemBackground
            imageSource: "asset:///images/white_item_bg.png"
            preferredWidth: 768
            preferredHeight: 178
        }
        Container {
            id: highlightContainer
            background: Color.create ("#75b5d3")
            opacity: 0.0
            preferredWidth: 760
            preferredHeight: 166
            horizontalAlignment: HorizontalAlignment.Center
        }
   
   //                topPadding: 20
   //                leftPadding: 20
   //                horizontalAlignment: HorizontalAlignment.Left
   //                verticalAlignment: VerticalAlignment.Top
        
        // The Item content an image and a text.
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            leftPadding: 3
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Center

            ImageView {
                // The image is bound to the data in models/recipemodel.xml image attribute.
                imageSource: ListItemData.image
                leftMargin: 3
            }
            Label {
                // The title is bound to the data in models/recipemodel.xml title attribute.
                text: ListItemData.title
                leftMargin: 30
                textStyle {
                    base: SystemDefaults.TextStyles.TitleText
                    color: Color.Black
                }
                verticalAlignment: VerticalAlignment.Center
            }
        }
    }
    function setHighlight (highlighted) {
        if (highlighted) {
            highlightContainer.opacity = 0.9;
        } else {
            highlightContainer.opacity = 0.0;
        }
    }

    ListItem.onActivationChanged: {
        setHighlight (ListItem.active);
    }
    ListItem.onSelectionChanged: {
        setHighlight (ListItem.selected);
    }

}
