
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

Page {

    function logMessage(message) {
        log.text += (qsTr("\n") + message );
    }

    Container {
        layout: StackLayout {
        }

        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        Label {
            text: qsTr("PropertyExample")
            textStyle {
                base: SystemDefaults.TextStyles.BigText
            }
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }

        TextField {
            id: tf
            objectName: "tf"
            // this causes the READ method bound to our property to be called
            text: my_prop.my_property
        }
        Button {
            text: "Change Property"
            horizontalAlignment: HorizontalAlignment.Fill
            onClicked: {
                // this causes the WRITE method bound to our property to be called
                my_prop.my_property = tf.text
            }
        }
        Button {
            text: "Reset"
            horizontalAlignment: HorizontalAlignment.Fill
            onClicked: {
                // this calls a C++ method that was exposed using Q_INVOKABLE
                my_prop.resetMyProperty();
            }
        }

        ScrollView {
            id: logScroller
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            TextArea {
                id: log
                text: ""
                editable: false
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    color: Color.Green
                    fontFamily: "Courier"
                    lineHeight: 1.1
                }
                onTextChanged: {
                    logScroller.scrollToPoint(0, Infinity);
                }
            }
        }
    }

    actions: [
        ActionItem {
            title: qsTr("About")
            imageSource: "asset:///images/about.png"
            onTriggered: {
                aboutInfo.open();
            }
        }
    ]

    attachedObjects: [
        AboutSheet {
            id: aboutInfo
        }
    ]

}
