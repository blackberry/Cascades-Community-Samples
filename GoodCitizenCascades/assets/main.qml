/*
 * Copyright (c) 2011, 2012 Research In Motion Limited.
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

NavigationPane {
    Page {
        id: nav
        content: Container {
            id: back
            objectName: "back"           
            layout: StackLayout {
            }
            background: Color.create("#262626")
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
            ForeignWindow {
                id: goodCitizenCascadesFW
                objectName: "goodCitizenCascadesFW"
                preferredWidth: 768
                preferredHeight: 1140
                visible: false
                layoutProperties: DockLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Top
                }
            }           
        }

        actions: [
            ActionItem {
                title: "Translate"
                imageSource: "asset:///images/actions/translate.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    _goodCitizen.setTouchMode("translate");
                    _navPane.deprecatedPushQmlByString("ToolAxis.qml");
                }
            },
            ActionItem {
                title: "Rotation"
                imageSource: "asset:///images/actions/rotate.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    _goodCitizen.setTouchMode("rotate");
                    _navPane.deprecatedPushQmlByString("ToolAxis.qml");
                }
            },
            ActionItem {
                title: "Scale"
                imageSource: "asset:///images/actions/scale.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    _goodCitizen.setTouchMode("scale");
                    _navPane.deprecatedPushQmlByString("ToolAxis.qml");
                }
            },
            ActionItem {
                title: "Color"
                imageSource: "asset:///images/actions/colour.png"
                onTriggered: {
                    _navPane.deprecatedPushQmlByString("Color.qml");
                }
            },
            ActionItem {
                title: "Objects"
                imageSource: "asset:///images/actions/objects.png"
                onTriggered: {
                    _navPane.deprecatedPushQmlByString("Objects.qml");
                }
            },
            ActionItem {
                title: "Reset"
                imageSource: "asset:///images/actions/reset.png"
                onTriggered: {
                    _goodCitizen.reset();
                }
            }
        ]
        actionBarVisibility: ChromeVisibility.Visible
    }
}
