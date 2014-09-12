/*
 * Copyright (c) 2014 BlackBerry Limited.
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

Page {

    Container {
        layout: DockLayout {

        }
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        Container {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

            Label {
                id: cacheSize
                property int byteSize: _app.cacheSize()

                text: "Cache size: " + bytesToSize(byteSize)
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.base: SystemDefaults.TextStyles.BodyText
                textStyle.fontWeight: FontWeight.W100

                function bytesToSize(bytes) {
                    if (isNaN(bytes)) {
                        return;
                    }
                    var units = [ ' bytes', ' KB', ' MB', ' GB', ' TB', ' PB', ' EB', ' ZB', ' YB' ];
                    var amountOf2s = Math.floor(Math.log(+ bytes) / Math.log(2));
                    if (amountOf2s < 1) {
                        amountOf2s = 0;
                    }
                    var i = Math.floor(amountOf2s / 10);
                    bytes = + bytes / Math.pow(2, 10 * i);

                    // Rounds to 3 decimals places.
                    if (bytes.toString().length > bytes.toFixed(3).toString().length) {
                        bytes = bytes.toFixed(3);
                    }
                    return bytes + units[i];
                }
            }
            Button {
                id: cacheButton

                horizontalAlignment: HorizontalAlignment.Center
                text: "Clear Cache"
                onClicked: {
                    _app.clearCache()
                    cacheSize.byteSize = _app.cacheSize()
                    cacheSize.setText("Cache size: " + cacheSize.bytesToSize(cacheSize.byteSize))
                }
            }
        }
    }
}