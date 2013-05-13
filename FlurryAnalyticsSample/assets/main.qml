/* Copyright (c) 2013 BlackBerry.
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
import bb.system 1.0

NavigationPane {
    id: navigationPane

    attachedObjects: [
        //The infamous PageTwo!!
        PageTwo {
            id: pageTwo
            objectName: "pageTwo"
        },
        SettingsSheet {
            id: settingsSheet
            objectName: "settingsSheet"
        },
        HelpSheet {
            id: helpSheet
        },
        // This toast will be used to provide clues to app users
        SystemToast {
            id: systemToast
            button.label: "Ok" //This line may display as an error but it works, I promise
        },
        // The background image for this container
        ImagePaintDefinition {
            id: crumpledPageBackground
            imageSource: "asset:///images/crumpledPage.png"
        }
    ]

	// Here we set up a few standard app menu items (accessible by swiping down)
    Menu.definition: MenuDefinition {
        settingsAction: SettingsActionItem {
            onTriggered: {
                cpp.logEvent(settingsSheet.objectName + " opened from app menu");
                settingsSheet.open();
            }
        }
        helpAction: HelpActionItem {
            onTriggered: {
                cpp.logEvent(helpSheet.objectName + " opened from app menu");
                helpSheet.open();
            }
        }
        actions: [
            ActionItem {
                id: page2titleMenuDistractor
                objectName: "page2titleMenuDistractor"
                title: "Page 2?"
                imageSource: "asset:///images/2actionMenu.png"
                onTriggered: {
                    cpp.logEvent(page2titleMenuDistractor.objectName + " clicked");
                    systemToast.body = "Not here. FYI: Context menus are neat places to hide clues"
                    systemToast.show();
                }
            }
        ]
    }

    Page {
        objectName: "pageOne"

		// Track when the ActionMenu is opened or closed
        onActionMenuVisualStateChanged: {
            if (actionMenuVisualState == ActionMenuVisualState.VisibleFull) {
                cpp.logEvent("ActionMenu opened");
            } else if (actionMenuVisualState == ActionMenuVisualState.Hidden) {
                cpp.logEvent("ActionMenu closed");
            }
        }

		// Distrator ActionItem added here, and of course logged
        actions: [
            ActionItem {
                id: page2actionMenuDistractor
                objectName: "page2actionMenuDistractor"
                title: "Is Page 2 here?"
                imageSource: "asset:///images/2actionMenu.png"
                onTriggered: {
                    cpp.logEvent(page2actionMenuDistractor.objectName + " clicked");
                    systemToast.body = "Nope. Try the app menu for help."
                    systemToast.show();
                }
            }
        ]

        Container {
            background: crumpledPageBackground.imagePaint
            layout: DockLayout {

            }
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            Container {
                id: page2containerDistractor
                objectName: "page2containerDistractor"
                
                // Distrator context ActionItem added here, and of course logged
                contextActions: ActionSet {
                    title: "Maybe, maybe, maybe"
                    ActionItem {
                        title: "Is Page 2 here?"
                        imageSource: "asset:///images/2actionMenu.png"
                        onTriggered: {
                            cpp.logEvent(page2containerDistractor.objectName + " clicked");
                            systemToast.body = "Page 2 is not here...try farther down"
                            systemToast.show();
                        }
                    }
                }
                ImageView {
                    imageSource: "asset:///images/page1Title.png"
                    verticalAlignment: VerticalAlignment.Top
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Divider {
                }
            }

            Container {
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center

                ImageView {
                    imageSource: "asset:///images/predicament.png"
                    preferredWidth: 650
                    scalingMethod: ScalingMethod.AspectFill
                }

                ImageView {
                    imageSource: "asset:///images/page2.png"
                    preferredWidth: 628
                    horizontalAlignment: HorizontalAlignment.Center
                    scalingMethod: ScalingMethod.AspectFill
                    
                    // This context ActionItem may or may not be the way to PageTwo...
                    contextActions: ActionSet {
                        title: "Could this be it?!?"
                        ActionItem {
                            title: "Page 2"
                            imageSource: "asset:///images/2actionMenu.png"
                            onTriggered: {
                                navigationPane.push(pageTwo);
                            }
                        }
                    }
                }
            }
        }
    }

	// The NavigationPane is notified when new pages are displayed on the stack, so this is an 
	// easy way to track all page changes in one spot
    onTopChanged: {
        cpp.logEvent(page.objectName + " page displayed");
    }

	// Let's log peek events as well 
    onPeekStarted: {
        cpp.logEvent(navigationPane.top.objectName + " peek started" );
    }
}
