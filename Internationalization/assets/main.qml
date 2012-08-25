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

TabbedPane {
    Tab {
        title: "static xml"
        Page {
            onCreationCompleted: {
                cs.updateLocale("");
            }
            content: Container {
                background: Color.Black
                layout: StackLayout {
                }
                Container {
                    background: Color.Black
                    layout: DockLayout {
                        leftPadding: 10
                        rightPadding: 10
                        bottomPadding: 10
                    }
                    Container {
                        background: Color.Black
                        translationY: 103
                        layout: DockLayout {
                        }
                        layoutProperties: DockLayoutProperties {
                            verticalAlignment: VerticalAlignment.Top
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        touchPropagationMode: TouchPropagationMode.PassThrough
                        ListView {
                            preferredHeight: 700
                            objectName: "listView"
                            id: listView
                            dataModel: XmlDataModel {
                                source: "creatures_en.xml"
                            }
                            listItemComponents: [
                                ListItemComponent {
                                    type: "item"
                                    Container {
                                        layout: StackLayout {
                                            layoutDirection: LayoutDirection.LeftToRight
                                        }
                                        id: itemRoot
                                        Container {
                                            ImageView {
                                                preferredWidth: 250
                                                preferredHeight: 200
                                                maxWidth: 250
                                                maxHeight: 200
                                                imageSource: ListItemData.imagePath
                                            }
                                        }
                                        Container {
                                            layout: StackLayout {
                                                layoutDirection: LayoutDirection.TopToBottom
                                            }
                                            TextArea {
                                                preferredHeight: 200
                                                preferredWidth: 520
                                                backgroundVisible: false
                                                editable: false
                                                text: ListItemData.infoText
                                                textStyle {
                                                    base: SystemDefaults.TextStyles.SmallText
                                                    color: Color.create("#ffffffff")
                                                }
                                            }
                                        }
                                    }
                                }
                            ]
                            onTouch: {
                                cs.suppressKeyboard(); //temp workaround for keyboard popup when readonly text area is touched
                            }
                        }
                    }
                    Container {
                        objectName: "languageContainer"
                        id: languageContainer
                        background: Color.Black
                        layoutProperties: DockLayoutProperties {
                            verticalAlignment: VerticalAlignment.Top
                        }
                        SegmentedControl {
                            layoutProperties: DockLayoutProperties {
                                verticalAlignment: VerticalAlignment.Bottom
                                horizontalAlignment: HorizontalAlignment.Center
                            }
                            objectName: "languageBar"
                            id: languageBar
                            Option {
                                text: qsTr("English")
                                value: "en"
                                selected: true
                            }
                            Option {
                                text: qsTr("French")
                                value: "fr_FR"
                            }
                            Option {
                                text: qsTr("Spanish")
                                value: "es_ES"
                            }
                            onSelectedIndexChanged: {
                                cs.updateLocale(languageBar.selectedValue());
                            }
                        }
                    }
                }
                // Top Contatiner with a RadioButtonGroup and title.
                Container {
                    layout: StackLayout {
                        topPadding: 20
                        leftPadding: 10
                    }
                    layoutProperties: StackLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                    Label {
                        objectName: "switchToLabel"
                        id: switchToLabel
                        bottomMargin: 0
                        text: qsTr("Switch to:")
                        textStyle {
                            base: SystemDefaults.TextStyles.SmallText
                            color: Color.create("#ffFDfDfD")
                        }
                        layoutProperties: StackLayoutProperties {
                            horizontalAlignment: HorizontalAlignment.Left
                        }
                    }
                    RadioGroup {
                        id: radioGroup1
                        objectName: "radioGroup1"
                        preferredWidth: 546
                        Option {
                            text: qsTr("Animals")
                            value: "creatures"
                            selected: true
                        }
                        Option {
                            text: qsTr("Objects")
                            value: "objects"
                        }
                        onSelectedIndexChanged: {
                            cs.loadXML(radioGroup1.selectedValue());
                        }
                    }
                    Label {
                        objectName: "currentLangLabel"
                        id: currentLangLabel
                        bottomMargin: 0
                        text: qsTr("Your current language is:  ") + cs.getCurrentLanguage()
                        textStyle {
                            color: Color.create("#ffFDfDfD")
                        }
                        layoutProperties: StackLayoutProperties {
                            horizontalAlignment: HorizontalAlignment.Left
                        }
                    }
                }
            }
        }
    }
}
