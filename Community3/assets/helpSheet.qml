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
import com.msohm.URLProvider 1.0

Sheet {
    id: helpSheet
    Page {
        titleBar: TitleBar {
            title: qsTr("Help")

            dismissAction: ActionItem {
                title: qsTr("Close")
                onTriggered: {
                    helpSheet.close();
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
                    text: qsTr("In order to author posts in the BlackBerry Support Forums you will need to configure your forum username and password in the application settings.  If you do not yet have a forum account, click ") +  "<a href=\"" + urlProvider.getForumWebURL() + "user/userregistrationpage\">" + qsTr("here") + "</a> " + qsTr("to create one.")
                    multiline: true
                    textFormat: TextFormat.Html
                }
                Label {
                    text: qsTr("To Reply, Like or Share a post long press on the post subject, not body.  Long pressing on the body allows you to copy contents of the body.")
                    multiline: true
                }                
                Label {
                    text: qsTr("This application stores your forum login information so you don't need to enter it each time.  Your credentials are encrypted, with the key to decrypt saved using BlackBerry Remote Encrypted Storage tied to your BlackBerry ID account.  You need to be logged into BlackBerry ID to use this feature.  If you change your BlackBerry ID account, decryption of your credentials will fail.  If that happens you will need to enter and save them again using the settings page.")
                    multiline: true
                }
            }
        }
        attachedObjects: [
            URLProvider {
                id: urlProvider
            }
        ] 
    }
}