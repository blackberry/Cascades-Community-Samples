import bb.cascades 1.0
import bb.system 1.0
import bb.cascades.pickers 1.0

Page {
    titleBar: TitleBar {
        title: qsTr("New Client") + Retranslate.onLocaleOrLanguageChanged
        acceptAction: ActionItem {
            id: acceptActionItem
            title: qsTr("Save") + Retranslate.onLocaleOrLanguageChanged
            enabled: false
            onTriggered: {
                // Add record to database
                _sql.addClient(firstName.text.replace(" ", ""), surname.text, phoneNumber.text, emailAddress.text);
                navigationPane1.pop()
            }
        }
        dismissAction: ActionItem {
            title: qsTr("Cancel") + Retranslate.onLocaleOrLanguageChanged
            onTriggered: {
                navigationPane1.pop()
            }
        }
    }
    ScrollView {
        Container {
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.TopToBottom
                    }
                    Header {
                        title: qsTr("First Name") + Retranslate.onLocaleOrLanguageChanged
                    }
                    Container {
                        leftPadding: 20
                        rightPadding: 20
                        topPadding: 20
                        bottomPadding: 20
                        TextField {
                            id: firstName
                            onTextChanging: {
                                acceptActionItem.enabled = true
                            }
                        }
                    }
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.TopToBottom
                    }
                    Header {
                        title: qsTr("Surname") + Retranslate.onLocaleOrLanguageChanged
                    }
                    Container {
                        leftPadding: 20
                        rightPadding: 20
                        topPadding: 20
                        bottomPadding: 20
                        TextField {
                            id: surname
                        }
                    }
                }
            }
            Header {
                title: qsTr("Phone Number") + Retranslate.onLocaleOrLanguageChanged
            }
            Container {
                leftPadding: 20
                rightPadding: 20
                topPadding: 20
                bottomPadding: 20
                TextField {
                    id: phoneNumber
                }
            }
            Header {
                title: qsTr("E-mail Address") + Retranslate.onLocaleOrLanguageChanged
            }
            Container {
                leftPadding: 20
                rightPadding: 20
                topPadding: 20
                bottomPadding: 20
                TextField {
                    id: emailAddress
                }
            }
        }
    }
    onCreationCompleted: {
        firstName.requestFocus()
    }
}