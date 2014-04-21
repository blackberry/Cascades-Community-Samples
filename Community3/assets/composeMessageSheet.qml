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
import com.msohm.MessageWorker 1.0
import bb.system 1.0

Sheet {
    property string subjectPrePop
    property string messageId
    property string boardId
    property string replyBody
    property string replyAuthor
    id: composeSheet
	    Page {
	        
	        titleBar: TitleBar {
                title: qsTr("Post Message")
	
	            dismissAction: ActionItem {
                    title: qsTr("Cancel")
	                onTriggered: {
	                    composeSheet.close();
	                }
	            }
	
	            acceptAction: ActionItem {
                    title: qsTr("Post")
	                onTriggered: {
	                    if (subject.text.length == 0) {
	                        errorDialog.show();
	                    } else {
	                        postingLabel.visible = true;
	                        postingProgress.running = true;
	                        postingProgress.visible = true;
	                        if (messageId.length > 0) {
	                            messageWorker.postReply(messageId, subject.text, body.text);
	                        } else {
	                            messageWorker.postMessage(boardId, subject.text, body.text);
	                        }
	                    }
	                }
	            }
	        }
            ScrollView
            {
	        Container {
	            leftPadding: 10.0
	            topPadding: 10.0
	            rightPadding: 10.0
	            bottomPadding: 10.0
	
	            Label {
	                id: errorLabel
	                visible: false
	                multiline: true
	
	            }
	            Divider {
	            }
	            TextField {
	                id: subject
	                text: composeSheet.subjectPrePop
                    hintText: qsTr("Subject")
	                focusHighlightEnabled: false 
	            }
	            Divider {
	            }
	            TextArea {
	                id: body
	                input.submitKey: SubmitKey.None
                    hintText: qsTr("Message Body")
	                minHeight: 200
	                maximumLength: 10000
	                focusHighlightEnabled: false
	            }
	            Divider {
	                
	            }
	            Label {
	                id: authorLabel
	            	visible: false
                    text: qsTr("Replying To: ")
	                textStyle.color: Color.DarkGreen
	            }
	            Divider {
	                id: replyDivider                
	            	visible: false
	            }
	            Label {
	                id: replySubjectLabel
	                text: composeSheet.subjectPrePop
	                visible: false
	                multiline: true
	                autoSize.maxLineCount: 2
                    textFormat: TextFormat.Html
	            }
	            Divider {
	                id: replySubjectDivider                
	                visible: false
	            }            
	            
	
	            ScrollView {
	                id: scrollView
	                visible: false
	                
	                // We let the scroll view scroll in both x and y and enable zooming,
	                // max and min content zoom property is set in the WebViews onMinContentScaleChanged
	                // and onMaxContentScaleChanged signal handlers.
	                scrollViewProperties {
	                    scrollMode: ScrollMode.Vertical
	                    pinchToZoomEnabled: true
	                }
	                
	                WebView {
	                    id: bodyWebView
	                    //This allows for horizontal scrolling with the content zoomed to the desired level.
	                    html: composeSheet.replyBody
	                    settings.textAutosizingEnabled: false
	                    settings.minimumFontSize: 30
	                    settings.defaultFontSizeFollowsSystemFontSize: true
	                    settings.zoomToFitEnabled: true
                        settings.background: Color.Transparent
	                    
	                    onMinContentScaleChanged: {
	                        // Update the scroll view properties to match the content scale
	                        // given by the WebView.
	                        scrollView.scrollViewProperties.minContentScale = minContentScale;
	                        
	                        // Let's show the entire page to start with.
	                        scrollView.zoomToPoint(0, 0, minContentScale, ScrollAnimation.None);
	                    
	                    }
	                    
	                    onMaxContentScaleChanged: {
	                        // Update the scroll view properties to match the content scale
	                        // given by the WebView.
	                        scrollView.scrollViewProperties.maxContentScale = maxContentScale;
	                    
	                    }
	                    
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
	
	                    settings.webInspectorEnabled: false
	                
	                }
	            }
	            Divider {
	                id: replyBodyDivider                
	                visible: false
	            }
	            
	            
	            
	            Label {
	                id: postingLabel
	                visible: false
	                horizontalAlignment: HorizontalAlignment.Center
                    text: qsTr("Posting message...")
	            }
	            ActivityIndicator {
	                id: postingProgress
	                visible: false
	                running: false 
	                horizontalAlignment: HorizontalAlignment.Center
	                preferredHeight: 120 
	                preferredWidth: 120
	            }
	        }
    }
    }

    attachedObjects: [

        MessageWorker {
            id: messageWorker
        },
        SystemDialog {
            id: errorDialog
            title: qsTr("Please Enter a Subject")
            body: qsTr("A subject is required. Message not posted.")

        },
        ComponentDefinition {
            id: settingsSheet
            source: "settingsSheet.qml"
        },
        ComponentDefinition {
            id: helpSheet
            source: "helpSheet.qml"
        }
    ]

    onCreationCompleted: {
        messageWorker.postComplete.connect(onPostComplete);
    }
    
    onOpened: {
        if (messageId.length > 0)
        {
            authorLabel.text = authorLabel.text + composeSheet.replyAuthor;
            replyDivider.visible = true;
            authorLabel.visible = true;
            replyBodyDivider.visible = true;
            scrollView.visible = true;
            replySubjectLabel.visible = true;
            replySubjectDivider.visible = true;
        }
    }

    signal postComplete()

    function onPostComplete(success, error) {
        postingLabel.visible = false;
        postingProgress.running = false;
        postingProgress.visible = false;
        
        if (success) {
            composeSheet.postComplete();
            composeSheet.close();
        } else {
            errorLabel.visible = true;
            errorLabel.text = error;
        }
    }
}