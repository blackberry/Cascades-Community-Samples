/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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
import "NameUtils.js" as NameUtils

Container {
    id: playerContainer 
    preferredWidth : 300
    layout: StackLayout {
        layoutDirection: LayoutDirection.RightToLeft
        bottomPadding: 1.0    
        topPadding: 1.0    
    }
    layoutProperties: StackLayoutProperties {
        verticalAlignment: VerticalAlignment.Bottom
    }

	attachedObjects: [
		TextStyleDefinition
		{
		  id: tsd
		  base: SystemDefaults.TextStyles.SubtitleText // like P4
          alignment: TextAlignment.ForceRight
 		}
	]

    Label 
    {
        objectName : "name"
        id: name
        text: "player"
        textStyle.base: tsd.style
    }            			            
    Label 
    {
        objectName : "minutes"
        id: minutes
        text: "50.7"
        textStyle.base: tsd.style
    } 

    function saveLocation(x, y)
    {
        NameUtils.positionX = x;
        NameUtils.positionY = y;
    }
    
    function getReboundsX()
    {
        return NameUtils.positionX;
    }

    function getReboundsY()
    {
        return NameUtils.positionY;
    }
    
    function hideMinutes()
    {
        minutes.opacity = 0;
    }
    
    function showMinutes()
    {
        minutes.opacity = 1;
    }        			            
           			            
}
