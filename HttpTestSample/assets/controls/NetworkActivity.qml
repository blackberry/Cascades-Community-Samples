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
*  This is a custom control to display network activity
*
*  You will learn how to:
*  -- Create a custom control
*  -- Create an ActivityIndicator
*  -- Create Javascript functions for setting the title, starting and stoping the ActivityIndicator
**/
Container {
    id : networkActivity
    background : Color.Gray
    layout : DockLayout {
    }
    Container {
	 	attachedObjects: [
	 		TextStyleDefinition
	 		{
	 		    id: tsd
	 		    base: SystemDefaults.TextStyles.SubtitleText
	 		    alignment: TextAlignment.ForceLeft
	  		}
	 	]
	 
	    layout : StackLayout {
            layoutDirection : LayoutDirection.TopToBottom
        }
        layoutProperties : DockLayoutProperties {
            horizontalAlignment : HorizontalAlignment.Center
            verticalAlignment : VerticalAlignment.Center
        }
        Label {
            id : activityLabel
            layoutProperties : StackLayoutProperties {
            }
            textStyle { base: SystemDefaults.TextStyles.BigText; color: Color.White }           
        }
        ActivityIndicator {
            id : activityId
            preferredHeight : 500
            preferredWidth : 500
            }
    }
    function setTitle(text)
    {
        activityLabel.text = text;
    }
    function startIndicator()
    {
        activityId.start();
    }
    function stopIndicator()
    {
        activityId.stop();
    }
}