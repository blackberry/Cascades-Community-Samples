/* Copyright (c) 2013 Chad Tetreault
 * http://palebanana.com - @chadtatro
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

BasePage {
    id: tabFeed
    property variant feedPhotos

    onCreationCompleted: {
        // you can change the head image to whatever you like for each page
        headerimage.imageSource = "asset:///images/header-filtermama.png";
    }

    // feed thumbnail listView
    ListView {
        id: photosList
        verticalAlignment: VerticalAlignment.Top
        horizontalAlignment: HorizontalAlignment.Left
        scrollIndicatorMode: ScrollIndicatorMode.Default
        snapMode: SnapMode.None
        flickMode: FlickMode.Momentum
        bufferedScrollingEnabled: true

        property alias feedDataModel: feedDataModel

        onCreationCompleted: {
            app.photosChanged.connect(addThumbToDataModel)
            loadPhotos()
        }

        layout: GridListLayout {
            headerMode: ListHeaderMode.None
            columnCount: 3
        }

        function itemType(data, indexPath) {
            return "item";
        }

        dataModel: ArrayDataModel {
            id: feedDataModel
        }

        // do this when a thumbnail is clicked
        function handleThumbClick(thumbData) {
            var path = thumbData.replace(".tmp/", "");
            app.invokePhotoViewer(path);
        }

        // add thumbnail to the datamodel
        function addThumbToDataModel(filepath) {
            var newThumb = [ "file://" + filepath ];
            feedDataModel.insert(0, newThumb[0]);
        }

        // load found thumbnails and insert into the feedDataModel
        function loadPhotos() {
            var photos = app.getPhotos();
            feedPhotos = photos.split(",");
            var length = feedPhotos.length;

            for (var i = 0; i < length; i ++) {
                if (feedPhotos[i] !== "") {
                    feedDataModel.insert(0, feedPhotos[i])
                }
            }
        }

        listItemComponents: [
            ListItemComponent {
                type: "item"
                Container {
                    id: feedItem

                    Container {
                        verticalAlignment: VerticalAlignment.Bottom
                        horizontalAlignment: HorizontalAlignment.Center

                        ImageButton {
                            preferredHeight: 334
                            preferredWidth: 334
                            defaultImageSource: ListItemData
                            pressedImageSource: ListItemData

                            onClicked: {
                                feedItem.ListItem.view.handleThumbClick(ListItemData)
                            }
                        }
                    } // container
                } // container
            } // listitemcomponent
        ]
    }

    attachedObjects: [
        ImagePaintDefinition {
            id: back
            repeatPattern: RepeatPattern.XY
            imageSource: "asset:///images/background-body-fullsize.png"
        }
    ]

    actions: [
        ActionItem {
            title: qsTr("Capture") + Retranslate.onLanguageChanged
            imageSource: "asset:///images/camera.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                filepicker.open();
            }
        }
    ]
}
