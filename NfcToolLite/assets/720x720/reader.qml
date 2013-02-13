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
import "common"

Page {
    content: MenuContainer {

	    id: readerPage
	    objectName: "readerPage"
	    
// ======== Properties =============
// ======== SIGNAL()s ==============
// ======== SLOT()s ================
	    
		function message(text) {
		    logMessage(text);    
		}

// ======== Local functions ========
	    
        function logMessage(message) {
            log.text += ( qsTr("\n") + message );
        }

        Container {
            layout: StackLayout {
            }

            topPadding: 5
            leftPadding: 30
            rightPadding: 30

            Label {
                text: "Read a Tag"
                textStyle {
                    base: SystemDefaults.TextStyles.TitleText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                }
            }

		    TextArea {
		        editable: false
		        text: "Newest events at bottom of screen"
		        textStyle {
                    base: SystemDefaults.TextStyles.BodyText
		            color: Color.LightGray
		            lineHeight: 1.1
		        }
		    }

		    Logger {
		        id: log
		    }
        }
	}

    onCreationCompleted: {
    }
}
