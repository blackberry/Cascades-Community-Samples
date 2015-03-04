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
import bb.system 1.0

Page {
    id: searchPage
    property string categoryId
    property string boardId
    
    
    titleBar: TitleBar {
        title: qsTr("Community Search")
    }      
    Container {
        leftPadding: 10.0
        topPadding: 10.0
        rightPadding: 10.0
        bottomPadding: 10.0
        
        SegmentedControl {
            Option {
                objectName: "basic"
                text: qsTr("Basic")
                selected: true

            }
            Option {
                objectName: "advanced"
                text: qsTr("Advanced")

            }
            onSelectedIndexChanged: {
			    
                if (selectedIndex == 0)
                {
                    phraseText.visible = false;
                    oneOrMoreText.visible = false;
                    withoutText.visible = false;
                }
                else if (selectedIndex == 1)
                {
                    phraseText.visible = true;
                    oneOrMoreText.visible = true;
                    withoutText.visible = true;
                }
			}
            accessibility.name: qsTr("Search Mode")
        }
        TextField {
            id: searchText
            hintText: qsTr("Search Terms")

        }
        TextField {
            id: phraseText
            hintText: qsTr("With the Exact Phrase")
            visible: false
        
        }
        TextField {
            id: oneOrMoreText
            hintText: qsTr("With one or More Words")
            visible: false
        
        }
        TextField {
            id: withoutText
            hintText: qsTr("Without the Words")
            visible: false
        
        }
        DropDown {
            title: qsTr("Search In")
            id: searchIn
            Option {
                id: currentLocation
                text: qsTr("Current Location")
                selected: true
            }
            Option {
                id: wholeCommunity
                text: qsTr("Whole Community")
            }
        }
        Button {
            id: doSearch
            text: qsTr("Search")
            horizontalAlignment: HorizontalAlignment.Right
            onClicked: {
                if (searchText.text.length == 0 && phraseText.text.length == 0 && oneOrMoreText.text.length == 0 && withoutText.text.length == 0)
                {
                    //No search criteria.
                    errortoast.show();
                }
                else 
                {
                    //Do the search.
                    var page = searchResultsPage.createObject();
                    
                    //Only populate these if they are searching from current location.
                    if (searchIn.selectedOption == currentLocation)
                    { 
                        page.categoryId = searchPage.categoryId;
                        page.boardId = searchPage.boardId;
                    }
                    page.searchTerms = searchText.text 
                    page.searchPhrase = phraseText.text;
                    page.searchOneOrMore = oneOrMoreText.text;
                    page.searchWithout = withoutText.text;
                    page.pageNo = 1;
                    page.myPosts = false;
                    page.requestContent();
                    
                    // Push the new Page.
                    navigationPane.push(page);
                }
            }
        }
    }
    
    attachedObjects: [
        SystemToast {
            id: errortoast
            body: qsTr("Nothing to search for.  Enter search values first.")
        },
        ComponentDefinition {
            id: searchResultsPage
            source: "forumSearchResultsList.qml"
        }
    ]    
    
    onCreationCompleted: {
        //Enable peek for this page.
        navigationPane.setPeekEnabled(true);
    } 
}
