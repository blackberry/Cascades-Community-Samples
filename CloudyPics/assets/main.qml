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
import bb.cascades.multimedia 1.0
import bb.multimedia 1.0
import bb.community.camera 1.0

NavigationPane {
    id: navigationPane
    Page {

        Container {
            layout: DockLayout {

            }
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            onCreationCompleted: {
                console.debug("All cameras accessible: " + camera.allCamerasAccessible);
                if (camera.allCamerasAccessible) {
                    console.debug("Opening camera");
                    camera.open(CameraUnit.Rear);
                }
            }
            Camera {
                id: camera
                objectName: "camera"
                property bool photoBeingTaken
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center

                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            if (! camera.photoBeingTaken) {
                                camera.photoBeingTaken = true;
                                camera.capturePhoto();
                                console.debug("+++++++ SNAP!");
                            }
                        }
                    }
                ]

                onCameraOpened: {

                    console.debug("+++++++ Camera opened");

                    //Restore previous settings
                    if (cameraSettingsStore.isEmpty()) {
                        camera.getSettings(cameraSettings);

                        console.debug("+++++++ No existing settings, setting some defaults");
                        cameraSettings.cameraMode = CameraMode.Photo;
                        cameraSettings.focusMode = CameraFocusMode.ContinuousAuto;
                        cameraSettings.shootingMode = CameraShootingMode.Stabilization;

                        camera.applySettings(cameraSettings);
                        console.log(cameraSettings);
                        cameraSettingsStore.saveSettings(cameraSettings);
                    } else {
                        console.debug("+++++++ Restoring previous settings");
                        cameraSettingsStore.restoreAndApplySettings(camera);
                        camera.getSettings(cameraSettings);
                    }

                    console.debug("+++++++ Camera roll path: " + cameraSettings.cameraRollPath);
                    console.debug("+++++++ Camera preview frame direction: " + camera.devicePreviewFrameDirection);

                    var captureSize = cameraSettings.captureResolution;
                    console.debug("+++++++ Camera size: " + captureSize.height + "h x " + captureSize.width + "w");
                    //Prepare a Camera Roll Dialog
                    cameraRollManager.createCameraRollDialog(cameraSettings.cameraRollPath);

                    //Now we can actually start the viewfinder
                    camera.startViewfinder();
                }
                onCameraOpenFailed: {
                    console.debug("Camera open FAILED: " + error);
                }
                onViewfinderStarted: {
                    console.debug("Viewfinder started");
                    photoBeingTaken = false;
                }
                onViewfinderStartFailed: {
                    console.debug("Viewfinder start FAILED: " + error);
                }
                onShutterFired: {
                    shutterSound.play();

                }
                onPhotoCaptureFailed: {
                    console.debug("Photo caputre FAILED: " + error);
                    photoBeingTaken = false;
                }
                onPhotoSaveFailed: {
                    console.debug("Photo save FAILED: " + error);
                    photoSavedLabel.text = "Error: " + error;
                    photoBeingTaken = false;
                }
                onPhotoSaved: {
                    console.debug("+++++++ Photo URI: " + fileName);
                    photoBeingTaken = false;
                    photoSavedLabel.text = "Saved: " + fileName;
                }
                onFocusStateChanged: {
                    focusRectangle.cameraFocused = state == CameraFocusState.Locked;
                }

            }
            Container {
                id: focusRectangleContainer
                property int width
                property int height

                property int leftBound: 0
                property int rightBound: width
                property int topBound: 0
                property int bottomBound: height

                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                overlapTouchPolicy: OverlapTouchPolicy.Allow

                function updateFocusBoundingBox() {
                    var captureSize = cameraSettings.captureResolution;
                    var captureRatio = captureSize.height / captureSize.width;
                    var screenRatio = height / width;
                    console.log("+++++++ Calculating bounding box...");
                    if (captureRatio < screenRatio) {

                        console.log("+++++++ Height is greater than width");
                        var cameraHeight = width * captureRatio;

                        topBound = (height - cameraHeight) / 2;
                        bottomBound = height - (height - cameraHeight) / 2;

                        leftBound = 0;
                        rightBound = width;

                    } else { // Not tested as app doesn't support landscape mode yet
                        var cameraWidth = height * captureRatio;
                        
                        leftBound = (width - cameraWidth) / 2;
                        rightBound = width - (width - cameraWidth) / 2
                        
                        topBound = 0;
                        bottomBound = height;
                    }

                    console.log("+++++++ Bounding box: (" + topBound + ", " + bottomBound + ", " + rightBound + ", " + leftBound + ")");
                }

                layout: AbsoluteLayout {
                }
                onTouch: {
                    console.debug("+++++++ Touch event: (" + event.windowX + ", " + event.windowY + ")");
                    if (event.isDown()) {

                    } else if (focusRectangle.moving && event.isMove()) {

                        focusRectangle.move(event.windowX, event.windowY)

                    } else {
                        focusRectangle.moving = false;
                    }
                }

                Container {
                    id: focusRectangle
                    property int width
                    property int height

                    property bool moving: false

                    property bool cameraFocused: false

					opacity: .1
					
                    overlapTouchPolicy: OverlapTouchPolicy.Allow

                    function move(x, y) {
                        var newX = Math.max(focusRectangleContainer.leftBound, x - absoluteLayout.touchDeltaX);
                        var newY = Math.max(focusRectangleContainer.topBound, y - absoluteLayout.touchDeltaY);

                        newX = Math.min(newX, focusRectangleContainer.rightBound - width);
                        newY = Math.min(newY, focusRectangleContainer.bottomBound - height);

                        console.log("+++++++ New position: (" + newX + ", " + newY + ")");

                        absoluteLayout.positionX = newX;
                        absoluteLayout.positionY = newY;

                    }

                    onMovingChanged: {
                    	if (!moving) {
                            console.log("+++++++ Setting focus region");
                            
                            var captureSize = cameraSettings.captureResolution;
                            var focusRegion = cameraSettings.focusRegion;
                            
                            //We can actually set these
                            focusRegion.x = (absoluteLayout.positionX - focusRectangleContainer.leftBound) * (captureSize.width / focusRectangleContainer.width);
                            focusRegion.y = (absoluteLayout.positionY - focusRectangleContainer.topBound) * (captureSize.height / focusRectangleContainer.height);
                            
                            focusRegion.width = width * captureSize.width / focusRectangleContainer.width;
                            focusRegion.height = height * captureSize.height / focusRectangleContainer.height;
                            
                            console.log("++++++ Focus region: " + focusRegion.x + ", " + focusRegion.y);
                            
                            cameraSettings.focusRegion = focusRegion;
                            camera.applySettings(cameraSettings);
                            
                    	}
                    }
                    layout: DockLayout {
                    }
                    layoutProperties: AbsoluteLayoutProperties {
                        id: absoluteLayout

                        property int touchDeltaX
                        property int touchDeltaY

                        positionX: focusRectangleContainer.width / 2 - focusRectangle.width / 2
                        positionY: focusRectangleContainer.height / 2 - focusRectangle.height / 2

                    }

                    ImageView {
                        overlapTouchPolicy: OverlapTouchPolicy.Allow
                        visible: ! focusRectangle.cameraFocused
                        imageSource: "asset:///rectangle.amd"
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        preferredHeight: Math.min(focusRectangleContainer.width, focusRectangleContainer.height) / 5
                        preferredWidth: Math.min(focusRectangleContainer.width, focusRectangleContainer.height) / 5
                    }
                    ImageView {
                        overlapTouchPolicy: OverlapTouchPolicy.Allow
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        visible: focusRectangle.cameraFocused
                        imageSource: "asset:///rectangle-focused.amd"
                        preferredHeight: Math.min(focusRectangleContainer.width, focusRectangleContainer.height) / 5
                        preferredWidth: Math.min(focusRectangleContainer.width, focusRectangleContainer.height) / 5
                    }
                    attachedObjects: [
                        LayoutUpdateHandler {
                            onLayoutFrameChanged: {
                                if (! focusRectangle.moving) {
                                    console.log("Layout focus rect: [" + layoutFrame.width + ", " + layoutFrame.height + "]");
                                    focusRectangle.width = layoutFrame.width;
                                    focusRectangle.height = layoutFrame.height;
                                    
                                }
                            }
                        }
                    ]
                    onTouch: {
                        console.debug("+++++++ Touch event: (" + event.windowX + ", " + event.windowY + ")");
                        if (event.touchType == TouchType.Down) {
                            moving = true;

                            absoluteLayout.touchDeltaX = event.windowX - absoluteLayout.positionX;
                            absoluteLayout.touchDeltaY = event.windowY - absoluteLayout.positionY;

                        }

                    }
                }
                attachedObjects: [
                    LayoutUpdateHandler {
                        onLayoutFrameChanged: {
                            console.log("Layout focus container: [" + layoutFrame.width + ", " + layoutFrame.height + "]");
                            focusRectangleContainer.width = layoutFrame.width;
                            focusRectangleContainer.height = layoutFrame.height;
                            
                            focusRectangle.opacity = .8;
                            focusRectangleContainer.updateFocusBoundingBox();
                            
                        }
                    }
                ]

            }
            Label {
                id: photoSavedLabel
                multiline: true
                visible: false
                text: "Debug mode enabled"
                textStyle.color: Color.White
                textStyle.textAlign: TextAlign.Center
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Bottom
            }
            Button {
                opacity: .001
                onClicked: {
                    photoSavedLabel.visible = ! photoSavedLabel.visible
                }
                horizontalAlignment: HorizontalAlignment.Left
                verticalAlignment: VerticalAlignment.Top
            }
            attachedObjects: [

                // Required to play the shutter sound.
                SystemSound {
                    id: shutterSound
                    sound: SystemSound.CameraShutterEvent
                },

                CameraSettings {
                    id: cameraSettings
                    objectName: "cameraSettings"
                },
                CameraRollManager {
                    id: cameraRollManager
                    onCameraRollPathUpdated: {
                        camera.getSettings(cameraSettings);
                        cameraSettings.cameraRollPath = cameraRollPath;
                        camera.applySettings(cameraSettings);
                        console.log("+++++++ Camera roll set to " + cameraRollPath);
                        cameraSettingsStore.saveSetting(CameraSettingsStore.CameraRollPath, cameraRollPath);

                    }
                },
                CameraSettingsStore {
                    id: cameraSettingsStore
                }

            ]
        }

    }
    onCreationCompleted: {
        //cameraRollManager.createCameraRollDialog();
    }
    Menu.definition: MenuDefinition {
        settingsAction: SettingsActionItem {
            onTriggered: {
                camera.getSettings(cameraSettings);
                cameraRollManager.promptCameraRollPath(cameraSettings.cameraRollPath);
            }
        }
    }
}
