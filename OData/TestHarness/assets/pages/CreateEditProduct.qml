import bb.cascades 1.0
import odata 1.0
import "controls" 1.0

Page {
    property variant updateModel
    property string updateCategory: ""
    property string updateSupplier: ""
    property bool create: true
    property variant modelStructure
    
    attachedObjects: [
        ODataObjectModel {
            id: objectModel
        },
        ODataListModel {
            id: suppliersList
            source: dataService.source + "/Suppliers"
        },
        ODataListModel {
            id: categoriesList
            source: dataService.source + "/Categories"
        },
        ComponentDefinition {
            id: option

            content: Option {
            }
        }
    ]
    
    actions: [
        ActionItem {
            title: qsTr("OK")
            imageSource: "asset:///icons/ic_done.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                if (create) {
                    createObject();
                }
                else {
                    updateObject();
                }
            }
        }
    ]

    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }

        Label {
            text: create ? qsTr("Create Product") : qsTr("Edit Product")
            textStyle.base: SystemDefaults.TextStyles.BigText
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
        ScrollView {
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }

                ItemInputRow {
                    id: idRow
                    label: qsTr("Id")
                    data: updateModel.content["m:properties"]["d:ID"][".data"]
                    visible: create
                }
                ItemInputRow {
                    id: productRow
                    label: qsTr("Product")
                    data: updateModel.title[".data"]
                }
                ItemInputRow {
                    id: descriptionRow
                    label: qsTr("Description")
                    data: updateModel.summary[".data"]
                }
                ItemInputRow {
                    id: costRow
                    label: qsTr("Cost")
                    data: updateModel.content["m:properties"]["d:Price"][".data"]
                }
                ItemInputRow {
                    id: ratingRow
                    label: qsTr("Rating")
                    data: updateModel.content["m:properties"]["d:Rating"][".data"]
                }
                ItemInputRow {
                    id: releasedRow
                    label: qsTr("Released")
                    data: updateModel.content["m:properties"]["d:ReleaseDate"][".data"]
                }
                ItemInputRow {
                    id: discontinuedRow
                    label: qsTr("Discontinued")
                    data: updateModel.content["m:properties"]["d:DiscontinuedDate"][".data"]
                }
                
                // Supplier List
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    horizontalAlignment: HorizontalAlignment.Fill

                    bottomMargin: 10

                    Label {
                        text: qsTr("Supplier")
                        textStyle.fontSize: FontSize.Medium
                        textStyle.fontWeight: FontWeight.W500
                        verticalAlignment: VerticalAlignment.Center

                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 3
                        }
                    }
                    DropDown {
                        id: supplierRow
                        verticalAlignment: VerticalAlignment.Bottom

                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 5
                        }
                    }
                }
                
                // Category list
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    horizontalAlignment: HorizontalAlignment.Fill

                    bottomMargin: 10

                    Label {
                        text: qsTr("Category")
                        textStyle.fontSize: FontSize.Medium
                        textStyle.fontWeight: FontWeight.W500
                        verticalAlignment: VerticalAlignment.Center

                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 3
                        }
                    }
                    DropDown {
                    	id: categoryRow
                        verticalAlignment: VerticalAlignment.Bottom
                        
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 5
                        }
                    }
                }
            }
        }
    }

    onCreationCompleted: {
        modelStructure = objectModel.getModelStructure("Product", dataService.getMetadata());

        // Get the lists for the drop downs
        suppliersList.itemsChanged.connect(bindSuppliersDropDown);
        suppliersList.childCount([]); // force the data hit
        categoriesList.itemsChanged.connect(bindCategoriesDropDown);
        categoriesList.childCount([]); // force the data hit

        _controller.createProductSuccess.connect(success);
        _controller.updateProductSuccess.connect(success);
    }
    
    function bindSuppliersDropDown() {
        supplierRow.removeAll();

        for (var i = 0; i < suppliersList.childCount([]); i++) {
            var opt = option.createObject();
            opt.text = suppliersList.data([ i ]).title[".data"];
            opt.value = suppliersList.data([ i ]).id;
            supplierRow.add(opt);
            
            if (!create && suppliersList.data([ i ]).id === updateSupplier) {
                supplierRow.setSelectedIndex(i);
            }
        }
    } 
    
    function bindCategoriesDropDown() {
        categoryRow.removeAll();

        for (var i = 0; i < categoriesList.childCount([]); i ++) {
            var opt = option.createObject();
            opt.text = categoriesList.data([ i ]).title[".data"];
            opt.value = categoriesList.data([ i ]).id;
            categoryRow.add(opt);
            
            if (!create && categoriesList.data([ i ]).id === updateCategory) {
                categoryRow.setSelectedIndex(i);
            }
        }
    }
    
    function createObject() {
        readForm();

        _controller.createProduct(modelStructure);
        
        toastMsg.body = "Creating";
        toastMsg.show();
    }
    
    function updateObject() {
        readForm();

        _controller.updateProduct(updateModel.id, modelStructure);
    
        toastMsg.body = "Updating";
        toastMsg.show();
    }
    
    function readForm() {
        var model = modelStructure;

        for (var i = 0; i < model.length; i++) {
            if (model[i].Name === "ID") {
                model[i].Data = idRow.getData();
            } 
            else if (model[i].Name === "Name") {
                model[i].Data = productRow.getData();
            } 
            else if (model[i].Name === "Description") {
                model[i].Data = descriptionRow.getData();
            } 
            else if (model[i].Name === "ReleaseDate") {
                model[i].Data = releasedRow.getData();
            } 
            else if (model[i].Name === "DiscontinuedDate") {
                model[i].Data = discontinuedRow.getData();
            } 
            else if (model[i].Name === "Rating") {
                model[i].Data = ratingRow.getData();
            } 
            else if (model[i].Name === "Price") {
                model[i].Data = costRow.getData();
            } 
            else if (model[i].Name === "Supplier") {
                model[i].Data = supplierRow.selectedValue;
            } 
            else if (model[i].Name === "Category") {
                model[i].Data = categoryRow.selectedValue;
            }
        }
        
        modelStructure = model;
    }
    
    function success() {
        toastMsg.body = "Success";
        toastMsg.show();

        var activeTab = tabPane.activeTab;
        activeTab.content.pop();
    }
}
