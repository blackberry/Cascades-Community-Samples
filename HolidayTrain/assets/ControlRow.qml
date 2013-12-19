import bb.cascades 1.2

Container {
    id: container
    signal command(string command)
    
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {

        }
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        ImageView {
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Center
            imageSource: "asset:///images/left.png"
        }
        rightMargin: 5
        leftMargin: 5
        gestureHandlers: [
            TapHandler {
                onTapped: {
                    container.command(1);
                }
            }
        ]
    }
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {
        
        }
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        background: Color.Red
        Label {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            text: "Stop"
        }
        rightMargin: 5
        leftMargin: 5
        gestureHandlers: [
            TapHandler {
                onTapped: {
                    container.command(0);
                }
            }
        ]
    }
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        layout: DockLayout {
        
        }
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1
        }
        ImageView {
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Center
            imageSource: "asset:///images/right.png"
        }
        rightMargin: 5
        leftMargin: 5
        gestureHandlers: [
            TapHandler {
                onTapped: {
                    container.command(-1);
                }
            }
        ]
    }
    attachedObjects: [
        LayoutUpdateHandler {
            onLayoutFrameChanged: {
                container.preferredHeight = layoutFrame.width/3*1.25;
            }
        }
    ]
}
