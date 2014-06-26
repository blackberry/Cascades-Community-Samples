/* Copyright (c) 2014 Simon HSU. bsebadmin(at)gmail.com
 * From Taiwan BB Dev Group
 */

import bb.cascades 1.2
import bb.platform 1.2
import smaatosdk 1.0
import smaatoapi 1.0

NavigationPane {
    id: navigationPane
    Menu.definition: MenuDefinition {
       
    }
    Page {
        id: page
        actionBarVisibility: ChromeVisibility.Overlay
        titleBar: TitleBar {
            id: _title
            title: qsTr("SmaatoSDKSample")
        }
        ScrollView {
            Container {
                preferredHeight: 1280
                id: mainContainer
                objectName: "_mainContainer"
                layout: StackLayout {
                }
                background: Color.Black
                Label {
                    id: adViewNormalLabel
                    text: qsTr("NormalSize Banner in QML\nSize: 320x48")
                    multiline: true

                }
                SSmaatoAdView {
                    id: adViewNormal
                    preferredWidth: 720
                    preferredHeight: 120
                    adSpaceId: "0" //Add your AdSpace ID here
                    publisherId: "0" //Add your Publisher ID here
                    viewSize: SSmaatoAdView.AdViewSizeNormal
                    format: 1
                    coppa: 0
                    gender: SSmaatoAPI.Male
                    

                    onAdUpdated: {
                        if (success) {
                            adViewNormal.autoRefreshPeriod = 0;
                            console.log("adViewNormal success");
                            // code for success update
                        } else {
                            
                            adViewNormal.autoRefreshPeriod = 10 // by seconds

                            console.log("adViewNormal failed");
                            // code for failed update
                        }
                    }
                }
                Divider {

                }
                Label {
                    id: adViewMediumLabel
                    text: qsTr("MediumSize Banner in QML\nSize: 300x250")
                    multiline: true

                }
                SSmaatoAdView {
                    id: adViewMedium
                    adSpaceId: "0"
                    publisherId: "0"
                    viewSize: SSmaatoAdView.AdViewSizeMedium
                    format: 1
                    coppa: 0
                    gender: SSmaatoAPI.Male

                    onAdUpdated: {
                        if (success) {
                            console.log("adViewMedium success");
                            adViewMedium.autoRefreshPeriod = 0
                            statusLabel.text = "Status: Ad Successfully Retrieved"
                            // code for success update
                        } else {
                            adViewMedium.autoRefreshPeriod = 10 // by seconds
                            console.log("adViewMedium failed");
                            statusLabel.text = "Status:No Ad Available at this time..Retrying with autoRefresh"
                            // code for failed update
                        }
                    }
                    
                    
                  
                }
                Divider {
                
                }
             
                Container {
                    id: cppAdContainer
                    preferredHeight: 320
                    layout: StackLayout {
                        
                    }
                    objectName: "_cppAdContainer"
                
                    Label {
                        id: cppLabel
                        text: qsTr("NormalSize Banner in Cpp \nSize: 320x48 ")
                        multiline: true
                    
                    }
                    
                }
                Divider {
                
                }                
                Container {
                    id: statusContainer
                    layout: StackLayout {
                        
                    }
                    Label {
                        id: statusLabel
                        text: qsTr("Status:")                    
                    
                    }

                 

                }

            }

        }
    }

}
