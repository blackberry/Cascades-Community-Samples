/* Copyright (c) 2012 Research In Motion Limited.
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

Page {
    // The content (images, slider ...) is stacked in this Container.
    Container {
        // Set the background color to off-white.
        background: Color.create("#f8f8f8")
        topPadding: 20
        
        layout: StackLayout {
        }

        // The two images are put on top of each other in this container.
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            
            layout: DockLayout {
            }

            // This is the main night image. The night image is at the same position as the
            // day image, but further back from the viewer.
            ImageView {
                id: night
                imageSource: "asset:///images/night.jpg"
                horizontalAlignment: HorizontalAlignment.Center
            }

            // Since this day image is on top of the night one, we can hide the
            // night image with changing the opacity value of this image.
            ImageView {
                id: day
                opacity: 0
                imageSource: "asset:///images/day.jpg"
                horizontalAlignment: HorizontalAlignment.Center
            }
        }

        // This is the slider you see at the bottom of the page
        // when it gets a onImmediateValueChanged event it changes the
        // image with id night's opacity to that value.
        // Two small images are put on each side of the slider.
        Container {
            leftPadding: 20
            rightPadding: 20
            topPadding: 25
            bottomPadding: 25
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }

            // The minimum position of the slider means night-time so an image of a
            // moon is added to illustrate this.
            ImageView {
                imageSource: "asset:///images/moon.png"
                verticalAlignment: VerticalAlignment.Center
            }

            // The slider component
            Slider {
                leftMargin: 20
                rightMargin: 20
                horizontalAlignment: HorizontalAlignment.Fill
                
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1
                }
                
                onImmediateValueChanged: {
                    // This is where the day-night opacity value is done.
                    day.opacity = immediateValue;
                }
            }

            // At the maximum position of the slider means day-time so an image of a
            // sun is added to illustrate this.
            ImageView {
                imageSource: "asset:///images/sun.png"
                verticalAlignment: VerticalAlignment.Center
                
            } // Sun icon Image
        } // Slider Container
    } // Content Container
}// Page
