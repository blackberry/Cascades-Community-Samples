/* Copyright (c) 2013 BlackBerry Limited
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
import my.library 1.0

Page {
    Container {
        background: Color.Black
        
        layout: DockLayout {
        }
        
        // the viewfinder window
        ForeignWindowControl {
         	id: vfForeignWindow
         	// when a window with this id joins the window group, it will be attached to this control
         	windowId: "vfWindowId"
         	// give the foreign window an objectName so we can look it up from C++
         	objectName: "vfForeignWindow"
         	// the cascades FWC will update the screen window size, position, and visibility  
    		updatedProperties: WindowProperty.Size | WindowProperty.Position | WindowProperty.Visible
    		// only becomes visible when the camera viewfinder window is attached
    		visible: boundToWindow
    		// center the window in the parent container.  we will be adjusting the size to maintain aspect
    		// ratio, so let's keep it symmetrical.
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
			onWindowAttached: {
			    Cam.windowAttached();
            }
			onWindowDetached: {
       	    }
        }
    }
        
    actions: [
        ActionItem {
            id: rearCamera
            objectName: "rearCameraButton"
            title: "Rear Camera"
            imageSource: "asset:///icons/ic_switch_camera.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
            enabled: Cam.hasRearCamera && (Cam.cameraUnit != BestCam.UnitRear) && !Cam.capturing
            onTriggered: {
             	Cam.setCameraUnit(BestCam.UnitRear);
            }
        },
        ActionItem {
            id: frontCamera
            objectName: "frontCameraButton"
            title: "Front Camera"
            imageSource: "asset:///icons/ic_switch_camera.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
            enabled: Cam.hasFrontCamera && (Cam.cameraUnit != BestCam.UnitFront) && !Cam.capturing
            onTriggered: {
                Cam.setCameraUnit(BestCam.UnitFront);   
            }
        },
        ActionItem {
	        id: photoMode
	        objectName: "photoModeButton"
	        title: "Photo"
            imageSource: "asset:///icons/ic_camera_mode.png"
	        ActionBar.placement: ActionBarPlacement.OnBar
            enabled: Cam.canDoPhoto && (Cam.vfMode != BestCam.ModePhoto) && !Cam.capturing 
	        onTriggered: {
	            Cam.setVfMode(BestCam.ModePhoto);
         	}    
        },
        ActionItem {
            id: videoMode
            objectName: "videoModeButton"
            title: "Video"
            imageSource: "asset:///icons/ic_video_mode.png"            
            ActionBar.placement: ActionBarPlacement.OnBar
            enabled: Cam.canDoVideo && (Cam.vfMode != BestCam.ModeVideo) && !Cam.capturing
            onTriggered: {
                Cam.setVfMode(BestCam.ModeVideo);
            }    
        },
        ActionItem {
         	id: capture
         	objectName: "captureButton"
         	// TODO: change text?
         	title: "Capture"
            imageSource: (Cam.vfMode==BestCam.ModeVideo)?
            			 (Cam.capturing?"asset:///icons/ic_stop.png":"asset:///icons/ic_rec_red.png"):
            			 "asset:///icons/ic_camera_mode.png"
         	ActionBar.placement: ActionBarPlacement.OnBar
         	enabled: Cam.canCapture
            onTriggered: {
                Cam.capture();
            }
        }
    ]
}
