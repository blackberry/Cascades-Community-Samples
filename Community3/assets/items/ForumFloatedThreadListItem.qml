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


//CustomListItem for displaying Floated Threads.

CustomListItem {
    preferredHeight: 265 
    dividerVisible: true
    highlightAppearance: HighlightAppearance.Frame
    Container {

        Label {
            text: ListItemData.target.messages.topic.subject[".data"]
            multiline: true
            autoSize.maxLineCount: 2
            textStyle.color: Color.DarkBlue
            textStyle.base: SystemDefaults.TextStyles.TitleText
            textFormat: TextFormat.Html
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight

            }

            horizontalAlignment: HorizontalAlignment.Fill
            Container {
                Label {
                    text: qsTr("Author: ")
                    textStyle.color: Color.DarkGray
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
                Label {
                    text: qsTr("Posted: ")
                    textStyle.color: Color.DarkGray
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
            }
            Container {
                Label {
                    text: ListItemData.target.messages.topic.author.login[".data"]
                    textStyle.color: Color.DarkGreen
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
                Label {
                    text: formatDate(ListItemData.target.messages.topic.post_time[".data"])
                    textStyle.color: Color.DarkGreen
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
            }

            Container {
                layout: DockLayout {

                }

                horizontalAlignment: HorizontalAlignment.Fill
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                ImageView {
                    //Image by Stephen Hutchings.  Source: https://www.iconfinder.com/icons/216358/outline_pin_icon
                    imageSource: "asset:///images/pin.png"
                    horizontalAlignment: HorizontalAlignment.Right

                }
            }

        }

    }
    
    function formatDate(dateString){
        var day, tz,
        rx=/^(\d{4}\-\d\d\-\d\d([tT][\d:\.]*)?)([zZ]|([+\-])(\d\d):(\d\d))?$/,
        p= rx.exec(dateString) || [];
        if(p[1]){
            day= p[1].split(/\D/);
            for(var i= 0, L= day.length; i<L; i++){
                day[i]= parseInt(day[i], 10) || 0;
            }
            day[1]-= 1;
            day= new Date(Date.UTC.apply(Date, day));
            if(!day.getDate()) return "";
            if(p[5]){
                tz= (parseInt(p[5], 10)*60);
                if(p[6]) tz+= parseInt(p[6], 10);
                if(p[4]== '+') tz*= -1;
                if(tz) day.setUTCMinutes(day.getUTCMinutes()+ tz);
            }
            
            return Qt.formatDateTime(day,"dddd MMMM d, yyyy h:mm AP");
        }
        return "";
    }    
    
}
