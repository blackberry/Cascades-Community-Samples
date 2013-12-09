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
        id: charDetailsPage
        property string statusMessage: ""
        property string selectedUuid: ""
        property string selectedDescription: ""
        property variant indexPath: null

        Container {
            layout: DockLayout {
            }
            onCreationCompleted: {
                cmgr.characteristicSelected.connect(characteristicSelected);
            }

            function characteristicSelected(uuid, indexPath) {
                charDetailsPage.indexPath = indexPath;
                charDetailsPage.selectedUuid = uuid;
                charDetailsPage.selectedDescription = cmgrModel.data(indexPath).characteristic_description;
                charDetailsPage.statusMessage = "Properties > " + charDetailsPage.selectedDescription + "(" + charDetailsPage.selectedUuid + ")";
                populateDetails();
            }

            function populateDetails() {
                var cc = cmgrModel.data(charDetailsPage.indexPath);

                if (cc.characteristic_prop_read) {
                    hex_value_label.text = "Value: 0x" + cc.characteristic_hex_value;
                } else {
                    hex_value_label.text = "Value: *** READ NOT PERMITTED ***";
                }

                read_label.text = cc.characteristic_prop_read ? "Reads are permitted" : "Reads are not permitted";
                write_label.text = cc.characteristic_prop_write ? "Writes are permitted" : "Writes are not permitted";
                write_noresp_label.text = cc.characteristic_prop_write_noresp ? "Writes without response are permitted" : "Writes without response are not permitted";
                notify_label.text = cc.characteristic_prop_notify ? "Notifications are permitted" : "Notifications are not permitted";
                indicate_label.text = cc.characteristic_prop_indicate ? "Indications are permitted" : "Indications are not permitted";
                write_signed_label.text = cc.characteristic_write_signed ? "Signed writes are permitted" : "Signed writes are not permitted";
                broadcast_label.text = cc.characteristic_prop_broadcast ? "Broadcasting is permitted" : "Broadcasting is not permitted";
                ext_prop_label.text = cc.characteristic_prop_ext_prop ? "Additional extended properties are defined" : "There are no extended properties defined";
            }

            Container {
                layout: StackLayout {
                }
                CharacteristicDetailsTopLabel {
                    
                }
                
                ScrollView {
                    id: logScroller
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill

                    Container {
                        
                        leftPadding: 10
                        
                        Label {
                            id: hex_value_label
                            text: ""
                            multiline: true
                        }

                        Label {
                            id: read_label
                            text: ""
                        }

                        Label {
                            id: write_label
                            text: ""
                        }

                        Label {
                            id: write_noresp_label
                            text: ""
                        }

                        Label {
                            id: notify_label
                            text: ""
                        }

                        Label {
                            id: indicate_label
                            text: ""
                        }

                        Label {
                            id: write_signed_label
                            text: ""
                        }

                        Label {
                            id: broadcast_label
                            text: ""
                        }
                        Label {
                            id: ext_prop_label
                            text: ""
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
    }
}
