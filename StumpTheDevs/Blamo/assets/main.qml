import bb.cascades 1.0

Page {
    content: Container {
        id: rootcontainerlayout
        background: Color.create(0.2, 0.2, 0.2)
        onCreationCompleted: {
            OrientationSupport.supportedDisplayOrientation = SupportedDisplayOrientation.All;
        }
        attachedObjects: [
            OrientationHandler {
                onUiOrientationChanging: {
                    if (uiOrientation == UiOrientation.Landscape) {
                        orientationtext.text = " Orientation: landscape"
                        layouts.layoutDirection = LayoutDirection.LeftToRight
                    } else {
                        orientationtext.text = " Orientation: portrait"
                        layouts.layoutDirection = LayoutDirection.TopToBottom
                    }
                }
            } // End of OrientationHandler
        ] // End of attachedObjects
        layout: StackLayout {
            id: layouts
            layoutDirection: LayoutDirection.TopToBottom
        }
        Container {
            id: blamobox
            background: Color.create(0.2, 0.2, 0.2)
            preferredWidth: 768.0
            preferredHeight: 800.0
            layout: DockLayout {
            }
            Label {
                text: "MAKE"
                rotationZ: -90.0
                translationY: 85.0
                translationX: -20.0
                textStyle {
                    size: 40
                    fontWeight: FontWeight.Bold
                    color: Color.White
                }
            }
            Label {
                text: "THIS!"
                translationX: 65.0
                translationY: 5.0
                textStyle {
                    size: 40
                    fontWeight: FontWeight.Bold
                    color: Color.White
                }
            }
            Container {
                background: Color.create(0.7, 0.1, 0.1)
                preferredWidth: 650.0
                preferredHeight: 700.0
                translationY: 50.0
                layout: DockLayout {
                }
                layoutProperties: DockLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Container {
                    rotationZ: -45.0
                    translationY: 80.0
                    translationX: 20.0
                    Button {
                        id: buttonID
                        text: "Blamo!"
                        onClicked: {
                        }
                        preferredWidth: 250.0
                    }
                }
                Container {
                    rotationZ: -45.0
                    preferredWidth: 300.0
                    translationY: 465.0
                    translationX: 425.0
                    Label {
                        text: "New Design:"
                        textStyle {
                            color: Color.White
                            size: 25
                        }
                    }
                    Label {
                        text: "Diagonal ListView"
                        textStyle {
                            color: Color.White
                            size: 25
                        }
                    }
                    Label {
                        text: "Orientation Aware!"
                        textStyle {
                            color: Color.White
                            size: 25
                        }
                    }
                }
                Container {
                    background: Color.White
                    preferredWidth: 400.0
                    preferredHeight: 700.0
                    layoutProperties: DockLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    translationY: -350.0
                    translationX: 350.0
                    rotationZ: 45.0
                }
                Container {
                    background: Color.White
                    preferredWidth: 400.0
                    preferredHeight: 700.0
                    layoutProperties: DockLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    translationY: 350.0
                    rotationZ: 45.0
                    translationX: -350.0
                }
                Container {
                    background: Color.White
                    preferredWidth: 400.0
                    layoutProperties: DockLayoutProperties {
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                    rotationZ: 45.0
                    ListView {
                        dataModel: XmlDataModel {
                            source: "models/phones.xml"
                        }
                        listItemComponents: [
                            ListItemComponent {
                                type: "header"
                                Container {
                                    background: Color.White
                                    preferredWidth: maxWidth
                                    rotationZ: -45.0
                                    topMargin: 50.0
                                    Label {
                                        text: ""
                                        textStyle {
                                            base: SystemDefaults.TextStyles.BodyText
                                            size: 20
                                            fontWeight: FontWeight.Bold
                                            color: Color.Black
                                        }
                                    }
                                }
                            },
                            ListItemComponent {
                                type: "item"
                                Container {
                                    background: Color.White
                                    preferredWidth: maxWidth
                                    rotationZ: -45.0
                                    layout: StackLayout {
                                        layoutDirection: LayoutDirection.LeftToRight
                                        topPadding: 50.0
                                    }
                                    Container {
                                        ImageView {
                                            id: imageViewID
                                            imageSource: ListItemData.thumbURL
                                            preferredWidth: 100
                                            preferredHeight: 100
                                            scalingMethod: ScalingMethod.AspectFill
                                        }
                                    }
                                    Container {
                                        Label {
                                            text: ListItemData.title
                                            textStyle {
                                                base: SystemDefaults.TextStyles.BodyText
                                                size: 40
                                                fontWeight: FontWeight.Bold
                                                color: Color.Black
                                            }
                                        }
                                        Label {
                                            text: ListItemData.description
                                            textStyle {
                                                base: SystemDefaults.TextStyles.BodyText
                                                size: 20
                                                fontWeight: FontWeight.Regular
                                                color: Color.Black
                                            }
                                        }
                                        Divider {
                                        }
                                    }
                                }
                            }
                        ]
                        onSelectionChanged: {
                        }
                        rotationZ: 0.0
                    }
                }
            }
        }
        Container {
            id: bottombox
            Label {
                id: orientationtext
                text: ""
                textStyle {
                    size: 40
                    fontWeight: FontWeight.Bold
                    color: Color.White
                }
            }
            ImageView {
                id: imageBBJam
                imageSource: "asset:///images/banner-americas-300x100.jpg"
                preferredWidth: width
                preferredHeight: height
            }
        }
    }
}
