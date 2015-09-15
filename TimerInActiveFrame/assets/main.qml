import bb.cascades 1.4
import Timer 1.0

Page {
    
    function onThumbnailed() {
        appCover.labelText = timeLabel.text
        // Start the timer for refresh app cover label
        changeAppCoverTimer.start();
    }
    
    function onFullscreen() {
        // Stop the timer for refresh app cover label
        changeAppCoverTimer.stop();
    }
    
    onCreationCompleted: {
        clockTimer.start();
        Application.thumbnail.connect(onThumbnailed);
        Application.fullscreen.connect(onFullscreen);
        Application.setCover(appCover);
        appCover.labelText = "";
    }
    attachedObjects: [
        // On timer timeout refresh the App Cover text label with the new time 
        Timer {
            id: changeAppCoverTimer
            singleShot: false
            interval: 100 //milliseconds
            onTimeout: {
                onThumbnailed();
            }
        },
        AppCover {
            id: appCover
        }
    ]
    Container {
        layout: DockLayout {
            
        }
        Label {
            id: timeLabel
            text: "" 
            textStyle {
                color: Color.Red
                fontSize: FontSize.XXLarge
            }
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            attachedObjects: [
                // On timer timeout refresh the text label with the new time 
                Timer {
                    id: clockTimer
                    singleShot: false
                    interval: 100
                    onTimeout: {
                        timeLabel.text = _app.clockString();
                    }
                }
            ]
        }
    }
}
