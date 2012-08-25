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
import Network.ExternalIP 1.0
import "controls"
import my.library 1.0

/**
*  This is a Page that displays your external IP address
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
                id : ipaddress
                visible : false
                textStyle { 
                    base: SystemDefaults.TextStyles.BigText; 
                    color: Color.White
                    fontWeight: FontWeight.Bold 
                    }           
                text : "Retrieving IP address"
            }
        }
        attachedObjects: [
            QTimer {
                id: timer
                interval: 1000
                onTimeout: {
                    // a second after page loads make the http get request
                    netip.getIP();
                }
            },
            ExternalIP {
                id : netip
                onComplete :{
                    // stop progress indicator and hide it
                    progressIndicator.stopIndicator();
                    progressIndicator.visible = false;
                    
                    // display external IP address
                    ipaddress.text = info;
                    ipaddress.visible = true;
                    timer.stop();
                }
            }
        ]
        onCreationCompleted: {
            //start progress indicator and timer
            progressIndicator.setTitle("Retrieving IP address");
            progressIndicator.startIndicator();
            timer.start();
        }
    }
}