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

Sheet {
    id: aboutSheet
    content: Page {
        titleBar: TitleBar {
            title: "About"
            dismissAction: ActionItem {
                title: qsTr("Close") + Retranslate.onLanguageChanged
                onTriggered: {
                    aboutSheet.close()
                    if (aboutSheet) {
                        aboutSheet.destroy();
                    }
                }
            }
        }

        ScrollView {
            scrollViewProperties.scrollMode: ScrollMode.Vertical
            scrollViewProperties.pinchToZoomEnabled: false
            scrollViewProperties.minContentScale: 1.0
            scrollViewProperties.overScrollEffectMode: OverScrollEffectMode.None
            preferredHeight: Infinity
            preferredWidth: Infinity
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill

            Container {
                leftPadding: 12
                rightPadding: 12
                topPadding: 40

                ImageView {
                    bottomMargin: 40
                    id: logo
                    imageSource: "asset:///images/about-logo.png"
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }

                Label {
                    id: labelAbout
                    text: "Filtermama Lite is brought to you by @chadtatro. It combines the flexibility of JavaScript, and the power of Cascades to deliver a true \"Hybrid\" application.\n\nSpecial thanks goes out to Kyle Fowler (@kfow35), Jerome Carty (@jcarty), Anzor Bashkhaz @anzorb), and Isaac Gordezky for getting me past roadblocks along the way. I'm sure that I'm forgetting somebody here, sorry brah. :S\n"
                    textStyle.fontSize: FontSize.PointValue
                    textStyle.fontSizeValue: 8
                    textStyle.fontWeight: FontWeight.W100
                    horizontalAlignment: HorizontalAlignment.Fill
                    multiline: true
                    content.flags: TextContentFlag.ActiveText

                }

                Label {
                    id: labelLegal
                    text: "Attribution: Filtermama 2 is powered in part by <a href=\"http://matthewruddy.github.io/jQuery-filter.me/\">Filter.me</a> by <a href=\"http://matthewruddy.com\">Matthew Ruddy</a> and is dual licensed under the MIT and GPL licenses. Fonts used in the logo, and headers are <a href=\"http://www.fonts4free.net/aaaiight-font.html\">\"Aaaiight!\"</a> by JW/SWE, licensed as Freeware."
                    textStyle.fontSize: FontSize.PointValue
                    textStyle.fontSizeValue: 6
                    textStyle.fontWeight: FontWeight.W100
                    horizontalAlignment: HorizontalAlignment.Fill
                    multiline: true
                    textFormat: TextFormat.Html
                    textStyle.color: Color.Gray
                    textStyle.fontStyle: FontStyle.Italic
                }
            }
        }
    }
}