import bb.cascades 1.0

Page {
    id: shareFilePage
    objectName: "shareFilePage"
    
    signal enableFileSharing()
    signal disableSharing()
    signal updatedFileList(string paths)
    function logMessage(message) {
        log.text += (qsTr("\n") + message );
    }
    function updateFileList() {
        var files = [
        ];
        if (_nfcSharing.sharingActive) {
	        for (var i = 0; i < boxes.controls.length; i ++) {
	            if (boxes.controls[i].checked) {
	                files.push(boxes.controls[i].file);
	            }
	        }
        	updatedFileList(files.join());
        }
    }
    content: Container {
        layout: StackLayout {
        }
        background: Color.create("#262626")
        topPadding: 10
        leftPadding: 30
        rightPadding: 30
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 100
            }
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
            ScrollView {
                Container {
                    id: boxes
                    bottomPadding: 20
                    MyCheckBox {
                        id: box1
                        checkBoxEnabled: _nfcSharing.sharingActive
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
                        checkBoxEnabled: _nfcSharing.sharingActive
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
                        checkBoxEnabled: _nfcSharing.sharingActive
                        message: "File " + file
                        file: "robbieDubya.png"
                        thumb: "robbieDubya.png"
                        onMyCheckChanged: {
                            updateFileList();
                            logMessage(message + (checked ? " selected" : " deselected"));
                        }
                    }
                    MyCheckBox {
                        id: box4
                        checkBoxEnabled: _nfcSharing.sharingActive
                        message: "File " + file
                        file: "_MG_0818.jpg"
                        thumb: "_MG_0818_thumb.jpg"
                        onMyCheckChanged: {
                            updateFileList();
                            logMessage(message + (checked ? " selected" : " deselected"));
                        }
                    }
                    MyCheckBox {
                        id: box5
                        checkBoxEnabled: _nfcSharing.sharingActive
                        message: "File " + file
                        file: "_MG_1308.jpg"
                        thumb: "_MG_1308_thumb.jpg"
                        onMyCheckChanged: {
                            updateFileList();
                            logMessage(message + (checked ? " selected" : " deselected"));
                        }
                    }
                    MyCheckBox {
                        id: box6
                        checkBoxEnabled: _nfcSharing.sharingActive
                        message: "File " + file
                        file: "_MG_2174.jpg"
                        thumb: "_MG_2174_thumb.jpg"
                        onMyCheckChanged: {
                            updateFileList();
                            logMessage(message + (checked ? " selected" : " deselected"));
                        }
                    }
                    MyCheckBox {
                        id: box7
                        checkBoxEnabled: _nfcSharing.sharingActive
                        message: "File " + file
                        file: "_MG_4353.jpg"
                        thumb: "_MG_4353_thumb.jpg"
                        onMyCheckChanged: {
                            updateFileList();
                            logMessage(message + (checked ? " selected" : " deselected"));
                        }
                    }
                    MyCheckBox {
                        id: box8
                        checkBoxEnabled: _nfcSharing.sharingActive
                        message: "File " + file
                        file: "_MG_5447.jpg"
                        thumb: "_MG_5447_thumb.jpg"
                        onMyCheckChanged: {
                            updateFileList();
                            logMessage(message + (checked ? " selected" : " deselected"));
                        }
                    }
                    MyCheckBox {
                        id: box9
                        checkBoxEnabled: _nfcSharing.sharingActive
                        message: "File " + file
                        file: "_MG_8240.jpg"
                        thumb: "_MG_8240_thumb.jpg"
                        onMyCheckChanged: {
                            updateFileList();
                            logMessage(message + (checked ? " selected" : " deselected"));
                        }
                    }
                    MyCheckBox {
                        id: box10
                        checkBoxEnabled: _nfcSharing.sharingActive
                        message: "File " + file
                        file: "_MG_8558.jpg"
                        thumb: "_MG_8558_thumb.jpg"
                        onMyCheckChanged: {
                            updateFileList();
                            logMessage(message + (checked ? " selected" : " deselected"));
                        }
                    }
                    MyCheckBox {
                        id: box11
                        checkBoxEnabled: _nfcSharing.sharingActive
                        message: "File " + file
                        file: "_MG_8746.jpg"
                        thumb: "_MG_8746_thumb.jpg"
                        onMyCheckChanged: {
                            updateFileList();
                            logMessage(message + (checked ? " selected" : " deselected"));
                        }
                    }
                }
            }
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                Button {
                    id: enable
                    enabled: ! _nfcSharing.sharingActive
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
        }
        Container {
            layoutProperties: StackLayoutProperties {
                spaceQuota: 30
            }
            Logger {
                id: log
            }
        }
    }
}
