/* Copyright (c) 2013 BlackBerry Limited.
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
import bb.utility 1.0
import bb.cascades.pickers 1.0

Page {
    actionBarVisibility: ChromeVisibility.Overlay
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        ImageView {
            id: imageView
            scalingMethod: ScalingMethod.AspectFit
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            attachedObjects: [
                ExifReader {
                    id: exifReader
                    onRotationChanged: {
                        imageView.rotationZ = rotation
                        imageView.visible = true
                    }
                }
            ]
            image: exifReader.image
            maxHeight: (exifReader.rotation == 270 || exifReader.rotation == 90) ? layout.layoutFrame.width : layout.layoutFrame.height
            maxWidth: (exifReader.rotation == 270 || exifReader.rotation == 90) ? layout.layoutFrame.height : layout.layoutFrame.width
            minHeight: maxHeight
            minWidth: maxWidth
            
            // adjust layout for image rotation
            translationY: (exifReader.rotation == 270 || exifReader.rotation == 90) ? (layout.layoutFrame.height - layout.layoutFrame.width)/2 : 0
        }
        attachedObjects: LayoutUpdateHandler {
            id: layout
        }
    }
    actions: [
        ActionItem {
            title: qsTr("select_an_image") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///ic_open.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered:picker.open()
            attachedObjects: [
                FilePicker {
                    id: picker
                    mode: FilePickerMode.Picker
                    type: FileType.Picture
                    onFileSelected: {
                        var src = "file://" + selectedFiles[0]
                        if (exifReader.imageSource == src)
                        	return
                        
                        imageView.visible = false
                        imageView.imageSource = exifReader.imageSource = src
                    }
                }
            ]
        }
    ]
}
