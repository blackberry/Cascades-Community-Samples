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

// panel for selecting object

Page {
    content: Container {
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
                id: cube
                imageSourceDefault: "asset:///images/objects/cube.png"
                imageSourceChecked: "asset:///images/objects/cubeSelected.png"
                imageSourceDisabledUnchecked: "asset:///images/objects/cube.png"
                imageSourceDisabledChecked: "asset:///images/objects/cubeSelected.png"
                imageSourcePressedUnchecked: "asset:///images/objects/cube.png"
                imageSourcePressedChecked: "asset:///images/objects/cubeSelected.png"
                checked: false
                onCheckedChanged: {
                    if (checked == true) {
                        pyramid.checked = false;
                        sphere.checked = false;
                        _goodCitizen.setModel("cube");
                    }
                }
            }
            ImageToggleButton {
                id: pyramid
                imageSourceDefault: "asset:///images/objects/pyramid.png"
                imageSourceChecked: "asset:///images/objects/pyramidSelected.png"
                imageSourceDisabledUnchecked: "asset:///images/objects/pyramid.png"
                imageSourceDisabledChecked: "asset:///images/objects/pyramidSelected.png"
                imageSourcePressedUnchecked: "asset:///images/objects/pyramid.png"
                imageSourcePressedChecked: "asset:///images/objects/pyramidSelected.png"
                checked: false
                onCheckedChanged: {
                    if (checked == true) {
                        cube.checked = false;
                        sphere.checked = false;
                        _goodCitizen.setModel("pyramid");
                    }
                }
            }
            ImageToggleButton {
                id: sphere
                imageSourceDefault: "asset:///images/objects/sphere.png"
                imageSourceChecked: "asset:///images/objects/sphereSelected.png"
                imageSourceDisabledUnchecked: "asset:///images/objects/sphere.png"
                imageSourceDisabledChecked: "asset:///images/objects/sphereSelected.png"
                imageSourcePressedUnchecked: "asset:///images/objects/sphere.png"
                imageSourcePressedChecked: "asset:///images/objects/sphereSelected.png"
                checked: false
                onCheckedChanged: {
                    if (checked == true) {
                        cube.checked = false;
                        pyramid.checked = false;
                        _goodCitizen.setModel("sphere");
                    }
                }
            }
        }
        
        onCreationCompleted: {
            var loadModel = _goodCitizen.model;
            console.log("loadModel: " + loadModel);
            
            if (loadModel == "cube") {
                cube.checked = true;
                pyramid.checked = false;
                sphere.checked = false;
            } else if (loadModel == "pyramid") {
                cube.checked = false;
                pyramid.checked = true;
                sphere.checked = false;
            } else if (loadModel == "sphere") {
                cube.checked = false;
                pyramid.checked = false;
                sphere.checked = true;
            }
        }
    }
    actionBarVisibility: ChromeVisibility.Visible    
}
