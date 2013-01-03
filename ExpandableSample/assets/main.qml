// Default empty project template
import bb.cascades 1.0

// creates one page with a label
Page {
    Container {
        layout: StackLayout {}
        Label {
            text: qsTr("Hello World")
            textStyle.base: SystemDefaults.TextStyles.BigText
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
        ExpandableItem {
            headerText: "click to expand 1"
            bodyText: "this is my first custom body text"
            expandImage: "asset:///images/expand.png";
            collapseImage: "asset:///images/collapse.png";
            bodyVisible: false
        }
        ExpandableItem {
            headerText: "click to expand 2"
            bodyText: "this is my second custom body text"
            expandImage: "asset:///images/expand.png";
            collapseImage: "asset:///images/collapse.png";
            bodyVisible: true
        }
    }
    
}

