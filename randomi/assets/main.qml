/*
 * Copyright (c) 2014 BlackBerry Limited.
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

import bb.cascades 1.4
/*
 * RandomI
 * 
 * This sample demonstrates how you can override the default navigation behaviour of Cascades controls.
 * 
 * Scrolling right or down will progress in alphabetical order, left or up will do the reverse.
 * 
 * Use this technique to give your custom layouts a logical scrolling order if the default feels wrong.
 * 
 */
Page {

    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }

            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            ImageButton {
                id: letterC
                defaultImageSource: "asset:///Images/C.png"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                navigation {
                    leftAllowed: true
                    rightAllowed: true
                    upAllowed: true
                    downAllowed: true
                    left: letterB
                    right: letterD
                    up: letterB
                    down: letterD
                }
            }
            ImageButton {
                id: letterG
                defaultImageSource: "asset:///Images/G.png"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                navigation {
                    leftAllowed: true
                    rightAllowed: true
                    upAllowed: true
                    downAllowed: true
                    left: letterF
                    right: letterH
                    up: letterF
                    down: letterH
                }
            }
            ImageButton {
                id: letterD
                defaultImageSource: "asset:///Images/D.png"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                navigation {
                    leftAllowed: true
                    rightAllowed: true
                    upAllowed: true
                    downAllowed: true
                    left: letterC
                    right: letterE
                    up: letterC
                    down: letterE
                }
            }
        }
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            margin.topOffset: 10
            margin.bottomOffset: 10

            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageButton {
                id: letterE
                defaultImageSource: "asset:///Images/E.png"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                navigation {
                    leftAllowed: true
                    rightAllowed: true
                    upAllowed: true
                    downAllowed: true
                    left: letterD
                    right: letterF
                    up: letterD
                    down: letterF
                }
            }
            ImageButton {
                id: letterI
                defaultImageSource: "asset:///Images/I.png"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                navigation {
                    leftAllowed: true
                    rightAllowed: true
                    upAllowed: true
                    downAllowed: true
                    left: letterH
                    right: letterA
                    up: letterH
                    down: letterA
                }
            }
            ImageButton {
                id: letterB
                defaultImageSource: "asset:///Images/B.png"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                navigation {
                    leftAllowed: true
                    rightAllowed: true
                    upAllowed: true
                    downAllowed: true
                    left: letterA
                    right: letterC
                    up: letterA
                    down: letterC
                }
            }
        }
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }

            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageButton {
                id: letterH
                defaultImageSource: "asset:///Images/H.png"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                navigation {
                    leftAllowed: true
                    rightAllowed: true
                    upAllowed: true
                    downAllowed: true
                    left: letterG
                    right: letterI
                    up: letterG
                    down: letterI
                }
            }
            ImageButton {
                id: letterA
                defaultImageSource: "asset:///Images/A.png"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                navigation {
                    leftAllowed: true
                    rightAllowed: true
                    upAllowed: true
                    downAllowed: true
                    left: letterI
                    right: letterB
                    up: letterI
                    down: letterB
                }
            }
            ImageButton {
                id: letterF
                defaultImageSource: "asset:///Images/F.png"
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                navigation {
                    leftAllowed: true
                    rightAllowed: true
                    upAllowed: true
                    downAllowed: true
                    left: letterE
                    right: letterG
                    up: letterE
                    down: letterG
                }
            }
        }
    }
}
