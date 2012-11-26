import bb.cascades 1.0

Page {
    id: shareDataPage
    objectName: "shareDataPage"
    
    signal updateMessage(string message, string dataType)
    signal enableDataSharing()
    signal disableSharing()
    
    function logMessage(message) {
        log.text += ( qsTr("\n") + message );
    }
                                        
    content: Container {
        layout: StackLayout {
        }

        background: Color.create ("#262626")

        topPadding: 10
        leftPadding: 30
        rightPadding: 30

        Label {
            text: "Share Data"
            textStyle {
                base: SystemDefaults.TextStyles.BigText
                color: Color.LightGray
                fontWeight: FontWeight.Bold
            }
        }

        Label {
            text: "Data to be Shared"
            textStyle {
	            base: SystemDefaults.TextStyles.BodyText
                color: Color.LightGray
            }
        }

        TextArea {
            id: message
            text: qsTr("Hello! I was shared via NFC.")
	        textStyle {
	            base: SystemDefaults.TextStyles.BodyText
	        }
	        onTextChanging: {
                updateMessage(message.text, dataType.text);
            }
            onTextChanged: {
            }
        }

        Label {
            text: "Mime type of data to be Shared"
            textStyle {
	            base: SystemDefaults.TextStyles.BodyText
                color: Color.LightGray
            }
        }

        TextArea {
            id: dataType
            text: "text/plain"
	        textStyle {
	            base: SystemDefaults.TextStyles.BodyText
	        }
	        onTextChanging: {
                updateMessage(message.text, dataType.text);
            }
            onTextChanged: {
            }
        }

	    Container {
	        layout: StackLayout {
	            orientation: LayoutOrientation.LeftToRight
	        }
	        Button {
	            id: enable
	            enabled: !_nfcSharing.sharingActive
	            text: "Enable"
                layoutProperties: StackLayoutProperties {
                                      spaceQuota: 1
                                  }
	            onClicked: {
	                enableDataSharing();
	                updateMessage(message.text, dataType.text);
	                logMessage("Data Sharing Enabled");
	            }
	        }
	        Button {
	            id: disable
	            enabled: _nfcSharing.sharingActive
	            text: "Disable"
                layoutProperties: StackLayoutProperties {
                                      spaceQuota: 1
                                  }
	            onClicked: {
	                disableSharing();
	                logMessage("Data Sharing Disabled");
	            }
	        }
	    }
        
	    Logger {
	        id: log
	    }
    }
}
