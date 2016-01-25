/* Copyright (c) 2016 BlackBerry Ltd.
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
 * The Lithium REST APIs are owned by Lithium Technologies, Inc. 
 * (http://www.lithium.com) and are not part of the “Works” licensed 
 * hereunder pursuant to the Apache 2.0 license.
 */

import bb.cascades 1.2

Page {
    Container {
        leftPadding: 10
        rightPadding: 10
        topPadding: 10
        bottomPadding: 10
        
        layout: StackLayout {
            
        }

        //Todo: fill me with QML
        Label {
            horizontalAlignment: HorizontalAlignment.Fill

            // Localized text with the dynamic translation and locale updates support
            text: qsTr("Hello Screen Capture") + Retranslate.onLocaleOrLanguageChanged
            textStyle.base: SystemDefaults.TextStyles.BigText
            textStyle.textAlign: TextAlign.Center
        }

        DropDown {
            id: source

            enabled: true
            title: "Select Source"

            Option {
                text: "Device Screen"
                value: 1
            }
            Option {
                text: "Second Screen (HDMI / Miracast)"
                value: 2
            }

            onSelectedValueChanged: {
                _screenCapture.setSource(selectedValue);
                
                _screenCapture.openCamera();
                
                if (_screenCapture.proceed) {
	                var resolutionOptions = _screenCapture.viewfinderResolutions;
	                
	                _app.updateDropDown("viewfinderResolution", resolutionOptions);
	                
	                viewfinderResolution.enabled = true;
	            }
            }
        }
        
        Label {
            horizontalAlignment: HorizontalAlignment.Fill
            
            // Localized text with the dynamic translation and locale updates support
            text: qsTr("Viewfinder Settings") + Retranslate.onLocaleOrLanguageChanged
            textStyle.base: SystemDefaults.TextStyles.NormalText
            textStyle.textAlign: TextAlign.Center
            textStyle.fontWeight: FontWeight.Bold
        }
        
        DropDown {
            id: viewfinderResolution
            objectName: "viewfinderResolution"
            
            enabled: false
            title: "Select Viewfinder Resolution"
            
            onSelectedValueChanged: {
                _screenCapture.setViewfinderResolution(selectedValue);

                
                var framerateOptions = _screenCapture.viewfinderFrameRates;
                
                _app.updateDropDown("viewfinderFramerate", framerateOptions);
                
                framerateOptions = _screenCapture.viewfinderFrameRates;
                
                _app.updateDropDown("videoFramerate", framerateOptions);
                
                viewfinderFramerate.enabled = true;                
            }
        }
        
        DropDown {
            id: viewfinderFramerate
            objectName: "viewfinderFramerate"
      
            enabled: false
            title: "Select Viewfinder Frame Rate"

            
            onSelectedValueChanged: {
                _screenCapture.setViewfinderFrameRate(selectedValue);
                
                viewfinderRotation.enabled = true;
            }
        }
        
        DropDown {
            id: viewfinderRotation
            
            enabled: false
            title: "Select Viewfinder Rotation"
            
            Option {
                text: "0"
                value: 0
            }
            Option {
                text: "90"
                value: 90
            }
            Option {
                text: "180"
                value: 180
            }
            Option {
                text: "270"
                value: 270
            }
            
            onSelectedValueChanged: {
                _screenCapture.setViewfinderRotation(selectedValue);
                
                viewfinderFrametype.enabled = true;
            }
        }
        
        DropDown {
            id: viewfinderFrametype
            
            enabled: false
            title: "Select Viewfinder Frame Type"
            
            Option {
                text: "NV12"
                value: 1
            }
            Option {
                text: "RGB8888"
                value: 2
            }
            
            onSelectedValueChanged: {
                _screenCapture.setViewfinderFrameType(selectedValue);
                
                videoFramerate.enabled = true;
            }
        }
        
        Label {
            horizontalAlignment: HorizontalAlignment.Fill
            
            // Localized text with the dynamic translation and locale updates support
            text: qsTr("Capture Settings") + Retranslate.onLocaleOrLanguageChanged
            textStyle.base: SystemDefaults.TextStyles.NormalText
            textStyle.textAlign: TextAlign.Center
            textStyle.fontWeight: FontWeight.Bold
        }
        
        DropDown {
            id: videoFramerate
            objectName: "videoFramerate"
            
            enabled: false
            title: "Select Video Frame Rate"
            
            onSelectedValueChanged: {
                _screenCapture.setVideoFrameRate(selectedValue);
                
                _screenCapture.setConfigured(true);
            }
        }
        
        Label {
            horizontalAlignment: HorizontalAlignment.Fill
            
            // Localized text with the dynamic translation and locale updates support
            text: qsTr("") + Retranslate.onLocaleOrLanguageChanged
            textStyle.base: SystemDefaults.TextStyles.NormalText
            textStyle.textAlign: TextAlign.Center
            textStyle.fontWeight: FontWeight.Bold
        }
        
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            
            }
            
            Button {
                text: "Start Video"
                
                onClicked: {                    
                    _screenCapture.openCamera();
                    
                    if (_screenCapture.proceed) {
                        _screenCapture.startViewfinder();
                    }
                    
                    _screenCapture.startVideo();			    
                }
            }
            
            Button {
                text: "Stop Video"
                
                onClicked: {
                    _screenCapture.stopVideo();
                    
                    _screenCapture.cleanup();			    
                }
            }
        
        }
        
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            
            }
            
            Button {
                text: "Start Encode"
                
                onClicked: {
                    _screenCapture.openCamera();
                    
                    if (_screenCapture.proceed) {
                        _screenCapture.startViewfinder();
                    }
                    
                    _screenCapture.startEncode();			    
                }
            }
            
            Button {
                text: "Stop Encode"
                
                onClicked: {
                    _screenCapture.stopEncode();		    
                    
                    _screenCapture.cleanup();			    
                }
            }
        
        }
    }
}
