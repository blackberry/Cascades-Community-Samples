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

//-- create one page with a label and text

Page {
    Container {
        layout: DockLayout {
        }
        ImageView {
            imageSource: "asset:///images/background.png"
        }
        Container {
            layout: StackLayout {
                topPadding: 50
                leftPadding: 50
                rightPadding: 50
            }
            LabelLabel {
                objectName: "qllStartupMode"
                label: "Startup Mode: "
                text: "--"
            }
            Divider {
            }
            LabelLabel {
                objectName: "qllSource"
                label: "Source: "
                text: "--"
            }
            Divider {
            }
            LabelLabel {
                objectName: "qllTarget"
                label: "Target: "
                text: "--"
            }
            Divider {
            }
            LabelLabel {
                objectName: "qllAction"
                label: "Action: "
                text: "--"
            }
            Divider {
            }
            LabelLabel {
                objectName: "qllMIME"
                label: "MIME Type: "
                text: "--"
            }
            Divider {
            }
            LabelLabel {
                objectName: "qllURI"
                label: "URI: "
                text: "--"
            }
            Divider {
            }
            LabelLabel {
                objectName: "qllData"
                label: "Data: "
                text: "--"
            }
        }
    }
}
