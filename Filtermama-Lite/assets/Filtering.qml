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
import bb.data 1.0

BasePage {
    id: filterPage
    property variant shareOrSave
    property variant activeThumb: ""

    onCreationCompleted: {
        // you can change the head image to whatever you like for each page
        headerimage.imageSource = "asset:///images/header-filtermama.png";
    }

    Container {
        id: rootContainer
        background: back.imagePaint
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill

        layout: DockLayout {
        }

        /* ==================================================
         *      scrollview
         * ==================================================
         */

        ScrollView {
            scrollViewProperties.scrollMode: ScrollMode.None
            scrollViewProperties.pinchToZoomEnabled: false
            scrollViewProperties.minContentScale: 1.0
            scrollViewProperties.overScrollEffectMode: OverScrollEffectMode.None
            preferredHeight: Infinity
            preferredWidth: Infinity
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Center

            /* ==================================================
             *      webview
             * ==================================================
             */

            WebView {
                id: webview
                preferredHeight: Infinity
                preferredWidth: Infinity
                url: 'local:///assets/htdocs/index.html'

                // make sure to comment out webinspector before releasing
                settings.webInspectorEnabled: true
                settings.background: Color.Black

                // viewport setup
                settings.viewport: {
                    'width': 'device-width',
                    'height': 'device-height',
                    'initial-scale': 1.0
                }

                // message received from webview
                onMessageReceived: {
                    var data = message.data;

                    // filtering completed
                    if (data === 'filter-done') {
                        resetMama();

                        // is this a base64 encoded image sent to us from the webview for saving/sharing?
                    } else if (data.indexOf('data:image/png;base64,') > -1) {

                        // save the photo
                        data = data.replace('data:image/png;base64,', '');
                        filePath = app.savePhoto(data);

                        if (filePath) {
                            fileSaved = true;
                        }

                        // share file
                        if (shareOrSave === "share") {
                            app.shareFile(filePath);
                        } else {
                            toast.body = "Photo saved!";
                            toast.show();
                        }
                    }
                    spinnerOff();
                }
            } // end webview
        } // end scrollview

        // background image for filter thumbnails
        ImageView {
            id: backgroundFiltersImg
            imageSource: "asset:///images/background-filters.png"
            preferredHeight: 214
            preferredWidth: 768
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Center
        }

        /* ==================================================
         *      listview - thumbnails
         * ==================================================
         */

        ListView {
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Center
            scrollIndicatorMode: ScrollIndicatorMode.None
            snapMode: SnapMode.LeadingEdge
            flickMode: FlickMode.Momentum
            preferredHeight: 180
            leftPadding: 14

            layout: StackListLayout {
                orientation: LayoutOrientation.LeftToRight
                headerMode: ListHeaderMode.None
            }

            function itemType(data, indexPath) {
                return "item"
            }

            dataModel: ArrayDataModel {
                id: theDataModel
            }

            // set image source of the listItem
            function setImageSource(filter) {
                if (filter.type == "default") {
                    return filter.thumbnail;
                }
            }

            function handleFilterClick(filterData, filterItem) {
                spinnerOn();

                if (activeThumb !== "") {
                    activeThumb.opacity = 1;
                }

                activeThumb = filterItem;
                filterItem.opacity = 0.2;

                var msg = {
                    'cmd': 'filter',
                    'data': filterData
                };

                var message = JSON.stringify(msg);
                webview.postMessage(message);
            }

            listItemComponents: [
                ListItemComponent {
                    type: "item"
                    Container {
                        rightPadding: 18
                        id: filterItem
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }

                        Container {
                            verticalAlignment: VerticalAlignment.Bottom
                            horizontalAlignment: HorizontalAlignment.Center

                            layout: DockLayout {
                            }

                            ImageButton {
                                id: filterImageButton
                                preferredHeight: 160
                                preferredWidth: 160
                                defaultImageSource: filterItem.ListItem.view.setImageSource(ListItemData)
                                pressedImageSource: filterItem.ListItem.view.setImageSource(ListItemData)
                                leftMargin: 50.0

                                onClicked: {
                                    filterItem.ListItem.view.handleFilterClick(ListItemData, filterItem);
                                }
                            }

                            Label {
                                text: ListItemData.title
                                verticalAlignment: VerticalAlignment.Bottom
                                horizontalAlignment: HorizontalAlignment.Center
                                textStyle.color: Color.create("#f1f1f1")
                                textStyle.fontWeight: FontWeight.W500
                                textStyle.textAlign: TextAlign.Center
                                textStyle.fontSizeValue: 0.75
                            }
                        }
                    } // container
                } // listitemcomponent
            ]
        } // listview

        /* ==================================================
         *      covers screen to prevent touching while filtering
         * ==================================================
         */

        Container {
            id: pageCover
            visible: false
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
        }

        /* ==================================================
         *      attached objects
         * ==================================================
         */

        attachedObjects: [
            DataSource {
                id: dataSource
                source: "asset:///filters/filters.json"

                onDataLoaded: {
                    for (var i = 0; i < data.length; i ++) {
                        theDataModel.append(data[i])
                    }
                }
            },

            // background image
            ImagePaintDefinition {
                id: back
                repeatPattern: RepeatPattern.XY
                imageSource: "asset:///images/background-body-fullsize.png"
            }
        ]

    }

    /* ==================================================
     *      spinner helpers
     * ==================================================
     */
    function spinnerOn() {
        spinner.visible = true;
        pageCover.visible = true;
    }

    function spinnerOff() {
        spinner.visible = false;
        pageCover.visible = false;
    }

    /* ==================================================
     *      update webview when new file is picked
     * ==================================================
     */

    function updateWebview(imgData) {
        toastProgress.show();

        resetMama();

        if (activeThumb !== "") {
            activeThumb.opacity = 1;
            activeThumb = "";
        }

        var msg = {
            'cmd': 'loadPhoto',
            'data': imgData
        };

        var message = JSON.stringify(msg);
        webview.postMessage(message);

        if (! thumbnailsLoaded) {

            // load default filters
            dataSource.load();
            thumbnailsLoaded = true;
        }

        tabs.activeTab = tabProcess;
    }

    /* ==================================================
     *      reset our globals
     * ==================================================
     */

    function resetMama() {
        filePicked = true;
        fileSaved = false;
        filePath = false;
    }

    /* ==================================================
     *      actions
     * ==================================================
     */

    actions: [

        ActionItem {
            title: qsTr("Capture") + Retranslate.onLanguageChanged
            imageSource: "asset:///images/camera.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                filepicker.open();
            }
        },

        ActionItem {
            title: qsTr("Save") + Retranslate.onLanguageChanged
            imageSource: "asset:///images/save.png"

            onTriggered: {
                shareOrSave = "save";

                if (fileSaved) {
                    toast.body = qsTr("Photo saved!") + Retranslate.onLanguageChanged;
                    toast.show();

                    // not saved yet, let's do it now!!
                } else {
                    spinnerOn();

                    var msg = {
                        'cmd': 'savePhoto',
                        'data': null
                    };

                    var message = JSON.stringify(msg);
                    webview.postMessage(message);
                }
            }
        },

        ActionItem {
            title: qsTr("Share") + Retranslate.onLanguageChanged
            imageSource: "asset:///images/share.png"

            onTriggered: {
                shareOrSave = "share";

                // if already saved, don't save, just share
                if (fileSaved) {
                    app.shareFile(filePath);

                    // not saved yet, let's do it now!!
                } else {
                    spinnerOn();

                    var msg = {
                        'cmd': 'savePhoto',
                        'data': null
                    };

                    var message = JSON.stringify(msg);
                    webview.postMessage(message);
                }
            }
        }

    ]
}