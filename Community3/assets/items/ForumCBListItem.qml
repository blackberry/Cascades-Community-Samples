/* Copyright (c) 2014 BlackBerry Ltd.
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
 * 
 * The Lithium REST APIs are owned by Lithium Technologies, Inc. 
 * (http://www.lithium.com) and are not part of the “Works” licensed 
 * hereunder pursuant to the Apache 2.0 license.
 */

import bb.cascades 1.2

//CustomListItem for displaying Categories and Boards.

CustomListItem {
    id: cbListItem
    preferredHeight: 175 
    dividerVisible: true
    highlightAppearance: HighlightAppearance.Full
    Container {
	    
	    Label {
	        text: ListItemData.short_title[".data"]
	        multiline: true
	        autoSize.maxLineCount: 2
            textStyle {
	            base: SystemDefaults.TextStyles.TitleText
	            fontWeight: FontWeight.Bold
	            color: Color.DarkBlue
	        }
            
	    }
	    Label {
	        text: ListItemData.description[".data"]
	        multiline: true
	        autoSize.maxLineCount: 4
            textStyle {
	            base: SystemDefaults.TextStyles.SmallText
	            color: Color.DarkGray
	        }
	    }
	}
	
}
