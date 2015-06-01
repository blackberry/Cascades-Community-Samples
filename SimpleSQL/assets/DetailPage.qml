import bb.cascades 1.0

Page {
    property alias clientName: titleBar.title
    property alias phoneNumber: phoneNumber.title
    property alias emailAddress: emailAddress.title
    titleBar: TitleBar {
        id: titleBar
        title: clientName
    }
    Container {
        Header {
            title: "Phone Number"
        }
        StandardListItem {
            id: phoneNumber
        }
        Header {
            title: "Email Address"
        }
        StandardListItem {
            id: emailAddress
        }
    }
}
