import bb.cascades 1.0

Page {
    id: shareUrlPage
    objectName: "shareUrlPage"
    
    signal updateUrl(string urlString)
    signal enableUrlSharing()
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
            text: "Share Url"
            textStyle {
                base: SystemDefaults.TextStyles.BigText
                color: Color.LightGray
                fontWeight: FontWeight.Bold
            }
        }

        Label {
            text: "Url to be Shared"
            textStyle {
	            base: SystemDefaults.TextStyles.BodyText
                color: Color.LightGray
            }
        }

        TextArea {
            id: sharedUrl
            text: qsTr("http://news.bbc.co.uk")
	        textStyle {
	            base: SystemDefaults.TextStyles.BodyText
	        }
	        onTextChanging: {
                updateUrl(sharedUrl.text);
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
	                enableUrlSharing();
	                updateUrl(sharedUrl.text);
	                logMessage("Url Sharing Enabled");
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
	                logMessage("Url Sharing Disabled");
	            }
	        }
	    }
        
	    Logger {
	        id: log
	    }
    }
}
