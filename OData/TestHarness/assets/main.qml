// Default empty project template
import bb.cascades 1.0
import bb.system 1.0
import "tabs" 1.0
import "pages" 1.0

TabbedPane {
    id: tabPane
    showTabsOnActionBar: false
    
    attachedObjects: [
        ComponentDefinition {
            id: productDetailsPage
            source: "asset:///pages/ProductDetails.qml"
        },
        ComponentDefinition {
            id: productListPage
            source: "asset:///pages/ProductList.qml"
        },
        ComponentDefinition {
            id: supplierDetailsPage
            source: "asset:///pages/SupplierDetails.qml"
        },
        ComponentDefinition {
            id: createEditProductPage
            source: "asset:///pages/CreateEditProduct.qml"
        },
        SystemToast {
            id: toastMsg
        }
    ]

	Tab {
	    title: qsTr("Products")
	    content: ProductsListTab {
	        title: qsTr("Products")
	        dataSource: dataService.source + "/Products"
        }
	    imageSource: "asset:///icons/ic_view_list.png"
	}
    Tab {
        title: qsTr("Categories")
        content: CategoriesListTab {
            dataSource: dataService.source + "/Categories"
        }
        imageSource: "asset:///icons/ic_info.png"
    }
    Tab {
        title: qsTr("Suppliers")
        content: SuppliersListTab {
            dataSource: dataService.source + "/Suppliers"
        }
        imageSource: "asset:///icons/ic_map.png"
    }
}