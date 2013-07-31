// Default empty project template
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
            //background: Color.create("#888800")
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            onCreationCompleted: {
                console.debug("All cameras acessible: " + camera.allCamerasAccessible);
                if (camera.allCamerasAccessible) {
                    console.debug("Opening camera");
                    camera.open(CameraUnit.Rear);
                }
            }
            //TODO: Wrap this in a non-transparent foreign window to fix cards
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
                        console.debug("SNAP!");
                    }
                }
                onCameraOpened: {
                    
                    //Camera roll stuff
                    console.debug("+++++++ Camera opened");
/*                    camera.getSettings(cameraSettings);
                    console.debug("+++++++ Settings got");
                    console.debug("+++++++ Camera roll path: " + cameraSettings.cameraRollPath);
                    cameraSettings.cameraRollPath ="/accounts/1000/removable/sdcard/camera/";


                    camera.applySettings(cameraSettings);
                    console.debug("+++++++ Settings applied");*/
/*                    camera.getSettings(cameraSettings);
                    cameraRollLabel.text = cameraSettings.cameraRollPath*/
                    console.debug("+++++++ Camera roll path: " + cameraSettings.cameraRollPath);
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
/*            	horizontalAlignment: HorizontalAlignment.Center
            	verticalAlignment: VerticalAlignment.Bottom
                TextField {
                    id: cameraRollText
                    text: cameraSettingsManager.getAppDirectory() + "/shared/Dropbox/Photos/"
                }
                Label {
                    id: cameraRollLabel

                }*/
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
                },
                CameraRollManager {
                    id: cameraRollManager
                    onCameraRollPathUpdated: {
                        camera.getSettings(cameraSettings);
                        cameraSettings.cameraRollPath = cameraRollPath;
                        camera.applySettings(cameraSettings);
                    }
                }

            ]
        }


    }
    onCreationCompleted: {
        cameraRollManager.createCameraRollDialog();
    }
    Menu.definition: MenuDefinition {
        settingsAction: SettingsActionItem {
            onTriggered: {
                cameraRollManager.promptCameraRollPath();
              }
        }
    }
}
