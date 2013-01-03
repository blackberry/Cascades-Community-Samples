import bb.cascades 1.0

Container {
    property alias headerText: btnHeader.text
    property alias bodyText: lblBody.text
    property alias bodyVisible: lblBody.visible
    property string collapseImage
    property string expandImage
    onBodyVisibleChanged: {
        if (bodyVisible) {
            btnHeader.imageSource = collapseImage;
        } else {
            btnHeader.imageSource = expandImage;
        }
    }
    Button {
        id: btnHeader
        text: "default header"
        preferredWidth: maxWidth
        imageSource: collapseImage
        onClicked: {
            if (bodyVisible) {
                lblBody.visible = false;
                btnHeader.imageSource = expandImage;
            } else {
                lblBody.visible = true;
                btnHeader.imageSource = collapseImage;
            }
        }
    }
    Label {
        id: lblBody
        text: "default body"
        textStyle.fontStyle: FontStyle.Italic
    }
}
