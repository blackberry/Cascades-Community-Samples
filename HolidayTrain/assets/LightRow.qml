/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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

Container {
    topPadding: 25
    id: container

    property string light: "a"
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }

    signal command(string command)

    property int lastVal: -1

    function sendLightValue(val) {
        if (val != lastVal) {
            command("lv" + light + val);
        }
    }
    Container {
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        ImageView {
            imageSource: "asset:///images/" + container.light + ".JPG"
        }
    }
    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Container {
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                    layout: DockLayout {
                    }
                    ImageButton {
                        id: solid
                        defaultImageSource: "asset:///images/solidA.png"
                        disabledImageSource: "asset:///images/solidB.png"
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        onClicked: {
                            command("lm" + light + "0");
                            twinkle.enabled = true;
                            solid.enabled = false;
                        }
                        attachedObjects: [
                            LayoutUpdateHandler {
                                onLayoutFrameChanged: {
                                    solid.preferredWidth = layoutFrame.height;
                                }
                            }
                        ]
                    }
                }
                Container {
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                    layout: DockLayout {

                    }
                    ImageButton {
                        id: twinkle
                        defaultImageSource: "asset:///images/twinkleA.png"
                        disabledImageSource: "asset:///images/twinkleB.png"
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        onClicked: {
                            command("lm" + light + "1");
                            twinkle.enabled = false;
                            solid.enabled = true;
                        }
                        attachedObjects: [
                            LayoutUpdateHandler {
                                onLayoutFrameChanged: {
                                    twinkle.preferredWidth = layoutFrame.height;
                                }
                            }
                        ]

                    }
                }
            }
        }
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            layout: DockLayout {
                
            }
            Slider {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                toValue: 127.0
                value: 64.0
                onImmediateValueChanged: {
                    sendLightValue(Math.floor(immediateValue));
                }
                onValueChanged: {
                    sendLightValue(Math.floor(value));
                }
            }
        }
        layoutProperties: StackLayoutProperties {
            spaceQuota: 2
        }
    }

    attachedObjects: [
        LayoutUpdateHandler {
            onLayoutFrameChanged: {
                container.preferredHeight = layoutFrame.width / 3;
            }
        }
    ]

}
