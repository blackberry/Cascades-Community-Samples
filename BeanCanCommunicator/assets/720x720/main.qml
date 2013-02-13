// Default empty project template
import bb.cascades 1.0

Page {
    id: thePage
    objectName: "talkPage"
    resizeBehavior: PageResizeBehavior.None
    onCreationCompleted: {
        note1.hideSendNote();
        note2.hideRecvNote();
    }
    content: Container {
        Container {
            layout: DockLayout {
            }
            id: talk
            objectName: "talky"
            // signals
            signal updateMessage(string message)

            // slots
            function messageReceived(message) {
                console.log("XXXX messageReceived:" + message);
                note2.messageTextRecvd.connect(talk.onMessageTextRecvd);
                note2.visible = true;
                note2.preRecv();
                note2.msg = message;
                note2.recvMessage();
            }
            function onMessageTextRecvd(message) {
                console.log("XXXX slot onMessageTextRecvd");
                lblreceived.text = "Received: " + message;
            }
            ImageView {
                imageSource: "images/can_left_full.png"
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
            }
            Container {
                layout: StackLayout {
                }
                preferredHeight: 720
                preferredWidth: 720
                TextField {
                    id: tfmessage
                    hintText: "Enter your message"
                    inputMode: TextFieldInputMode.Text
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                    onFocusedChanged: {
                        if (focused) {
                            tfmessage.text = "";
                            note1.msg = tfmessage.text;
                            note1.visible = true;
                            note1.showNote();
                        }
                    }
                    onTextChanging: {
                        console.log("XXXX onTextChanging:" + tfmessage.text);
                        if (tfmessage.text.length > 0) {
                            note1.visible = true;
                            note1.msg = tfmessage.text;
                        } else {
                            note1.visible = false;
                        }
                    }
                    onTextChanged: {
                        console.log("XXXX onTextChanged:" + tfmessage.text);
                        if (tfmessage.text.length > 0) {
                            talk.updateMessage(tfmessage.text);
                            note1.sendMessage();
                        }
                    }
                }
                Label {
                    id: lblreceived
                }
                Container {
                    layout: DockLayout {
                    }
                    preferredHeight: 1280
                    preferredWidth: 768
                    layoutProperties: {
                        horizontalAlignment:
                        HorizontalAlignment.Fill
                        verticalAlignment:
                        VerticalAlignment.Fill
                    }
                    Note {
                        id: note1
                        property int initialRotation: 0
//                        layoutProperties: AbsoluteLayoutProperties {
//                            positionX: 94
//                            positionY: 184
//                        }
                        visible: false
                    } // First Note
                    Note {
                        id: note2
                        property int initialRotation: 0
//                        layoutProperties: AbsoluteLayoutProperties {
//                            positionX: 800
//                            positionY: 600
//                        }
                        visible: false
                    } // Second Note
                }
            }
        }
    }
}
