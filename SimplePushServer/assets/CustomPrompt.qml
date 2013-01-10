/* Copyright (c) 2012 Research In Motion Limited.
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

Dialog {
    id: dialog
    property alias text: askLabel.text
    property alias confirmButtonText: confirmButton.text
    property alias cancelButtonText: cancelButton.text
    signal confirmed()
    signal cancelled()
    Container {
        preferredHeight: maxHeight
        preferredWidth: maxWidth
        layout: DockLayout {
        }
        Container {
            preferredHeight: maxHeight
            preferredWidth: maxWidth
            background: Color.Gray
            opacity: 0.45
        }
        Container {
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            leftPadding: 25
            rightPadding: 25
            layout: DockLayout {
            }
            ImageView {
                imageSource: "asset:///images/backgroundFrame.png.amd"
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
            }
            Container {
                leftPadding: 50
                rightPadding: 50
                topPadding: 50
                bottomPadding: 50
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                Label {
                    id: askLabel
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.textAlign: TextAlign.Center
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Button {
                        id: confirmButton
                        onClicked: {
                            confirmed();
                        }
                    }
                    Button {
                        id: cancelButton
                        onClicked: {
                            cancelled();
                        }
                    }
                }
            }
        }
    }
}
