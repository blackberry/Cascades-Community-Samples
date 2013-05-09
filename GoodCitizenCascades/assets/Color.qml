/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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

// Color sliders for changing object color
Page {
    id: page
    
    property alias show: panel.visible
		 
    content: Container {
        id: panel
        visible: false
        
        background: Color.create("#262626")
        horizontalAlignment: HorizontalAlignment.Fill
        layout: StackLayout {
        }
        
        ForeignWindowControl {
            id: viewWindow
            visible: true // becomes visible once attached
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
        }
        
        Container {
            leftPadding: 20
            rightPadding: leftPadding
            topPadding: 10
            bottomPadding: topPadding
            horizontalAlignment: HorizontalAlignment.Center
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            

            // A label that presents the current Slider values (with one digit precision).
            Label {
                text: "R"
                preferredWidth: 150
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontWeight: FontWeight.Bold
                    color: Color.White
                }
            }

            // A simple Slider changing values from 0 to 1.
            Slider {
                id: redSlider
                fromValue: 0
                toValue: 1
                value: 1
	            horizontalAlignment: HorizontalAlignment.Fill
                onValueChanged: {
                    // Updating the Label with value from 0 to 255
                    redtext.text = Math.round(value * 255.0);
                    updateObjectColor();
                }
            }
            Label {
                id: redtext
                text: "0"
                preferredWidth: 200
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontWeight: FontWeight.Normal
                    color: Color.White
                    textAlign: TextAlign.Right
                }
            }
        }
        
        Container {
            leftPadding: 20
            rightPadding: leftPadding
            topPadding: 10
            bottomPadding: topPadding
            horizontalAlignment: HorizontalAlignment.Center
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            // A label that presents the current Slider values (with one digit precision).
            Label {
                text: "G"
                preferredWidth: 150
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontWeight: FontWeight.Bold
                    color: Color.White
                }
            }

            // A simple Slider changing values from 0 to 1.
            Slider {
                id: greenSlider
                fromValue: 0
                toValue: 1
                value: 0
	            horizontalAlignment: HorizontalAlignment.Fill
                onValueChanged: {                    
                    // Updating the Label with value from 0 to 255
                    greentext.text = Math.round(value * 255.0);
                    updateObjectColor();
                }
            }
            Label {
                id: greentext
                text: "0"
                preferredWidth: 200
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontWeight: FontWeight.Normal
                    color: Color.White
                    textAlign: TextAlign.Right
                }
            }
        }
        Container {
            leftPadding: 20
            rightPadding: leftPadding
            topPadding: 10
            bottomPadding: topPadding
            horizontalAlignment: HorizontalAlignment.Center
                
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            // A label that presents the current Slider values (with one digit precision).
            Label {
                text: "B"
                preferredWidth: 150
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontWeight: FontWeight.Bold
                    color: Color.White
                }
            }

            // A simple Slider changing values from 0 to 1.
            Slider {
                id: blueSlider
                fromValue: 0
                toValue: 1
                value: 0
	            horizontalAlignment: HorizontalAlignment.Fill
                onValueChanged: {
                    // Updating the Label with value from 0 to 255
                    bluetext.text = Math.round(value * 255.0);
                    updateObjectColor();
                }
            }
            Label {
                id: bluetext
                text: "0"
                preferredWidth: 200
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontWeight: FontWeight.Normal
                    color: Color.White
                    textAlign: TextAlign.Right
                }
            }
        }
        Container {
            leftPadding: 20
            rightPadding: leftPadding
            topPadding: 10
            bottomPadding: topPadding
            horizontalAlignment: HorizontalAlignment.Center
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            // A label that presents the current Slider values (with one digit precision).
            Label {
                text: "A"
                preferredWidth: 150
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontWeight: FontWeight.Bold
                    color: Color.White
                }
           }

            // A simple Slider changing values from 0 to 1.
            Slider {
                id: alphaSlider
                fromValue: 0
                toValue: 1
                value: 1
	            horizontalAlignment: HorizontalAlignment.Fill
                onValueChanged: {                    
                    // Updating the Label with value from 0 to 255
                    alphatext.text = Math.round(value * 255.0);
                    updateObjectColor();
                }
            }
            Label {
                id: alphatext
                text: "255"
                preferredWidth: 200
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    fontWeight: FontWeight.Normal
                    color: Color.White
                    textAlign: TextAlign.Right
                }
            }
        }
        
        onCreationCompleted: {
            var loadColor = _goodCitizen.objectColor;
            console.log(loadColor + ":" + loadColor[0] + ":" + loadColor[1] + ":" + loadColor[2] + ":" + loadColor[3]);
            
            redSlider.value = loadColor[0];
            greenSlider.value = loadColor[1];
            blueSlider.value = loadColor[2];
            alphaSlider.value = loadColor[3];
            
            redtext.text = Math.round(loadColor[0] * 255.0);
            greentext.text = Math.round(loadColor[1] * 255.0);
            bluetext.text = Math.round(loadColor[2] * 255.0);
            alphatext.text = Math.round(loadColor[3] * 255.0);
            
            page.show = true;
        }
    }

    function updateObjectColor() {
        if (page.show == true) {
	        var newObjectColor = new Array();
	        newObjectColor[0] = redSlider.value;
	        newObjectColor[1] = greenSlider.value;
	        newObjectColor[2] = blueSlider.value;
	        newObjectColor[3] = alphaSlider.value;
	        _goodCitizen.setObjectColor(newObjectColor);
	    }
    }
    
    actionBarVisibility: ChromeVisibility.Visible    
}

