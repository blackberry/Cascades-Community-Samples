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

/**
* This class creates a custom Button rather than rely on the Button class. This is 
* done for a few reasons:
*    1) It allows us to specify a custom image including a shadow
*    2) It allows us to customize the text
*    3) It allows us to customize the animation when it is interacted with by the user
*/

Container {
    id: container
    property alias imageSource: cloudImage.imageSource
    property alias shadowImageSource: shadowImage.imageSource
    property alias text: label.text
    signal selected()
    
    layout: DockLayout {
    }
    
    ImageView {
        id: shadowImage
        opacity: 0.5
    }
     
    ImageView {
        id: cloudImage
    }
  	       
    Label {
        id: label
        text: "<Text>"
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Center
        textStyle {
            fontSize: FontSize.XXLarge
	        color: Color.create("#5a5a5a")
        }
	}
	
	// When the button is pressed/released we display a slight animation
	// to let the end-user know that something has occurred
    onTouch: {
        if (event.isDown()) {
            container.rotationZ = 10
            container.opacity = .5
            shadowImage.translationX = -3;
            shadowImage.translationY = -3;
            cloudImage.translationX = 2;
            cloudImage.translationY = 2;
        } else if (event.isUp()) {
            container.rotationZ = 0
            container.opacity = 1
            shadowImage.translationX = 0
            shadowImage.translationY = 0
            cloudImage.translationX = 0;
            cloudImage.translationY = 0;
            //Emit the selected() signal to notify the app that the button is pressed
            selected();  
        }
    }
    
    
}
