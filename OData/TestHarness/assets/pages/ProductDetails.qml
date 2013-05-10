import bb.cascades 1.0
import odata 1.0
import "controls" 1.0

Page {
    property string dataSource
    property variant dataModel
    property variant supplierModel
    property variant categoryModel
    
    actions: [
        ActionItem {
            title: qsTr("Update")
            imageSource: "asset:///icons/ic_edit.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                var activeTab = tabPane.activeTab;

                var createEditPage = createEditProductPage.createObject();
                createEditPage.updateModel = dataModel;
                createEditPage.create = false;
                createEditPage.updateCategory = categoryModel.id;
                createEditPage.updateSupplier = supplierModel.id;
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

                odataModel.refreshModel();
            }
        },
        ActionItem {
            title: qsTr("Delete")
            imageSource: "asset:///icons/ic_delete.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
            	odataModel.deleteModel();
            	
            	toastMsg.body = "Deleting";
            	toastMsg.show();
            }
        }
    ]

	attachedObjects: [
	    ODataObjectModel{
	        id: odataModel
	        source: dataSource
	    },
        ODataObjectModel {
            id: supplierODataModel
        },
        ODataObjectModel {
            id: categoryODataModel
        }
    ]

    content: Container {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        Label {
            text: "Product Details"
            textStyle.base: SystemDefaults.TextStyles.BigText
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
        ScrollView {
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }
                DetailsItemRow {
                    label: qsTr("Product")
                    data: dataModel.title[".data"]
                }
                DetailsItemRow {
                    label: qsTr("")
                    data: dataModel.summary[".data"]
                }
                DetailsItemRow {
                    label: qsTr("Cost")
                    data: dataModel.content["m:properties"]["d:Price"][".data"]
                }
                DetailsItemRow {
                    label: qsTr("Rating")
                    data: dataModel.content["m:properties"]["d:Rating"][".data"]
                }
                DetailsItemRow {
                    label: qsTr("Released")
                    data: dataModel.content["m:properties"]["d:ReleaseDate"][".data"]
                }
                DetailsItemRow {
                    label: qsTr("Discontinued")
                    data: dataModel.content["m:properties"]["d:DiscontinuedDate"][".data"]
                }
                DetailsItemRow {
                    label: qsTr("Updated")
                    data: dataModel.updated
                }

                // Supplier Linked Row
                Container {
                    topMargin: 30
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                activeTab = tabPane.activeTab;

                                var supplierDetails = supplierDetailsPage.createObject();
                                supplierDetails.dataSource = supplierModel.id;
                                activeTab.content.push(supplierDetails);
                            }
                        }
                    ]

                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    horizontalAlignment: HorizontalAlignment.Fill

                    Label {
                        text: qsTr("Supplier")
                        textStyle.fontSize: FontSize.Medium
                        textStyle.fontWeight: FontWeight.W500
                        verticalAlignment: VerticalAlignment.Top

                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 3
                        }
                    }
                    Label {
                        text: supplierModel.title[".data"]
                        textStyle.fontSize: FontSize.Medium
                        textStyle.color: Color.create("#FF8EC1DA")
                        verticalAlignment: VerticalAlignment.Top
                        multiline: true

                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 5
                        }
                    }
                }

                // Category Linked Row
                Container {
                    topMargin: 30

                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                                var activeTab = tabPane.activeTab;

                                var productList = productListPage.createObject();
                                productList.title = categoryModel.title[".data"] + " - " + qsTr("Products");
                                productList.dataSource = dataService.source + "/" + categoryModel.link[1]["href"];
                                activeTab.content.push(productList);
                            }
                        }
                    ]

                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    horizontalAlignment: HorizontalAlignment.Fill

                    Label {
                        text: qsTr("Category")
                        textStyle.fontSize: FontSize.Medium
                        textStyle.fontWeight: FontWeight.W500
                        verticalAlignment: VerticalAlignment.Top

                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 3
                        }
                    }
                    Label {
                        text: categoryModel.title[".data"]
                        textStyle.fontSize: FontSize.Medium
                        textStyle.color: Color.create("#FF8EC1DA")
                        verticalAlignment: VerticalAlignment.Top
                        multiline: true

                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 5
                        }
                    }
                }
            }
        }
    }

    onCreationCompleted: {
        odataModel.modelReady.connect(bindToDataModel);
        supplierODataModel.modelReady.connect(bindSupplier);
        categoryODataModel.modelReady.connect(bindCategory);

        odataModel.modelDeleted.connect(deleteSuccess);
    }
    
    function bindToDataModel() {
        dataModel = odataModel.model;

        supplierODataModel.source = dataService.source + "/" + dataModel.link[2]["href"];
        categoryODataModel.source = dataService.source + "/" + dataModel.link[1]["href"];
    }
    
    function bindSupplier() {
        supplierModel = supplierODataModel.model;
    }

    function bindCategory() {
        categoryModel = categoryODataModel.model;
    }
    
    function deleteSuccess() {
        toastMsg.body = "Success";
        toastMsg.show();
        
        var activeTab = tabPane.activeTab;
        activeTab.content.pop();
    }
}
