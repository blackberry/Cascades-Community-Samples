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
 */

import bb.cascades 1.2
import com.msohm.MessageWorker 1.0
import bb.system 1.0
import com.msohm.URLProvider 1.0

//CustomListItem for displaying Threads.

CustomListItem {
    id: forumThreadItem
    preferredHeight: 5000 //Set to a large number which should accomodate most posts.  It's shrunk after the post content size is calculated.
    preferredWidth: 5000 //Large number so wide posts fit in the WebView.
    dividerVisible: true
    highlightAppearance: HighlightAppearance.Frame
    Container {
        
        Label {
            text: ListItemData.subject[".data"]
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
            Container {
                Label {
                    text: qsTr("Author: ")
                    textStyle.color: Color.DarkGray
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
                Label {
                    text: qsTr("Last Update: ")
                    textStyle.color: Color.DarkGray
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
                Label {
                    text: qsTr("Likes: ")
                    textStyle.color: Color.DarkGray
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
            }
            Container {
                Label {
                    text: ListItemData.author.login[".data"]
                    textStyle.color: Color.DarkGreen
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
                Label {
                    text: formatDate(ListItemData.last_edit_time[".data"])
                    textStyle.color: Color.DarkGreen
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
                Label {
                    text: ListItemData.kudos.count[".data"]
                    textStyle.color: Color.DarkGreen
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                }
            }
        }
        ScrollView {
            id: scrollView

            // We let the scroll view scroll in both x and y and enable zooming,
            // max and min content zoom property is set in the WebViews onMinContentScaleChanged
            // and onMaxContentScaleChanged signal handlers.
            scrollViewProperties {
                //scrollMode: ScrollMode.Vertical
                scrollMode: ScrollMode.Both
                pinchToZoomEnabled: true
                
            }
            
            WebView {
                id: bodyWebView
                //This allows for horizontal scrolling with the content zoomed to the desired level.
                html: "<html><head><script language=\"javascript\" type=\"text/javascript\"> function getHeight() { var theHeight = document.getElementById(\"heightProvider\").offsetHeight; return theHeight;} </script></head><body><div id=\"heightProvider\">" + ListItemData.body[".data"] + "</div></body></html>"
                settings.zoomToFitEnabled: false
                settings.background: Color.Transparent
                settings.viewport: { "initial-scale" : 1.0, "width" : "device-width"}

                onNavigationRequested: {
                    //Open links in the browser.  Opening in the little WebView is a poor experience.
                    //Only allow other, which is us setting the HTML content.
                    if (WebNavigationType.Other == request.navigationType) {
                        request.action = WebNavigationRequestAction.Accept;
                    } else {
                        request.action = WebNavigationRequestAction.Ignore;
                        linkInvocation.query.uri = request.url;
                    }
                }
                
                //Get the height of the forum content after it's loaded.
                onLoadingChanged: {
                    if (loadRequest.status == WebLoadStatus.Succeeded) {
                        bodyWebView.evaluateJavaScript("getHeight();", JavaScriptWorld.Normal);
                    }
                }
                
                //Reset the CustomListItem's height based on the post content.
                //Take ScrollView scale into consideration so we don't have extra white space.
                //ScrollView will zoom out for wide content.
                onJavaScriptResult: {
                    forumThreadItem.preferredHeight = 380 + (result * bodyWebView.settings.devicePixelRatio);
                }
                settings.webInspectorEnabled: false

            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            horizontalAlignment: HorizontalAlignment.Right
            
            ImageButton {
                defaultImageSource: "asset:///images/ic_reply.png"
                accessibility.name: qsTr("Reply")
                
                onClicked: {
                    doReply();
                }
            }
            
            ImageButton {
                //Image by Designerz Base: Source https://www.iconfinder.com/icons/186399/like%2Cthumbs%2Cthumbs_up_icon#size=256
                defaultImageSource: "asset:///images/Like.png"
                accessibility.name: qsTr("Like")
                
                onClicked: {
                    doLike();
                }
            }
        }
    }
    
    contextActions: [
        ActionSet {
            
            title: qsTr("BlackBerry Forums")
            
            ActionItem {
                title: qsTr("Reply")
                imageSource: "asset:///images/ic_reply.png"
                onTriggered: {
                    doReply();
                }
            }
            
            ActionItem {
                id: likeAction
                title: qsTr("Like")
                //Image by Designerz Base: Source https://www.iconfinder.com/icons/186399/like%2Cthumbs%2Cthumbs_up_icon#size=256
                imageSource: "asset:///images/Like.png"
                onTriggered: {
                    doLike();
                }
            }
            
            InvokeActionItem {
                id: shareThread
                query {
                    uri: getThreadURL()
                }
            }
        }
    ]
    
    function getThreadURL() {
        //Get the last element in the href.
        var boardHref = ListItemData.board.href;
        boardHref = boardHref.split("/").pop();
        
        //Get the last element in the href.
        var threadHref = ListItemData.root.href;
        threadHref = threadHref.split("/").pop();
        
        return urlProvider.getForumWebURL() + boardHref + "/" + threadHref + "/m-p/" + threadHref + "#M" + ListItemData.board_id[".data"];
    
    }
    
    attachedObjects: [
        ComponentDefinition {
            id: composeSheet
            source: "./../composeMessageSheet.qml"
        },
        Invocation {
            id: linkInvocation
            property bool auto_trigger: false
            query {
                uri: "http://developer.blackberry.com"
                
                onUriChanged: {
                    linkInvocation.query.updateQuery();
                }
            }
            onArmed: {
                // don't auto-trigger on initial setup
                if (auto_trigger) {
                    trigger("bb.action.OPEN");
                }
                auto_trigger = true; // allow re-arming to auto-trigger
            }
        },
        MessageWorker {
            id: messageWorker
        },
        SystemToast {
            id: kudosErrorToast
        },
        URLProvider {
            id: urlProvider
        }
    ]
    
    onCreationCompleted: {
        messageWorker.postComplete.connect(onKudosComplete);
    }
    
    signal kudosAdded()
    
    function onKudosComplete(success, error) {
        if (success) {
            forumThreadItem.ListItem.view.onRefresh();
        } else {
            kudosErrorToast.body = qsTr("Error: ") + error;
            kudosErrorToast.show();
        }
    }
    
    function doReply()
    {
        var compose = composeSheet.createObject();
        compose.subjectPrePop = ListItemData.subject[".data"];
        compose.messageId = ListItemData.id[".data"];
        compose.replyAuthor =  ListItemData.author.login[".data"];
        compose.replyBody = bodyWebView.html;
        compose.postComplete.connect(forumThreadItem.ListItem.view.onRefresh);
        compose.open();
    }
    
    function doLike()
    {
        messageWorker.kudosMessage(ListItemData.id[".data"], ListItemData.author["href"]);
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