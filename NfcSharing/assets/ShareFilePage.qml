import bb.cascades 1.0

Page {
    id: shareFilePage
    objectName: "shareFilePage"
    
    signal enableFileSharing()
    signal disableSharing()
    signal updatedFileList(string paths)
    
    function logMessage(message) {
        log.text += ( qsTr("\n") + message );
    }
                                            
    function updateFileList() {
        var files = [];

        for (var i = 0; i < boxes.controls.length; i++ ) {
            if (boxes.controls[i].checked) {
                files.push(boxes.controls[i].file);
            }
        }

        updatedFileList(files.join());
    }
    
    content: Container {
        layout: StackLayout {
        }

        background: Color.create ("#262626")

        topPadding: 10
        leftPadding: 30
        rightPadding: 30

        Label {
            text: "Share Files"
            textStyle {
                base: SystemDefaults.TextStyles.BigText
                color: Color.LightGray
                fontWeight: FontWeight.Bold
            }
        }

        Label {
            text: "Check image to be Shared"
            textStyle {
	            base: SystemDefaults.TextStyles.BodyText
                color: Color.LightGray
            }
        }

        Container {
            id: boxes
            
            bottomPadding: 20

	        MyCheckBox {
	            id: box1
		        message: "File " + file
		        file: "jcmrim.png"
		        thumb: "jcmrim.png"
		        onMyCheckChanged: {
			        updateFileList();
		            logMessage(message + (checked ? " selected" : " deselected"));
		        }
	        }
	
	        MyCheckBox {
	            id: box2
		        message: "File " + file
		        file: "mdwrim.png" 
		        thumb: "mdwrim.png"
		        onMyCheckChanged: {
			        updateFileList();
		            logMessage(message + (checked ? " selected" : " deselected"));
		        }
	        }
		    
	        MyCheckBox {
	            id: box3
		        message: "File " + file
		        file: "robbieDubya.png"
		        thumb: "robbieDubya.png"
		        onMyCheckChanged: {
			        updateFileList();
		            logMessage(message + (checked ? " selected" : " deselected"));
		        }
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
	                enableFileSharing();
	                logMessage("File Sharing Enabled");
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
	                logMessage("File Sharing Disabled");
	            }
	        }
	    }
	    
	    Logger {
	        id: log
	    }
    }
}
