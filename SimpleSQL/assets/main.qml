import bb.cascades 1.0
import bb.data 1.0
import custom 1.0 // This gives you access to the SQL functionalities

NavigationPane {
    id: navigationPane1
    Page {
        titleBar: TitleBar {
            title: "Simple SQL"
        }
        Container {
            layout: DockLayout {
            }
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            ListView {
                id: listView1
                dataModel: dataModel1

                property variant sqlAlias: _sql
                property alias dataSourceAlias: dataSource
                property alias dataModelAlias: dataModel1
                property alias editPageAlias: editPage
                property alias navigationPane1Alias: navigationPane1

                listItemComponents: [
                    ListItemComponent {
                        type: "item"
                        StandardListItem {
                            id: rootItem
                            title: ListItemData.firstName + " " + ListItemData.lastName
                            contextActions: [
                                ActionSet {
                                    title: ListItemData.firstName + " " + ListItemData.lastName
                                    actions: [
                                        ActionItem {
                                            title: qsTr("Edit Client") + Retranslate.onLocaleOrLanguageChanged
                                            imageSource: "asset:///IMG/ic_edit.png"
                                            onTriggered: {
                                                var detail = rootItem.ListItem.view.editPageAlias.createObject();

                                                detail.firstName = ListItemData.firstName
                                                detail.lastName = ListItemData.lastName
                                                detail.clientID = ListItemData.clientID
                                                detail.phoneNumber = ListItemData.phoneNumber
                                                detail.emailAddress = ListItemData.emailAddress

                                                // Referencing has to be made (rootItem) as this function
                                                // is withn the listView
                                                rootItem.ListItem.view.navigationPane1Alias.push(detail);
                                            }
                                        },
                                        DeleteActionItem {
                                            title: qsTr("Delete Client") + Retranslate.onLocaleOrLanguageChanged
                                            onTriggered: {
                                                rootItem.ListItem.view.sqlAlias.deleteClient(ListItemData.clientID);
                                                rootItem.ListItem.view.dataSourceAlias.loadData();
                                            }
                                        }
                                    ]
                                }
                            ]
                        }
                    }
                ]
                onTriggered: {
                    var selectedItem = dataModel.data(indexPath);
                    var detail = detailPage.createObject();

                    // Passing in data from the listView, to the pushed page
                    detail.clientName = selectedItem.firstName + " " + selectedItem.lastName
                    detail.phoneNumber = selectedItem.phoneNumber
                    detail.emailAddress = selectedItem.emailAddress

                    navigationPane1.push(detail);
                }
            }
            Container {
                id: noClientsContainer
                //Hide this container when items are added to the listView
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                layout: StackLayout {
                    orientation: orientation.TopToBottom
                }
                ImageView {
                    horizontalAlignment: HorizontalAlignment.Center
                    imageSource: "asset:///IMG/No_Clients.png"
                }
                TextArea {
                    text: qsTr("You currently do not have any clients.") + Retranslate.onLocaleOrLanguageChanged
                    topMargin: 20
                    bottomMargin: 20
                    editable: false
                    textStyle.textAlign: TextAlign.Center
                }
                Button {
                    text: qsTr("Add Client") + Retranslate.onLocaleChanged
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    onClicked: {
                        navigationPane1.push(addPage.createObject());
                        backButtonsVisible = false
                        peekEnabled = false
                    }
                }
            }
        }
        actions: [
            ActionItem {
                title: qsTr("Add Client") + Retranslate.onLocaleOrLanguageChanged
                imageSource: "asset:///IMG/ic_add_friend.png"
                ActionBar.placement: ActionBarPlacement.OnBar

                onTriggered: {
                    navigationPane1.push(addPage.createObject());
                }
            }
        ]
        attachedObjects: [
            GroupDataModel {
                id: dataModel1
                sortingKeys: [ "firstName", "lastName" ]
                grouping: ItemGrouping.ByFirstChar
                onItemAdded: {
                    // Hide the placeholder when there are items in the data model
                    noClientsContainer.visible = false
                }
            },
            MySqlDataSource {
                id: dataSource
                // Source must be the same as defined in C++
                source: "data/database.db"
                query: "SELECT * FROM Clients"

                onDataLoaded: {
                    // After the data is loaded, insert it into the data model
                    console.log("Data Loaded")
                    dataModel1.clear();
                    dataModel1.insertList(data);
                }
            },
            ComponentDefinition {
                id: detailPage
                source: "DetailPage.qml"
            },
            ComponentDefinition {
                id: editPage
                source: "EditPage.qml"
            },
            ComponentDefinition {
                id: addPage
                source: "AddPage.qml"
            }
        ]
    }
    onPopTransitionEnded: {
        page.destroy();
        dataSource.loadData();
    }
    onCreationCompleted: {
        dataSource.loadData();
    }
}