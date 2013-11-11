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
import com.msohm.WDClient 1.0

Page {
    Container {
        leftPadding: 10.0
        topPadding: 10.0
        rightPadding: 10.0
        bottomPadding: 10.0

        layout: StackLayout {

        }
        horizontalAlignment: HorizontalAlignment.Fill

        Label {
            id: statusLabel
            text: "Press and hold an arrow to move the automatron."
            autoSize.maxLineCount: 2
            multiline: true
            textStyle.fontSize: FontSize.Large
        }

        ImageButton {
            id: upButton
            // Image by OCAL Source http://www.clker.com/clipart-3588.html
            defaultImageSource: "asset:///images/up.png"
            horizontalAlignment: HorizontalAlignment.Center
            pressedImageSource: "asset:///images/upPress.png"
            
            onTouch: {
                if (event.touchType == TouchType.Down)
                {
                    wifiDirection.wdClient.sendCommand("UP\r\n");
                }
                else if (event.touchType == TouchType.Up)
                {
                    wifiDirection.wdClient.sendCommand("STOP\r\n");
                }
            }

        }
        Container {
            layout: DockLayout {

            }
            horizontalAlignment: HorizontalAlignment.Fill
            ImageButton {
                id: leftButton
                //Image by OCAL Source: http://www.clker.com/clipart-3590.html
                defaultImageSource: "asset:///images/left.png"
                horizontalAlignment: HorizontalAlignment.Left
                pressedImageSource: "asset:///images/leftPress.png"
                
                onTouch: {
                    if (event.touchType == TouchType.Down)
                    {
                        wifiDirection.wdClient.sendCommand("LEFT\r\n");
                    }
                    else if (event.touchType == TouchType.Up)
                    {
                        wifiDirection.wdClient.sendCommand("STOP\r\n");
                    }
                }                
            }
            ImageButton {
                id:rightButton
                //Image by OCAL Source: http://www.clker.com/clipart-3589.html
                defaultImageSource: "asset:///images/right.png"
                horizontalAlignment: HorizontalAlignment.Right
                pressedImageSource: "asset:///images/rightPress.png"
                
                onTouch: {
                    if (event.touchType == TouchType.Down)
                    {
                        wifiDirection.wdClient.sendCommand("RIGHT\r\n");
                    }
                    else if (event.touchType == TouchType.Up)
                    {
                        wifiDirection.wdClient.sendCommand("STOP\r\n");
                    }
                }

            }

        }

        ImageButton {
            id: downButton
            //Image by OCAL Source: http://www.clker.com/clipart-3587.html
            defaultImageSource: "asset:///images/down.png"
            horizontalAlignment: HorizontalAlignment.Center
            pressedImageSource: "asset:///images/downPress.png"
            
            onTouch: {
                if (event.touchType == TouchType.Down)
                {
                    wifiDirection.wdClient.sendCommand("DOWN\r\n");
                }
                else if (event.touchType == TouchType.Up)
                {
                    wifiDirection.wdClient.sendCommand("STOP\r\n");
                }
            }

        }
    }
    
    onCreationCompleted: {
        //wdClient = wifiDirection.wdClient;
        wifiDirection.wdClient.connectionLost.connect(onConnectionLost);
    }
    
    function onConnectionLost()
    {
        upButton.defaultImageSource = "asset:///images/disconnected.png";
        leftButton.enabled = false;
        rightButton.enabled = false;
        downButton.enabled = false;
        statusLabel.text = "Lost connection to automatron."
    }

}
