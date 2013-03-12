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
 
Page {
    Container {
        layout: StackLayout {
        }
        ScrollView {
            id: scrollView
            minHeight: 30
            scrollViewProperties {
                scrollMode: ScrollMode.Both
                pinchToZoomEnabled: true
            }
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1.0
            }
            Container {
                background: Color.LightGray
                WebView {
                    id: webView
                    url: "http://www.google.com"

                    onMinContentScaleChanged: {
                        scrollView.scrollViewProperties.minContentScale = minContentScale;
                    }
                    onMaxContentScaleChanged: {
                        scrollView.scrollViewProperties.maxContentScale = maxContentScale;
                    }
                    onUrlChanged: {
                        textfield.text = url;
                        back.enabled = true;
                    }
                }
            }
        }
        Divider {
            
        }
        Container {
            id: navcontainer
            leftPadding: 30
            rightPadding: leftPadding
            preferredHeight: 30
            verticalAlignment: VerticalAlignment.Top
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                id: back
                text: "<-"
                enabled: false
                preferredWidth: preferredHeight
                onClicked: {
                    webView.goBack();
                }
            }
            TextField {
                id: textfield
                text: "http://www.google.com"
            }
            Button {
                text: "Go"
                preferredWidth: preferredHeight
                onClicked: {
                     webView.url = BBBBBBrowser.formatURLStuff(textfield.text);
                }
            }
        }
    }
}
