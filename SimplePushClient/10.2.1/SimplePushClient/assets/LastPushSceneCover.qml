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
 * This class is used to display details of the last received
 * push message on the SceneCover when the app is thumbnailed/
 * minimized.
 */

Container {
    property alias title: title.text
    property alias body: body.text
    horizontalAlignment: HorizontalAlignment.Fill
    property alias priority: priorityHeader.priority
    PriorityHeader {
        id: priorityHeader
    }
    Label {
        id: title
        textStyle {
            base: SystemDefaults.TextStyles.TitleText
        }
    }
    Label {
        id: body
        multiline: true
    }
}