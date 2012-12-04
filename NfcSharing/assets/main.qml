import bb.cascades 1.0

TabbedPane {
    showTabsOnActionBar: true
    signal disableSharing()
    Tab {
        title: qsTr("Share Files")
        imageSource: "images/share.png"
        ShareFilePage {
        }
    }
    Tab {
        title: qsTr("Share Data")
        imageSource: "images/share.png"
        ShareDataPage {
        }
    }
    Tab {
        title: qsTr("Share Url")
        imageSource: "images/share.png"
        ShareUrlPage {
        }
    }
    Tab {
        title: qsTr("About")
        imageSource: "images/about.png"
        AboutPage {
        }
    }
    onActiveTabChanged: {
        console.log("XXXX active tab changed");
        disableSharing();
    }
}
