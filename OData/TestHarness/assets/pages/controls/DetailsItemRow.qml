import bb.cascades 1.0

Container {
    property string label
    property string data
    
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    horizontalAlignment: HorizontalAlignment.Fill

    Label {
        text: label
        textStyle.fontSize: FontSize.Medium
        textStyle.fontWeight: FontWeight.W500
        verticalAlignment: VerticalAlignment.Top
        
        layoutProperties: StackLayoutProperties {
            spaceQuota: 3
        }
    }
    Label {
        text: data
        textStyle.fontSize: FontSize.Medium
        verticalAlignment: VerticalAlignment.Top
        multiline: true 
        
        layoutProperties: StackLayoutProperties {
            spaceQuota: 5
        }
    }
}
