// Default empty project template
import bb.cascades 1.0

// creates one page with a label

Page {
    property bool playedSound: false
    function crack() {
    }
    onCreationCompleted: {
        animation.play();
    }
    Container {
        layout: StackLayout {
        }
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
        ImageView {
            id: img_egg
            imageSource: "asset:///images/brown_egg.png"
            onTouch: {
                crack();
                if (event.isDown()) {
                    playedSound = false;
                } else {
                    if (event.isUp()) {
                        if (! playedSound) {
                            animation.play();
                            app.playSound("chicken1.wav");
                            playedSound = true;
                        }
                    }
                }
            }
            attachedObjects: [
                SequentialAnimation {
                    id: animation
                    animations: [
                        ScaleTransition {
                            duration: 2000
                            toX: 1.0
                            fromX: 0.0
                            toY: 1.0
                            fromY: 0.0
                        }
                    ]
                }
            ]
        }
    }
}
