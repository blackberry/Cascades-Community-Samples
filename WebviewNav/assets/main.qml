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
    id: thePage
    ///Hide the action bar during a scroll event
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    titleBar: TitleBar {
        scrollBehavior: TitleBarScrollBehavior.NonSticky
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties {
            
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                rightPadding: 10
                leftPadding: 10
                Label {
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                    text: "url"
                    verticalAlignment: VerticalAlignment.Center
                }
                TextField {
                    id: urlBar
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 8
                    }
                    hintText: "Enter and address"
                    verticalAlignment: VerticalAlignment.Center
                    textFormat: TextFormat.Plain
                    onFocusedChanged: {
                        if (focused == true) {

                        }
                    }
                    inputMode: TextFieldInputMode.Url
                    input.submitKey: SubmitKey.Go
                    input {
                        onSubmitted: {
                            if (urlBar.text.indexOf("http:") != -1 || urlBar.text.indexOf("https:") != -1) {
                                webView.url = urlBar.text
                            } else {
                                webView.url = "http://" + urlBar.text
                            }
                        }
                    }
                }
            }

        }
    }
    actions: [
        ActionItem {
            title: "Back"
            imageSource: "asset:///images/ic_previous.png"
            enabled: webView.canGoBack
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                webView.goBack()
            }
        },
        ActionItem {
            title: "Reload"
            imageSource: "asset:///images/ic_refresh.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                webView.reload()
            }
        },
        ActionItem {
            title: "Forward"
            imageSource: "asset:///images/ic_next.png"
            enabled: webView.canGoForward
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                webView.goForward()
            }
        }
    ]

    Container {
        layout: DockLayout {
        }
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        // To enable scrolling in the WebView, it is put inside a ScrollView.
        ScrollView {
            // Scrolling is restricted to vertical direction only, in this particular case.
            scrollRole: ScrollRole.Main
            scrollViewProperties.scrollMode: ScrollMode.Vertical
            scrollViewProperties.pinchToZoomEnabled: true
            scrollViewProperties.minContentScale: 1.0
            scrollViewProperties.maxContentScale: 4.0
            onContentScaleChanged: {
                //here we allow for scrolling beth vertically and horixontally if the user zooms in
                if (contentScale > 1.0) {
                    scrollViewProperties.scrollMode = ScrollMode.Both
                } else {
                    scrollViewProperties.scrollMode = ScrollMode.Vertical
                }
            }

            WebView {
                id: webView
                // The url that is loaded points to OSBB.
                url: "http://opensourcebb.com"
                onUrlChanged: {
                    reload()
                    progressIndicator.setOpacity(1.0)
                }
                onLoadProgressChanged: {
                    // Update the ProgressBar while loading.
                    progressIndicator.value = loadProgress / 100.0
                }
                onLoadingChanged: {

                    //All this pertains to the progress bar and dislpays some HTML if there is an error loading
                    if (loadRequest.status == WebLoadStatus.Started) {
                        // Show the ProgressBar when loading started.
                        progressIndicator.opacity = 1.0
                    } else if (loadRequest.status == WebLoadStatus.Succeeded) {
                        // Hide the ProgressBar when loading is complete.
                        progressIndicator.opacity = 0.0
                    } else if (loadRequest.status == WebLoadStatus.Failed) {
                        // If loading failed, fallback to inline HTML, by setting the HTML property directly.
                        html = "<html><head><title>Fallback HTML on Loading Failed</title><style>* { margin: 0px; padding 0px; }body { font-size: 48px; font-family: monospace; border: 1px solid #444; padding: 4px; }</style> </head> <body>Oh ooh, loading of the URL that was set on this WebView failed. Perhaps you are not connected to the Internet?</body></html>"
                        progressIndicator.opacity = 0.0
                    }
                }

                // This is the Navigation-requested signal handler so just print to console to illustrate usage.
                onNavigationRequested: {
                    //just console stuff
                    console.debug("NavigationRequested: " + request.url + " navigationType=" + request.navigationType)
                }
            }
        } // ScrollView

        // A progress indicator that is used to show the loading status
        Container {
            topPadding: 1
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Top

            ProgressIndicator {
                id: progressIndicator
                opacity: 0
            }
        }
    } // mainContainer
}
