//-- Default empty project template
import bb.cascades 1.0

Page {
    Container {
        Button {
            text: "Login to Facebook"
            onClicked: {
                app.login();
            }
        }
        Button {
            text: "Logout of Facebook"
            onClicked: {
                app.logout();
            }
        }
        Button {
            text: "Get My Profile"
            onClicked: {
                app.showUserProfile();
            }
        }
        Button {
            text: "Post to wall"
            onClicked: {
                app.publishPost();
            }
        }
        Button {
            text: "Get User's Friends"
            onClicked: {
                app.showUserFriends();
            }
        }
        Divider {
        }
        ScrollView {
            scrollViewProperties {
                scrollMode: ScrollMode.Both
            }
            TextArea {
                objectName: "status"
                editable: false
                focusable: Focusable.None
            }
        }
    }
    attachedObjects: [
        CustomDialog {
            objectName: "authDialog"
        }
    ]
}
