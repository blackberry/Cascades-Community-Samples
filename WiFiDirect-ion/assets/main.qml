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

import bb.cascades 1.2
NavigationPane {
    id: navPane
    Page {
        Container {
            horizontalAlignment: HorizontalAlignment.Center

            leftPadding: 10.0
            topPadding: 10.0
            rightPadding: 10.0
            bottomPadding: 10.0
            Label {
                text: "Are you the Operator or the Automatron?"
            }
            Divider {
                
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Button {
                    text: "I am the Operator"
                    onClicked: {
                        var page = operatorPage.createObject();
                        navPane.push(page);
                    }
                    verticalAlignment: VerticalAlignment.Center
                }
                ImageView {
                    imageSource: "asset:///images/operator.png"
                    //Operator by OCAL Source: http://www.clker.com/clipart-25783.html

                }
            }
            Divider {
                
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Button {
                    text: "I am the Automatron"
                    onClicked: {
                        var page = automatronPage.createObject();
                        navPane.push(page);
                    }
                    verticalAlignment: VerticalAlignment.Center
                }
                ImageView {
                    //Robot by Teresa Amasia  Source: http://www.clker.com/clipart-robot-hands-up-1.html
                    imageSource: "asset:///images/robot-hands-up-md.png"

                }
            }
            Divider {
                
            }

        }
    }

    attachedObjects: [
        ComponentDefinition {
            id: operatorPage
            source: "Operator.qml"
        },
        ComponentDefinition {
            id: automatronPage
            source: "Automatron.qml"
        }
    ]
}
