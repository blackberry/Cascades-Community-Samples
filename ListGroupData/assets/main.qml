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
    Container {
        ListView {
            id: ourList
            dataModel: GroupDataModel {
                id: groupData
                sortingKeys: [ "name" ]
            }
            listItemComponents: [
                ListItemComponent {
                    //Header item type is created by the GroupDataModel, not an actual data instance
                    type: "header"
                    Header {
                        //ListItemData in a header is the letter of the sorting key
                        title: ListItemData + " I can define the header as I wish "
                    }
                },
                ListItemComponent {
                    type: "item"
                    Container {
                        Label {
                            text: ListItemData.name
                        }
                        Label {
                            text: ListItemData.last
                        }
                        Divider {}
                    }
                }
            ]
        }
    }
    actions: [
        ActionItem {
            title: "Remove L"
            onTriggered: {
                groupData.remove({
                        name: "L",
                        last: "description"
                    })
            }
        },
        ActionItem {
            title: "Add item"
            onTriggered: {
                groupData.insert({
                        name: "item",
                        last: "number " + groupData.size()
                    })
            }
        },
        ActionItem {
            title: "Turn off sort"
            onTriggered: {
                groupData.sortingKeys = []
            }
        },
        ActionItem {
            title: "Turn on sort"
            onTriggered: {
                groupData.sortingKeys = [ "name" ]
            }
        },
        ActionItem {
          title: "Update 1st"
          onTriggered: {
              //You need the actual data object to manipulate before calling updateItem
              var curData = groupData.data([0,0])
              curData.name = curData.name + "1"
              groupData.updateItem([0,0],curData)
          }  
        },
        ActionItem {
            title: "Remove 1st"
            onTriggered: {
                groupData.removeAt([0,0])
            }
        },
        ActionItem {
            title: "Remove Last"
            onTriggered: {
                groupData.removeAt(groupData.last())
            }   
        },
        ActionItem {
            title: "Add a lot"
            onTriggered: {
                for(var i = 0; i < 250000; i++){
                groupData.insert({
                        name: "item",
                        last: "number " + groupData.size()
                    })
                }
            }
        },
        ActionItem {
            title: "Scroll to top"
            onTriggered: {
                ourList.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.Smooth)
            }
        },
        ActionItem {
            title: "Scroll to bottom"
            onTriggered: {
                ourList.scrollToPosition(ScrollPosition.End, ScrollAnimation.Smooth)
            }
        },
        ActionItem {
            title: "Toggle ascending"
            onTriggered: {
                groupData.sortedAscending = ! groupData.sortedAscending
            }
        }
    ]
    onCreationCompleted: {
        groupData.insert({
                name: "List item",
                last: "Secondary text"
            })
        //This is how you generate a QVariantList in qml
        groupData.insertList([ {
                    name: "Well",
                    last: "description"
                },
                {
                    name: "K",
                    last: "description"
                }
            ])
        groupData.insert({
                name: "L",
                last: "description"
            })
    }
} 