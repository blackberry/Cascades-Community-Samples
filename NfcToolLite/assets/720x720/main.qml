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

//  ======== Properties ========
	    
    property bool readerVisible: false
    property variant currentChildPage: null
	    
//  ======== SIGNAL()s ========

    signal messageToChild(string text);

//  ======== SLOT()s ========
	    
    function launchReader(text) {
        if (!readerVisible) {
            var readerFile = "";

            for ( var i = 0; i < menuDataModel.childCount([]); i++) {
                console.log("XXXX i: " + i);
                var foo = menuDataModel.data([i]);
                if (foo.itemName === "item_read") {
                    readerFile = foo.file;
                }
            }

            if (readerFile !== "") {
                targetPageDefinition.source = readerFile;
            } else {
                console.log("XXXX Unable to locate Reader QML file");
            }

	        var newPage = targetPageDefinition.createObject();
	        currentChildPage = newPage;
            readerVisible = true;
	        nav.push(newPage);
	        nav.messageToChild.connect(newPage.content.message);
        }
        message(text);
    }
    
    function message(text) {
        console.log("XXXX message received from C++>>> " + text);
        messageToChild(text);
    }
	    
//  ======== Local functions ========

//  function splat(text) {}
    
    Page {
        id: menuListPage
        
        content: Container {
            background: Color.create ("#262626")
            preferredWidth: 720
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
                        id: menuDataModel
                        source: "models/menumodel.xml"
                    }
                                        
                    listItemComponents: [
                        ListItemComponent {
                            type: "menuitem" 
                            MenuItem {
                            }
                        }
                    ]
                    onTriggered: {
                        console.log("Triggered");
                        if ( indexPath.length === 1 ) {
                            var selectedItem = dataModel.data(indexPath);
                            targetPageDefinition.source = selectedItem.file;
                            var newPage = targetPageDefinition.createObject();
                            currentChildPage = newPage;
	                        nav.push(newPage);

                            if ( selectedItem.itemName === "item_read" ) {
                                readerVisible = true;
		                        nav.messageToChild.connect(newPage.content.message);
                            } else {
                                readerVisible = false;
                            }
                        }
                    }
                    attachedObjects: [
                        ComponentDefinition {
                            id: targetPageDefinition
                        }
                    ]
                }
            }
        }
    }

    onTopChanged: {
        if (page == menuListPage) {
            console.log("XXXX main onTopChanged");
            // Clear selection when returning to the menu list page.
            readerVisible = false;
            menuList.clearSelection();
        }
    }

	onPopTransitionEnded: {
	    page.destroy();
	}
	    
    onCreationCompleted: {
        console.log("XXXX main creation complete");
    }
}