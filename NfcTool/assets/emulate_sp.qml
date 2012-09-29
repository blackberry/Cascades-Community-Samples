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

	    id: emulateSp
	    objectName: "emulateSp"
	    signal emulateSpRequested()
        
        Container {
            layout: StackLayout {
            }

            topPadding: 10
            leftPadding: 30
            rightPadding: 30

            Label {
                text: "Emulate a Smart Tag"
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                }
            }

		    Label {
		        text: "Enter the URI for your tag"
		        textStyle {
		            base: SystemDefaults.TextStyles.BodyText
		            color: Color.LightGray
		            lineHeight: 1.1
		        }
		    }
		
		    TextArea {
		        id: txf_uri
		        objectName: "txf_uri"
		        hintText: "Enter the URI for your tag"
		        text: _emulateSp._uri
		        textStyle {
		            base: SystemDefaults.TextStyles.BodyText
		        }
		    }
		    
		    Label {
		        text: "Enter some text for your tag"
		        textStyle {
		            base: SystemDefaults.TextStyles.BodyText
		            color: Color.LightGray
		            lineHeight: 1.1
		        }
		    }
		
		    TextArea {
		        id: txf_text
		        objectName: "txf_text"
		        hintText: "Enter some text for your tag"
		        text: _emulateSp._text
		        textStyle {
		            base: SystemDefaults.TextStyles.BodyText
		        }
		    }   

        }
	}
	
	actions: [
	    ActionItem {
	        id: emulate_sp_action
	        title: "Emulate"
	        imageSource: "images/tag.png"
	        ActionBar.placement: ActionBarPlacement.OnBar
	        onTriggered: {
	            console.log("Emulate Sp Triggered");
                emulateSp.emulateSpRequested(); 
	        }
	    }
	]
}
