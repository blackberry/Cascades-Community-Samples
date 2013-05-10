import bb.cascades 1.0
import "../pages" 1.0

NavigationPane {
    id: productTab // we need an id to specify which property we are referring to
    property string title
    property string dataSource
    
    ProductList {
        title: productTab.title
        dataSource: productTab.dataSource
    }
    
    onPopTransitionEnded: {
        page.destroy();
    }
}
