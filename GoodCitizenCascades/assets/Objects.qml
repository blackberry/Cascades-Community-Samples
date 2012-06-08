import bb.cascades 1.0

// Color sliders for changing object color
Page {
    content: Container {
        background: Color.create ("#262626")
        preferredWidth: 768
        
        layout: StackLayout {
        }
        
        Container {
            layout: StackLayout {
                layoutDirection: LayoutDirection.LeftToRight
                leftPadding: 20
                rightPadding: leftPadding
                topPadding: 20
                bottomPadding: topPadding
            }

	        layoutProperties: StackLayoutProperties {
	            horizontalAlignment: HorizontalAlignment.Center
	        }


	        Container {
	            layout: DockLayout {
	                leftPadding: 20
	                rightPadding: leftPadding
	            }
	
		        layoutProperties: StackLayoutProperties {
		            horizontalAlignment: HorizontalAlignment.Center
		        }
		        
		        ImageView {
		            id: cubeSelected
		            imageSource: "asset:///images/objects/cubeSelected.png"
		            preferredWidth: 200
		            preferredHeight: 200
		            opacity: 0.0
		            
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }
		        }
	
		        ImageView {
		            id: cube
		            imageSource: "asset:///images/objects/cube.png"
		            preferredWidth: 200
		            preferredHeight: 200
		
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }

	                onTouch: {
	                    cube.opacity = 0.0;
	                    cubeSelected.opacity = 1.0;
	                    pyramid.opacity = 1.0;
	                    pyramidSelected.opacity = 0.0;
	                    sphere.opacity = 1.0;
	                    sphereSelected.opacity = 0.0;
	                    
	                    _goodCitizen.setModel("cube");                    
	                }
		        }
		        
	        }


	        Container {
	            layout: DockLayout {
	                leftPadding: 20
	                rightPadding: leftPadding
	            }
	
		        layoutProperties: StackLayoutProperties {
		            horizontalAlignment: HorizontalAlignment.Center
		        }
		        
		        ImageView {
		            id: pyramidSelected
		            imageSource: "asset:///images/objects/pyramidSelected.png"
		            preferredWidth: 200
		            preferredHeight: 200
		            opacity: 0.0
		            
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }
		        }
	
		        ImageView {
		            id: pyramid
		            imageSource: "asset:///images/objects/pyramid.png"
		            preferredWidth: 200
		            preferredHeight: 200
		
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }

	                onTouch: {
	                    cube.opacity = 1.0;
	                    cubeSelected.opacity = 0.0;
	                    pyramid.opacity = 0.0;
	                    pyramidSelected.opacity = 1.0;
	                    sphere.opacity = 1.0;
	                    sphereSelected.opacity = 0.0;
	                    
	                    _goodCitizen.setModel("pyramid");                    
	                }
		        }
		        
	        }



	        Container {
	            layout: DockLayout {
	                leftPadding: 20
	                rightPadding: leftPadding
	            }
	
		        layoutProperties: StackLayoutProperties {
		            horizontalAlignment: HorizontalAlignment.Center
		        }
		        
		        ImageView {
		            id: sphereSelected
		            imageSource: "asset:///images/objects/sphereSelected.png"
		            preferredWidth: 200
		            preferredHeight: 200
		            opacity: 0.0
		            
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }
		        }		        
	
		        ImageView {
		            id: sphere
		            imageSource: "asset:///images/objects/sphere.png"
		            preferredWidth: 200
		            preferredHeight: 200
		
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }

	                onTouch: {
	                    cube.opacity = 1.0;
	                    cubeSelected.opacity = 0.0;
	                    pyramid.opacity = 1.0;
	                    pyramidSelected.opacity = 0.0;
	                    sphere.opacity = 0.0;
	                    sphereSelected.opacity = 1.0;
	                    
	                    _goodCitizen.setModel("sphere");                    
	                }
		        }
	        }
        }
        
        ForeignWindow {
             id: viewWindow
             preferredWidth: 768
             preferredHeight: 950
             visible: true // becomes visible once attached

	         layoutProperties: DockLayoutProperties {
	              horizontalAlignment: HorizontalAlignment.Center
	              verticalAlignment: VerticalAlignment.Top
	         }
        }
          
	    onCreationCompleted : {
            var loadModel = _goodCitizen.model;
            console.log(loadModel);

            cube.opacity = 1.0;
            pyramid.opacity = 1.0;
            sphere.opacity = 1.0;
            cubeSelected.opacity = 0.0;
            pyramidSelected.opacity = 0.0;
            sphereSelected.opacity = 0.0;
            
            if (loadModel == "cube") {
	            cube.opacity = 0.0;
	            cubeSelected.opacity = 1.0;
            } else
            if (loadModel == "pyramid") {
	            pyramidSelected.opacity = 1.0;
	            pyramid.opacity = 0.0;
            } else
            if (loadModel == "sphere") {
	            sphereSelected.opacity = 1.0;
	            sphere.opacity = 0.0;
            }
        }
        
    }
/*        
    actions: [
        ActionItem {
            title: "Back"
            imageSource: "asset:///images/actions/back.png"
            
            onTriggered: {
                // _navPane is set in code to make it available for all recipe pages.
                _navPane.pop ();
            }
        }
    ]
*/  
}
