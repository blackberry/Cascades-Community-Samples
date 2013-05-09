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
        id: sendVcard
        objectName: "sendVcard"

// ======== Properties =============
	    
	    property alias ndefFirstName: txf_first_name.text
	    property alias ndefLastName:  txf_last_name.text
	    property alias ndefAddress:   txf_address.text
	    property alias ndefEmail:     txf_email.text
	    property alias ndefMobile:    txf_mobile.text

// ======== SIGNAL()s ==============
// ======== SLOT()s ================
// ======== Local functions ========

        Container {
            layout: StackLayout {
            }

            topPadding: 5
            leftPadding: 30
            rightPadding: 30

            Label {
                text: "SNEP: Send a vCard"
                textStyle {
                    base: SystemDefaults.TextStyles.TitleText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                }
            }

            ScrollView {
                scrollViewProperties {
                    scrollMode: ScrollMode.Vertical
                }
                Container {
		            Container {
		                layout: StackLayout {
			                orientation: LayoutOrientation.LeftToRight
			            }
			            Container {
			                layout: StackLayout {
			                    orientation: LayoutOrientation.TopToBottom
			                }
				            TextArea {
				                editable: false
				                text: "First Name:"
				                textStyle {
				                    base: SystemDefaults.TextStyles.BodyText
				                    color: Color.LightGray
				                }
				            }
				            TextArea {
				                id: txf_first_name
				                objectName: "txf_first_name"
				                hintText: "Enter your first name"
				                text: "Fred"
				                maxWidth: 335
				                horizontalAlignment: HorizontalAlignment.Right
				                textStyle {
				                    base: SystemDefaults.TextStyles.BodyText
				                }
				            }
			            }
			            Container {
				            layout: StackLayout {
			                    orientation: LayoutOrientation.TopToBottom
				            }
				            TextArea {
				                editable: false
				                text: "Last Name:"
				                textStyle {
				                    base: SystemDefaults.TextStyles.BodyText
				                    color: Color.LightGray
				                }
				            }
				            TextArea {
				                id: txf_last_name
				                objectName: "txf_last_name"
				                hintText: "Enter your last name"
				                text: "Flintstone"
				                maxWidth: 335
				                horizontalAlignment: HorizontalAlignment.Right
				                textStyle {
				                    base: SystemDefaults.TextStyles.BodyText
				                }
				            }
			            }
		            }
		            TextArea {
		                editable: false
		                text: "Address:"
		                textStyle {
		                    base: SystemDefaults.TextStyles.BodyText
		                    color: Color.LightGray
		                }
		            }
		            TextArea {
		                id: txf_address
		                objectName: "txf_address"
		                hintText: "Enter your address"
		                text: "301 Cobblestone Way, Bedrock"
		                maxWidth: 685
		                horizontalAlignment: HorizontalAlignment.Right
		                textStyle {
		                    base: SystemDefaults.TextStyles.BodyText
		                }
		            }
		            Container {
						layout: StackLayout {
				            orientation: LayoutOrientation.LeftToRight
						}                
			            TextArea {
			                editable: false
			                text: "Email:"
			                textStyle {
			                    base: SystemDefaults.TextStyles.BodyText
			                    color: Color.LightGray
			                }
		                    maxWidth: 180
		                }
			            TextArea {
			                id: txf_email
			                objectName: "txf_email"
			                hintText: "Enter your email address"
			                text: "fred@dinomail.com"
			                textStyle {
			                    base: SystemDefaults.TextStyles.BodyText
			                }
			            }
		            }
		            Container {
						layout: StackLayout {
			                orientation: LayoutOrientation.LeftToRight
						}                
			            TextArea {
			                editable: false
			                text: "Mobile:"
			                textStyle {
			                    base: SystemDefaults.TextStyles.BodyText
			                    color: Color.LightGray
			                }
		                    maxWidth: 180
			            }
			            TextArea {
			                id: txf_mobile
			                objectName: "txf_mobile"
			                hintText: "Enter your mobile number"
			                text: "+11231231234"
			                textStyle {
			                    base: SystemDefaults.TextStyles.BodyText
			                }
			            }
		            }
		
		            TextArea {
		                editable: false
		                text: "Now click the Share icon in the Action Bar to write the data to a tag."
		                textStyle {
		                    base: SystemDefaults.TextStyles.BodyText
		                    color: Color.LightGray
		                    lineHeight: 1.1
		                }
		            }
                }
            }
        }
    }

	actions: [
        InvokeActionItem {
            id: sharedNdefData
	        ActionBar.placement: ActionBarPlacement.OnBar
            query {
                mimeType: "application/vnd.rim.nfc.ndef"
                invokeActionId: "bb.action.SHARE"
            }
            handler: InvokeHandler {
                id: shareHandler
                onInvoking: {
                    sharedNdefData.data = _ndefFactory.getNdefVcardMessage(sendVcard.ndefFirstName, sendVcard.ndefLastName, sendVcard.ndefAddress, sendVcard.ndefEmail, sendVcard.ndefMobile);
                    shareHandler.confirm();
                }
            }
        }
	]
}
