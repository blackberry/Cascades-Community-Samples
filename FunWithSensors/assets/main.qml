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
import threads.sensors 1.0

/**
*  This is a Page layout that displays readings from the device's built in sensors
*
*  You will learn how to:
*  -- Import a C++ object and access it's members
*  -- Define custom properties and reuse them
**/
Page {
    attachedObjects: [
        // Sensors is bound to the SensorsThread C++ object
        Sensors {
            id: sensors
            onStarted: {
                document.sensorStarted(temperature, light, magnetometer, accelerometer, gyroscope);
            }
            onPulse: {
                document.sensorPulse(celciusTemperature, illuminance, xyzMagnet, xyzAccel, xyzGyro);
            }
        },
        // text style for all the labels in this page
        TextStyleDefinition {
            id: labelStyle
            base: SystemDefaults.TextStyles.TitleText
            color: Color.White
            alignment: TextAlignment.Center
        }, 
        // left aligned docklayout properties
        DockLayoutProperties {
            id: leftAlign
            horizontalAlignment: HorizontalAlignment.Left
        }, 
        // right aligned docklayout properties
        DockLayoutProperties {
            id: rightAlign
            horizontalAlignment: HorizontalAlignment.Right
        }
    ]
    content: Container {
        id: document
        background: Color.Black
        layout: StackLayout {
        }
        onCreationCompleted: {
            sensors.start(); // start the Sensors thread, will now emit pulses
        }
        signal sensorPulse(int celciusTemperature, int illuminance, variant xyzMagnet, variant xyzAccel, variant xyzGyro)
        signal sensorStarted(bool temperature, bool light, bool magnetometer, bool accelerometer, bool gyroscope)
        onSensorPulse: {
            // handle light sensor
            illuminanceLabel.text = Math.round(illuminance);
            var rgbIlluminance = illuminance / 36000;
            document.background = Color.create(rgbIlluminance, rgbIlluminance, rgbIlluminance);
            labelStyle.color = Color.create(1 - rgbIlluminance, 1 - rgbIlluminance, 1 - rgbIlluminance);
                                    
            // handle accelerometer sensor                
            accelerometerXLabel.text = xyzAccel[0].toFixed(3);
            accelerometerYLabel.text = xyzAccel[1].toFixed(3);
            accelerometerZLabel.text = xyzAccel[2].toFixed(3);
                                    
            // handle magnetometer sensor
            magnetometerXLabel.text = xyzMagnet[0].toFixed(3);
            magnetometerYLabel.text = xyzMagnet[1].toFixed(3);
            magnetometerZLabel.text = xyzMagnet[2].toFixed(3);
                                    
            // handle gyroscope sensor
            gyroscopeXLabel.text = xyzGyro[0].toFixed(3);
            gyroscopeYLabel.text = xyzGyro[1].toFixed(3);
            gyroscopeZLabel.text = xyzGyro[2].toFixed(3);
        }
        onSensorStarted: {
            // when the thread starts, verify support for each sensor
            if (! temperature) {
                temperatureLabel.text = "Not Supported";
            }
            if (! light) {
                illuminanceLabel.text = "Not Supported";
            }
            if (! magnetometer) {
                magnetometerXLabel.text = magnetometerYLabel.text = magnetometerZLabel.text = "Not Supported";
            }
            if (! accelerometer) {
                accelerometerXLabel.text = accelerometerYLabel.text = accelerometerZLabel.text = "Not Supported";
            }
        }
        
        // temperature sensor reading
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Temperature:"
            }
            Label {
                layoutProperties: rightAlign
                id: temperatureLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Divider {
        }
        
        // light sensor reading
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Illuminance:"
            }
            Label {
                layoutProperties: rightAlign
                id: illuminanceLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Divider {
        }
        
        // magnetometer sensor reading
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Magnetometer X:"
            }
            Label {
                layoutProperties: rightAlign
                id: magnetometerXLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Magnetometer Y:"
            }
            Label {
                layoutProperties: rightAlign
                id: magnetometerYLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Magnetometer Z:"
            }
            Label {
                layoutProperties: rightAlign
                id: magnetometerZLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Divider {
        }

        // accelerometer sensor reading
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Accelerometer X:"
            }
            Label {
                layoutProperties: rightAlign
                id: accelerometerXLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Accelerometer Y:"
            }
            Label {
                layoutProperties: rightAlign
                id: accelerometerYLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Accelerometer Z:"
            }
            Label {
                layoutProperties: rightAlign
                id: accelerometerZLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Divider {
        }
        
        // gyroscope sensor reading
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Gyroscope X:"
            }
            Label {
                layoutProperties: rightAlign
                id: gyroscopeXLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Gyroscope Y:"
            }
            Label {
                layoutProperties: rightAlign
                id: gyroscopeYLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
        Container {
            layout: DockLayout {
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
            }
            preferredWidth: 768
            Label {
                layoutProperties: leftAlign
                textStyle.base: labelStyle.style
                text: "Gyroscope Z:"
            }
            Label {
                layoutProperties: rightAlign
                id: gyroscopeZLabel
                textStyle.base: labelStyle.style
                text: "Not Started"
            }
        }
    }
}
