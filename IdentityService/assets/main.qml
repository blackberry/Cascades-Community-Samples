import bb.cascades 1.2

TabbedPane {
    tabs: [
        Tab {
            title: "BlackBerry ID"
            id: tabBBID
            imageSource: "asset:///icons/id.png"
            IDPage{}
        },
        Tab {
            title: "BlackBerry Profile"
            id: tabBBProfile
            imageSource: "asset:///icons/profile.png"
            ProfilePage{}
        }
    ]
}
