import bb.cascades 1.4

Page {
    titleBar: TitleBar {
        title: "Swipe Example"
    }
    Container {
        property int downX: 0
        property int downY: 0
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        topPadding: 20
        leftPadding: 20
        rightPadding: 20
        onTouch: {
            if (event.isDown()) {
                downX = event.windowX;
                downY = event.windowY;
            } else if (event.isUp()) {
                var yDiff = downY - event.windowY;
                // Take absolute value of yDiff
                if (yDiff < 0) yDiff = -1 * yDiff;
                //If the y movement is less than 200: don't move left or right
                if ((yDiff) < 200) {
                    if ((downX - event.windowX) > 320) {
                        // move right
                        console.log("RIGHT!")
                        segmentedControl.selectedOption = control1
                    } else if ((event.windowX - downX) > 320) {
                        // move left
                        console.log("LEFT!")
                        segmentedControl.selectedOption = control2
                    }
                }
            }
        }
        ScrollView {
            id: scrollView
            scrollViewProperties.overScrollEffectMode: OverScrollEffectMode.None
            scrollRole: ScrollRole.Main
            property int startX
            property int stopX
            property int minimumDistance: ui.du(36)
            scrollViewProperties {
                scrollMode: ScrollMode.Both
                pinchToZoomEnabled: true
            }
            eventHandlers: [
                TouchKeyboardHandler {
                    onTouch: {
                        if (event.touchType == TouchType.Down) {
                            scrollView.startX = event.screenX
                            return
                        }
                        if (event.touchType == TouchType.Up) {
                            scrollView.stopX = event.screenX
                            var distance = scrollView.stopX - scrollView.startX
                            if (distance > scrollView.minimumDistance) {
                                console.log("Swiped right " + distance)
                                segmentedControl.selectedOption = control1
                            } else {
                                if (distance < (0 - scrollView.minimumDistance)) {
                                    console.log("Swiped left " + distance)
                                    segmentedControl.selectedOption = control2
                                } else {
                                    console.log("Distance too short...")
                                }
                            }
                            return
                        }
                    }
                }
            ]
            Container {
                SegmentedControl {
                    id: segmentedControl
                    options: [
                        Option {
                            id: control1
                            text: "One"
                        },
                        Option {
                            id: control2
                            text: "Two"
                        }
                    ]
                    onSelectedOptionChanged: {
                        if (selectedOption == control1) {
                            container1.visible = true
                            container2.visible = false
                        } else if (selectedOption == control2) {
                            container1.visible = false
                            container2.visible = true
                        }
                    }
                }
                Container {
                    id: container1
                    Label {
                        text: "Container1"
                        textStyle.base: SystemDefaults.TextStyles.BigText
                    }
                }
                Container {
                    id: container2
                    visible: false
                    Label {
                        text: "Container2"
                        textStyle.base: SystemDefaults.TextStyles.BigText
                    }
                }
            }
        }
    }
}