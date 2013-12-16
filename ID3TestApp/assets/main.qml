/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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
import bb.cascades.pickers 1.0

Page {

    Container {
        id: mainpage
        objectName: "mainpage"

        signal showMetaData(string file_url)

        Label {
            text: qsTr("ID3lib Test Application") + Retranslate.onLocaleOrLanguageChanged
            verticalAlignment: VerticalAlignment.Top
            horizontalAlignment: HorizontalAlignment.Center
        }
        ImageView {
            id: mp3_file
            objectName: "mp3_file"
            visible: true
            scalingMethod: ScalingMethod.AspectFit
            imageSource: "images/music.png"
            onTouch: {
                if (event.isUp()) {
                    filePicker.open()
                }
            }
            onImageSourceChanged: {
                console.log("XXXX imageSourceChanged to " + imageSource);
            }
            horizontalAlignment: HorizontalAlignment.Center
        }
        Label {
            text: qsTr("Touch icon to select mp3 file") + Retranslate.onLocaleOrLanguageChanged
            textStyle.color: Color.Blue
            verticalAlignment: VerticalAlignment.Top
            horizontalAlignment: HorizontalAlignment.Center
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                background: Color.Gray
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    id: lbl_artist
                    multiline: true
                    text: qsTr("Artist")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }
            }
            Container {
                background: Color.LightGray
                Label {
                    id: val_artist
                    multiline: true
                    text: app.artist
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 70
                }
            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                background: Color.Gray
                Label {
                    id: lbl_album
                    multiline: true
                    text: qsTr("Album")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }
            }
            Container {
                background: Color.LightGray
                Label {
                    id: val_album
                    multiline: true
                    text: app.album
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 70
                }
            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                background: Color.Gray
                Label {
                    id: lbl_title
                    multiline: true
                    text: qsTr("Title")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }
            }
            Container {
                background: Color.LightGray
                Label {
                    id: val_title
                    multiline: true
                    text: app.title
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 70
                }
            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                background: Color.Gray
                Label {
                    id: lbl_genre
                    multiline: true
                    text: qsTr("Genre")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }
            }
            Container {
                background: Color.LightGray
                Label {
                    id: val_genre
                    multiline: true
                    text: app.genre
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 70
                }
            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                background: Color.Gray
                Label {
                    id: lbl_year
                    multiline: true
                    text: qsTr("Year")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }
            }
            Container {
                background: Color.LightGray
                Label {
                    id: val_year
                    multiline: true
                    text: app.year
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 70
                }
            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                background: Color.Gray
                Label {
                    id: lbl_bitrate
                    multiline: true
                    text: qsTr("Bit Rate")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }
            }
            Container {
                background: Color.LightGray
                Label {
                    id: val_bitrate
                    multiline: true
                    text: app.bitrate
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 70
                }
            }
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                background: Color.Gray
                Label {
                    id: lbl_frequency
                    multiline: true
                    text: qsTr("Frequency")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }
            }
            Container {
                background: Color.LightGray
                Label {
                    id: val_frequency
                    multiline: true
                    text: app.frequency
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 70
                }
            }
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                background: Color.Gray
                Label {
                    id: lbl_version
                    multiline: true
                    text: qsTr("Version")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }
            }
            Container {
                background: Color.LightGray
                Label {
                    id: val_version
                    multiline: true
                    text: app.version
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 70
                }
            }
        }

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                background: Color.Gray
                Label {
                    id: lbl_layer
                    multiline: true
                    text: qsTr("Layer")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 30
                }
            }
            Container {
                background: Color.LightGray
                Label {
                    id: val_layer
                    multiline: true
                    text: app.layer
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 70
                }
            }
        }

    }

    attachedObjects: [
        AboutSheet {
            id: aboutInfo
        },
        FilePicker {
            id: filePicker
            type: FileType.Picture
            title: "Select MP3 File"
            directories: [ "/accounts/1000/shared/music" ]
            filter: [ "*.mp3" ]
            onFileSelected: {
                console.log("fileSelected signal received : " + selectedFiles);
                mainpage.showMetaData(selectedFiles[0]);
            }
            onCanceled: {
                console.log("canceled signal received");
            }
        }
    ]
    actions: [
        ActionItem {
            title: qsTr("About")
            imageSource: "asset:///images/about.png"
            onTriggered: {
                aboutInfo.open();
            }
        }
    ]

}
