import bb.cascades 1.2

NavigationPane {
    id: navPane
    Menu.definition: MenuDefinition {
        settingsAction: SettingsActionItem {

            onTriggered: {
                var page = settingsPage.createObject();
                navPane.push(page)
            }
        }
    }

    Page {
        Container {
            layout: DockLayout {
            }
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill

            ScrollView {
                scrollViewProperties.scrollMode: ScrollMode.Vertical

                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                WebView {
                    url: "http://opensourcebb.com"
                }
            }
        }
    }
    attachedObjects: [
        ComponentDefinition {
            id: settingsPage
            source: "SettingsPage.qml"
        }
    ]
}