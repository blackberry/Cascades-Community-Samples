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
*/import bb.cascades 1.0
import "common"

Page {
    content: MenuContainer {
        id: theEventLog
        objectName: "theEventLog"
        signal eventLogBackButton()
        Container {
            layout: StackLayout {
            }
            topPadding: 50
            leftPadding: 30
            rightPadding: 30

            Label {
                text: (_el.message == "" || _el.message == undefined) ? "Event log" : "Event log\n" + _el.message
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                }
            }
            Label {
                text: "Newest items first"
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    color: Color.LightGray
                    lineHeight: 1.1
                }
            }
            ScrollView {
                scrollViewProperties {
                    scrollMode: ScrollMode.Vertical
                }
                Container {
                    ListView {
                        dataModel: _model
                        listItemProvider: _manager
                        preferredWidth: 768
                        preferredHeight: 800
                    }
                }
            }
        }
    }
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            onTriggered: {
                console.log("Event Log Back Button Triggered");
                theEventLog.eventLogBackButton();
            }
        }
    }
}
