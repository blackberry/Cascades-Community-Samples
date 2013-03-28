import bb.cascades 1.0

Page {
    content: Container {
        id: createOrLoginContainer
        layout: DockLayout {
        }
        Container {
            leftPadding: 80
            rightPadding: leftPadding
            topPadding: 200
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                TextField {
                    id: calledNumber
                    objectName: "calledNumber"
                    hintText: "00441234567890"
                    topMargin: 200
                    inputMode: TextFieldInputMode.PhoneNumber
                }
            }
            Container {
                leftPadding: 80
                rightPadding: leftPadding
                topPadding: 50
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                Button {
                    id: callButton
                    objectName: "calledButton"
                    text: "Call"
                    onClicked: {
                        //pjsipEngine.call(calledNumber.text);
                    }
                }
            }
        }
    }
}
