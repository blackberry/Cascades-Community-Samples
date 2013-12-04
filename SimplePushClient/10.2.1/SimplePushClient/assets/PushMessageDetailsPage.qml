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

import bb.cascades 1.2

/*
 * This control is essentially a wrapper around the LastPushSceneCover
 * control that also adds a timestamp. This is used to display more
 * information included in a push message by adding it to a new Page.
 */

Page {
    property alias priority: lastPushSceneCover.priority
    property alias title: lastPushSceneCover.title
    property alias body: lastPushSceneCover.body
    property alias timeReceived: timestamp.text
    Container {
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill
        LastPushSceneCover {
            id: lastPushSceneCover
        }
        Label {
            id: timestamp
            textStyle.fontStyle: FontStyle.Italic
            verticalAlignment: VerticalAlignment.Bottom
        }
    }
}