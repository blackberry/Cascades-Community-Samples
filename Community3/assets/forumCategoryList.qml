/* Copyright (c) 2014 BlackBerry Ltd.
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
 * 
 * The Lithium REST APIs are owned by Lithium Technologies, Inc. 
 * (http://www.lithium.com) and are not part of the “Works” licensed 
 * hereunder pursuant to the Apache 2.0 license.
 */

import bb.cascades 1.2

import com.msohm.CategoryProvider 1.0
import "items"

NavigationPane {
    id: navigationPane
    
    Page {
        id: categoryPage
        property string categoryId
        property string pageTitle
        
        titleBar: TitleBar {
            title: categoryPage.pageTitle
        }
            
        Container {
            leftPadding: 10.0
            topPadding: 10.0
            rightPadding: 10.0
            bottomPadding: 10.0
            
            Label {
                id: errorLabel
                visible: false
                multiline: true

            }

            Label {
                id: loadingLabel
                visible: true
                horizontalAlignment: HorizontalAlignment.Center
                text: qsTr("Loading...")
            }
            ActivityIndicator {
                id: loadingProgress
                visible: true
                running: true 
                horizontalAlignment: HorizontalAlignment.Center
                preferredHeight: 120 
                preferredWidth: 120
            }

            ListView {
                id: categoryList
                dataModel: categoryProvider.model

                listItemComponents: [
                    ListItemComponent {
                        type: ""
                        ForumCBListItem{
                            
                        }
                    }
                ]
                onTriggered: {
                    // When an item is selected, we push the board list page.
                    var chosenItem = dataModel.data(indexPath);

                    var page = boardListPage.createObject();
                    page.categoryId = chosenItem.id[".data"];
                    page.pageTitle = chosenItem.title[".data"];
                    page.requestContent();

                    // Push the new Page.
                    navigationPane.push(page);
                }
            }
        }

        attachedObjects: [

            CategoryProvider {
                id: categoryProvider

            },
            ComponentDefinition {
                id: boardListPage
                source: "forumBoardList.qml"
            },
            ComponentDefinition {
                id: recentPostListPage
                source: "forumThreadList.qml"
            },
            ComponentDefinition {
                id: searchPage
                source: "searchPage.qml"
            },
            ComponentDefinition {
                id: searchResultPage
                source: "forumSearchResultsList.qml"
            }
        ]

        onCreationCompleted: {
            categoryPage.requestContent.connect(onRequestContent)
            categoryProvider.categoryRequestFailed.connect(onCategoryRequestFailed);
            categoryProvider.categoryRequestReady.connect(onCategoryRequestReady);
            
            //Enable peek for this page.
            navigationPane.setPeekEnabled(true);
        }

        //The slot used when there is an error downloading the categories.
        function onCategoryRequestFailed(error) {
            errorLabel.visible = true;
            errorLabel.text = error;
            loadingLabel.visible = false;
            loadingProgress.visible = false;
            loadingProgress.running = false;
        }

        signal requestContent()

        function onRequestContent() {
            categoryProvider.makeCategoryRequest(categoryPage.categoryId);
        }
        
        function onCategoryRequestReady()
        {
            loadingLabel.visible = false;
            loadingProgress.visible = false;
            loadingProgress.running = false;
            errorLabel.visible = false;
        }
        actions: [
            ActionItem {
                id: viewAllNewTOpics
                title: qsTr("New Topics")
                imageSource: "asset:///images/ic_view_post.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    var page = recentPostListPage.createObject();
                    page.boardId = "recentpostspage";
                    page.pageTitle = qsTr("All New Topics");
                    page.requestContent();
                    
                    // Push the new Page.
                    navigationPane.push(page);
                    
                }
            },
            ActionItem {
                id: categorySearch
                imageSource: "asset:///images/ic_search.png"
                title: qsTr("Search")
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    var page = searchPage.createObject();
                    page.categoryId = categoryPage.categoryId;
                    navigationPane.push(page);
                    
                }
            },
            ActionItem {
                id: myPosts
                imageSource: "asset:///images/ic_textmessage.png"
                title: qsTr("My Posts")
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    var page = searchResultPage.createObject();
                    page.myPosts = true;
                    page.pageNo = 1;
                    page.requestContent();
                    navigationPane.push(page);
                
                }
            }
        ]        
    }
}