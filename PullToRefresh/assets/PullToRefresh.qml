import bb.cascades 1.0
import "../tart.js" as Tart

Container {
    signal refreshTriggered

    layout: StackLayout {}

    ImageView {
        id: imgRefreshIcon
        imageSource: "asset:///images/refreshicon.png"
        horizontalAlignment: HorizontalAlignment.Center
        topMargin: 25
    }

    Label {
        id: lblRefresh
        text: "Pull down to refresh results..."
        horizontalAlignment: HorizontalAlignment.Center
        topMargin: 25
        bottomMargin: 25
    }

    Divider {
        opacity: 0.0
    }

    attachedObjects: [
        LayoutUpdateHandler {
            id: refreshHandler

            onLayoutFrameChanged: {
                imgRefreshIcon.rotationZ = layoutFrame.y;

                if (layoutFrame.y >= 0.0 * layoutFrame.height) {
                    lblRefresh.text = "Release to refresh results..."

                    if (layoutFrame.y == 0) {
                        refreshTriggered();
                    }
                } else {
                    lblRefresh.text = "Pull down to refresh results..."
                }
            }
        }
    ]
}
