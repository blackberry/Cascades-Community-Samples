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
import bb.cascades 1.2
import bb.system.phone 1.0

Container {
    property alias phoneNumber:  phoneNumbertxt.text
    property int topMarginVal: 15
    property int leftMarginVal: 80
    property int callBtnLeftMarginVal: 105
    TextField {
        id: phoneNumbertxt
        objectName: "phoneNumbertxt"
        hintText: "Enter Number"
        backgroundVisible: false
        focusPolicy: FocusPolicy.None
        textStyle.color: Color.White
        textStyle.fontSize: FontSize.XXLarge
        textStyle.fontFamily: "Tahoma"
        textStyle.textAlign: TextAlign.Center
    }
    Container {
        topMargin: topMarginVal
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            ImageButton {
                defaultImageSource: "asset:///images/Number-1-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "1";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
            ImageButton {
                leftMargin: leftMarginVal
                defaultImageSource: "asset:///images/Number-2-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "2";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
            ImageButton {
                leftMargin: leftMarginVal
                defaultImageSource: "asset:///images/Number-3-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "3";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
        }
        Container {
            topMargin: topMarginVal
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            ImageButton {
                defaultImageSource: "asset:///images/Number-4-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "4";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
            ImageButton {
                leftMargin: leftMarginVal
                defaultImageSource: "asset:///images/Number-5-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "5";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
            ImageButton {
                leftMargin: leftMarginVal
                defaultImageSource: "asset:///images/Number-6-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "6";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
        }
        Container {
            topMargin: topMarginVal
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            ImageButton {
                defaultImageSource: "asset:///images/Number-7-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "7";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
            ImageButton {
                leftMargin: leftMarginVal
                defaultImageSource: "asset:///images/Number-8-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "8";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
            ImageButton {
                leftMargin: leftMarginVal
                defaultImageSource: "asset:///images/Number-9-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "9";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
        }
        Container {
            topMargin: topMarginVal
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageButton {
                defaultImageSource: "asset:///images/Math-multiply-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "*";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
            ImageButton {
                leftMargin: leftMarginVal
                defaultImageSource: "asset:///images/Number-0-icon.png"
                onClicked: {}
                gestureHandlers: [
                    LongPressHandler {
                        onLongPressed: {
                            phoneNumbertxt.text += "+";
                            communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                        }
                    },
                    TapHandler {
                        onTapped: {
                            phoneNumbertxt.text += "0";
                            communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                        }
                    }
                ]
            }
            ImageButton {
                leftMargin: leftMarginVal
                defaultImageSource: "asset:///images/Hash-icon.png"
                onClicked: {
                    phoneNumbertxt.text += "#";
                    communicatorApp.PlaySound("cell-phone-1-nr0.wav");
                }
            }
        }
        Container {
            topMargin: topMarginVal
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
                leftPadding: 0
                background: Color.Transparent
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                ImageButton {
                    defaultImageSource: "asset:///images/phone81.png"
                    onClicked: {
                        if(phoneNumbertxt.text.length > 0){
                            Qt.phone.initiateCall(phoneNumbertxt.text);
                        }
                    }
                }
                Label {
                    id: callLab
                    text: "Call"
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.BigText
                    textStyle.color: Color.White
                    
                    onTouch: {
                        if (event.touchType == TouchType.Down) {
                            callLab.textStyle.color = Color.Red
                            if(phoneNumbertxt.text.length > 0){
                                Qt.phone.initiateCellularCall(phoneNumbertxt.text);
                            }
                        }
                        if (event.touchType == TouchType.Up) {
                            callLab.textStyle.color = Color.White
                        }
                    }
                }
            }
            ImageButton {
                leftMargin: callBtnLeftMarginVal
                defaultImageSource: "asset:///images/Math-lower-than-icon.png"
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            var str = phoneNumbertxt.text
                            str = str.substring(0, str.length - 1)
                            phoneNumbertxt.text = str;
                        }
                    },
                    LongPressHandler {
                        onLongPressed: {
                            phoneNumbertxt.text = "";
                        }
                    }
                ]
            }
        }
    }
}
