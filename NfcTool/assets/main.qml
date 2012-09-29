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
import "common"

NavigationPane {
    id: nav
    objectName: "navPane"
    
    function updateData() {
    }
    
    Page {
        id: menuListPage
        
        content: Container {
            background: Color.create ("#262626")
            preferredWidth: 768
            layout: DockLayout {
            }

            // A Container for the list, padded at the top and bottom to make room for decorations.
            Container {
                layout: DockLayout {
                }
                topPadding: 2
                bottomPadding: 2

                ListView {
                    id: menuList;
                    objectName: "list"
                    dataModel: XmlDataModel {
                        source: "models/menumodel.xml"
                    }
                                        
                    listItemComponents: [
                        ListItemComponent {
                            type: "menuitem" 
                            MenuItem {
                            }
                        }
                    ]
                    
                }
            }
        }
    }
    onTopChanged: {
        if (page == menuListPage) {
            // Clear selection when returning to the menu list page.
            menuList.clearSelection ();
        }
    }
}
