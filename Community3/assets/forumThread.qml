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

import com.msohm.ThreadProvider 1.0
import com.msohm.BoardProperties 1.0
import "items"

Page {
    id: threadPage
    property string threadId
    property string boardId
    property int pageNo
    property string pageTitle
    property bool jumpToPost
    property bool jumpToEnd
    property int scrollTo
    
    titleBar: TitleBar {
        id: threadTitle
        title: threadPage.pageTitle
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
            id: threadList
            visible: false
            dataModel: threadProvider.model
            
            function onRefresh()
            {
                onRequestContent();
            }
            
            listItemComponents: [
                ListItemComponent {
                    type: "first"
                    ForumThreadFirstItem {
                    }
                },
                ListItemComponent {
                    type: "regular"
                    ForumThreadItem {
                    }
                },
                ListItemComponent {
                    type: "solution"
                    ForumThreadSolutionItem {
                    }
                },
                ListItemComponent {
                    type: "solved"
                    ForumThreadFirstSolvedItem {
                    }
                }
            ]
            
            function itemType(data, indexPath) {
                if (indexPath == 0)
                {
                    return "first";
                }
                else if (threadProvider.isSolution(data.id[".data"]))
                {
                    return "solution";
                }
                else if (threadProvider.hasSolution && indexPath == 0)
                {
                    return "solved";
                }
                else 
                {
                    return "regular";
                }
            }
            
            //Locate the solution message and scroll to it.
            function scrollToSolution()
            {
                threadPage.threadId = threadProvider.firstSolution;
                threadPage.pageNo = 1;
                threadPage.jumpToPost = true;
                threadPage.jumpToEnd = false;
                onRequestContent();
            }
            
            function canMarkSolved()
            {
                return threadProvider.canMarkSolved;
            }
        }
    }

    actions: [
        ActionItem {
            id: prevMessages
            title: qsTr("Previous Messages")
            imageSource: "asset:///images/ic_previous.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            enabled: false
            onTriggered: {
                loadingLabel.visible = true;
                loadingProgress.visible = true;
                loadingProgress.running = true;
                
                threadPage.pageNo--;
                threadPage.jumpToPost = false;
                threadPage.jumpToEnd = false;
                nextMessages.enabled = true;
                
                if (threadPage.pageNo == 1)
                {
                    prevMessages.enabled = false;
                }
                
                onRequestContent();
            }
            
        },
        ActionItem {
            id: nextMessages
            title: qsTr("Next Messages")
            imageSource: "asset:///images/ic_next.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            enabled: false
            onTriggered: {
                loadingLabel.visible = true;
                loadingProgress.visible = true;
                loadingProgress.running = true;
                
                threadPage.pageNo++;
                threadPage.jumpToPost = false;
                threadPage.jumpToEnd = false;
                prevMessages.enabled = true;
                
                if ((threadPage.pageNo * 10) > threadProvider.noOfMessages)
                {
                    nextMessages.enabled = false;
                }
                
                onRequestContent();
            }
        },
        ActionItem {
            id: gotoLast
            title: qsTr("Go To Last Message")
            imageSource: "asset:///images/ic_sort.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
            enabled: true
            onTriggered: {
                threadPage.jumpToPost = false;
                threadPage.jumpToEnd = true;
                nextMessages.enabled = false;
                
                //Calculate the number of pages of messages, truncating to whole number. 
                var lastPage = Math.floor(threadProvider.noOfMessages / 10);
                
                //Add one if there is a partial page of messages at the end.
                if (threadProvider.noOfMessages % 10 > 0)
                {
                    lastPage++;
                }
                
                if (lastPage > 1)
                {
                    prevMessages.enabled = true;
                }
                
                //If on the last page scroll to the last message,
                //otherwise load the last page.
                if (threadPage.pageNo == lastPage)
                {
                    onScrollListView();
                }
                else 
                {
                    threadPage.pageNo = lastPage;
                    loadingLabel.visible = true;
                    loadingProgress.visible = true;
                    loadingProgress.running = true;
                    onRequestContent();
                }
            
            }
        },        
        ActionItem {
            id: threadSearch
            imageSource: "asset:///images/ic_search.png"
            title: qsTr("Search")
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                var page = searchPage.createObject();
                page.boardId = threadPage.boardId;
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

        ThreadProvider {
            id: threadProvider
        },
        ComponentDefinition {
            id: searchPage
            source: "searchPage.qml"
        },
        ComponentDefinition {
            id: searchResultPage
            source: "forumSearchResultsList.qml"
        },
        BoardProperties{
            id: boardProperties
        }
    ]

    onCreationCompleted: {
        pageNo = 1;
        threadPage.requestContent.connect(onRequestContent);
        threadProvider.threadRequestReady.connect(onthreadRequestReady);
        threadProvider.threadRequestFailed.connect(onThreadRequestFailed);
        threadProvider.scrollListView.connect(onScrollListView);
        boardProperties.boardNameReady.connect(onBoardNameReady);
        
        //Disable peek to allow for easier horizontal scrolling of post content.
        navigationPane.setPeekEnabled(false);
    }

    //The slot used when there is an error downloading the thread.
    function onThreadRequestFailed(error) {
        errorLabel.visible = true;
        errorLabel.text = error;
        loadingLabel.visible = false;
        loadingProgress.visible = false;
        loadingProgress.running = false;
    }

    signal requestContent()
    
    function onRequestContent()
    {
        threadProvider.makeThreadRequest(threadPage.threadId, threadPage.pageNo);
        
        //Look up the board name to use in the title if it wasn't already set.
        if (threadPage.pageTitle.length == 0)
        {
            //Use empty string so that the title bar is displayed while the
            //board name is looked up.
            threadPage.pageTitle = " ";
            boardProperties.getBoardName(boardId);
        }
    }
    
    function onBoardNameReady(boardName)
    {
        threadPage.pageTitle = boardName;
    }     
    
    function onthreadRequestReady()
    {
        var foundRecord = false;
        threadPage.scrollTo = 0;
        
        //Scroll to the message within the thread.  Used from search results and "My Posts" list.
        if (threadPage.jumpToPost == true)
        {
	        var indexArray = [0];
	        var total;
	        
	        if (threadProvider.noOfMessages > 10)
	        {
	            total = 10;
	        }
	        else 
	        {
	            total = threadProvider.noOfMessages;
	        }
	        
	        for (var count = 0; count < total; count++)
	        {
	            indexArray[0] = count;
	            var record = threadList.dataModel.data(indexArray);
	            
	            //If message to scroll to is found, save ListView position and load controls.
	            //Then scrollListViewDelay is called, need to wait about a second for the
	            //ListView to load/display the data before scrollToItem can be called.
	            if (record.id[".data"] == threadPage.threadId)
	            {
	                foundRecord = true;
                    threadPage.scrollTo = count;
                    postsLoaded();
                    threadProvider.scrollListViewDelay();
	                break;
	            }
	        }
        }
        //Scroll to the last message.
		else if (threadPage.jumpToEnd == true)
		{
		            threadProvider.scrollListViewDelay();
		            postsLoaded();
		}
        else 
        {
            postsLoaded();
        }
        
        //Remove loading indicators if message found or jumptToPost not needed (start at first message in thread).
        if (foundRecord == false && threadPage.jumpToPost == true)
        {
            pageNo++;
            prevMessages.enabled = true;
            onRequestContent();
        }
        else if (foundRecord == true && threadPage.jumpToPost == true)
        {
            //Disable Next button if we're on the last page.
            if ((threadPage.pageNo * 10) > threadProvider.noOfMessages)
            {
                nextMessages.enabled = false;
            }
        }
        
    }

	//Called a second after the data starts to load.
    //Need to wait about a second for the
    //ListView to load/display the data before scrollToItem can be called.
	function onScrollListView()
	{
        if (threadPage.jumpToEnd == true)
        {
            threadList.scrollToPosition(ScrollPosition.End, ScrollAnimation.None);
        }
	    else if (threadPage.scrollTo > 0)
	    {
	    	threadList.scrollToItem([threadPage.scrollTo], ScrollAnimation.None);
	    }    
	
	}
    
    //Called when data has been loaded.
    //Remove loading indicators and display listView.
    function postsLoaded()
    {
        loadingLabel.visible = false;
        loadingProgress.visible = false;
        loadingProgress.running = false;
        errorLabel.visible = false;
        threadList.visible = true;
        
        if ((threadPage.pageNo * 10) < threadProvider.noOfMessages)
        {
            nextMessages.enabled = true;
        }	  
    }
}
