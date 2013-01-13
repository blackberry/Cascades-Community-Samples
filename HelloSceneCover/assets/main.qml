// Default empty project template
import bb.cascades 1.0

// creates one page with a label
Page {
    Container {
        layout: DockLayout {}
        Label {
            text: qsTr("Hello World")
            textStyle.base: SystemDefaults.TextStyles.BigText
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
    }
    attachedObjects: [
	    AppCover {
	        id: appCover
	    }
	 ]
        
	onCreationCompleted: {
	    
			Application.setCover(appCover);
			Application.manualExit.connect(exitApp);
			Application.setAutoExit(false);
			
	    }     
    
    
    function exitApp()
    {
        /* This is needed to stop the application generating a 
            "application has been slayed" error on close. Should
            not be needed on final release of BB10!!
        */
        Application.setCover(0);
        
        Application.quit();   
    } 
}



