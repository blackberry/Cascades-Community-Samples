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
*/import bb.cascades 1.0

Container {
    property alias text: frequencyValue.text
    property alias value: warpDriveSpeedScanner.value
    clipContentToBounds: false
    Container {
        preferredWidth: maxWidth
        clipContentToBounds: false
        layout: DockLayout {
        }
        ImageView {
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Center
            }
            imageSource: "asset:///images/compass_base.png"
        }
        ImageView {
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Center
            }
            imageSource: "asset:///images/compass_pointer.png"
            animations: [
                FadeTransition {
                    id: fadeout
                    fromOpacity: 1
                    toOpacity: .5
                    duration: (_locationDiagnostics.getValueFor(warpDriveSpeedScanner.objectName, "0.0")) + 200
                    onEnded: {
                        fadein.play();
                    }
                },
                FadeTransition {
                    id: fadein
                    duration: fadeout.duration
                    fromOpacity: .5
                    toOpacity: 1
                    onEnded: {
                        rotate.play();
                    }
                },
                RotateTransition {
                    id: rotate
                    fromAngleZ: 0
                    toAngleZ: 360
                    easingCurve: StockCurve.CubicOut
                    duration: fadeout.duration
                    onEnded: {
                        fadeout.play();
                    }
                }
            ]
        }
        onCreationCompleted: {
            fadeout.play();
            
            // The animation speed at start up should be taken from the stored value of the slider.
            fadeout.duration = (_locationDiagnostics.getValueFor(warpDriveSpeedScanner.objectName, "0.0")) + 200;
        }
    }
    Container {
        clipContentToBounds: false
        layout: StackLayout {
        }
        layoutProperties: StackLayoutProperties {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Bottom
        }
        Label {
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
            id: frequencyValue
            textStyle {
                base: SystemDefaults.TextStyles.BodyText
                fontWeight: FontWeight.Bold
            }
        }
        Container {
            layout: StackLayout {
            }
            clipContentToBounds: false
            Slider {
                preferredWidth: maxWidth
                id: warpDriveSpeedScanner
                objectName: "warpDriveSpeedScanner"
                fromValue: 1
                toValue: 500
                value: _locationDiagnostics.getValueFor(warpDriveSpeedScanner.objectName, "0.0")
                onValueChanging: {
                    frequencyValue.text = "Frequency: " + Math.floor(value) + "s"
                    fadeout.duration = (value) + 200;
                }
                onValueChanged: {
                    frequencyValue.text = "Frequency: " + Math.floor(value) + "s"
                    _locationDiagnostics.saveValueFor(warpDriveSpeedScanner.objectName, value)
                }
            }
        }
    }
}
