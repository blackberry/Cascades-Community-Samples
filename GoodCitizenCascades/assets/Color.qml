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
    content: Container {
        background: Color.create("#262626")
        preferredWidth: 768
        layout: StackLayout {
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.LeftToRight
                leftPadding: 20
                rightPadding: leftPadding
                topPadding: 10
                bottomPadding: topPadding
            }
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
            }

            // A label that presents the current Slider values (with one digit precision).
            Label {
                text: "R"
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    fontWeight: FontWeight.Bold
                    color: Color.White
                }
            }

            // A simple Slider changing values from 0 to 1.
            Slider {
                id: redSlider
                fromValue: 0
                toValue: 1
                value: 0
                preferredWidth: 450
                onValueChanging: {
                    // Updating the Label with value from 0 to 255
                    redtext.text = (value * 255.0).toFixed(0);
                    updateObjectColor();
                }
            }
            Label {
                id: redtext
                text: "0"
                preferredWidth: 120
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    fontWeight: FontWeight.Normal
                    alignment: TextAlignment.ForceRight
                    color: Color.White
                }
            }
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.LeftToRight
                leftPadding: 20
                rightPadding: leftPadding
                topPadding: 10
                bottomPadding: topPadding
            }
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
            }

            // A label that presents the current Slider values (with one digit precision).
            Label {
                text: "G"
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
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
                preferredWidth: 450
                onValueChanging: {                    
                    // Updating the Label with value from 0 to 255
                    greentext.text = (value * 255.0).toFixed(0);
                    updateObjectColor();
                }
            }
            Label {
                id: greentext
                text: "0"
                preferredWidth: 120
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    fontWeight: FontWeight.Normal
                    alignment: TextAlignment.ForceRight
                    color: Color.White
                }
            }
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.LeftToRight
                leftPadding: 20
                rightPadding: leftPadding
                topPadding: 10
                bottomPadding: topPadding
            }
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
            }

            // A label that presents the current Slider values (with one digit precision).
            Label {
                text: "B"
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
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
                preferredWidth: 450
                onValueChanging: {
                    // Updating the Label with value from 0 to 255
                    bluetext.text = (value * 255.0).toFixed(0);
                    updateObjectColor();
                }
            }
            Label {
                id: bluetext
                text: "0"
                preferredWidth: 120
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    fontWeight: FontWeight.Normal
                    alignment: TextAlignment.ForceRight
                    color: Color.White
                }
            }
        }
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.LeftToRight
                leftPadding: 20
                rightPadding: leftPadding
                topPadding: 10
                bottomPadding: topPadding
            }
            layoutProperties: StackLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
            }

            // A label that presents the current Slider values (with one digit precision).
            Label {
                text: "A"
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
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
                preferredWidth: 450
                onValueChanging: {                    
                    // Updating the Label with value from 0 to 255
                    alphatext.text = (value * 255.0).toFixed(0);
                    updateObjectColor();
                }
            }
            Label {
                id: alphatext
                text: "255"
                preferredWidth: 120
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    fontWeight: FontWeight.Normal
                    alignment: TextAlignment.ForceRight
                    color: Color.White
                }
            }
        }
        ForeignWindow {
            id: viewWindow
            preferredWidth: 768
            preferredHeight: 650
            visible: true // becomes visible once attached
            layoutProperties: DockLayoutProperties {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Top
            }
        }
        onCreationCompleted: {
            var loadColor = _goodCitizen.objectColor;
            console.log(loadColor + ":" + loadColor[0] + ":" + loadColor[1] + ":" + loadColor[2] + ":" + loadColor[3]);
            redSlider.value = loadColor[0];
            greenSlider.value = loadColor[1];
            blueSlider.value = loadColor[2];
            alphaSlider.value = loadColor[3];
            redtext.text = (loadColor[0] * 255.0).toFixed(0);
            greentext.text = (loadColor[1] * 255.0).toFixed(0);
            bluetext.text = (loadColor[2] * 255.0).toFixed(0);
            alphatext.text = (loadColor[3] * 255.0).toFixed(0);
        }
    }
    /*
    actions: [
        ActionItem {
            title: "Back"
            imageAsset: "images/actions/back.png"
            //imageSource: "asset:///images/actions/back.png"
            
            onTriggered: {
                // _navPane is set in code to make it available for all recipe pages.
                _navPane.pop ();
            }
        }
    ]
*/
    function updateObjectColor() {
        var newObjectColor = new Array();
        newObjectColor[0] = redSlider.value;
        newObjectColor[1] = greenSlider.value;
        newObjectColor[2] = blueSlider.value;
        newObjectColor[3] = alphaSlider.value;
        _goodCitizen.setObjectColor(newObjectColor);
    }
}
