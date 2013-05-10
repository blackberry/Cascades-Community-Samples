import bb.cascades 1.0
import odata 1.0

NavigationPane {
    property string dataSource

    Page {

        Container {
            layout: StackLayout {
            }
            Label {
                text: qsTr("Categories")
                textStyle.base: SystemDefaults.TextStyles.BigText
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }
            ListView {
                dataModel: ODataListModel {
                    source: dataSource
                }
                onTriggered: {
                    
                    var selectedItem = dataModel.data(indexPath); // get the selected item
                    var activeTab = tabPane.activeTab;

                    var productList = productListPage.createObject();
                    productList.title = selectedItem.title[".data"] + " - " + qsTr("Products");
                    productList.dataSource = dataService.source + "/" + selectedItem.link[1]["href"];
                    activeTab.content.push(productList);
                }
                listItemComponents: [
                    ListItemComponent {
                        StandardListItem {
                            title: ListItemData.title[".data"]
                        }
                    }
                ]
            }
        }
    }
    onPopTransitionEnded: {
        page.destroy();
    }
}
