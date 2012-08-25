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

//NavigationPane {
   // id: nav
	
	//Main page container
	Page {
	    objectName: "page1"
	    content: Container {
	        layout: AbsoluteLayout {
	                }
	                  
            ImageView {
			    id: imageHexBackground
			    imageSource: "asset:///images/hex_background.jpg"
			    preferredWidth: 768
			    preferredHeight: 1280	
			}   
			//This container stacks the button group and main text field vertically.
			Container {
			    layout: StackLayout {
			    }
			    preferredWidth: 768
			    TextArea {
	                visible : true
	                editable : false
	                text: "Logging & Exceptions"
	                backgroundVisible : true
	                textStyle { 
	                    base: SystemDefaults.TextStyles.BodyText; 
	                    color: Color.White
	                    alignment : TextAlignment.Center
	                }    
	                preferredHeight: 75        
                }
                //Container used to stack both buttons into a horizontal button group.
                Container {   
                    layout: StackLayout {
                        layoutDirection: LayoutDirection.LeftToRight
                    }
		            Button {
			        	objectName: "generateExceptionButton"
			            text: "Generate Exception"
			            layoutProperties: StackLayoutProperties {
			                horizontalAlignment: HorizontalAlignment.Center
		                 }
			            preferredWidth: 400
			        }
			        Button {
			        	objectName: "clearLogButton"
			            text: "Clear"
			            layoutProperties: StackLayoutProperties {
			                horizontalAlignment: HorizontalAlignment.Center
		                 }
			            preferredWidth: 400
			        }
		        }
		        //The main text area in which we display the log file's contents.
		        TextArea {
                objectName: "logTextArea"
                visible : true
                editable : false
                text: "Cleared"
                backgroundVisible : true
                textStyle { 
                    base: SystemDefaults.TextStyles.BodyText; 
                    color: Color.White
                    alignment : TextAlignment.Left
                    size:25
                    }   
                    preferredHeight: 800        
                }
	       } 
	      
	    }
	    
	}
	
//}