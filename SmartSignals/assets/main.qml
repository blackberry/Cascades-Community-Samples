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
import bb.cascades.bps 1.0

/**
 * Use the controller.exe bundled with the simulator to manipulate the sensor
 * readings and get a feel for how these events can be used within your application.
 */

Page {
    content: Container {
        layout: AbsoluteLayout {
        }
        preferredWidth: 1280
        preferredHeight: 768
        onCreationCompleted: {
            lightService.setEnableSkipDuplicates(true);
        }
        attachedObjects: [
            /*
             * This entire block of code initializes the BlackBerry Platform Services (BPS)
             * Monitor (BPSMonitor) class written in C++ and registers the sensors for which
             * we would like to receive events
             */
            BPSMonitor {
                id: bpsMonitor
                //Receive events when the Azimuth Pitch and Roll change
                azimuthPitchRollService: AzimuthPitchRollService {
                    onAzimuthPitchRollData: {
                        cloud1.rotationZ = roll
                        cloud2.rotationZ = pitch
                        cloud3.rotationZ = azimuth
                    }
                }
                //Receive events when the ambient light around the device changes
                lightService: LightService {
                    id: lightService
                    onIlluminanceData: {
                        if (illuminance == 0) {
                            alertSign.opacity = 1;
                        } else {
                            // This calculation is a bit ugly but it gives a linear scale when 
                            // simulating illuminance changes from controller.exe. It may be best
                            // to remove this when testing with a real device.
                            var logIlluminance = 1 - Math.pow(illuminance, 1 / 5) / 9.177;
                            // Set the opacity of the AlertSign to reflect the amount of light 
                            // the device is currently receiving
                            alertSign.opacity = logIlluminance;
                        }
                    }
                }
                //Receive events when the keyboard becomes visible or hides
                virtualKeyboardService: VirtualKeyboardService {
                    id: virtualKeyboardService
                    // Swipe from the bottom-left bezel towards the center of the
                    // screen to force the keyboard to be visible 
                    onKeyboardVisible: {
                        statusBar.translationY = 768 - statusBar.preferredHeight - 300
                        statusBar.setText("Keyboard Visible");
                    }
                    // Press the hide keyboard button on the keyboard to hide it
                    onKeyboardHidden: {
                        statusBar.translationY = 768 - statusBar.preferredHeight
                        statusBar.setText("Keyboard Hidden");
                    }
                }
                //Receive events when the device changes locations
                geolocationService: GeolocationService {
                    id: geolocationService
                    period: 20 //Receive updates every 20 seconds
                    onLocationUpdate: {
                        statusBar.setText("Location Update: " + latitude + ", " + longitude);
                    }
                }
                //Receive events when the device proximity sensor registers that the device
                // has entered close proximity of another object or exited
                proximityService: ProximityService {
                    id: proximityService
                    property int proximityVal: 1
                    onProximityData: {
                        if (proximity != proximityVal) {
                            proximityVal = proximity;
                            if (proximityVal == 0) {
                                statusBar.setText("I need some space!");
                            } else {
                                statusBar.setText("Space received, thank-you");
                            }
                        }
                    }
                }
            }
        ]

        // sky background
        ImageView {
            imageSource: "asset:///images/Sky.png"
        }
        AlertSign {
            id: alertSign
        }
        ImageView {
            id: hills
            imageSource: "asset:///images/Hills.png"
            translationY: 314.0
        }
    
        Container {
            preferredHeight: 350
            preferredWidth: 1280
            layout: DockLayout {
            }
            // Add 3 buttons to the screen, take a look at the onSelected functions
            // to see what happens when these are selected.
            Container {
                id: cloud1
                CloudButton {
                    imageSource: "asset:///images/Cloud1.png"
                    shadowImageSource: "asset:///images/Cloud1_shadow.png"
                    text: "Azimuth"
                    onSelected: {
                        alertSign.activate()
                        statusBar.setText("Azimuth Pressed")
                    }
                }
                layoutProperties: DockLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Left
                    verticalAlignment: VerticalAlignment.Center
                }
                rotationZ: -10.0
                translationX: 19.0
            }
            CloudButton {
                id: cloud2
                imageSource: "asset:///images/Cloud2.png"
                shadowImageSource: "asset:///images/Cloud2_shadow.png"
                text: "Pitch"
                layoutProperties: DockLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Top
                }
                rotationZ: 1.0
                translationY: 13.0
                onSelected: {
                    alertSign.deactivate()
                    statusBar.setText("Pitch Pressed")
                }
            }
            Container {
                id: cloud3
                CloudButton {
                    imageSource: "asset:///images/Cloud3.png"
                    shadowImageSource: "asset:///images/Cloud3_shadow.png"
                    text: "Roll"
                    onSelected: {
                        statusBar.setText("Roll Pressed")
                    }
                }
                layoutProperties: DockLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: VerticalAlignment.Center
                }
                rotationZ: 28.0
            }
        }
        
        // The StatusBar will display some text notifications to the end user
        StatusBar {
            id: statusBar
            preferredHeight: 150
            preferredWidth: 1280
            translationY: 768 - statusBar.preferredHeight
        }
    }
}
