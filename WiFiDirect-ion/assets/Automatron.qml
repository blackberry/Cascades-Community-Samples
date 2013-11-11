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

import bb.cascades 1.2

Page {
    Container {
        leftPadding: 10.0
        topPadding: 10.0
        rightPadding: 10.0
        bottomPadding: 10.0
        
        Label {
            id: statusLabel
            text:  "Searching for operators..."
            multiline: true
            autoSize.maxLineCount: 5
            textStyle.fontSize: FontSize.Large
        }
        
        ActivityIndicator {
            id: scanningIndicator
            running: true
            horizontalAlignment: HorizontalAlignment.Center
        }
        ListView {
            dataModel: wifiDirection.model
            
            listItemComponents: [
                ListItemComponent {
                    type: "item"
                    
                    StandardListItem {
                        title: ListItemData.Name
                        description: ListItemData.HardwareAddress
                    }
                }
            ]
            onTriggered: {
                //Initiate the connection to the operator.
                var chosenItem = dataModel.data(indexPath);
                wifiDirection.requestConnect(chosenItem.HardwareAddress);
            }
        }        
    }
    
    onCreationCompleted: {
        wifiDirection.scanStartResult.connect(onScanStart);
        wifiDirection.scanComplete.connect(onScanComplete);
        wifiDirection.connectionAttempt.connect(onConnectionAttempt);
        wifiDirection.devConnected.connect(onDeviceConnected);
        wifiDirection.startScanning();
    }
    
    function onScanStart(success, errorMessage)
    {
        if (!success)
        {
            scanningIndicator.running = false;
            scanningIndicator.visible = false;
            statusLabel.text = errorMessage;
        }
    }
    
    function onScanComplete()
    {
        scanningIndicator.running = false;
        scanningIndicator.visible = false;
        statusLabel.text = "Operator scan complete.  No operator found."

    }    
    
    function onConnectionAttempt(success, errorMessage)
    {
        if (!success)
        {
            statusLabel.text = errorMessage;
            scanningIndicator.running = false;
            scanningIndicator.visible = false;
        }
    }
    
    function onDeviceConnected()
    {
        wifiDirection.listenForOperator();
        var page = controlleePage.createObject();
        navPane.pop();
        navPane.push(page);
    }
    
    attachedObjects:[
        
        ComponentDefinition {
            id: controlleePage
            source: "Controlee.qml"
        }
    ] 
}
