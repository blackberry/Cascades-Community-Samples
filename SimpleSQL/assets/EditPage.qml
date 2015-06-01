import bb.cascades 1.3
import bb.system 1.2
import bb.cascades.pickers 1.0

Page {
    property alias firstName: firstName.text
    property alias lastName: surname.text
    property alias clientID: clientID.text
    property alias phoneNumber: phoneNumber.text
    property alias emailAddress: emailAddress.text
    titleBar: TitleBar {
        title: qsTr("Edit Client") + Retranslate.onLocaleOrLanguageChanged
        acceptAction: ActionItem {
            title: qsTr("Save") + Retranslate.onLocaleOrLanguageChanged
            onTriggered: {
                // Add record to database
                _sql.editClient(firstName.text.replace(" ", ""), surname.text, clientID.text, phoneNumber.text, emailAddress.text);
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
                        Label {
                            id: clientID
                            visible: false
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
                DropDown {
                    id: phoneNumberDropdown
                    visible: false
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
                DropDown {
                    id: emailAddressDropdown
                    visible: false
                }
            }
        }
    }
    onCreationCompleted: {
        firstName.requestFocus()
    }
}
