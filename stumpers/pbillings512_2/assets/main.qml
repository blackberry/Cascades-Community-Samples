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

Page {
    Container {
        id: maincontainer
        leftPadding: 20
        rightMargin: leftPadding
        topMargin: leftPadding
        bottomMargin: leftPadding
        
        // This is the hidden container that we want to show or hide
        Container {
            id: hiddenContainer
            visible: false
            objectName: "hiddenContainer"
            preferredHeight: 400
            preferredWidth: 768
            Label {
                multiline: true
                text: "Loads of information that was previously hidden!"
                textStyle.base: SystemDefaults.TextStyles.BigText
            }
        }
        
        // This is what's shown all the time
        Container {
            layout: StackLayout {
            }
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            Label {
                multiline: true
                text: "This area just shows a lot of UI - stuff. It's gonna be superduper and even more aweseom when it's filled with stuff that actually matters to ppl, like BBM or sports results or stocks.\n\n The button will do the trick but it's even more fun to swipe down!"
                textStyle.base: SystemDefaults.TextStyles.BodyText
            }
        }
        
        // A button for testing purposes, does the same as the signal.
        Button {
            text: "swap"
            onClicked: {
                if (hiddenContainer.visible) {
                    hiddenContainer.visible = false
                } else {
                    hiddenContainer.visible = true
                }
            }
        }
    }
}
