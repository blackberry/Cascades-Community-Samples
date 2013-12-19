import bb.cascades 1.2
import com.robwilliamsjnr.holidaytrain 1.0

Page {
    titleBar: TitleBar {
        title: "Holiday Railway"
    }

    property BluetoothManager bluetoothManager

    onCreationCompleted: {
        bluetoothManager.connectionChanged.connect(connectionChanged);
    }

    content: ScrollView {
        content: Container {
            id: container

            function command(command) {
                console.log("Doing " + command);
                bluetoothManager.connection.sendMessage(command);
            }

            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            Container {
                visible: bluetoothManager.connection==null
                horizontalAlignment: HorizontalAlignment.Fill
                layout: DockLayout {
                }
                topPadding: 80
                bottomPadding: 80
                background: Color.Red
                Label {
                    
                    horizontalAlignment: HorizontalAlignment.Center
                 	text: "No bluetooth connection"
                    textStyle.color: Color.create("#ff7f7f00")
                }
            }
            ImageView {
                imageSource: "asset:///images/engine.JPG"
                scalingMethod: ScalingMethod.AspectFill

            }
            ControlRow {
                topMargin: 5
                bottomMargin: 5
                onCommand: {
                    container.command("d" + command);
                }
            }
            LightRow {
                light: "a"
                onCommand: {
                    container.command(command);
                }
            }
            LightRow {
                light: "b"
                onCommand: {
                    container.command(command);
                }
            }
            LightRow {
                light: "c"
                onCommand: {
                    container.command(command);
                }
            }
            LightRow {
                light: "d"
                onCommand: {
                    container.command(command);
                }
            }
        }
    }
}
