import bb.cascades 1.0

Container {

    id: myCheckBox

    property string message
    property string file
    property string thumb
    property alias checked: fileBox.checked 
    property alias checkBoxEnabled: fileBox.enabled
    
    signal myCheckChanged(bool checked)

    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }

    bottomMargin: 10

    CheckBox {
        verticalAlignment: VerticalAlignment.Center
        id: fileBox
	    onCheckedChanged: {
	        myCheckBox.myCheckChanged(checked);
	    }
    }

    ImageView {
        imageSource: "asset:///files/" + myCheckBox.thumb
        preferredWidth: 625

        onTouch: {
            if (event.isUp()) {
                checked = checked ? false : true;
            }
        }
    }
}
