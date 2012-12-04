import bb.cascades 1.0

ScrollView {
    id: logScroller

    property alias text: log.text
    
    content: Container {
        TextArea {
            id: log
            text: "Log:"
            editable: false
            textStyle {
                base: SystemDefaults.TextStyles.BodyText
                color: Color.LightGray
                lineHeight: 1.1
            }
            onTextChanged: { 
                logScroller.scrollToPoint(0, Infinity);
            }
        }
    }
}
