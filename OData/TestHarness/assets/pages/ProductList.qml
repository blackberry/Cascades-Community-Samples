import bb.cascades 1.0
import odata 1.0
    
Page {

    property string title
    property string dataSource
    
    actions: [
        ActionItem {
            title: qsTr("Create")
            imageSource: "asset:///icons/ic_add.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                var activeTab = tabPane.activeTab;

                var createEditPage = createEditProductPage.createObject();
                activeTab.content.push(createEditPage);
            }
        },
        ActionItem {
            title: qsTr("Refresh")
            imageSource: "asset:///icons/ic_rotate.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                toastMsg.body = "Refreshing";
                toastMsg.show();

                productsList.dataModel.refresh();
            }
        }
    ]

    Container {
        layout: StackLayout {
        }
        Label {
            text: title
            textStyle.base: SystemDefaults.TextStyles.BigText
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
        ListView {
            id: productsList
            
            dataModel: ODataListModel {
                source: dataSource
            }
            onTriggered: {
                var selectedItem = dataModel.data(indexPath); // get the selected item
                var activeTab = tabPane.activeTab;

                var productDetails = productDetailsPage.createObject();
                productDetails.dataSource = selectedItem.id;
                activeTab.content.push(productDetails);

            }
            listItemComponents: [
                ListItemComponent {
                    StandardListItem {
                        title: ListItemData.title[".data"]
                        description: ListItemData.summary[".data"] + " - Cost: " + ListItemData.content["m:properties"]["d:Price"][".data"]
                    }
                }
            ]
        }
    }
}
