import bb.cascades 1.2

Page {
    titleBar: TitleBar {
        title: "AES Crypto"
    }
    onCreationCompleted: {
        plainText.text = app.plainText();
        key.text = app.key();
        iv.text = app.iv();
        cipherText.text = app.cipherText();
        recoveredPlainText.text = app.recoveredPlainText();

    }
    actions: [
        ActionItem {
            title: "Encrypt"
            ActionBar.placement: ActionBarPlacement.OnBar
            enabled: app.cryptoAvailable
            onTriggered: {
                cipherText.text = "";
                recoveredPlainText.text = "";
                if (app.encrypt()) {
                    cipherText.text = app.cipherText();
                }
            }
            imageSource: "asset:///images/g3131.png"
        },
        ActionItem {
            title: "Decrypt"
            enabled: app.cryptoAvailable
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                recoveredPlainText.text = "";
                if (app.decrypt()) {
                    recoveredPlainText.text = app.recoveredPlainText();
                }
            }
            imageSource: "asset:///images/g3137.png"
        },
        ActionItem {
            title: "Generate key"
            enabled: app.cryptoAvailable
            ActionBar.placement: ActionBarPlacement.InOverflow
            onTriggered: {
                key.text = app.generate(); // will trigger onchanged...
            }
            imageSource: "asset:///images/g3189.png"

        },
        ActionItem {
            title: "Generate IV"
            enabled: app.cryptoAvailable
            ActionBar.placement: ActionBarPlacement.InOverflow
            onTriggered: {
                iv.text = app.generate(); // will trigger onchanged...
            }
            imageSource: "asset:///images/g3154.png"
        }
    ]
    actionBarVisibility: actionBarVisibility.Visible
    ScrollView {
        content: Container {
            Label {
                text: "Plain Text"
            }
            TextArea {
                id: plainText
                onTextChanged: {
                    app.setPlainText(text);
                }
            }
            Label {
                text: "Key"
            }
            TextArea {
                id: key
                onTextChanged: {
                    app.setKey(text);
                }
            }
            Label {
                text: "IV"
            }
            TextField {
                id: iv
                onTextChanged: {
                    app.setIV(text);
                }
            }
            Label {
                text: "Cipher Text"
            }
            TextArea {
                id: cipherText
                onTextChanged: {
                    app.setCipherText(cipherText.text);
                }
            }
            Label {
                text: "Recovered plain text"
            }
            TextArea {
                id: recoveredPlainText
            }
        }
    }
}
