/* Copyright (c) 2014 BlackBerry Ltd.
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
 * 
 * The Lithium REST APIs are owned by Lithium Technologies, Inc. 
 * (http://www.lithium.com) and are not part of the “Works” licensed 
 * hereunder pursuant to the Apache 2.0 license.
 */

import bb.cascades 1.2
import com.msohm.Credentials 1.0
import com.msohm.URLProvider 1.0
import com.msohm.ApplicationSettings 1.0

Sheet {
    id: settingsSheet
    Page {
        titleBar: TitleBar {
            title: qsTr("Settings")

            dismissAction: ActionItem {
                title: qsTr("Cancel")
                onTriggered: {
                    settingsSheet.close();
                }
            }

            acceptAction: ActionItem {
                title: qsTr("Save")
                onTriggered: {
                    if (defaultTabDropDown.selectedIndex != -1)
                    {
                        appSettings.setDefaultTab(defaultTabDropDown.selectedValue);
                    }

                    forumCredentials.setCredentials(forumUsername.text, forumPassword.text);
                    
                    settingsSheet.close();
                }
            }
        }
        ScrollView {
            
	        Container {
	            leftPadding: 10.0
	            topPadding: 10.0
	            rightPadding: 10.0
	            bottomPadding: 10.0
	            
	            Label {
	                text: qsTr("BlackBerry Community App Settings")
	                textStyle.fontWeight: FontWeight.Bold
                    multiline: true
	            }
	            DropDown {
	                id: defaultTabDropDown
	                title: qsTr("Choose Default Tab")
	                selectedIndex: appSettings.getDefaultTab();
	                Option {
	                    text: qsTr("Blog")
	                    value: 0
	                }
	                Option {
	                    text: qsTr("Forum")
	                    value: 1
	                }
	                //ToDo Add other options if you add more tabs in main.qml.
	            }
	            Divider {
	            }
	
	            Label {
	                text: qsTr("BlackBerry Forum Settings")
	                textStyle.fontWeight: FontWeight.Bold
                    multiline: true
	            }
	            Divider {
	
	            }
	            Label {
	                text: qsTr("If you do not yet have a forum account, click ") +  "<a href=\"" + urlProvider.getForumWebURL() + "user/userregistrationpage\">" + qsTr("here") + "</a> " + qsTr("to create one.")
	                textFormat: TextFormat.Html
	                multiline: true
	                autoSize.maxLineCount: 5
	
	            }
	            Divider {
	
	            }
	            Label {
	                text: qsTr("Username")
	            }
	            TextField {
	                id: forumUsername
                    text: forumCredentials.username
	            }
	            Divider {
	
	            }
	            Label {
	                text: qsTr("Password")
	            }
	            TextField {
	                id: forumPassword
	                inputMode: TextFieldInputMode.Password
                    text: forumCredentials.password
	
	            }
	            Divider {
	
	            }
	            Label {
	                text: "<a href=\"" + urlProvider.getForumWebURL() + "authentication/forgotpasswordpage\">" + qsTr("Forgot your BlackBerry Forum username or password?") + "</a>"
	                textFormat: TextFormat.Html
	                multiline: true
	            }
	            Divider {
	            
	            }
	            Label {
	                id: loggedInLabel
	                multiline: true
	            }
	            Divider {
	            
	            }
	        }
	    }
        attachedObjects: [
            
            Credentials {
                id: forumCredentials
            },
            URLProvider {
                id: urlProvider
            },
            ApplicationSettings {
                id: appSettings
            }
        ]        
    }
    
    onCreationCompleted: {
        if (forumCredentials.loggedIn)
        {
            loggedInLabel.text = qsTr("You are logged into the BlackBerry Forums");
            loggedInLabel.textStyle.color = Color.DarkGreen;
        }
        else 
        {
            loggedInLabel.text = qsTr("You are NOT logged into the BlackBerry Forums");
            loggedInLabel.textStyle.color = Color.Red;
        }
    }
}
