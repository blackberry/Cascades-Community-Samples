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
*
* Note: The video file packaged is in this application titled 
* "BB10DevAlpha.wmv" in the "assets" folder is NOT under the Apache 2.0 license.
* Only Research In Motion Limited has the distribution rights for this video file.
*
*/

import bb.cascades 1.0
import bb.multimedia 1.0

Page {
    content: Container {
        id: maxContainer
        layout: AbsoluteLayout {
        }
        
        //NOTE: Using absolute values here
        preferredWidth: 768
        preferredHeight: 1280
        property int xDown
        property int yDown
        property int startingX
        property int startingY
        property string localVideoPath: "/app/native/assets/BB10DevAlpha.wmv"
        
        property double minScreenScale: 0.2        //THIS IS THE MINIMUM SCALE FACTOR THAT WILL BE APPLIED TO THE SCREEN SIZE
        property double maxScreenScale: 2.0        //THIS IS THE MAXIMUM SCALE FACTOR THAT WILL BE APPLIED TO THE SCREEN SIZE
        property double initialScreenScale: 1.0    // Starts the video with original dimensions (that is, scale factor 1.0)
                                                    //NOTE: this is not to be confused with the "initialScale" property of the ForeignWindow below
                                                    //        They both start with the same value but the "initialScale" value is different for every new pinch 
        
        overlapTouchPolicy: OverlapTouch.Allow

        onCreationCompleted: {
            // set orientation to SupportedDisplayOrientation.All when the
            // document is loaded
            OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.All;
        }
        
        //background
        ImageView {
            imageSource: "asset:///images/background.jpg"
            preferredWidth: maxContainer.preferredWidth
            preferredHeight: maxContainer.preferredHeight
        }
        ForeignWindow {
            id: videoSurface
            windowId: "VideoSurface"
            visible: attached // becomes visible once attached
            //NOTE: Disabling the automatic updating of the window frame.
            //      This is a workaround for a current bug in ForeignWindow.
            //      Ideally, this wouldn't be required and the the source and position 
            //      wouldn't need to be updated manually on the signal ControlFrameChanged (below)
            windowFrameUpdateEnabled : false
            // This custom property stores the initial scale of the ForeignWindow
            // when a pinch gesture begins
            property double initialScale: maxContainer.initialScreenScale
             
            // This custom property determines how quickly the ForeignWindow grows
            // or shrinks in response to the pinch gesture
            property double scaleFactor: 0.1
            
            // Temporary variable, used in computation for pinching everytime
            property double newScaleVal
            
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Fill
            }
            
            onControlFrameChanged: {
                //NOTE: This method here is a workaround for a current bug in the ForeignWindow. Ideally, this wouldn't be required
                //      Manually set the position (x,y) and the size (width,height) of the windowHandle 
                app.setPositionAndSize(windowHandle, x, y, width, height);
            }
            
        } // ForeignWindow
        
        // To pan the video
        onTouch: {
            if (event.isDown()) {
                xDown = event.localX;
                yDown = event.localY;
                startingX = videoSurface.translationX;
                startingY = videoSurface.translationY;
            } else if (event.isMove()) {
                videoSurface.translationX = event.localX - xDown + startingX;
                videoSurface.translationY = event.localY - yDown + startingY;
                
            } else if (event.isUp() ) {
                // Since the playPauseImage is on top of this layer and since overlap touch has been set to be true, 
                // pressing down on the playPauseImage and letting it go will result into here
                // For this edge case, reset the scale values of this image (which is being used a button, below)
                playPauseImage.scaleX = 1.00;
                playPauseImage.scaleY = 1.00;
                playPauseImage.opacity = 1.00;
            }
        } // onTouch

        gestureHandlers: [
            // Add a handler for pinch gestures
            PinchHandler {
                // When the pinch gesture starts, save the initial scale
                // of the window
                onPinchStart: {
                    console.log("onPinchStart: videoSurface.scaleX = " + videoSurface.scaleX);
                    videoSurface.initialScale = videoSurface.scaleX;
                }
                         
                // As the pinch expands or contracts, change the scale of
                // the image
                onPinchUpdate: {
                    console.log("onPinchUpdate");
                    
                    videoSurface.newScaleVal = videoSurface.initialScale + ((event.pinchRatio - 1) * videoSurface.scaleFactor);
                    console.log ("onPinchUpdate: videoSurface.initialScale = " + videoSurface.initialScale + ": event.pinchRatio-1= " + event.pinchRatio-1 + " : newScaleVal = " + videoSurface.newScaleVal);
                    
                    if ( videoSurface.newScaleVal < maxContainer.maxScreenScale &&  videoSurface.newScaleVal > maxContainer.minScreenScale ) {
                        videoSurface.scaleX = videoSurface.newScaleVal;
                        videoSurface.scaleY = videoSurface.newScaleVal;
                        // align this with the zoomSlider as well
                        zoomSlider.value = videoSurface.newScaleVal;
                    }
                }// onPinchUpdate
            }// PinchHandler
        ]// attachedObjects
        
        attachedObjects: [
            MediaPlayer {
                id: player
                source : {
                    app.getAbsolutePath(maxContainer.localVideoPath);
                }
                videoOutput: MediaPlayer.VideoOutputPrimary // internal display
                windowId: videoSurface.windowId // name of the window to create
                onPlaying: {
                    console.log("player onPlaying");
                    playPauseImage.playing = true;
                    playPauseImage.imageSource = "asset:///images/pause.png";
                    
                    if ( player.windowId == "" ) {
                        console.log ("...setting the player's windowID = " + videoSurface.windowId);
                        player.windowId = videoSurface.windowId; // name of the window created
                    } //
                    
                } // onPlaying
                onPaused: {
                    console.log("player onPaused");
                    if (! progressSlider.seekInProgress) {
                        playPauseImage.playing = false;
                        playPauseImage.imageSource = "asset:///images/play.png";
                    }
                }
                onStopped: {
                    console.log("player onStopped");
                    playPauseImage.playing = false;
                    playPauseImage.imageSource = "asset:///images/play.png";
                }
                onPositionChanged: {
                    if (! progressSlider.seekInProgress) {
                        progressSlider.value = position;
                        currentTime.text = position;
                    }
                }
                onDurationChanged: {
                    console.log("Setting the duration = " + duration);
                    totalTime.text = duration;
                }
                
                onMetaDataChanged: {
                    console.log("player onMetaDataChanged");
                    console.log("--------------------------------bit_rate=" + metaData.bit_rate);
                    console.log("-----------------------------------genre=" + metaData.genre);
                    console.log("-----------------------------sample_rate=" + metaData.sample_rate);
                    console.log("-----------------------------------title=" + metaData.title);
                }//
                
                onVideoDimensionsChanged: {
                    console.log ("onVideoDimensionsChanged: width = " + dimensions.width + " : height = " + dimensions.height + " : videoSurface.windowHandle = " + videoSurface.windowHandle);
                    
                    videoSurface.preferredWidth = dimensions.width;
                    videoSurface.minWidth = dimensions.width;
                    videoSurface.maxWidth = dimensions.width;
                    
                    videoSurface.preferredHeight = dimensions.height;
                    videoSurface.minHeight = dimensions.height;
                    videoSurface.maxHeight = dimensions.height;
                    
                    videoSurface.translationY = maxContainer.preferredHeight / 2 - (videoSurface.preferredHeight / 2);
                    videoSurface.translationX = maxContainer.preferredWidth / 2 - (videoSurface.preferredWidth / 2);
                }
            } // MediaPlayer ends here
            , // continuing the attachedObjects
            OrientationHandler {
                id: handler
                onUiOrientationChanging: {
                    // set some UI to be shown during orientation change
                    if (uiOrientation == UiOrientation.Portrait) {
                        // make ui changes related to portrait
                        console.log("onUiOrientationChanging = Portrit");
                        maxContainer.preferredWidth = 768;
                        maxContainer.preferredHeight = 1280;
                        // centre the videosurface
                        videoSurface.translationY = maxContainer.preferredHeight / 2 - (videoSurface.preferredHeight / 2);
                        videoSurface.translationX = maxContainer.preferredWidth / 2 - (videoSurface.preferredWidth / 2);

                    } else if (uiOrientation == UiOrientation.Landscape) {
                        // make ui changes related to landscape
                        console.log("onUiOrientationChanging = Landscape");
                        maxContainer.preferredWidth = 1280;
                        maxContainer.preferredHeight = 768;
                        // centre the videosurface
                        videoSurface.translationY = maxContainer.preferredHeight / 2 - (videoSurface.preferredHeight / 2);
                        videoSurface.translationX = maxContainer.preferredWidth / 2 - (videoSurface.preferredWidth / 2);                        
                    } // else if
                } //onUiOrientationChanging
            }//OrientationHandler
        ]//attachedObjects ends
        
        Container {
            layout: DockLayout {
            }
            // Set the max height and width so that rest it can be stretched to put components at the bottom
            preferredWidth: maxContainer.preferredWidth
            preferredHeight: maxContainer.preferredHeight
            //background: Color.Yellow
                
            // Used for controlling zoom level of the window
            Slider {
                id: zoomSlider
                property double requestedValue
                layoutProperties: DockLayoutProperties {
                    verticalAlignment: VerticalAlignment.Top
                    horizontalAlignment: HorizontalAlignment.Center
                }
                fromValue: maxContainer.minScreenScale
                toValue: maxContainer.maxScreenScale
                //NOTE: Using absolute value here
                value: maxContainer.initialScreenScale     // starting with the default size of the video
                onValueChanging: {
                    requestedValue = value;
                    
                    videoSurface.preferredWidth = player.videoDimensions.width * value;
                    videoSurface.minWidth = player.videoDimensions.width  * value;
                    videoSurface.maxWidth = player.videoDimensions.width  * value;
                    videoSurface.preferredHeight = player.videoDimensions.height  * value;
                    videoSurface.minHeight = player.videoDimensions.height * value;
                    videoSurface.maxHeight = player.videoDimensions.height * value;
                } // onValueChanging
                    
            } // ZoomSlider
              
            Container {
                layout: StackLayout {
                }
                layoutProperties: DockLayoutProperties {
                    verticalAlignment: VerticalAlignment.Bottom
                    horizontalAlignment: HorizontalAlignment.Fill
                }
                
                //background: Color.Red
                ImageView {
                    id: playPauseImage
                    preferredWidth: 120
                    preferredHeight: 150
                    imageSource: "asset:///images/play.png"
                    property bool playing: false
                    layoutProperties: StackLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Top
                    }
                    onTouch: {
                        
                        if (event.isDown() ) {
                            // changing the size of the picture a bit to do implicit animation of the button represented by this image
                            scaleX = 1.10;
                            scaleY = 1.20;
                            opacity = 0.6;
                            
                        } else if (event.isUp()) {
                            // do the actual operations of the button on release of the touch
                            
                            // first, set the scaling and opacity back to the original
                            scaleX = 1.00;
                            scaleY = 1.00;
                            opacity = 1.00;
                            
                            if (playing) {
                                var result = player.pause();
                                if (result != MediaError.MediaErrorNone) {
                                    // Error handling
                                    console.log("ERROR: ImageView : onTouch : player.pause()");
                                }
                            } else {
                                var result = player.play();
                                if (result != MediaError.MediaErrorNon) {
                                    // Error handling
                                    console.log("ERROR: ImageView : onTouch : player.play()");
                                }
                            } // else
                        }
                    } // onTouch
                } // ImageView
                
                Container {
                    // Container for the Play/Pause button and the progress slider+labels
                    id: bottomContainer
                    layout: StackLayout {
                        layoutDirection: LayoutDirection.LeftToRight
                        leftPadding: 10
                        rightPadding: 10
                    }
                    layoutProperties: StackLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                    }
                    //background: Color.Green
                    Label {
                        id: currentTime
                        // the "text" will be set when media plays
                        textStyle {
                            color: Color.White
                            fontWeight: FontWeight.Normal
                        }
                        layoutProperties: StackLayoutProperties {
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Top
                        }
                    } // currentTimeLabel
                    Slider {
                        id: progressSlider
                        property bool seekInProgress: false
                        property int requestedValue
                        property bool wasPlaying: false
                        //NOTE: Using absolute value here for the preferredWidth
                        preferredWidth : maxContainer.preferredWidth - 2*85
                        layoutProperties: StackLayoutProperties {
                            verticalAlignment: VerticalAlignment.Bottom
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        fromValue: 0
                        toValue: player.duration
                        onTouch: {
                            if (event.isDown()) {
                                
                                seekInProgress = true;
                                if (playPauseImage.playing) {
                                    wasPlaying = true;
                                    // pause the playback so that the it doesn't keep trying to play it while being dragged/seeked (which causes audio disrupt)
                                    player.pause();
                                }
                            } else if (event.isUp() || event.isCancel()) {
                                
                                seekInProgress = false;
                                if (wasPlaying) {
                                    player.play();
                                }
                                wasPlaying = false;
                            } // else if
                        }
                        onValueChanging: {
                            requestedValue = value
                            if (seekInProgress) {
                                player.seekTime(value);
                            }
                        }
                    } // progressSlider   
                    Label {
                        id: totalTime
                        // Note: the "text" attribute will be set when the media plays
                        textStyle {
                            color: Color.White
                            fontWeight: FontWeight.Normal
                        }
                        layoutProperties: StackLayoutProperties {
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Top
                        }
                    } // totalTime Label                      
                } // Container - DockLayout
            } // Container - StackLayout (contains Play/Pause button, time labels and progress slider        
        } // Container - DockLayout
    } //Container - AbsoluteLayout - maxContainer
}
