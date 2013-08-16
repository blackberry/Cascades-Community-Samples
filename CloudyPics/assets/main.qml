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
                onTouch: {

                    if (event.touchType == TouchType.Down && ! photoBeingTaken) {
                        photoBeingTaken = true;
                        camera.capturePhoto();
                        console.debug("+++++++ SNAP!");
                    }
                }
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

            }
            Container {

            }
            Label {
                id: photoSavedLabel
                multiline: true;
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
