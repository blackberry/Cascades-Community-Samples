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

	    id: writeSp
	    objectName: "writeSp"
	    signal writeSpRequested()
        
        Container {
            layout: StackLayout {
                topPadding: 10
                leftPadding: 30
                rightPadding: 30
            }

            TextArea {
                text: "Write a Smart Tag"
                editable: false
                textStyle {
                    base: SystemDefaults.TextStyles.BigText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                }
            }

			Container {
			    layout: StackLayout {
			    }
			
			    topMargin: 70
			    bottomMargin: 50;
			
			    TextArea {
			        editable: false
			        text: "Enter the URI you wish to write to the tag"
			        textStyle {
			            base: SystemDefaults.TextStyles.BodyText
			            color: Color.LightGray
			            lineSpacing: 1.1
			        }
			    }
			
			    TextArea {
			        preferredHeight: 65
			        id: txf_uri
			        objectName: "txf_uri"
			        hintText: "Enter the URI you wish to write to the tag"
			        text: _writeSpMenu._uri
			        textStyle {
			            base: SystemDefaults.TextStyles.BodyText
			        }
			    }
			    
			    TextArea {
			        editable: false
			        text: "Enter some text for your smart poster"
			        textStyle {
			            base: SystemDefaults.TextStyles.BodyText
			            color: Color.LightGray
			            lineSpacing: 1.1
			        }
			    }
			
			    TextArea {
			        preferredHeight: 65
			        id: txf_text
			        objectName: "txf_text"
			        hintText: "Enter some text for your smart poster"
			        text: _writeSpMenu._text
			        textStyle {
			            base: SystemDefaults.TextStyles.BodyText
			        }
			    }   
			}

            TextArea {
                editable: false
                text: "Once you are happy with the values you have specified in the fields above press the Write button in the Action Bar below.\n\nYou will then be presented with an Event Log screen where you will be able to present a tag to the handset to have your data written to it."
                textStyle {
                    base: SystemDefaults.TextStyles.BodyText
                    color: Color.LightGray
                    lineSpacing: 1.1
                }
            }
        }
	}
	
	actions: [
	    ActionItem {
	        id: write_sp_action
	        title: "Write"
	        imageSource: "images/write.png"
	        onTriggered: {
	            console.log("Write Sp Triggered");
                writeSp.writeSpRequested(); 
	        }
	    }
	]
}
