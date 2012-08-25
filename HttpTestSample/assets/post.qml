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
import Network.PostHttp 1.0
import "controls"
import my.library 1.0

/**
*  This is a simple Page that allows you to enter HTML form values and POST them to a newtwork resource
*
*  You will learn how to:
*  -- Import C++ objects
*  -- Use attachedObjects
*  -- Use a QTimer for timing operations
**/
Page
{
    content: Container {
        background : Color.Gray
        layout : DockLayout {
        }
        Container {
            layout : StackLayout {
                layoutDirection : LayoutDirection.TopToBottom
            }
            layoutProperties : DockLayoutProperties {
                horizontalAlignment : HorizontalAlignment.Center
                verticalAlignment : VerticalAlignment.Center
            }
            NetworkActivity {
                id : progressIndicator
            }
            Label {
                id : postresponselabel
                visible : false
                text : "Response:"
            }
            TextArea {
                id : postresponsebody
                visible : false
                editable : false
                backgroundVisible : false
                textStyle { 
                    base: SystemDefaults.TextStyles.BodyText; 
                    color: Color.White
                    alignment : TextAlignment.Center
                    }           
            }
        }
        attachedObjects: [
            QTimer {
                id: timer
                interval: 1000
                onTimeout: {
                    netpost.post(httpsample.getPostBody());
                }
            },
            PostHttp {
                id : netpost
                onComplete :{
                    progressIndicator.stopIndicator();
                    progressIndicator.visible = false;
                    postresponsebody.text = info;
                    postresponsebody.visible = true;
                    postresponselabel.visible = true;
                    timer.stop();
                }
            }
        ]
        onCreationCompleted: {
            progressIndicator.setTitle("Posting..");
            progressIndicator.startIndicator();
            timer.start();
        }
    }
}