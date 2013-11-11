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
import com.msohm.WDServer 1.0

Page {
    Container {
        leftPadding: 10.0
        topPadding: 10.0
        rightPadding: 10.0
        bottomPadding: 10.0

        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill
        Label {
            id: statusLabel
            text:  "Operator Connected.  Move in the direction shown."
            autoSize.maxLineCount: 2
            multiline: true
            textStyle.fontSize: FontSize.Large
        }
        
        ImageView {
            id: commandImage
            visible: false
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

        }
        
    }
    
    onCreationCompleted: {
        wifiDirection.wdServer.connectionLost.connect(onConnectionLost);
        wifiDirection.wdServer.commandReceived.connect(onCommandReceived);
    }    
    
    function onCommandReceived(command)
    {
        if (command.search("STOP") == 0)
        {
            commandImage.visible = false;
        }
        else if (command.search("LEFT") == 0)
        {
            commandImage.visible = true;
            commandImage.imageSource = "asset:///images/left.png";
        }
        else if (command.search("UP") == 0)
        {
            commandImage.visible = true;
            commandImage.imageSource = "asset:///images/up.png";
        }
        else if (command.search("DOWN") == 0)
        {
            commandImage.visible = true;
            commandImage.imageSource = "asset:///images/down.png";
        }
        else if (command.search("RIGHT") == 0)
        {
            commandImage.visible = true;
            commandImage.imageSource = "asset:///images/right.png";
        }
        
    }
    
    function onConnectionLost()
    {
        statusLabel.text = "Lost connection to automatron.";
        
        //Image by boblet  Source: http://www.clker.com/clipart-disconnect.html
        commandImage.visible = true;
        commandImage.imageSource = "asset:///images/disconnected.png";
    }    
 
}
