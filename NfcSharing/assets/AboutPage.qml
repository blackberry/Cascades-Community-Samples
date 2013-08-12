import bb.cascades 1.0

Page {
    id: aboutPage
    Container {
        preferredWidth: 768
        preferredHeight: 1280
        background: Color.create("#262626")
        layout: StackLayout {
        }
        Container {
            topPadding: 20
            leftPadding: 20
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Top
            Container {
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                Label {
                    text: " Welcome to NFC Sharing V1.1.1"
                    textStyle {
                        base: SystemDefaults.TextStyles.TitleText
                        fontWeight: FontWeight.W100
                        color: Color.White
                    }
                }
                Label {
                    text: "Demonstrates the BlackBerry 10 NfcShareManager API"
                    multiline: true
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontStyle: FontStyle.Italic
                        color: Color.White
                        lineHeight: 1.1
                    }
                }
            }
            Container {
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Center
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Container {
                    leftPadding: 20
                    ImageView {
                        imageSource: "asset:///images/jcmrim_head.png"
                        onTouch: {
                            if (event.isDown()) {
                                animation1.play()
                            }
                        }
                        attachedObjects: [
                            SequentialAnimation {
                                id: animation1
                                animations: [
                                    RotateTransition {
                                        fromAngleZ: 0
                                        toAngleZ: 360
                                        duration: 500
                                    }
                                ]
                            }
                        ]
                    }
                    Label {
                        text: "@jcmrim"
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
                    horizontalAlignment: HorizontalAlignment.Center
                    ImageView {
                        leftMargin: 40
                        layoutProperties: StackLayoutProperties {
                        }
                        imageSource: "asset:///images/mdwrim_head.png"
                        onTouch: {
                            if (event.isDown()) {
                                animation2.play()
                            }
                        }
                        attachedObjects: [
                            SequentialAnimation {
                                id: animation2
                                animations: [
                                    RotateTransition {
                                        fromAngleZ: 0
                                        toAngleZ: 360
                                        duration: 250
                                    }
                                ]
                            }
                        ]
                    }
                    Label {
                        text: "@mdwrim"
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
                    horizontalAlignment: HorizontalAlignment.Center
                    ImageView {
                        leftMargin: 40
                        layoutProperties: StackLayoutProperties {
                        }
                        imageSource: "asset:///images/robbieDubya_head.png"
                        onTouch: {
                            if (event.isDown()) {
                                animation3.play()
                            }
                        }
                        attachedObjects: [
                            SequentialAnimation {
                                id: animation3
                                animations: [
                                    RotateTransition {
                                        fromAngleZ: 0
                                        toAngleZ: 360
                                        duration: 750
                                    }
                                ]
                            }
                        ]
                    }
                    Label {
                        text: "@robbieDubya"
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
            Container {
                layout: StackLayout {
                }
                leftPadding: 20
                bottomPadding: 20
                horizontalAlignment: HorizontalAlignment.Left
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                Label {
                    text: "Written by:"
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontWeight: FontWeight.Bold
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
                Label {
                    text: "Photographs by:"
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontWeight: FontWeight.Bold
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
                Label {
                    text: "About:"
                    enabled: false
                    preferredWidth: 650
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontWeight: FontWeight.Bold
                        fontStyle: FontStyle.Default
                        color: Color.White
                    }
                }
                Label {
                    text: "NfcSharing V1.1.0"
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
