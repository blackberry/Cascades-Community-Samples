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

TabbedPane {
    id: pane
    tabs: [
        Tab {
            id: bluetoothTab
            title: "Bluetooth"
            content: BluetoothSettings {
                bluetoothManager: bluetoothManager
            }
        },
        Tab {
            id: controlsTab
            title: "Controls"
            content: Controls {
                bluetoothManager: bluetoothManager
            }
            imageSource: "asset:///images/controls.png"
        }
    ]
    attachedObjects: [
        BluetoothManager {
            id: bluetoothManager
            onConnectionChanged: {
                if (connection == null) {
                    pane.activeTab = bluetoothTab;
                } else {
                    pane.activeTab = controlsTab;
                }
            }
        }
    ]
}