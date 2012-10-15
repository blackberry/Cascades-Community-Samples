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

Container {
    
    background: Color.create("#c6f66f")
    preferredWidth: 999
    topPadding: 20
    leftPadding: 20
    rightPadding: 20
    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        Container {
            objectName: "avatarContainer"
            ImageView {
                objectName: "avatarImageView"
                preferredHeight: 300
                preferredWidth: 300
                                //Image by Decosigner; http://openclipart.org/detail/104977/help-orb-button-by-decosigner
                imageSource: "images/avatarPlaceHolder.png"
            }
        }
        Button {
            objectName: "getAvatarButton"
            text: "Get avatar"
        }
    }
    Divider {
    }
    Container {
        minHeight: 50
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        ImageView {
            objectName: "busyIcon"
            imageSource: "images/busy.png"
            visible: false
        }
        TextArea {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            objectName: "nameLabel"
            editable: false
            backgroundVisible: false
            textStyle {
                color: Color.Black
                fontWeight: FontWeight.Bold
            }
        }
    }
    TextArea {
        objectName: "statusMessage"
        editable: false
        backgroundVisible: false
        text: "Status Message:"
        textStyle {
            color: Color.Black
            fontWeight: FontWeight.Bold
        }
    }
    TextArea {
        objectName: "personalMessage"
        editable: false
        backgroundVisible: false
        text: "Personal Message:"
        textStyle {
            color: Color.Black
            fontWeight: FontWeight.Bold
        }
    }
    TextArea {
        objectName: "ppid"
        editable: false
        backgroundVisible: false
        text: "Ppid:"
        textStyle {
            color: Color.Black
            fontWeight: FontWeight.Bold
        }
    }
    TextArea {
        objectName: "appVersion"
        editable: false
        backgroundVisible: false
        text: "App version:"
        textStyle {
            color: Color.Black
            fontWeight: FontWeight.Bold
        }
    }
    TextArea {
        objectName: "handle"
        editable: false
        backgroundVisible: false
        text: "handle:"
        textStyle {
            color: Color.Black
            fontWeight: FontWeight.Bold
        }
    }
    TextArea {
        objectName: "platformVersion"
        editable: false
        backgroundVisible: false
        text: "Platform version:"
        textStyle {
            color: Color.Black
            fontWeight: FontWeight.Bold
        }
    }
}
