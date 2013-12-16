/* Copyright (c) 2013 BlackBerry Limited.
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

Sheet {
    id: root

    Page {
        id: charPage
        property string statusMessage: ""
        property string selectedDeviceName: ""
        property string selectedServiceName: ""

        Container {

            layout: DockLayout {
            }
            onCreationCompleted: {
                cmgr.scanStarted.connect(startActivityIndicator);
                cmgr.scanStopped.connect(stopActivityIndicator);
                cmgr.connectionError.connect(connectionError);
            }

            function startActivityIndicator(name, service) {
                charPage.selectedDeviceName = name;
                charPage.selectedServiceName = service;
                charPage.statusMessage = top_label.getActivityText();
                activityIndicator.start();
            }

            function stopActivityIndicator() {
                charPage.statusMessage = charPage.selectedDeviceName + " > " + charPage.selectedServiceName;
                activityIndicator.stop();
            }

            function connectionError(error) {
                root.close();
            }

            ActivityIndicator {
                id: activityIndicator
                preferredWidth: 300
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
            Container {
                layout: StackLayout {
                }

                CharacteristicsTopLabel {
                	id: top_label
                }

                ListView {
                    id: rootView
                    dataModel: cmgrModel
                    listItemComponents: [
                        ListItemComponent {
                            type: "header"
                            StandardListItem {
                                title: ListItemData.characteristic_description
                                description: "UUID: " + ListItemData.characteristic_uuid + ", Handle: " + ListItemData.characteristic_handle + ", Value Handle: " + ListItemData.characteristic_value_handle
                            }
                        }
                    ]
                    onTriggered: {
                        var selectedItem = dataModel.data(indexPath);
                        console.log("QQQQ selected item(" + indexPath + ") : " + selectedItem.characteristic_uuid);
                        // TODO:
                        cmgr.selectCharacteristic(selectedItem.characteristic_uuid, indexPath);
                        charDetailsSheet.open();
                        charDetailsSheet.setPeekEnabled(false);
                    }
                    function itemType(data, indexPath) {
                        console.log("QQQQ indexPath: " + indexPath);
                        console.log("QQQQ indexPath.length: " + indexPath.length);

                        if (indexPath.length == 1) {
                            return "header";
                        } else {
                            return "listItem";
                        }
                    }
                }
            }
        }
        actions: [
            ActionItem {
                id: action_close
                title: "Close"
                imageSource: "asset:///images/previous.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    root.close();
                }
            }
        ]
        attachedObjects: [
            CharacteristicDetailsSheet {
                id: charDetailsSheet
            }
        ]
    }
}
