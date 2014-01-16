/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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
import com.robwilliamsjnr.holidaytrain 1.0

Page {
    titleBar: TitleBar {
        title: "Holiday Train"
    }

    property BluetoothManager bluetoothManager

    onCreationCompleted: {
        bluetoothManager.connectionChanged.connect(connectionChanged);
    }

    content: ScrollView {
        content: Container {
            id: container

            function command(command) {
                console.log("Doing " + command);
                bluetoothManager.connection.sendMessage(command);
            }

            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            Container {
                visible: bluetoothManager.connection==null
                horizontalAlignment: HorizontalAlignment.Fill
                layout: DockLayout {
                }
                topPadding: 80
                bottomPadding: 80
                background: Color.Red
                Label {
                    
                    horizontalAlignment: HorizontalAlignment.Center
                 	text: "No bluetooth connection"
                    textStyle.color: Color.create("#ff7f7f00")
                }
            }
            ImageView {
                imageSource: "asset:///images/engine.JPG"
                scalingMethod: ScalingMethod.AspectFill

            }
            ControlRow {
                topMargin: 5
                bottomMargin: 5
                onCommand: {
                    container.command("d" + command);
                }
            }
            LightRow {
                light: "a"
                onCommand: {
                    container.command(command);
                }
            }
            LightRow {
                light: "b"
                onCommand: {
                    container.command(command);
                }
            }
            LightRow {
                light: "c"
                onCommand: {
                    container.command(command);
                }
            }
            LightRow {
                light: "d"
                onCommand: {
                    container.command(command);
                }
            }
        }
    }
}
