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
import bb.cascades.pickers 1.0
import bb.system 1.0

Page {
    id: utilsPage
    property alias filepicker: picker
    property alias headerimage: headerImage
    property alias toast: toast
    property alias toastProgress: toastProgress
    property alias spinner: spinner

    /* ==================================================
     *      titlebar
     * ==================================================
     */
    titleBar: TitleBar {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties {

            // left side
            Container {
                leftPadding: 8
                preferredWidth: Infinity
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Fill

                layout: DockLayout {
                }

                // header image
                ImageView {
                    id: headerImage
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Left
                    scalingMethod: ScalingMethod.None

                }

                // spinner
                Container {
                    id: spinner
                    visible: false
                    verticalAlignment: VerticalAlignment.Fill
                    horizontalAlignment: HorizontalAlignment.Fill
                    opacity: 1

                    ActivityIndicator {
                        id: theSpinner
                        verticalAlignment: VerticalAlignment.Top
                        horizontalAlignment: HorizontalAlignment.Right
                        preferredWidth: 106
                        preferredHeight: 106
                        running: true
                    }
                }
            }
        }
    }

    attachedObjects: [
        // toasts
        SystemToast {
            id: toast
        },

        // progress toast
        SystemProgressToast {
            id: toastProgress
            body: qsTr("Loading ...") + Retranslate.onLanguageChanged

        },

        // filepicker
        FilePicker {
            id: picker
            property string pickedPhoto
            title: qsTr("Pick a photo") + Retranslate.onLanguageChanged
            type: FileType.Picture
            imageCropEnabled: true

            onFileSelected: {
                filePicked = true;
                var pickedPhoto = selectedFiles[0];
                var b64 = app.copyToData(pickedPhoto);
                tabProcess.updateImage(b64)
            }

            onCanceled: {
                tabs.activeTab = tabs.lastTab;
            }
        }
    ]
}
