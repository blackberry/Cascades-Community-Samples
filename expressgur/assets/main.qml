/* Copyright (c) 2014 BlackBerry.
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
 
import bb.cascades 1.2
import bb.data 1.0
import com.example.httpimage 1.0
Page {
    titleBar: TitleBar {
        title: "DeviantArt search"
        dismissAction: ActionItem {
            title: "Cancel"
            onTriggered: {
                root.cancel();
            }
        }
    }
    property variant appScene: Application.scene
    id: root
    signal cancel()
    signal select(string filePath)
    Container {

        Container {
            ActivityIndicator {
                id: searchActivity
                preferredWidth: 100
                preferredHeight: 100

            }

            ListView {
                dataModel: dataModel
                onTriggered: {

                    if (indexPath.length > 1) {
                        var chosenItem = dataModel.data(indexPath);
                        httpDownloader.url = chosenItem.imageSource;

                    }

                }
                listItemComponents: [
                    ListItemComponent {
                        type: "imageItem"
                        ImageSearchListItem {
                            src: ListItemData.imageSource
                        }
                    }, //we get a header from the rss feed, but we dont want to display it
                    ListItemComponent {
                        type: "header"
                        Container {

                        }
                    }
                ]
                accessibility.name: "Listing"
                // Item type-mapping
                function itemType(data, indexPath) {
                    if (indexPath.length == 1) {
                        return 'header';
                    } else {
                        if (data.imageSource != "") {
                            // The data contain an image return an item that can display an image.
                            return 'imageItem'
                        } else {
                            // No image in the data display a text item.
                            return 'textItem'
                        }
                    }
                }
            }

        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            topPadding: 20
            leftPadding: 20
            rightPadding: 20
            bottomPadding: 20

            TextField {
                id: searchField
                // minWidth: 600
                hintText: "Keywords"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }

                //  focusHighlightEnabled: true
                inputMode: TextFieldInputMode.Default

                input {

                    submitKey: SubmitKey.Search
                    onSubmitted: {
                        rssSource.source = "http://backend.deviantart.com/rss.xml?type=deviation&q=" + searchField.text;

                    }
                }
            }
            Button {
                text: "Search"
                objectName: "doneButton"
                onClicked: {
                    rssSource.source = "http://backend.deviantart.com/rss.xml?type=deviation&q=" + searchField.text;
                    searchActivity.start();
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 15
                }
            }
        }

        attachedObjects: [
            GroupDataModel {
                id: dataModel
            },
            RSSDataSource { // load and parse the rss feed
                id: rssSource
                // The GroupDataModel above that is populated with data.
                dataModel: dataModel
                onDataLoaded: {
                    searchActivity.stop();
                }

            },

            HTTPImage {
                id: httpDownloader
                onImageDownloadComplete: {
                    var fullPath = httpDownloader.saveImage("imagesearch");
                    root.select(fullPath);
                }
            }
        ]
        enabled: true
    }
    onAppSceneChanged: {
        // This triggers focus if the application scene is set to
        // this QML Page.
        searchField.requestFocus();
    }
}
