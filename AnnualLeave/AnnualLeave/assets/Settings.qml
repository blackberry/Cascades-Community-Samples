/*
 * Copyright (c) 2014 BlackBerry Limited.
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

Sheet {
    id: settings
    Page {
        actionBarVisibility: ChromeVisibility.Visible

        titleBar: TitleBar {
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                        CustomButton {
                            id: title_btn1
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            text: "Back"
                            onClicked: {
                                close();
                            }
                            leftPadding: 10
                            topPadding: 15
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Center
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 60
                        }
                        Label {
                            text: qsTr("Settings")
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontWeight: style.heading_weight
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                        CustomButton {
                            id: title_btn2
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            text: "Save"
                            onClicked: {
                                if (tf3_validator.state == ValidationState.Valid && tf4_validator.state == ValidationState.Valid && tf5_validator.state == ValidationState.Valid) {
                                    app.setting1value = dd_email_accounts.selectedValue
                                    app.setting3value = tfSetting3_value.text
                                    app.setting4value = tfSetting4_value.text
                                    app.setting5value = tfSetting5_value.text
                                    app.requestAdapterStatus();
                                    lbl_warning.text = ""
                                    close();
                                } else {
                                    lbl_warning.text = "Some of the values supplied are not valid!"
                                    lbl_warning.visible = true
                                }
                            }
                            rightPadding: 10
                            topPadding: 15
                        }
                    }
                }
            }
        }
        Container {
            layout: StackLayout {
            }
            leftPadding: 20
            rightPadding: 20
            verticalAlignment: VerticalAlignment.Fill

            Style {
                id: style
            }

            ScrollView {

                Container {

                    Label {
                        id: lbl_warning
                        horizontalAlignment: HorizontalAlignment.Center
                        visible: ! app.adapter_configured
                        text: "Adapter is not properly configured!"
                        textStyle.fontWeight: style.settings_label_style
                        textStyle {
                            color: style.error_text_color
                        }
                    }

                    DropDown {
                        id: dd_email_accounts
                        objectName: "dd_email_accounts"
                        title: "Email Account"
                    }

                    // Approver Name
                    Label {
                        id: lblSetting3_name
                        horizontalAlignment: HorizontalAlignment.Center
                        textStyle.fontWeight: style.settings_label_style
                        text: app.setting3name
                        visible: (app.setting3name != "")
                    }
                    TextField {
                        id: tfSetting3_value
                        text: app.setting3value
                        visible: (app.setting3name != "")
                        input.flags: TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoPeriodOff | TextInputFlag.PredictionOff | TextInputFlag.SpellCheckOff
                        textStyle {
                            textAlign: TextAlign.Center
                        }
                        validator: Validator {
                            id: tf3_validator
                            mode: ValidationMode.Immediate
                            errorMessage: "Approver name must be supplied"
                            onValidate: {
                                if (tfSetting3_value.text.length > 0) {
                                    tf3_validator.state = ValidationState.Valid
                                } else {
                                    tf3_validator.state = ValidationState.Invalid
                                }
                            }
                        }

                    }

                    // Approver email address
                    Label {
                        id: lblSetting4_name
                        horizontalAlignment: HorizontalAlignment.Center
                        textStyle.fontWeight: style.settings_label_style
                        text: app.setting4name
                        visible: (app.setting4name != "")
                    }
                    TextField {
                        id: tfSetting4_value
                        text: app.setting4value
                        visible: (app.setting4name != "")
                        input.flags: TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoPeriodOff | TextInputFlag.PredictionOff | TextInputFlag.SpellCheckOff
                        textStyle {
                            textAlign: TextAlign.Center
                        }
                        inputMode: TextFieldInputMode.EmailAddress
                        validator: Validator {
                            id: tf4_validator
                            mode: ValidationMode.Immediate
                            errorMessage: "Invalid email address"
                            onValidate: {
                                if (app.isValidEmailAddress(tfSetting4_value.text)) {
                                    tf4_validator.state = ValidationState.Valid
                                } else {
                                    tf4_validator.state = ValidationState.Invalid
                                }
                            }
                        }
                    }

                    // User ID
                    Label {
                        id: lblSetting5_name
                        horizontalAlignment: HorizontalAlignment.Center
                        textStyle.fontWeight: style.settings_label_style
                        text: app.setting5name
                        visible: (app.setting5name != "")
                    }
                    TextField {
                        id: tfSetting5_value
                        text: app.setting5value
                        visible: (app.setting5name != "")
                        input.flags: TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoPeriodOff | TextInputFlag.PredictionOff | TextInputFlag.SpellCheckOff
                        textStyle {
                            textAlign: TextAlign.Center
                        }
                        validator: Validator {
                            id: tf5_validator
                            mode: ValidationMode.Immediate
                            errorMessage: "User ID must be supplied"
                            onValidate: {
                                if (tfSetting5_value.text.length > 0) {
                                    tf5_validator.state = ValidationState.Valid
                                } else {
                                    tf5_validator.state = ValidationState.Invalid
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
