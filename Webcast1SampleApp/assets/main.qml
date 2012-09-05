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

// Importing all our cascades functions.
import bb.cascades 1.0

//Our root element will be a NavigationPane so we are able to push the other samples to the screen
NavigationPane {
    id: appPane
    Page {
        id: mainPage
        // A container is used to gather visual items together.
        content: Container {
            
            //This ListView will be the base of our application, allowing us
            //to launch all samples from this main page
            ListView {
                dataModel: XmlDataModel {
                    //Load up the XML file bundled with our application
                    source: "models/samples.xml"
                }
                //The below block is where we check the type of each item in the list and
                //compose and populate a new element to our liking
                listItemComponents: [
                    ListItemComponent {
                        type: "sample"
                        StandardListItem {
                            title: ListItemData.name
                            description: ListItemData.description
                            imageSource: "asset:///images/icons/" + ListItemData.icon
                        }
                    }
                ]
                //When the user clicks an item in the list we will push the respective sample
                //QML to the stack
                onSelectionChanged: {
                    if (selected) {
                        //This line retrieves the selected item from the XMLDataModel based on the 
                        //indexPath variable passed to the onSelectionChanged handler
                        var selectedItem = dataModel.data (indexPath);
                        appPane.deprecatedPushQmlByString (selectedItem.qmlFile);
                        
                        //Clear the selection so when we come back to the list the user can
                        //reselect the same item
                        clearSelection ();  
                    }
                }
            }
        }
    }
}
