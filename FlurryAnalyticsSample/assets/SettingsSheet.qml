/* Copyright (c) 2013 BlackBerry.
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

/**
 * The SettingsSheet is meant as a place to see how things like User ID, age and 
 * gender could be logged. This is not really a practical scenario as there is no
 * context or use for this info in this app and none of it gets recorded locally.
 */

Sheet {
    id: settingsSheet
    objectName: "settingsSheet"
    property bool ageChanged: false
    property bool genderChanged: false

    content: Page {
        titleBar: TitleBar {
            title: "User Settings"
            
            // If the user closes this sheet we check to see if any values were changed and log them if so
            dismissAction: ActionItem {
                title: "Close"
                onTriggered: {
                    if (ageChanged) {
                        cpp.setAge(parseInt(ageSelection.value));
                        ageChanged = false;
                    }
                    if (genderChanged) {
                        cpp.setGender(genderSelection.selectedValue);
                        genderChanged = false;
                    }
                    close();
                }
            }
        }
        Container {
            layout: DockLayout {

            }
            ImageView {
                imageSource: "asset:///images/crumpledPage.png"
            }
            BackgroundContainer {
            }
            Container {
                topPadding: 20.0
                leftPadding: 20.0
                rightPadding: 20.0

                // Mock username and password fields
                TextField {
                    id: username
                    hintText: "Username"
                }
                TextField {
                    id: password
                    hintText: "Password"
                    inputMode: TextFieldInputMode.Password
                }

				// The following container provides a mock-login UX. It really just validates
				// that the username and passwords are non-empty
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Button {
                        id: loginButton
                        text: "Log in"
                        onClicked: {
                            superficialLoginValidator.start();
                        }
                    }
                    Label {
                        id: loginValidationResult
                        textStyle.fontStyle: FontStyle.Italic
                        verticalAlignment: VerticalAlignment.Center
                    }
                    ActivityIndicator {
                        id: superficialLoginValidator
                        verticalAlignment: VerticalAlignment.Fill
                        onStarted: {
                            loginValidationResult.textStyle.color = Color.LightGray
                            loginValidationResult.text = "Logging in..."
                            username.enabled = false;
                            password.enabled = false;
                            loginButton.enabled = false;
                            if (username.text.length > 0 && password.text.length > 1) {
                                loginValidationResult.textStyle.color = Color.DarkGreen
                                loginValidationResult.text = "Logged in"
                                cpp.setUserId(username.text);
                            } else {
                                loginValidationResult.textStyle.color = Color.DarkRed
                                loginValidationResult.text = "Log in failed"
                            }
                            superficialLoginValidator.stop();
                        }
                        onStopped: {
                            username.enabled = true;
                            password.enabled = true;
                            loginButton.enabled = true;
                        }
                    }
                }
                Divider {
                }
                
                // Gender selection
                Label {
                    text: "Gender"
                }
                SegmentedControl {
                    id: genderSelection
                    options: [
                        Option {
                            text: "Male"
                            value: "M"
                        },
                        Option {
                            text: "Not Specified"
                            value: ""
                            selected: true
                        },
                        Option {
                            text: "Female"
                            value: "F"
                        }
                    ]
                    onSelectedIndexChanged: {
                        genderChanged = genderSelection.selectedValue.length != "";
                    }
                }
                Divider {
                }
                
                // Age selector
                Label {
                    text: "Age: " + parseInt(ageSelection.immediateValue)
                }
                Slider {
                    id: ageSelection
                    fromValue: 0
                    toValue: 137
                    value: 0
                    onValueChanged: {
                        ageChanged = (value > 0);
                    }
                }
            }
        }
    }
    
    // Start and stop time events when the sheet is displayed/closed respectively
    onOpened: {
        cpp.logEvent(objectName + " page displayed", true);
    }
    onClosed: {
        cpp.endTimedEvent(objectName + " page displayed");
    }
}