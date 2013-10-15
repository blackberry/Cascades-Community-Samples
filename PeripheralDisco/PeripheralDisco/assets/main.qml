import bb.cascades 1.2

TabbedPane {
    activeTab: serial
    tabs: [
        Tab {
            id: disco
            title: "Peripheral Disco"
            content: Page {

                actions: [
                    ActionItem {
                        title: "Clear Log"
                        onTriggered: {
                            log.text = "<div></div>"
                        }
                        ActionBar.placement: ActionBarPlacement.OnBar
                    }
                ]
                onCreationCompleted: {
                    app.log.connect(onLog);
                }
                function onLog(toAdd) {
                    var current = log.text;
                    if (current.length != 0) {
                        toAdd = "\n" + toAdd;
                    }
                    current += toAdd;
                    log.text = current;
                }
                titleBar: TitleBar {
                    title: "Peripheral Disco"
                }
                content: Container {
                    layout: DockLayout {
                    }
                    verticalAlignment: VerticalAlignment.Fill
                    horizontalAlignment: HorizontalAlignment.Fill
                    preferredHeight: maxHeight
                    TextArea {
                        textFormat: TextFormat.Html
                        verticalAlignment: VerticalAlignment.Fill
                        horizontalAlignment: HorizontalAlignment.Fill
                        objectName: "log"
                        id: log
                    }
                }
                actionBarVisibility: ChromeVisibility.Visible
            }
            imageSource: "asset:///discoSmall.png"
        },
        Tab {
            id: serial
            title: "Serial Comms"
            content: Page {
                titleBar: TitleBar {
                    title: "What does the USB say?"
                }
                actions: [
                    ActionItem {
                        ActionBar.placement: ActionBarPlacement.OnBar
                        title: "Connect"
                        enabled: app.serialPossible && !app.serialConnected
                        onTriggered: {
                         	app.openSerial();   
                        }
                        imageSource: "asset:///connect.png"

                    },
                    ActionItem {
                        id: closeAction
                        ActionBar.placement: ActionBarPlacement.OnBar
                        title: "Disco Nect"
                        enabled: app.serialPossible && app.serialConnected
                        onTriggered: {
                         	app.closeSerial();   
                        }
                        imageSource: "asset:///disconnect.png"
                    }
                ]
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Fill
                    layout: DockLayout {

                    }
                    Slider {
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Fill
                        onValueChanged: {
                            app.writeSerial(value);
                        }
                        onImmediateValueChanged: {
                            app.writeSerial(immediateValue);
                        }
                        value: 0.5;
                        enabled: closeAction.enabled
                    }
                }
            }
            imageSource: "asset:///serial.png"
        }
    ]
}
