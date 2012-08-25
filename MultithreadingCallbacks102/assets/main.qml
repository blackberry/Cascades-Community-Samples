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
    content: Container {
        background: Color.Black
        layout: StackLayout {
            layoutDirection: LayoutDirection.TopToBottom
        }
        ScrollView {
            scrollViewProperties {
                scrollMode: ScrollMode.Vertical
            }
            Container {
                objectName: "photoContainer"
                id: photoContainer
                visible: true
                layoutProperties: StackLayoutProperties {
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image1Indicator"
                        id: image1Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image1"
                        id: image1
                        visible: false
                    }
                    Label {
                        objectName: "image1Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image1Indicator.start();
                    }
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image2Indicator"
                        id: image2Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image2"
                        id: image2
                    }
                    Label {
                        objectName: "image2Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image2Indicator.start();
                    }
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image3Indicator"
                        id: image3Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image3"
                        id: image3
                    }
                    Label {
                        objectName: "image3Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image3Indicator.start();
                    }
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image4Indicator"
                        id: image4Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image4"
                        id: image4
                    }
                    Label {
                        objectName: "image4Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image4Indicator.start();
                    }
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image5Indicator"
                        id: image5Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image5"
                        id: image5
                    }
                    Label {
                        objectName: "image5Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image5Indicator.start();
                    }
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image6Indicator"
                        id: image6Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image6"
                        id: image6
                    }
                    Label {
                        objectName: "imag6Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image6Indicator.start();
                    }
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image7Indicator"
                        id: image7Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image7"
                        id: image7
                    }
                    Label {
                        objectName: "image7Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image7Indicator.start();
                    }
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image8Indicator"
                        id: image8Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image8"
                        id: image8
                    }
                    Label {
                        objectName: "image8Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image8Indicator.start();
                    }
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image9Indicator"
                        id: image9Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image9"
                        id: image9
                    }
                    Label {
                        objectName: "image9Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image9Indicator.start();
                    }
                }
                Container {
                    preferredHeight: 500
                    ActivityIndicator {
                        objectName: "image10Indicator"
                        id: image10Indicator
                        preferredHeight: 300
                    }
                    ImageView {
                        objectName: "image10"
                        id: image10
                    }
                    Label {
                        objectName: "image10Label"
                        visible: false
                    }
                    onCreationCompleted: {
                        image10Indicator.start();
                    }
                }
            }
            onCreationCompleted: {
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/6/62/Peace_riding_in_a_triumphal_chariot_Bosio_Carrousel_-_2012-05-28.jpg", 1);
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/a/af/Crepuscular_rays_with_reflection_in_GGP.jpg", 2);
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/2/2a/Anodorhynchus_hyacinthinus_-Hyacinth_Macaw_-side_of_head.jpg", 3);
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/2/29/Bataille_Waterloo_1815_reconstitution_2011_cuirassier.jpg", 4);
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/e/ec/Armadillo_Aerospace_Pixel_Hover.jpg", 5);
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/f/f5/A_sculpture_at_the_entrance_to_the_palace_of_Versailles.jpg", 6);
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/6/6d/Firehole_river_at_Upper_Geyser_Basin-2008-june.jpg", 7);
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/7/7c/Peugeot_206_WRC.jpg", 8);
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/9/97/Toda_Hut.JPG", 9);
                cs.loadImage("http://upload.wikimedia.org/wikipedia/commons/d/dc/Marriott_Center_1.JPG", 10);
            }
        }
    }
}
