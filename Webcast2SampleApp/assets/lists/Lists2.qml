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

//In this example the listItemComponents attribute it used to define how each
// ListItemComponent will be created

Page {
    content: Container {
        background: Color.create(0.2, 0.4, 0.5)
        ListView {
            dataModel: XmlDataModel {
                source: "../models/samplenominees.xml"
            }
            
            listItemComponents: [
                //See assets/models/samplenominees.xml
                ListItemComponent {
                    //If the List Item type is "header" then we will use a Label to 
                    // represent the information
                    type: "header"
                    Container {
                        background: Color.Gray
                        preferredWidth: maxWidth
                        Label {
                            //The ListItemData element will allow retrieval of the data
                            // passed to each element for binding
                            text: ListItemData.title
                            textStyle {
                                base: SystemDefaults.TextStyles.BodyText
                                size: 50
                                fontWeight: FontWeight.Bold
                                color: Color.White
                            }
                        }
                    }
                },
                ListItemComponent {
                    //If the List Item type is "header" then we will use a StandardListItem  
                    // to represent the information
                    type: "sample"
                    StandardListItem {
                        title: ListItemData.title
                        description: ListItemData.description
                        imageSource: "../images/ballotimages/" + ListItemData.imageName
                    }
                }
            ]
        }
    }
}
