/* Copyright (c) 2014 BlackBerry Ltd.
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
 * 
 * The Lithium REST APIs are owned by Lithium Technologies, Inc. 
 * (http://www.lithium.com) and are not part of the “Works” licensed 
 * hereunder pursuant to the Apache 2.0 license.
 */

import bb.cascades 1.2

import bb.cascades 1.2
import com.msohm.EULA 1.0

Sheet {
    id: eulaSheet
    Page {
        titleBar: TitleBar {
            title: eula.eulaTitle
        }
        Container {
            leftPadding: 10.0
            topPadding: 10.0
            rightPadding: 10.0
            bottomPadding: 10.0

            ScrollView {
                scrollViewProperties.scrollMode: ScrollMode.Vertical
                TextArea {
                    id: eulaBody
                    text: eula.eulaBody
                    textFormat: TextFormat.Html
                    editable: false
                }
            }

            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight

                }
                horizontalAlignment: HorizontalAlignment.Fill

                Button {
                    id: agreeButton
                    text: eula.eulaAgreeButton
                    onClicked: {
                        eula.agreeToEULA();
                        eulaSheet.close();
                    }
                }
                Button {
                    id: disAgreeButton
                    text: eula.eulaDisagreeButton
                    horizontalAlignment: HorizontalAlignment.Right
                    onClicked: {
                        Application.quit();
                    }
                }
            }
        }
        attachedObjects: [

            EULA {
                id: eula
            }
        ]
    }
}
