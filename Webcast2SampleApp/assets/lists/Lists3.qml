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
//Import the customcomponents directory so we can reference the ImageBallotComponent
// component used in the Ballot sample
import "../customcomponents" 

Page {
    content: Container {
        background: Color.create(0.4, 0.5, 0.5)
        ListView {
            dataModel: XmlDataModel {
                id: dataModel
                source: "../models/samplenominees.xml"
            }
            listItemComponents: [
                ListItemComponent {
                    type: "header"
                    Container {
                        background: Color.Gray
                        preferredWidth: maxWidth
                        Label {
                            text: ListItemData.title
                            textStyle {
                                color: Color.White
                                fontWeight: FontWeight.Bold
                                base: blackBoldStyle.style
                            }
                        }
                    }
                },
                ListItemComponent {
                    //If the List Item type is "header" then we will use a ImageBallotComponent  
                    // to represent the information
                    type: "sample"
                    ImageBallotComponent {
                        titleText: ListItemData.title
                        descriptionText: ListItemData.description
                        imageSource: "../images/ballotimages/" + ListItemData.imageName
                        //If this element is selected we set the 'selected' property of the 
                        // ImageBallotComponent respectively
                        selected: ListItem.selected
                    }
                }
            ]
        }
    }
}
