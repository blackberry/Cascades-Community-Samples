import bb.cascades 1.4
import SceneCover 1.0

SceneCover {    
    property string labelText
    content: Container {
        layout: DockLayout {
            
        }
        horizontalAlignment: HorizontalAlignment.Fill
        Label {
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            textStyle {
                fontFamily: 'Arial Black'
                fontSize: FontSize.XLarge
                color: Color.Red
            }
            text: labelText
        }
    }
}
