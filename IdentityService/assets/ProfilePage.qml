import bb.cascades 1.2

/**
 * BlackBerry Profile data isn't available instantly - you are probably going to
 * have to press Refresh after adding / deleting something...
 */
Page {
    titleBar: TitleBar {
        title: "BlackBerry Profile"
    }
    function onPropertyCountChanged(count) {
        console.log("QML property count changed " + count);
        containerNoProps.visible = count == 0;
        containerProps.visible = count > 0;
        containerRefreshProps.visible = count < 0;
        console.log("Did that work?");
    }
    onCreationCompleted: {
        app.propertyCountChanged.connect(onPropertyCountChanged);
        onPropertyCountChanged(-1);
    }
    actions: [
        ActionItem {
            ActionBar.placement: ActionBarPlacement.OnBar
            title: "Add"
            imageSource: "asset:///icons/add.png"
            onTriggered: {
                app.addProperty();
            }
        },
        ActionItem {
            ActionBar.placement: ActionBarPlacement.OnBar
            title: "Refresh"
            imageSource: "asset:///icons/refresh.png"
            onTriggered: {
                app.refreshProperties();
            }
        }

    ]
    Container {
        Container {
            id: containerProps
            visible: false
            ListView {
                id: listView
                function handleEdit(scope, name) {
                    app.editProperty(scope, name);
                }
                function handleDelete(scope, name) {
                    app.deleteProperty(scope, name);
                }
                dataModel: app.propertyData
                listItemComponents: [
                    ListItemComponent {
                        type: "header"
                        Header {
                            title: ListItemData
                        }
                    },
                    ListItemComponent {
                        type: "item"
                        id: item
                        Container {
                            id: itemContainer
                            contextActions: ActionSet {
                                ActionItem {
                                    title: "Edit"
                                    imageSource: "asset:///icons/edit.png"
                                    onTriggered: {
                                        itemContainer.ListItem.view.handleEdit(ListItemData.scope, ListItemData.name);
                                    }
                                }
                                DeleteActionItem {
                                    title: "Delete"
                                    onTriggered: {
                                        itemContainer.ListItem.view.handleDelete(ListItemData.scope, ListItemData.name);
                                    }
                                }
                            }
                            Label {
                                text: ListItemData.name
                                textStyle.fontSize: FontSize.XLarge
                            }
                            Label {
                                text: ListItemData.value
                            }
                        }
                    }
                ]
            }
        }
        Container {
            visible: false
            id: containerNoProps
            Label {
                text: "No properties found!"
            }
        }
        Container {
            visible: false
            id: containerRefreshProps
            Label {
                text: "Getting properties"
            }
        }
    }
}
