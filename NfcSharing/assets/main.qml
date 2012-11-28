import bb.cascades 1.0

TabbedPane {

    showTabsOnActionBar: true
    
    signal disableSharing()
    
    Tab {
        title: qsTr("Share Files")
        ShareFilePage {
        }
    }

    Tab {
        title: qsTr("Share Data")
        ShareDataPage{
        }
    }

    Tab {
        title: qsTr("Share Url")
        ShareUrlPage{
        }
    }

    onActiveTabChanged: {
        console.log("XXXX active tab changed");
        disableSharing();
    }
}
