import bb.cascades 1.0
import "../tart.js" as Tart

ListItemComponent {
    Container {
        property int spacing: 25

        topPadding: spacing
        leftPadding: spacing
        rightPadding: spacing

        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            background: Color.create("#404040");
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            minHeight: 150

            property int border: 1

            topPadding: border
            bottomPadding: border
            leftPadding: border
            rightPadding: border

            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                background: Color.create("#e2e2e2");
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                minHeight: 150

                property int padding: 5

                topPadding: padding
                bottomPadding: padding
                leftPadding: padding
                rightPadding: padding

                Container {
                    layout: DockLayout {}
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 2.0
                    }

                    ImageView {
                        imageSource: ListItemData.user.avatar_image.url
                        preferredWidth: 200
                        preferredHeight: 200
                        scalingMethod: ScalingMethod.AspectFit
                        loadEffect: ImageViewLoadEffect.Subtle
                    }
                }

                Container {
                    layout: StackLayout {}
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 8.0
                    }

                    leftMargin: 25

                    Label {
                        text: ListItemData.user.username
                        horizontalAlignment: HorizontalAlignment.Left

                        textStyle {
                            fontWeight: FontWeight.Bold
                        }
                    }

                    Label {
                        text: "<html>" + ListItemData.html + "</html>"
                        multiline: true
                        horizontalAlignment: HorizontalAlignment.Left
                    }

                    Container {
                        layout: DockLayout {}
                        horizontalAlignment: HorizontalAlignment.Right

                        Label {
                            text: ListItemData.created_at
                            horizontalAlignment: HorizontalAlignment.Right
                            verticalAlignment: VerticalAlignment.Bottom
                            textStyle {
                                textAlign: TextAlign.Right
                            }
                        }
                    }
                }
            }
        }
    }
}
