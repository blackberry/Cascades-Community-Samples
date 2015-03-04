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

import com.msohm.Searcher 1.0
import "items"

Page {
    id: searchListPage
    property string categoryId
    property string boardId
    property string searchTerms
    property string searchPhrase
    property string searchOneOrMore
    property string searchWithout
    property bool myPosts
    property int pageNo
    
    titleBar: TitleBar {
        id: theTitle
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
            id: searchList
            dataModel: searcher.model

            listItemComponents: [
                ListItemComponent {
                    type: ""
                    ForumThreadSearchListItem {
                    }
                }
            ]
            onTriggered: {
                // When an item is selected, we push the post list page.
                var chosenItem = dataModel.data(indexPath);

                var page = threadPage.createObject();
                
                page.threadId = chosenItem.id[".data"];
                page.pageTitle = chosenItem.subject[".data"];
                page.jumpToPost = true;
                
                page.boardId = chosenItem.board["href"];
                page.requestContent();

                // Push the new Page.
                navigationPane.push(page);
            }
        }
    }

    actions: [
        ActionItem {
            id: prevThreads
            title: qsTr("Previous")
            imageSource: "asset:///images/ic_previous.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            enabled: false
            onTriggered: {
                loadingLabel.visible = true;
                loadingProgress.visible = true;
                loadingProgress.running = true;
                
                searchListPage.pageNo--;
                if (searchListPage.pageNo == 1) {
                    prevThreads.enabled = false;
                }

                onRequestContent();
            }

        },
        ActionItem {
            id: nextThreads
            title: qsTr("Next")
            imageSource: "asset:///images/ic_next.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                loadingLabel.visible = true;
                loadingProgress.visible = true;
                loadingProgress.running = true;
                
                searchListPage.pageNo++;
                prevThreads.enabled = true;
                onRequestContent();
            }
        },
        ActionItem {
            id: newThread
            title: qsTr("New Post")
            imageSource: "asset:///images/ic_compose.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                var compose = composeSheet.createObject();
                compose.postComplete.connect(onRequestContent);
                compose.boardId = boardId;
                compose.open();
            }
        }
    ]

    attachedObjects: [
        Searcher {
            id: searcher
        },
        ComponentDefinition {
            id: threadPage
            source: "forumThread.qml"
        },
        ComponentDefinition {
            id: composeSheet
            source: "composeMessageSheet.qml"
        },
        ComponentDefinition {
            id: searchPage
            source: "searchPage.qml"
        }
    ]

    onCreationCompleted: {
        searchListPage.pageNo = 1;
        searchListPage.requestContent.connect(onRequestContent);
        searcher.searchRequestFailed.connect(onSearchRequestFailed);
        searcher.searchRequestReady.connect(onSearchRequestReady);
        
        //Enable peek for this page.
        navigationPane.setPeekEnabled(true);
    }

    //The slot used when there is an error downloading the threads.
    function onSearchRequestFailed(error) {
        errorLabel.visible = true;
        errorLabel.text = error;
        loadingLabel.visible = false;
        loadingProgress.visible = false;
        loadingProgress.running = false;
        nextThreads.enabled = false;
        prevThreads.enabled = false;
    }

    signal requestContent()

    function onRequestContent() {
        //Searching for my posts or by search criteria?
        if (myPosts)
        {
            theTitle.title = qsTr("Your Recent Posts");
            searcher.findUsersPosts(searchListPage.pageNo);
        }
        else
        {
            theTitle.title = qsTr("Community Search Results");
        	searcher.makeSearchRequest(searchListPage.categoryId, searchListPage.boardId, searchListPage.pageNo, searchListPage.searchTerms,
        		searchListPage.searchPhrase, searchListPage.searchOneOrMore, searchListPage.searchWithout);
		}
    }
    
    function onSearchRequestReady()
    {
        loadingLabel.visible = false;
        loadingProgress.visible = false;
        loadingProgress.running = false;
        errorLabel.visible = false;
    }

}
