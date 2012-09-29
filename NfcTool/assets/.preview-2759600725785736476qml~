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
import "common"

Page {
    content: MenuContainer {
        onCreationCompleted: {
            animation.play ();
        }
        Container {
            preferredWidth: 768
            preferredHeight: 1280
            
            layout: DockLayout {
            }

            Container {
                layout: StackLayout {
                }

                topPadding: 20
                leftPadding: 20
                horizontalAlignment: HorizontalAlignment.Left
                verticalAlignment: VerticalAlignment.Top

                Label {
                    text: " Welcome to NFC Tool V" + _about.appVersion
                    textStyle {
                        base: SystemDefaults.TextStyles.TitleText
                        fontWeight: FontWeight.W100
                        color: Color.White
                    }
                }

                Label {
                    text: "Demonstrates the BlackBerry 10 NFC APIs"
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        fontWeight: FontWeight.W100
                        fontStyle: FontStyle.Italic
                        color: Color.White
                        lineHeight: 1.1
                    }
                }

                ImageView {
	                layoutProperties: StackLayoutProperties {
	                }
                    imageSource: "asset:///images/generic_tag_larger.png"
                    onTouch: {
                        if (event.isDown ()) {
                            animation.play ()
                        }
                    }
                    attachedObjects: [
                        SequentialAnimation {
                            id: animation
                            animations: [
                                FadeTransition {
                                    duration: 1000
                                    fromOpacity: 0.0
                                    toOpacity: 1.0
                                },
                                RotateTransition {
                                    toAngleZ: -10
                                    duration: 125
                                },
                                RotateTransition {
                                    toAngleZ: 10
                                    duration: 250
                                },
                                RotateTransition {
                                    toAngleZ: -10
                                    duration: 250
                                },
                                RotateTransition {
                                    toAngleZ: 10
                                    duration: 250
                                },
                                RotateTransition {
                                    toAngleZ: 0
                                    duration: 125
                                }
                            ]
                        }
                    ]
                }

                Label {
                    text: "Written by:"
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontWeight: FontWeight.W100
                        fontStyle: FontStyle.Default
                        color: Color.White
                    }
                }
                Label {
                    text: "John Murray (@jcmrim)"
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontWeight: FontWeight.W100
                        fontStyle: FontStyle.Default
                        color: Color.White
                    }
                }
                Label {
                    text: "Martin Woolley (@mdwrim)"
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontWeight: FontWeight.W100
                        fontStyle: FontStyle.Default
                        color: Color.White
                    }
                }
            }

            Container {
                layout: StackLayout {
                }
                leftPadding: 20
                bottomPadding: 20
                verticalAlignment: VerticalAlignment.Bottom
                horizontalAlignment: HorizontalAlignment.Left

                Label {
                    text: "NFC Tool V" + _about.appVersion
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontWeight: FontWeight.W100
                        fontStyle: FontStyle.Default
                        color: Color.White
                    }
                }
                Label {
                    text: "© 2012 Research In Motion Limited."
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontWeight: FontWeight.W100
                        fontStyle: FontStyle.Default
                        color: Color.White
                    }
                }
            }
        }
    }
}
