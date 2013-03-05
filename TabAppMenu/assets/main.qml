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

TabbedPane {
    showTabsOnActionBar: true
    
    /* Note: 
     * To push a NavigationPane Page from the ApplicationMenu, we will need  
     * to keep track which NavigationPane to push it to (that is, which is the active NavigationPane)
     * Tab cannot be added to NavigationPane (but the other way around is possible)
     */ 
    property NavigationPane currentNavigationPane: tab1Handle.navHandle
    
    // Update the NavigationPane handle when the Tab is changed
    onActiveTabChanged: {
        console.log("TabbedPane : onActiveTabChanged : activeTab.description = " + activeTab.description);
        currentNavigationPane = activeTab.navHandle
    }
    
    // Define the Page and Sheet objects that would be used from the Application menu
    attachedObjects: [
        ComponentDefinition {
            id: settingsPage
            source: "SettingsPage.qml"
        },
        Sheet {
            id: infoSheet
            // The following page refers to the InfoPage.qml
            InfoPage {
                id: infoPage
                // Handle the custom signal from InfoPage.qml
                onDone : {
                    infoSheet.close();
                }
            }
        }
    ] // attachedObjects
    
    Menu.definition: MenuDefinition {
        id: menu
        actions: [
            ActionItem {
                title: "Info"
                imageSource: "images/ic_info.png"
                onTriggered: {
                    console.log("Info: ActionItem : onTriggered");
                    // For InfoPage, we will use Sheet
                    infoSheet.open();
                }
            }
        ]
        settingsAction: SettingsActionItem {
            onTriggered: {
                console.log("Settings: ActionItem : onTriggered");
                // For Settings, we will use NavigationPane
                var settingsPageObj = settingsPage.createObject();
                Application.menuEnabled = false;
                currentNavigationPane.push(settingsPageObj);
            }
        }
    } // MenuDefinition
    
    Tab {
        id: tab1Handle
        title: qsTr("Tab 1")
        description: "Tab 1 Description"
        property alias navHandle: nav1
        
        NavigationPane {
            id: nav1
                        
            onPopTransitionEnded: {
                console.log("Tab1 : NavigationPane - onPopTransitionEnded");
                Application.menuEnabled = true;
            }
            
            Page {
                id: tab1
                Container {
                    layout: DockLayout{}
                    // Define tab content here
                    Label {
                        text: "Tab 1"
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Top
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontStyle: FontStyle.Italic
                    }
                    Label {
                        text: "Swipe down on top edge for Application Menu"
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }
    } // Tab1
    
    Tab {
        id:tab2Handle
        property alias navHandle: nav2
        title: qsTr("Tab 2")
        description: "Tab 2 Description"
        
        NavigationPane {
            id: nav2
            
            onPopTransitionEnded: {
                console.log("Tab2 : NavigationPane - onPopTransitionEnded");
                Application.menuEnabled = true;
            }
            
            Page {
                id: tab2
                Container {
                    layout: DockLayout{}
                    // Define tab content here
                    Label {
                        text: "Tab 2"
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Top
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontStyle: FontStyle.Italic
                    }
                    Label {
                        text: "Swipe down on top edge for Application Menu"
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }
    } // tab2
    
    Tab {
        id: tab3Handle
        property alias navHandle: nav3
        title: qsTr("Tab 3")
        description: "Tab 3 Description"
        
        NavigationPane {
            id: nav3
            
            onPopTransitionEnded: {
                console.log("Tab3 : NavigationPane - onPopTransitionEnded");
                Application.menuEnabled = true;
            }
            
            Page {
                id: tab3
                Container {
                    layout: DockLayout{}
                    // Define tab content here
                    Label {
                        text: "Tab 3"
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Top
                        textStyle.fontSize: FontSize.Large
                        textStyle.fontStyle: FontStyle.Italic
                    }
                    Label {
                        text: "Swipe down on top edge for Application Menu"
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }
    } // tab3
} //TabbedPane
