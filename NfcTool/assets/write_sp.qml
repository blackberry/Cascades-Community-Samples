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
            }

            topPadding: 10
            leftPadding: 30
            rightPadding: 30

            Label {
                text: "Write a Smart Tag"
                textStyle {
                    base: SystemDefaults.TextStyles.TitleText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                }
            }

		    Label {
		        text: "URI"
		        textStyle {
		            base: SystemDefaults.TextStyles.BodyText
		            color: Color.LightGray
		            lineHeight: 1.1
		        }
		    }
		
		    TextArea {
		        id: txf_uri
		        objectName: "txf_uri"
		        hintText: "Enter the URI you wish to write to the tag"
		        input.flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff | TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoPeriodOff | TextInputFlag.WordSubstitutionOff		        
   		        inputMode: TextAreaInputMode.Text
		        content.flags: TextContentFlag.ActiveTextOff
		        text: _writeSpMenu._uri
		        textStyle {
		            base: SystemDefaults.TextStyles.BodyText
		        }
		    }
		    
		    Label {
		        text: "Text"
		        textStyle {
		            base: SystemDefaults.TextStyles.BodyText
		            color: Color.LightGray
		            lineHeight: 1.1
		        }
		    }
		
		    TextArea {
		        id: txf_text
		        objectName: "txf_text"
		        hintText: "Enter some text for your smart poster"
		        input.flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff | TextInputFlag.AutoCapitalizationOff | TextInputFlag.AutoCorrectionOff | TextInputFlag.AutoPeriodOff | TextInputFlag.WordSubstitutionOff		        
		        text: _writeSpMenu._text
		        textStyle {
		            base: SystemDefaults.TextStyles.BodyText
		        }
		    }   

            Label {
                text: "Touch the Write button when ready..."
                textStyle {
                    base: SystemDefaults.TextStyles.SmallText
                    color: Color.LightGray
                    lineHeight: 1.1
                }
            }
        }
	}
	
	actions: [
	    ActionItem {
	        id: write_sp_action
	        title: "Write"
	        imageSource: "images/write.png"
	        ActionBar.placement: ActionBarPlacement.OnBar
	        onTriggered: {
	            console.log("Write Sp Triggered");
                writeSp.writeSpRequested(); 
	        }
	    }
	]
}
