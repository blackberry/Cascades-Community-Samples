import bb.cascades 1.2

Page {

    Container {
        layout: DockLayout {

        }
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        Container {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

            Label {
                id: cacheSize
                property int byteSize: _app.cacheSize()

                text: "Cache size: " + bytesToSize(byteSize)
                horizontalAlignment: HorizontalAlignment.Center
                textStyle.base: SystemDefaults.TextStyles.BodyText
                textStyle.fontWeight: FontWeight.W100

                function bytesToSize(bytes) {
                    if (isNaN(bytes)) {
                        return;
                    }
                    var units = [ ' bytes', ' KB', ' MB', ' GB', ' TB', ' PB', ' EB', ' ZB', ' YB' ];
                    var amountOf2s = Math.floor(Math.log(+ bytes) / Math.log(2));
                    if (amountOf2s < 1) {
                        amountOf2s = 0;
                    }
                    var i = Math.floor(amountOf2s / 10);
                    bytes = + bytes / Math.pow(2, 10 * i);

                    // Rounds to 3 decimals places.
                    if (bytes.toString().length > bytes.toFixed(3).toString().length) {
                        bytes = bytes.toFixed(3);
                    }
                    return bytes + units[i];
                }
            }
            Button {
                id: cacheButton

                horizontalAlignment: HorizontalAlignment.Center
                text: "Clear Cache"
                onClicked: {
                    _app.clearCache()
                    cacheSize.byteSize = _app.cacheSize()
                    cacheSize.setText("Cache size: " + cacheSize.bytesToSize(cacheSize.byteSize))
                }
            }
        }
    }
}