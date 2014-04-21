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

import com.msohm.ThreadListProvider 1.0
import "items"

Page {
    id: threadListPage
    property string boardId
    property int pageNo
    property string pageTitle
    
    titleBar: TitleBar {
        title: threadListPage.pageTitle
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
            id: threadsList
            dataModel: threadListProvider.model

            listItemComponents: [
                ListItemComponent {
                    type: "regular"
                    ForumThreadListItem {

                    }
                },
                ListItemComponent {
                    type: "floated"
                    ForumFloatedThreadListItem {
                    }
                },
                ListItemComponent {
                    type: "recent"
                    ForumRecentThreadListItem {
                    }
                }
            ]
            
            onTriggered: {
                // When an item is selected, we push the post list page.
                var chosenItem = dataModel.data(indexPath);

                var page = threadPage.createObject();
                
                //ID location varies on whether it's a floated message or recent post.
                if (threadListPage.boardId == "recentpostspage")
                {
                    page.threadId = chosenItem.messages.topic.id[".data"];
                    page.boardId = chosenItem.messages.topic.board["href"];
                }
                else if (indexPath >= threadListProvider.numberOfFloatedMessages)
                {
                    page.threadId = chosenItem.id[".data"];
                    page.pageTitle = threadListPage.pageTitle;
                    page.boardId = threadListPage.boardId;
                }
                else 
                {
                    page.threadId = chosenItem.target.messages.topic.id[".data"];
                    page.pageTitle = threadListPage.pageTitle;
                    page.boardId = threadListPage.boardId;
                }
                
                page.jumpToPost = false;
                page.requestContent();

                // Push the new Page.
                navigationPane.push(page);
            }
            
            function itemType(data, indexPath) {
                //Use a different ListItem based on wiether it's a floated message.
                if (threadListPage.boardId == "recentpostspage")
	            {
	                return "recent";
	            }
	            else if (indexPath >= threadListProvider.numberOfFloatedMessages)
	            {
	                return "regular";
	            }
	            else 
	            {
	                return "floated"
	            }
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
                
                threadListPage.pageNo--;
                if (threadListPage.pageNo == 1) {
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
                
                threadListPage.pageNo++;
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
        },
        ActionItem {
            id: threadSearch
            imageSource: "asset:///images/ic_search.png"
            title: qsTr("Search")
            onTriggered: {
                var page = searchPage.createObject();
                page.boardId = threadListPage.boardId;
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

    attachedObjects: [

        ThreadListProvider {
            id: threadListProvider
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
        },
        ComponentDefinition {
            id: searchResultPage
            source: "forumSearchResultsList.qml"
        }
        
    ]

    onCreationCompleted: {
        threadListPage.pageNo = 1;
        threadListPage.requestContent.connect(onRequestContent);
        threadListProvider.threadsRequestFailed.connect(onThreadsRequestFailed);
        threadListProvider.threadsRequestReady.connect(onThreadsRequestReady);
        
        //Enable peek for this page.
        navigationPane.setPeekEnabled(true);
    }

    //The slot used when there is an error downloading the threads.
    function onThreadsRequestFailed(error) {
        errorLabel.visible = true;
        errorLabel.text = error;
        loadingLabel.visible = false;
        loadingProgress.visible = false;
        loadingProgress.running = false;
    }

    signal requestContent()

    function onRequestContent() {
        threadListProvider.makeThreadsRequest(threadListPage.boardId, threadListPage.pageNo);
    }
    
    function onThreadsRequestReady()
    {
        loadingLabel.visible = false;
        loadingProgress.visible = false;
        loadingProgress.running = false;
        errorLabel.visible = false;
    }

}
