import bb.cascades 1.2

/**
 * Simple UI to show the fields retrieved from the BlackBerry ID.
 */
Page {
    titleBar: TitleBar {
        title: "BlackBerry ID"
    }
    ScrollView {
        content: Container {
            Label {
                text: "User Name"
            }
            TextArea {
                text: app.userName
                editable: false
            }
            
            Label {
                text: "Screen Name"
            }
            TextArea {
                text: app.screenName
                editable: false
            }
            
            Label {
                text: "First Name"
            }
            TextArea {
                text: app.firstName
                editable: false
            }
            
            Label {
                text: "Last Name"
            }
            TextArea {
                text: app.lastName
                editable: false
            }
        }
    }
}
