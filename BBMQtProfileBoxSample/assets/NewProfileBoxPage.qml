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
    background: Color.create("#121212")    

    Label{
        textStyle.color: Color.create("#fafafa")
        text:"Create New Profile Box";
	}
        
    Divider {}
        
    Label{
        textStyle.color: Color.create("#fafafa")
        text:"Profile Box Text";
    }
    
    TextField {
        objectName: "profileBoxTextField"
        hintText: "Profile Box Text Here" 
    }
    
    Divider {}

    Label{
        textStyle.color: Color.create("#fafafa")
        text:"Pick an icon from below";
    }    
    
    RadioGroup {
        objectName: "iconsRadioGroup"
        Option { 
            objectName: "images/apple.png"; 
            imageSource: "images/apple.png"; 
            selected: true }
        Option { 
            objectName: "images/pear.png";  
            imageSource: "images/pear.png"}
        Option { 
            objectName: "images/orange.png";  
            imageSource: "images/orange.png"}           
    }
    
    Button {      
        objectName: "saveProfileBoxButton"
        text: "Save Profile Box"       
    }    
} 