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

/*
 * This control creates a header for the push message categories
 * to let users more easily identify which priority of message was
 * received.
 */

Container {
    property int priority: -1
    id: container
    preferredWidth: maxWidth
    topPadding: 10
    leftPadding: 10
    bottomPadding: 10
    Label {
        id: label
        textStyle {
            base: SystemDefaults.TextStyles.TitleText
            color: Color.White
            fontWeight: FontWeight.Bold
        }
    }

    /**
     * This nifty signal handler was created for us by the Qt framework, it allows
     * us to listen and be notified whenever the 'priority' property changes,
     * allowing this element to be re-used and us to update the UI visuals.
     */
    onPriorityChanged: {
        switch (priority) {
            case 0: //Low
                label.text = " !";
                container.background = Color.DarkBlue;
                break;
            case 1: //Medium
                container.background = Color.DarkYellow;
                label.text = "! !";
                break;
            case 2: //High
                container.background = Color.DarkRed;
                label.text = "!!!";
                break;
            default: //Invalid or no data
                container.background = Color.DarkGray;
                label.text = "No data";
                break;
        }
        console.log(label.text + ":" + container.background);
    }
}
