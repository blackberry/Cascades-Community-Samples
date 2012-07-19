/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
                topPadding: 10
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
		            id: xAxisSelected
		            imageSource: "asset:///images/axes/xAxisSelected.png"
		            preferredWidth: 200
		            preferredHeight: 200
		            opacity: 0.0
		            
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }
		        }
	
		        ImageView {
		            id: xAxis
		            imageSource: "asset:///images/axes/xAxis.png"
		            preferredWidth: 200
		            preferredHeight: 200
		
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }

	                onTouch: {
	                    xAxis.opacity = 0.0;
	                    xAxisSelected.opacity = 1.0;
	                    yAxis.opacity = 1.0;
	                    yAxisSelected.opacity = 0.0;
	                    zAxis.opacity = 1.0;
	                    zAxisSelected.opacity = 0.0;
	                    
	                    _goodCitizen.setToolAxis("X");                    
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
		            id: yAxisSelected
		            imageSource: "asset:///images/axes/yAxisSelected.png"
		            preferredWidth: 200
		            preferredHeight: 200
		            opacity: 0.0
		            
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }
		        }
	
		        ImageView {
		            id: yAxis
		            imageSource: "asset:///images/axes/yAxis.png"
		            preferredWidth: 200
		            preferredHeight: 200
		
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }

	                onTouch: {
	                    xAxis.opacity = 1.0;
	                    xAxisSelected.opacity = 0.0;
	                    yAxis.opacity = 0.0;
	                    yAxisSelected.opacity = 1.0;
	                    zAxis.opacity = 1.0;
	                    zAxisSelected.opacity = 0.0;
	                    
	                    _goodCitizen.setToolAxis("Y");                    
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
		            id: zAxisSelected
		            imageSource: "asset:///images/axes/zAxisSelected.png"
		            preferredWidth: 200
		            preferredHeight: 200
		            opacity: 0.0
		            
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }
		        }		        
	
		        ImageView {
		            id: zAxis
		            imageSource: "asset:///images/axes/zAxis.png"
		            preferredWidth: 200
		            preferredHeight: 200
		
		            layoutProperties: DockLayoutProperties {
		                horizontalAlignment: HorizontalAlignment.Center
		                verticalAlignment: VerticalAlignment.Center
		            }

	                onTouch: {
	                    xAxis.opacity = 1.0;
	                    xAxisSelected.opacity = 0.0;
	                    yAxis.opacity = 1.0;
	                    yAxisSelected.opacity = 0.0;
	                    zAxis.opacity = 0.0;
	                    zAxisSelected.opacity = 1.0;
	                    
	                    _goodCitizen.setToolAxis("Z");                    
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
            var loadAxis = _goodCitizen.toolAxis;
            console.log(loadAxis);

            xAxis.opacity = 1.0;
            yAxis.opacity = 1.0;
            zAxis.opacity = 1.0;
            xAxisSelected.opacity = 0.0;
            yAxisSelected.opacity = 0.0;
            zAxisSelected.opacity = 0.0;
            
            if (loadAxis == "X") {
	            xAxis.opacity = 0.0;
	            xAxisSelected.opacity = 1.0;
            } else
            if (loadAxis == "Y") {
	            yAxisSelected.opacity = 1.0;
	            yAxis.opacity = 0.0;
            } else
            if (loadAxis == "Z") {
	            zAxisSelected.opacity = 1.0;
	            zAxis.opacity = 0.0;
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
