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

Container{   
    Label{
        text:"Update Profile";
                        textStyle {
                            base: SystemDefaults.TextStyles.BigText
                            color: Color.Black
                            fontWeight: FontWeight.Bold
                        }
	}
        
    Divider {}
    
    background: Color.create ("#c6f66f")   
    
    Label{
        text:"Personal Message";
                        textStyle {
                            base: SystemDefaults.TextStyles.BigText
                            color: Color.Black
                            fontWeight: FontWeight.Bold
                        }
    }
    
    TextField {
        objectName: "PersonalMessageField"
        hintText: "Personal Message Here" 

    }
    
    Button {      
        objectName: "savePersonalMsgButton"
        text: "Save personal message"       
    } 

    Label{
        text:"Status Message";
                        textStyle {
                            base: SystemDefaults.TextStyles.BigText
                            color: Color.Black
                            fontWeight: FontWeight.Bold
                        }
    }        
    
    TextField {
            objectName: "StatusMessageField"
            hintText: "Status Message Here" 
            
    }
    
    

    Container {
        layout: StackLayout{
            orientation: LayoutOrientation.LeftToRight  
        }
        
        CheckBox{
            objectName:"ShowBusyCheckBox"
            
            topMargin: 10
            bottomMargin: 10
            leftMargin: 10
            rightMargin: 10
        }
        Label{
            text:"Show Busy";
                            textStyle {
                                base: SystemDefaults.TextStyles.BigText
                                color: Color.Black
                                fontWeight: FontWeight.Bold
                            }
        }
    }
    
    Button {      
        objectName: "saveStatusButton"
        text: "Save status"       
    }    
} 