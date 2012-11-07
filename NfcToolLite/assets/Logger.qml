import bb.cascades 1.0

ScrollView {
    id: logScroller

    property alias text: log.text
    
    Container {
	    TextArea { 
	        id: log
	        editable: false
	        text: qsTr("")
	        textStyle {
	            base: SystemDefaults.TextStyles.BodyText
	            color: Color.Green
	            lineHeight: 1.1
	        }
            onTextChanged: { 
                logScroller.scrollToPoint(0, Infinity);
            }
	    }
    }
}
