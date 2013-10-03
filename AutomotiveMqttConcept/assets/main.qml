import bb.cascades 1.2
import CustomTimer 1.0

Page {

    property variant digit: [ "asset:///images/orange0.png", "asset:///images/orange1.png", "asset:///images/orange2.png",
        "asset:///images/orange3.png", "asset:///images/orange4.png", "asset:///images/orange5.png",
        "asset:///images/orange6.png", "asset:///images/orange7.png", "asset:///images/orange8.png",
        "asset:///images/orange9.png" ]

    property variant dash: "asset:///images/orangeh.png"
    property variant blank: "asset:///images/orange0.png"
    property int p_monitoring: 1
    property int p_beat_timer_running: 0
    property string p_dev_addr
    property string p_dev_name

    function logHeartRate(rate) {
        console.log("XXXX QML has received rate:" + rate);
        p_monitoring = 1;
        if (p_beat_timer_running == 0) {
            beat_timer.start();
        }
        var beat_freq_ms = Math.floor((60 / rate) * 1000);
        beat_timer.time_limit = beat_freq_ms;
        console.log("XXXX heart beat timer set to " + beat_freq_ms);
        var index1 = Math.floor(rate / 100);
        var index2 = Math.floor(((rate - (index1 * 100)) / 10));
        var index3 = Math.floor(((rate - (index1 * 100) - (index2 * 10))));
        var image1_url = digit[index1];
        var image2_url = digit[index2];
        var image3_url = digit[index3];

        if (index1 === 0) {
            imgDigit1.imageSource = blank;
        } else {
            imgDigit1.imageSource = digit[index1];
        }
        if ((index1 === 0) && (index2 === 0)) {
            imgDigit2.imageSource = blank;
        } else {
            imgDigit2.imageSource = digit[index2];
        }
        imgDigit3.imageSource = digit[index3];

        if (rate < 100) {
            imageHeart.imageSource = "asset:///images/green_disc.png";
        } else {
            if (rate < 120) {
                imageHeart.imageSource = "asset:///images/orange_disc.png";
            } else {
                imageHeart.imageSource = "asset:///images/red_disc.png";

            }
        }
    }

    Menu.definition: MenuDefinition {
        actions: [
            ActionItem {
                title: "Settings"
                imageSource: "images/settings.png"

                onTriggered: {
                    settings_page.open();
                }
            },
            ActionItem {
                title: "About"
                imageSource: "images/about.png"

                onTriggered: {
                    aboutInfo.open();
                }
            }
        ]
    }
    
    attachedObjects: [
        AboutSheet {
            id: aboutInfo
        },
        Settings {
                    id: settings_page
        }
    ]


    Container {
        id: container_outer
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }

        Container {
            id: container_driver
            layoutProperties: StackLayoutProperties {
                spaceQuota: 33
            }

            Container {

                Label {
                    text: "Driver"
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 20
                    }
                    leftPadding: 5
                    ImageView {
                        id: image_heart
                        function tweak() {
                            heartbeat.play()
                        }
                        imageSource: "asset:///images/heart_75.png"
                        verticalAlignment: VerticalAlignment.Center
                        attachedObjects: [
                            SequentialAnimation {
                                id: heartbeat
                                animations: [
                                    ScaleTransition {
                                        duration: 100
                                        toX: 1.0
                                        fromX: 0.9
                                        toY: 1.0
                                        fromY: 0.9
                                    }
                                ]
                            }
                        ]

                    }
                    Container {
                        id: container_bpm
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        LedDigit {
                            id: imgDigit1
                            objectName: "imgDigit1"
                            imageSource: "asset:///images/orange0.png"
                        }
                        LedDigit {
                            id: imgDigit2
                            objectName: "imgDigit2"
                            imageSource: "asset:///images/orange0.png"
                        }
                        LedDigit {
                            id: imgDigit3
                            objectName: "imgDigit3"
                            imageSource: "asset:///images/orange0.png"
                        }
                    }
                    Timer {
                        id: beat_timer
                        time_limit: 1000
                        onTimeout: {
                            imgDigit1.tweak();
                            imgDigit2.tweak();
                            imgDigit3.tweak();
                            image_heart.tweak();
                        }
                    }
                }
                Container {
                    id: container_heart_indication
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 10
                    }
                    leftPadding: 5
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    ImageView {
                        id: imageHeart
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 13
                        }
                        imageSource: "asset:///images/green_disc.png"
                    }
                    Label {
                        id: lblHeart
                        text: " Heart"
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 87
                        }
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Yellow
                        }
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                }
                Container {
                    id: container_stress
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 10
                    }
                    leftPadding: 5
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    ImageView {
                        id: imageStress
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 13
                        }
                        imageSource: "asset:///images/green_disc.png"
                    }
                    Label {
                        id: lblStress
                        text: " Stress"
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 87
                        }
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Yellow
                        }
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                }
                Container {
                    id: container_glucose
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 10
                    }
                    leftPadding: 5
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    ImageView {
                        id: imageGlucose
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 13
                        }
                        imageSource: "asset:///images/green_disc.png"
                    }
                    Label {
                        id: lblGlucose
                        text: " Glucose"
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 87
                        }
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Yellow
                        }
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                }
                Container {
                    id: container_notifications
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 50
                    }
                    leftPadding: 5
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    Label {
                        id: lblNotifications
                        text: "In 15 minutes (10 miles) you will be driving into an area of high pollen count"
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.White
                        }
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                }
            }
        }
        Container {
            id: container_driving
            layoutProperties: StackLayoutProperties {
                spaceQuota: 33
            }
            Label {
                text: "Driving"
                horizontalAlignment: HorizontalAlignment.Center
            }
            Container {
                id: container_distance
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 20
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                LedDigit {
                    id: imgDist1
                    objectName: "imgDistDigit1"
                    imageSource: "asset:///images/orange2.png"
                }
                LedDigit {
                    id: imgDist2
                    objectName: "imgDistDigit2"
                    imageSource: "asset:///images/orange7.png"
                }
            }
            Container {
                id: container_driving_distance
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 10
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                Label {
                    id: lblDrivingText
                    text: "metres from vehicle"
                    textStyle {
                        color: Color.Yellow
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                }
            }
            Container {
                id: container_driving_assessment
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 10
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                ImageView {
                    id: image_driving_style
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 13
                    }
                    imageSource: "asset:///images/checkOk.png"
                }
                Label {
                    id: lblDrivingAssessment
                    text: "You are driving well"
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 87
                    }
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Yellow
                    }
                    horizontalAlignment: HorizontalAlignment.Left
                }
            }
            Container {
                id: container_dummy1
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 10
                }
            }
            Container {
                id: container_driving_notifications
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                Label {
                    id: lblDrivingNotifications1
                    text: "CAUTION!"
                    multiline: true
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Red
                    }
                    horizontalAlignment: HorizontalAlignment.Left
                }
                Label {
                    id: lblDrivingNotifications2
                    text: "The road is wet"
                    multiline: true
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Red
                    }
                    horizontalAlignment: HorizontalAlignment.Left
                }
            }
        }
        Container {
            id: container_vehicle
            layoutProperties: StackLayoutProperties {
                spaceQuota: 34
            }
            Label {
                text: "Vehicle"
                horizontalAlignment: HorizontalAlignment.Center
            }
            Container {
                id: container_speed
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 20
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                LedDigit {
                    id: imgSpeed1
                    objectName: "imgSpeedDigit1"
                    imageSource: "asset:///images/orange5.png"
                }
                LedDigit {
                    id: imgSpeed2
                    objectName: "imgSpeedDigit2"
                    imageSource: "asset:///images/orange8.png"
                }
            }
            Container {
                id: container_speed_text
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 10
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                Label {
                    id: lblSpeedText
                    text: "MPH"
                    textStyle {
                        color: Color.Yellow
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                }
            }
            Container {
                id: container_wheel1
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 50
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                ImageView {
                    id: image_tyres
                    imageSource: "asset:///images/tyre_pressures.png"
                }
            }
            Container {
                id: container_dummy2
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 20
                }
            }
        }
    }

}
