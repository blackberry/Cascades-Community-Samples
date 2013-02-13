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
import CustomTimer 1.0

TabbedPane {
    // used to tab selections as there are some conditional behaviours that depends on which tab we came "from"
    property int p_prev_tab: 0
    // constants
    property int p_login_tab: 0
    property int p_welcome_tab: 1
    property int p_about_tab: 2
    property int p_profile_tab: 3
    showTabsOnActionBar: true
    id: tabbedPane
    onCreationCompleted: {
        p_prev_tab = p_login_tab;
        tabbedPane.remove(welcome_tab);
        tabbedPane.remove(profile_tab);
    }
    onActiveTabChanged: {
        tfUserId.text = "";
        tfPassword.text = "";
        if (tabbedPane.activeTab.title == "Profile") {
            tfNewFirstName.text = data_mgr.getFirstname();
            tfNewLastName.text = data_mgr.getLastname();
        }
    }
    Tab {
        id: login_tab
        title: "Login"
        imageSource: "asset:///img/login.png"
        onTriggered: {
            if (p_prev_tab == p_login_tab || p_prev_tab == p_welcome_tab) {
                var ok = sec_mgr.isCredentialsOK(tfUserId.text, tfPassword.text);
                if (ok) {
                    tabbedPane.remove(login_tab);
                    tabbedPane.remove(about_tab);
                    tabbedPane.add(welcome_tab);
                    tabbedPane.add(profile_tab);
                    tabbedPane.add(about_tab);
                    welcome_tab.title = "Logout";
                    tabbedPane.activeTab = welcome_tab;
                    sec_mgr.setLoggedIn(true);
                } else {
                    message.text = "Sorry, those details are not right.";
                    message_timer.start();
                }
            } else {
                p_prev_tab = p_login_tab;
            }
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                //        background: Color.Red
                Label {
                    id: lblHeading
                    text: qsTr("Login")
                    textStyle.base: SystemDefaults.TextStyles.BigText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                TextField {
                    id: tfUserId
                    hintText: "Enter your user ID"
                    inputMode: TextFieldInputMode.Text
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                }
                TextField {
                    id: tfPassword
                    hintText: "Enter your password"
                    inputMode: TextFieldInputMode.Password
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                }
                Label {
                    id: message
                    text: ""
                }
                Timer {
                    id: message_timer
                    time_limit: 5000
                    onTimeout: {
                        message.text = ""
                    }
                }
                ImageView {
                    imageSource: "asset:///img/closed_padlock.png"
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    onCreationCompleted: appear.play()
                    onTouch: wobble.play()
                    attachedObjects: [
                        SequentialAnimation {
                            id: appear
                            animations: [
                                ScaleTransition {
                                    duration: 500
                                    toX: 1.0
                                    fromX: 0.0
                                    toY: 1.0
                                    fromY: 0.0
                                }
                            ]
                        },
                        SequentialAnimation {
                            id: wobble
                            animations: [
                                RotateTransition {
                                    toAngleZ: -10
                                    duration: 50
                                },
                                RotateTransition {
                                    toAngleZ: 10
                                    duration: 50
                                },
                                RotateTransition {
                                    toAngleZ: -10
                                    duration: 50
                                },
                                RotateTransition {
                                    toAngleZ: 10
                                    duration: 50
                                },
                                RotateTransition {
                                    toAngleZ: 0
                                    duration: 50
                                }
                            ]
                        }
                    ]
                }
            }
        }
    }
    Tab {
        id: welcome_tab
        title: "Logout"
        imageSource: "asset:///img/logout.png"
        onTriggered: {
            welcome_tab.title = "Logout"
            if (p_prev_tab == p_login_tab || p_prev_tab == p_welcome_tab) {
                // remove all and add the ones we want to maintain the correct ordering
                tabbedPane.remove(welcome_tab);
                tabbedPane.remove(profile_tab);
                tabbedPane.remove(about_tab);
                tabbedPane.add(login_tab);
                tabbedPane.add(about_tab);
                tabbedPane.activeTab = login_tab;
                p_prev_tab = p_welcome_tab;
                message.text = ""
                tfUserId = ""
                tfPassword = ""
                sec_mgr.setLoggedIn(false);
            }
            p_prev_tab = p_welcome_tab;
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                //        background: Color.Red
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    id: lblHeading_welcome
                    text: qsTr("Welcome")
                    textStyle.base: SystemDefaults.TextStyles.BigText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: welcome_message
                    text: "Welcome! You have successfully logged in to the PasswordSample application"
                    horizontalAlignment: HorizontalAlignment.Center
                    multiline: true
                }
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    topPadding: 200
                    ImageView {
                        imageSource: "asset:///img/open_padlock.png"
                        onCreationCompleted: {
                            appear_welcome.play()
                        }
                        onTouch: {
                            if (event.isDown()) {
                                spin_welcome.play()
                            }
                        }
                        attachedObjects: [
                            SequentialAnimation {
                                id: appear_welcome
                                animations: [
                                    ScaleTransition {
                                        duration: 500
                                        toX: 1.0
                                        fromX: 0.0
                                        toY: 1.0
                                        fromY: 0.0
                                    }
                                ]
                            },
                            SequentialAnimation {
                                id: spin_welcome
                                animations: [
                                    RotateTransition {
                                        fromAngleZ: 0
                                        toAngleZ: 360
                                        duration: 500
                                    }
                                ]
                            }
                        ]
                    }
                }
            }
        }
    }
    Tab {
        id: profile_tab
        title: "Profile"
        imageSource: "asset:///img/profile.png"
        onTriggered: {
            if (p_prev_tab == p_welcome_tab || p_prev_tab == p_login_tab) {
                welcome_tab.title = "Welcome"
            }
            p_prev_tab = p_profile_tab;
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                //        background: Color.Red
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    id: lblHeading_profile
                    text: qsTr("User Profile")
                    textStyle.base: SystemDefaults.TextStyles.BigText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                TextField {
                    id: tfCurrentUserId
                    hintText: "Enter your current user ID"
                    inputMode: TextFieldInputMode.Text
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                }
                TextField {
                    id: tfCurrentPassword
                    hintText: "Enter your current password"
                    inputMode: TextFieldInputMode.Password
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                }
                TextField {
                    id: tfNewUserId
                    hintText: "Enter your new user ID"
                    inputMode: TextFieldInputMode.Text
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                }
                TextField {
                    id: tfNewPassword
                    hintText: "Enter your new password"
                    inputMode: TextFieldInputMode.Password
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                }
                TextField {
                    id: tfNewPassword2
                    hintText: "Enter your new password again if changing it"
                    inputMode: TextFieldInputMode.Password
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                }
                TextField {
                    id: tfNewFirstName
                    hintText: "Enter your first name"
                    inputMode: TextFieldInputMode.Text
                }
                TextField {
                    id: tfNewLastName
                    hintText: "Enter your last name"
                    inputMode: TextFieldInputMode.Text
                }
                Button {
                    text: "Save"
                    horizontalAlignment: HorizontalAlignment.Fill
                    onClicked: {
                        var ok = true;
                        if (sec_mgr.isCredentialsOK(tfCurrentUserId.text, tfCurrentPassword.text)) {
                            data_mgr.setUserid(tfNewUserId.text);
                            data_mgr.setFirstname(tfNewFirstName.text);
                            data_mgr.setLastname(tfNewLastName.text);
                            if (tfNewPassword.text != "") {
                                if (tfNewPassword.text == tfNewPassword2.text) {
                                    data_mgr.setPassword(tfNewPassword.text);
                                    lblProfileMessage.text = "";
                                } else {
                                    lblProfileMessage.text = "Error: Password values do not match";
                                    profile_timer.start();
                                    ok = false;
                                }
                            }
                        } else {
                            ok = false;
                            lblProfileMessage.text = "Current user_id/password incorrect";
                            profile_timer.start();
                        }
                        if (ok) {
                            lblProfileMessage.text = "New details saved";
                            if (tfNewUserId != "") {
                                tfCurrentUserId.text = tfNewUserId.text;
                            }
                            profile_timer.start();
                        }
                    }
                }
                Label {
                    id: lblProfileMessage
                }
                Timer {
                    id: profile_timer
                    time_limit: 5000
                    onTimeout: {
                        lblProfileMessage.text = ""
                    }
                }
            }
        }
    }
    Tab {
        id: about_tab
        title: "About"
        imageSource: "asset:///img/about.png"
        onTriggered: {
            if (p_prev_tab == p_welcome_tab || p_prev_tab == p_login_tab) {
                welcome_tab.title = "Welcome"
            }
            p_prev_tab = p_about_tab;
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                //        background: Color.Red
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    id: lblHeading_about
                    text: qsTr("About PasswordSample")
                    textStyle.base: SystemDefaults.TextStyles.BigText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: lblHeading_version
                    text: qsTr("V1.0.1")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: lblBody_about
                    text: qsTr("The PasswordSample application demonstrates how the BlackBerry 10 Security Builder APIs can be used to exploit one way encryption for the purposes of securing a password which must be stored. In this case the password is stored locally in a QSettings object but it could just as easily be stored remotely.")
                    multiline: true
                }
                Label {
                    id: lblBody_about2
                    text: qsTr("The default user ID and password are 'guest' and 'password'.")
                    multiline: true
                }
                Label {
                    id: lblAuthors
                    text: qsTr("Authors:")
                    multiline: true
                }
                Label {
                    id: lblAuthors1
                    text: qsTr("Martin Woolley (@mdwrim)")
                    multiline: true
                }
                Label {
                    id: lblAuthors2
                    text: qsTr("John Murray (@jcmrim)")
                    multiline: true
                }
            }
        }
    }
}
