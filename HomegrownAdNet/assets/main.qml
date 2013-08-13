/* Copyright (c) 2013 BlackBerry Limited.
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

import bb.cascades 1.00

Page {
    titleBar: TitleBar {
        title: "Garett's Horrible Calculator"
    }

    Container {
        /**
         * HGBanner (for HomegrownBanner) is where this sample will focus. You can
         * modify assets/models/admodel.xml to add your own ads to the Banner
         * carousel. Images for the ads should be added to assets/images/advertimages.
         * The size of the Banner displayed is entirely up to you, but I would 
         * recommend using the same sizes for all to maintain uniformity.
         */
        HGBanner {
            refreshRate: 60
            width: 720
            height: 125
            horizontalAlignment: HorizontalAlignment.Center
        }
        
        //Everything below this line is just sample fodder, something to display on the screen
        // to keep the sample more interesting. The real exciting piece is in the above HGBanner 
        // control.
        Container {
            topMargin: 50
            bottomMargin: 20
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Label {
                text: "Number 1: "
                verticalAlignment: VerticalAlignment.Center
            }
            TextField {
                id: number1
                inputMode: TextFieldInputMode.NumbersAndPunctuation
                hintText: ""
                textStyle.textAlign: TextAlign.Right
                onFocusedChanged: {
                    if (focused) {
                        errorLabel.text = "";
                    }
                }
            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Button {
                text: "+"
                onClicked: {
                    if (validateNumbers()) {
                        result.text = parseFloat(number1.text) + parseFloat(number2.text);
                    }
                }
            }
            Button {
                text: "-"
                onClicked: {
                    if (validateNumbers()) {
                        result.text = parseFloat(number1.text) - parseFloat(number2.text);
                    }
                }
            }
            Button {
                text: "/"
                onClicked: {
                    if (validateNumbers()) {
                        result.text = parseFloat(number1.text) / parseFloat(number2.text);
                    }
                }
            }
            Button {
                text: "X"
                onClicked: {
                    if (validateNumbers()) {
                        result.text = parseFloat(number1.text) * parseFloat(number2.text);
                    }
                }
            }
            Button {
                text: "^"
                onClicked: {
                    if (validateNumbers()) {
                        result.text = Math.pow(parseFloat(number1.text), parseFloat(number2.text));
                    }
                }
            }
        }
        Container {
            topMargin: 20
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Label {
                text: "Number 2: "
                verticalAlignment: VerticalAlignment.Center
            }
            TextField {
                id: number2
                inputMode: TextFieldInputMode.NumbersAndPunctuation
                hintText: ""
                textStyle.textAlign: TextAlign.Right
                onFocusedChanged: {
                    if (focused) {
                        errorLabel.text = "";
                    }
                }
            }
        }
        Divider {
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            horizontalAlignment: HorizontalAlignment.Right
            Label {
                text: "Result: "
            }
            Container {
                background: Color.LightGray
                topPadding: 5
                leftPadding: 5
                rightPadding: 5
                bottomPadding: 5
                Container {
                    background: Color.White
                    Label {
                        id: result
                        preferredWidth: 300
                        onTextChanged: {
                            if (! isNaN(text) && parseFloat(text) < 0) {
                                result.textStyle.color = Color.Red;
                            } else {
                                result.textStyle.color = Color.Black;
                            }
                        }
                        textStyle.textAlign: TextAlign.Right
                    }
                }
            }
        }
        Label {
            id: errorLabel
            text: ""
            textStyle.color: Color.DarkRed
            textStyle.fontWeight: FontWeight.Bold
            textStyle.textAlign: TextAlign.Right
        }
    }

    function validateNumbers() {

        if (isNaN(number1.text) || number1.text == 0) {
            errorLabel.text = "Number 1 must be a non-zero number"
            return false;
        }
        if (isNaN(number2.text) || number2.text == 0) {
            errorLabel.text = "Number 2 must be a non-zero number"
            return false;
        }
        return true;
    }

}
