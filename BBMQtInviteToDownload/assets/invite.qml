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
import com.bbmqtinvitetodownload.invitetodownload 1.0

Container {
    layout: StackLayout {
    }
    attachedObjects: [
        InviteToDownload {
            id: inviteToDownload
        }
    ]
    background: Color.create("#c6f66f")
    horizontalAlignment: HorizontalAlignment.Center
    TextArea {
        text: "Press the button to invite your BlackBerry Messenger contacts to download this application from App World."
        editable: false
    }
    Button {
        id: inviteButton
        text: "Invite to Download"
        onClicked: {
            inviteToDownload.sendInvite();
        }
        horizontalAlignment: HorizontalAlignment.Center
    }
    TextArea {
        text: "Note that this sample is not in App World so the final invite button will be disabled."
        editable: false
    }
}
