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
import threads.counter 1.0

Page {

    content: Container {
        background: Color.Black
        layout: DockLayout {
        }
        Container {
            layout: StackLayout {
            }
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Top
                horizontalAlignment: HorizontalAlignment.Center
            }
            Label {
                id: counter1Label
                textStyle.base: counterStyle.style
                text: "Not started"
                signal count(int num)
                onCount: {
                    text = num;
                }
            }
            Button {
                id: counter1Btn
                text: "Start"
                onClicked: {

                    if (counter1.paused) counter1.start(),counter1.pause(false); else counter1.pause(true);
                }
            }
        }
        Container {
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Right
            }
            Label {
                id: counter2Label
                textStyle.base: counterStyle.style
                text: "Not started"
                signal count(int num)
                onCount: {
                    text = num;
                }
            }
            Button {
                id: counter2Btn
                text: "Start"
                onClicked: {

                    if (counter2.paused) counter2.start(),counter2.pause(false); else counter2.pause(true);
                }
            }
        }
        Container {
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Left
            }
            Label {
                id: counter3Label
                textStyle.base: counterStyle.style
                text: "Not started"
                signal count(int num)
                onCount: {
                    text = num;
                }
            }
            Button {
                id: counter3Btn
                text: "Start"
                onClicked: {
                    if (counter3.paused) counter3.start(),counter3.pause(false); else counter3.pause(true);
                }
            }
        }
        Container {
            layoutProperties: DockLayoutProperties {
                verticalAlignment: VerticalAlignment.Bottom
                horizontalAlignment: HorizontalAlignment.Center
            }
            Label {
                id: counter4Label
                textStyle.base: counterStyle.style
                text: "Not started"
                signal count(int num)
                onCount: {
                    text = num;
                }
            }
            Button {
                id: counter4Btn
                text: "Start"
                onClicked: {

                    if (counter4.paused) counter4.start(),counter4.pause(false); else counter4.pause(true);
                }
            }
        }
        attachedObjects: [
            Counter {    
                //increments every seconds, updates ui every second
                id: counter1
                pulseInterval: 1
                updateInterval: 1000
                onPulse: {
                    counter1Label.count(counter);
                }
                onPaused: {
                    counter1Btn.text = "Start"
                }
                onCountingStarted: {
                    counter1Btn.text = "Pause"
                }
            },
            Counter {    
                //increments every 2 seconds, updates ui every second
                id: counter2
                pulseInterval: 1
                updateInterval: 2000
                onPulse: {
                    counter2Label.count(counter);
                }
                onPaused: {
                    counter2Btn.text = "Start"
                }
                onCountingStarted: {
                    counter2Btn.text = "Pause"
                }
            },
            Counter {    
                // increments every 500milliseconds, updates ui every second
                id: counter3
                pulseInterval: 5
                updateInterval: 500
                onPulse: {
                    counter3Label.count(counter);
                }
                onPaused: {
                    counter3Btn.text = "Start"
                }
                onCountingStarted: {
                    counter3Btn.text = "Pause"
                }
            },
            Counter {    
                // increments every 100 milliseconds, updates ui every 100 milliseconds
                id: counter4
                startValue: 100
                pulseInterval: 1
                updateInterval: 100
                onPulse: {
                    counter4Label.count(counter);
                }
                onPaused: {
                    counter4Btn.text = "Start";
                }
                onCountingStarted: {
                    counter4Btn.text = "Pause";
                }
            },
            TextStyleDefinition {
                id: counterStyle
                base: SystemDefaults.TextStyles.TitleText
                color: Color.White
                alignment: TextAlignment.Center
            }
        ]
    }
}
