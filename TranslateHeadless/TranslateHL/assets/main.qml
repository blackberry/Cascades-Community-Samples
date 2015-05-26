/*
 * Copyright (c) 2013-2014 BlackBerry Limited.
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
    Menu.definition: MenuDefinition {
        actions: [
            ActionItem {
                title: qsTr("Shutdown")
                onTriggered: {
                    // Only used during app development
                    app.sendToHeadless("SHUTDOWN")
                }
            }
        ]
    }
    Container {
        layout: DockLayout {}
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            Label {
                // Localized text with the dynamic translation and locale updates support
                text: qsTr("The app use the default language : English") + Retranslate.onLocaleOrLanguageChanged
                horizontalAlignment: HorizontalAlignment.Center
                multiline: true
            }
            Label {
                // Localized text with the dynamic translation and locale updates support
                text: qsTr("Available languages : English, French, Italian and Spanish") + Retranslate.onLocaleOrLanguageChanged
                horizontalAlignment: HorizontalAlignment.Center
                multiline: true
            }
            Button {
                // Localized text with the dynamic translation and locale updates support
                text: qsTr("Change system language") + Retranslate.onLocaleOrLanguageChanged
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    app.invokeSettings()
                }
            }
            Container {
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                horizontalAlignment: HorizontalAlignment.Center
                DropDown {
                    id: dropDown
                    maxWidth: 250
                    Option { text: "en_US"; selected: true }
                    Option { text: "en_GB" }
                    Option { text: "es" }
                    Option { text: "fr_CA" }
                    Option { text: "fr_FR" }
                    Option { text: "it" }
                }
                Button {
                    // Localized text with the dynamic translation and locale updates support
                    text: qsTr("Simulate language change") + Retranslate.onLocaleOrLanguageChanged
                    horizontalAlignment: HorizontalAlignment.Center
                    maxWidth: 600
                    onClicked: {
                        if (dropDown.selectedIndex >= 0) {
                            app.onSystemLanguageChanged(dropDown.selectedOption.text)
                            app.sendToHeadless("LANGUAGE_CHANGE", dropDown.selectedOption.text)
                        }
                    }
                }
            }
        }
    }
}
