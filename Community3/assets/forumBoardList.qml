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

import com.msohm.BoardProvider 1.0
import "items"

Page {
    id: boardPage
    property string categoryId
    property string pageTitle
    
    titleBar: TitleBar {
        title: boardPage.pageTitle
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
            id: boardList
            dataModel: boardProvider.model
            
            listItemComponents: [
                ListItemComponent {
                    type: ""
                    ForumCBListItem{
                    
                    }                    
                }
            ]
            onTriggered: {
                // When an item is selected, we push the post list page.
                var chosenItem = dataModel.data(indexPath);
                
                var page = postListPage.createObject();
                page.boardId = chosenItem.id[".data"];
                page.pageTitle = chosenItem.title[".data"];
                page.requestContent();
                
                // Push the new Page.
                navigationPane.push(page);
            }
        }
    }

    attachedObjects: [

        BoardProvider {
            id: boardProvider

        },
        ComponentDefinition {
            id: postListPage
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
    
    actions: [
        ActionItem {
            id: boardSearch
            imageSource: "asset:///images/ic_search.png"
            title: qsTr("Search")
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                var page = searchPage.createObject();
                page.categoryId = boardPage.categoryId;
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

    onCreationCompleted: {
        boardPage.requestContent.connect(onRequestContent)
        boardProvider.boardRequestFailed.connect(onBoardRequestFailed);
        boardProvider.boardRequestReady.connect(onBoardRequestReady);
        
        //Enable peek for this page.
        navigationPane.setPeekEnabled(true);
    }

    //The slot used when there is an error downloading the boards.
    function onBoardRequestFailed(error) {
        errorLabel.visible = true;
        errorLabel.text = error;
        loadingLabel.visible = false;
        loadingProgress.visible = false;
        loadingProgress.running = false;
    }

    signal requestContent()
    
    function onRequestContent()
    {
        boardProvider.makeBoardRequest(boardPage.categoryId);
    }
    
    function onBoardRequestReady()
    {
        loadingLabel.visible = false;
        loadingProgress.visible = false;
        loadingProgress.running = false;
        errorLabel.visible = false;
    }    
    
    

}
