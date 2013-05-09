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
import CustomTimer 1.0

NavigationPane {
    id: navigationPane
    Page {
        Container {
            id: mainContainer
            property bool connecting: false
            property bool selectImage: false

            background: Color.Black
            layout: StackLayout {
            }
            Container {
	            layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
	            Button {
	                horizontalAlignment: HorizontalAlignment.Center
	                verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 10
                    }
                    text: qsTr("Off")
	                imageSource: "asset:///images/monitor.png"
	                onClicked: {
	                    _btLe.sendAlertOff();
	                }
                    enabled: _btLe.findMeServiceIsConnected
                }
	            Button {
	                horizontalAlignment: HorizontalAlignment.Center
	                verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 10
                    }
                    text: qsTr("Mild")
	                imageSource: "asset:///images/monitor.png"
	                onClicked: {
	                    _btLe.sendAlertMild();
	                }
                    enabled: _btLe.findMeServiceIsConnected
                }
	            Button {
	                horizontalAlignment: HorizontalAlignment.Center
	                verticalAlignment: VerticalAlignment.Center
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 10
                    }
                    text: qsTr("High")
	                imageSource: "asset:///images/monitor.png"
	                onClicked: {
	                    _btLe.sendAlertHigh();
	                }
                    enabled: _btLe.findMeServiceIsConnected
                }
	        }
            ImageLabel {
                topPadding: 20
                label: qsTr("Bluetooth Status")
                image: _btLe.bluetoothActive ? "asset:///images/led_green_black.png" : "asset:///images/led_red_black.png"
            }
            ImageLabel {
                label: ( _btLe.bluetoothActive & _btLe.remoteDeviceSelected ) ? _btLe.remoteDeviceInfo.name : qsTr("No device selected yet")
                image: ( _btLe.bluetoothActive & _btLe.remoteDeviceSelected ) ? "asset:///images/led_green_black.png" : "asset:///images/led_red_black.png"
            }
            ImageLabel {
                id: connectedLabel
                visible: !mainContainer.connecting
                label: qsTr("Immediate Alert Service Connected")
                image: _btLe.findMeServiceIsConnected ? "asset:///images/led_green_black.png" : "asset:///images/led_red_black.png"
            }
            ImageLabel {
                id: connectingLabel
                visible: mainContainer.connecting
                label: qsTr("Immediate Alert Service Connecting")
                image: mainContainer.selectImage ? "asset:///images/led_green_black.png" : "asset:///images/led_red_black.png"
            }

            Divider {}
			
            Label {
                id: labelSelect
                text: _btLe.findMeServiceIsConnected ? "Select Alert Level from top buttons" : "Please Select a Bluetooth Device"
                topPadding: 20
                leftPadding: 20
                rightPadding: 20
                bottomPadding: 20
                textStyle {
                    fontWeight: FontWeight.Bold
                }
                horizontalAlignment: HorizontalAlignment.Center
            }

            Container {
                topPadding: 20
                leftPadding: 20
                rightPadding: 20
                bottomPadding: 20
		        
		        ListView {
		            dataModel: _btLe.deviceListing.model
		
		            listItemComponents: [
		                ListItemComponent {
		                    type: "listItem"
		                    StandardListItem {
		                        title: ListItemData.deviceName
		                        description: ListItemData.deviceAddress
		                    }
		                }
		            ]
		
		            onTriggered: {
		                var selectedItem = dataModel.data(indexPath);
		                
		                _btLe.setRemoteDevice(selectedItem.deviceAddress);

                        if (!_btLe.serviceIsInitialised) {
                            _btLe.initialiseService();
                        }
                        if (_btLe.findMeServiceIsConnected) {
                            _btLe.disconnnectFromImmediateAlertService();
                            // seem to have to do this to avoid a resource busy situation
                            connectionTimer.start();
                        } else {
                            _btLe.connectToImmediateAlertService();
                        }
                        mainContainer.connecting = true;
                        flashConnectingState.start();
                    }
		
		            function itemType(data, indexPath) {
		                if (indexPath.length == 1) {
		                    return "header";
		                } else {
		                    return "listItem";
		                }
		            }
		        }
		    }
            Timer {
                id: connectionTimer
                time_limit: 1000
                onTimeout: {
                    connectionTimer.stop();
                    _btLe.connectToImmediateAlertService();
                }
            }
            Timer {
                id: flashConnectingState
                time_limit: 500
                onTimeout: {
                    if (_btLe.findMeServiceIsConnected) {
                        mainContainer.connecting = false;
                        flashConnectingState.stop();
                    } else {
                        if (mainContainer.selectImage) {
                            mainContainer.selectImage = false;
                        } else {
                            mainContainer.selectImage = true;
                        }
                    }
                }
            }
        }

        actions: [
            ActionItem {
		        title: qsTr("Scan")
		        imageSource: "asset:///images/scan.png"
		        onTriggered: {
		            _btLe.deviceListing.discover();
		        }
		    },
            ActionItem {
                title: qsTr("About")
                imageSource: "asset:///images/about.png"
                onTriggered: {
                    aboutInfo.open();
                }
            }
        ]

        attachedObjects: [
            AboutSheet {
                id: aboutInfo
            }
        ]
    }

    onCreationCompleted: {
        console.log("XXXX NavigationPane - onCreationCompleted()");
        OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.All;
    }
    
    onPopTransitionEnded: {
        console.log("XXXX NavigationPane - onPopTransitionEnded()");
        page.destroy();
    }
    
}
