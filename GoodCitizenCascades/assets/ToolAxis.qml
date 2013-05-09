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
 */import bb.cascades 1.0

// panel for changing tool axis

Page {
    content: Container {
        background: Color.create("#262626")
        
        layout: StackLayout {
        }
        
        Container {
	        background: Color.create("#262626")
	        horizontalAlignment: HorizontalAlignment.Fill
	        layout: StackLayout {
            }
        
	        ForeignWindowControl {
	            id: viewWindow
	            visible: true // becomes visible once attached
	            horizontalAlignment: HorizontalAlignment.Fill
	            verticalAlignment: VerticalAlignment.Fill
	        }
	        
	        Container {
	            leftPadding: 10
	            rightPadding: leftPadding
	            topPadding: 10
	            bottomPadding: topPadding
	            horizontalAlignment: HorizontalAlignment.Center
	            layout: StackLayout {
	                orientation: LayoutOrientation.LeftToRight
	            }
	
	            ImageToggleButton {
	                id: xAxis
	                imageSourceDefault: "asset:///images/axes/xAxis.png"
	                imageSourceChecked: "asset:///images/axes/xAxisSelected.png"
	                imageSourceDisabledUnchecked: "asset:///images/axes/xAxis.png"
	                imageSourceDisabledChecked: "asset:///images/axes/xAxisSelected.png"
	                imageSourcePressedUnchecked: "asset:///images/axes/xAxis.png"
	                imageSourcePressedChecked: "asset:///images/axes/xAxisSelected.png"
	                checked: false
	                onCheckedChanged: {
	                    if (checked == true) {
	                        yAxis.checked = false;
	                        zAxis.checked = false;
	                        _goodCitizen.setToolAxis("X");
	                   }
	                }
	            }
	            ImageToggleButton {
	                id: yAxis
	                imageSourceDefault: "asset:///images/axes/yAxis.png"
	                imageSourceChecked: "asset:///images/axes/yAxisSelected.png"
	                imageSourceDisabledUnchecked: "asset:///images/axes/yAxis.png"
	                imageSourceDisabledChecked: "asset:///images/axes/yAxisSelected.png"
	                imageSourcePressedUnchecked: "asset:///images/axes/yAxis.png"
	                imageSourcePressedChecked: "asset:///images/axes/yAxisSelected.png"
	                checked: false
	                onCheckedChanged: {
	                    if (checked == true) {
	                        xAxis.checked = false;
	                        zAxis.checked = false;
	                        _goodCitizen.setToolAxis("Y");
	                    }
	                }
	            }
	            ImageToggleButton {
	                id: zAxis
	                imageSourceDefault: "asset:///images/axes/zAxis.png"
	                imageSourceChecked: "asset:///images/axes/zAxisSelected.png"
	                imageSourceDisabledUnchecked: "asset:///images/axes/zAxis.png"
	                imageSourceDisabledChecked: "asset:///images/axes/zAxisSelected.png"
	                imageSourcePressedUnchecked: "asset:///images/axes/zAxis.png"
	                imageSourcePressedChecked: "asset:///images/axes/zAxisSelected.png"
	                checked: false
	                onCheckedChanged: {
	                    if (checked == true) {
	                        xAxis.checked = false;
	                        yAxis.checked = false;
	                        _goodCitizen.setToolAxis("Z");
	                    }
	                }
	            }
	        }
        }
        
        onCreationCompleted: {
            var loadAxis = _goodCitizen.toolAxis;
            console.log(loadAxis);
            if (loadAxis == "X") {
                xAxis.checked = true;
                yAxis.checked = false;
                zAxis.checked = false;
            } else if (loadAxis == "Y") {
                xAxis.checked = false;
                yAxis.checked = true;
                zAxis.checked = false;
            } else if (loadAxis == "Z") {
                xAxis.checked = false;
                yAxis.checked = false;
                zAxis.checked = true;
            }
        }
    }
/*    
    paneProperties: NavigationPaneProperties {
        backButton: ActionItem {
            title: "Previous page"
            //imageSource: "asset:///back.png"
            onTriggered: { nav.pop(); }
        }
    }
*/    
    actionBarVisibility: ChromeVisibility.Visible    
}