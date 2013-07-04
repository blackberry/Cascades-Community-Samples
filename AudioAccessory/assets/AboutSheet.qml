/* Copyright (c) 2013 Research In Motion Limited.
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
    id: root

    Page {
        Container {
            layout: StackLayout {
            }
            verticalAlignment: VerticalAlignment.Fill
            Label {
                id: lblHeading_about
                text: qsTr("About AudioAccessory")
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            Label {
                id: lblHeading_version
                text: qsTr("V1.0.0")
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            Label {
                id: lblBody_about
                text: qsTr("The AudioAccessory application demonstrates how to use the BlackBerry 10 audio jack as a data communication channel")
                multiline: true
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontFamily: "Courier"
                    lineHeight: 1.1
                }
            }
            Label {
                id: lblAuthors
                text: qsTr("Authors:")
                multiline: true
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontFamily: "Courier"
                    lineHeight: 1.1
                }
            }
            Label {
                id: lblAuthors2
                text: qsTr("John Murray (@jcmrim)")
                multiline: true
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontFamily: "Courier"
                    lineHeight: 1.1
                }
            }
            Label {
                id: lblAuthors1
                text: qsTr("Martin Woolley (@mdwrim)")
                multiline: true
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontFamily: "Courier"
                    lineHeight: 1.1
                }
            }

            Divider {
            }
            Button {
            	horizontalAlignment: HorizontalAlignment.Center
            	text: qsTr("Close")
                onClicked: root.close()
            }
        }
    }
}
