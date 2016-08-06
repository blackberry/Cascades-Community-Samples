import bb.cascades 1.0

Container {
    property string label
    property string data

    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    horizontalAlignment: HorizontalAlignment.Fill
    
    bottomMargin: 10

    Label {
        text: label
        textStyle.fontSize: FontSize.Medium
        textStyle.fontWeight: FontWeight.W500
        verticalAlignment: VerticalAlignment.Center

        layoutProperties: StackLayoutProperties {
            spaceQuota: 3
        }
    }
    TextField {
        id: dataField
        text: data
        textStyle.fontSize: FontSize.Medium
        verticalAlignment: VerticalAlignment.Bottom

        layoutProperties: StackLayoutProperties {
            spaceQuota: 5
        }
    }
    
    function getData() {
        return dataField.text;
    }
}
