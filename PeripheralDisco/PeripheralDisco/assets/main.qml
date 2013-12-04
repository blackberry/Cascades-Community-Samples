/* Copyright (c) 2013 BlackBerry.
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

TabbedPane {
    activeTab: serial
    tabs: [
        Tab {
            id: disco
            title: "Peripheral Disco"
            content: Page {

                actions: [
                    ActionItem {
                        title: "Clear Log"
                        onTriggered: {
                            log.text = "<div></div>"
                        }
                        ActionBar.placement: ActionBarPlacement.OnBar
                    }
                ]
                onCreationCompleted: {
                    app.log.connect(onLog);
                }
                function onLog(toAdd) {
                    var current = log.text;
                    if (current.length != 0) {
                        toAdd = "\n" + toAdd;
                    }
                    current += toAdd;
                    log.text = current;
                }
                titleBar: TitleBar {
                    title: "Peripheral Disco"
                }
                content: Container {
                    layout: DockLayout {
                    }
                    verticalAlignment: VerticalAlignment.Fill
                    horizontalAlignment: HorizontalAlignment.Fill
                    preferredHeight: maxHeight
                    TextArea {
                        textFormat: TextFormat.Html
                        verticalAlignment: VerticalAlignment.Fill
                        horizontalAlignment: HorizontalAlignment.Fill
                        objectName: "log"
                        id: log
                    }
                }
                actionBarVisibility: ChromeVisibility.Visible
            }
            imageSource: "asset:///discoSmall.png"
        },
        Tab {
            id: serial
            title: "Serial Comms"
            content: Page {
                titleBar: TitleBar {
                    title: "What does the USB say?"
                }
                actions: [
                    ActionItem {
                        ActionBar.placement: ActionBarPlacement.OnBar
                        title: "Connect"
                        enabled: app.serialPossible && !app.serialConnected
                        onTriggered: {
                         	app.openSerial();   
                        }
                        imageSource: "asset:///connect.png"

                    },
                    ActionItem {
                        id: closeAction
                        ActionBar.placement: ActionBarPlacement.OnBar
                        title: "Disco Nect"
                        enabled: app.serialPossible && app.serialConnected
                        onTriggered: {
                         	app.closeSerial();   
                        }
                        imageSource: "asset:///disconnect.png"
                    }
                ]
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Fill
                    layout: DockLayout {

                    }
                    Slider {
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Fill
                        onValueChanged: {
                            app.writeSerial(value);
                        }
                        onImmediateValueChanged: {
                            app.writeSerial(immediateValue);
                        }
                        value: 0.5;
                        enabled: closeAction.enabled
                    }
                }
            }
            imageSource: "asset:///serial.png"
        }
    ]
}
