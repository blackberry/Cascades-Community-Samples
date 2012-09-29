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

ScrollView {
    scrollViewProperties {
	    scrollMode: ScrollMode.Horizontal
    }

	Container {
	    id: aListItemRoot
	    preferredWidth: 768 // Colt scren width in landscape
	    preferredHeight: 45  // height of each row entry we want to render (font dependency)
	    
	    layout: DockLayout { } // to allow docking of decoration relative to the row element
	    
	    Container {
	        id: background
	        preferredHeight: aListItemRoot.preferredHeight-1 // leave one pixel for divider
	        
            verticalAlignment: VerticalAlignment.Top    // dock to top of the row
            horizontalAlignment: HorizontalAlignment.Fill // and full width
	    }
	
		Label {
			id: log_entry
            verticalAlignment: VerticalAlignment.Top        // docks at top of row
            horizontalAlignment: HorizontalAlignment.Fill   // full width
			// text: _item.title + "-" + _item.itemLabel + "-" + _item.description
			text: _item.description
		    textStyle {
		        base: SystemDefaults.TextStyles.BodyText
		        color: Color.create("#00ff00")
		        lineHeight: 1.1
		    }
		}
	
	    Container {
	        id: divider
	        preferredHeight: 1 // 1 pixel in height
	        background: Color.create("#262626") // divider colour
	
            verticalAlignment: VerticalAlignment.Bottom // docks at bottom of line row
            horizontalAlignment: HorizontalAlignment.Fill // full width 
	    }
	
	}
}