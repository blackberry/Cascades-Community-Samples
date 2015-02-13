/* Copyright (c) 2012, 2013, 2014, 2015 BlackBerry Limited.
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
import bb.cascades 1.4

Page {
    // Set the background color of this container to off-white
    // and stack the images and the slider in this container
    Container {
        id: containerID
        background: Color.create("#f8f8f8")
        topPadding: ui.du(2.0)
        
        layout: StackLayout {
        }

        // Put the two images on top of each other 
        // in the images container
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            
            layout: DockLayout {
            }

            // Add the night image at the same position  
            // as the day image and behind it
            ImageView {
                id: night
                imageSource: "asset:///images/night.jpg"
                horizontalAlignment: HorizontalAlignment.Center
            }

            // Add the day image in front of the night image and
            // hide the day image by setting its opacity value to 0
            ImageView {
                id: day
                opacity: 0
                imageSource: "asset:///images/day.jpg"
                horizontalAlignment: HorizontalAlignment.Center
            }
        } // End of the images container

        // Add a container for the slider at the bottom of the page
        // When the slider gets an onImmediateValueChanged() signal,
        // change the opacity of the image so that it disaappears

        Container {
            leftPadding: ui.du(1.0)
            rightPadding: ui.du(1.0)
            topPadding: ui.du(4.0)
            bottomPadding: ui.du(2.0)
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            // The minimum position of the slider means 
            // nighttime, so add an image of a moon at this
            // side of the slider
            ImageView {
                imageSource: "asset:///images/moon.png"
                verticalAlignment: VerticalAlignment.Center
            }

            // Add the slider component
            Slider {
                leftMargin: ui.du(1.0)
                rightMargin: ui.du(1.0)
                horizontalAlignment: HorizontalAlignment.Fill
                
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                
                onImmediateValueChanged: {
                    // Change the opacity of the day image to 
                    // make it appear
                    day.opacity = immediateValue;
                }
            }

            // The maximum position of the slider means 
            // daytime, so add an image of a sun at this 
            // side of the slider
            ImageView {
                imageSource: "asset:///images/sun.png"
                verticalAlignment: VerticalAlignment.Center
                
            } // Sun icon Image
        } // Slider Container
    } // Content Container
} // Page
