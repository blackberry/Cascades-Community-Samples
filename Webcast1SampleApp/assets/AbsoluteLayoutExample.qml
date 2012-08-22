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

Page {
    content: Container {
        //Define our AbsoluteLayout to allow precise placement of elements
        layout: AbsoluteLayout {
        }
        background: Color.create ("#87CEFA")
        LayoutBlock {
            id: block
            text: block.layoutProperties.positionX + "," + block.layoutProperties.positionY
            layoutProperties: AbsoluteLayoutProperties {
                //If the values in the TextFields ae not numbers we will default to 0
                //Note that this is common JavaScript being used
                positionX: isNaN (xCoord.text) ? 0 : xCoord.text
                positionY: isNaN (yCoord.text) ? 0 : yCoord.text
            }
        }
        
        //The below TextFields will allow user-defined values for the block's X and Y coordinates
        TextField {
            id: xCoord
            hintText: "X coordinate"
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 500
                positionY: 10
            }
        }
        TextField {
            id: yCoord
            hintText: "Y coordinate"
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 500
                positionY: 100
            }
        }
    }
}
