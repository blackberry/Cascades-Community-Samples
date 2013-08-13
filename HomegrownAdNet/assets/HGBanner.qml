/* Copyright (c) 2013 BlackBerry Limited.
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
import std.qt4 4.8 //Imported for QTimer

/**
 * This is the brain of the HomegrownAdNet sample, the HGBanner contains the 
 * ListView housing all of the ads and the logic to control the auto-scroll
 * of those ads.
 * 
 * To change how the ListView functions, just jump down to the ListView 
 * declaration section
 */

Container {
    id: banner
    
    // The width and height limits of the banner are hard set to prevent odd
    //   blank space from appearing around the Container
    property alias width: banner.maxWidth
    property alias height: banner.maxHeight
    //The rate the carousel scrolls is set in seconds, defaulting to 60
    property int refreshRate: 60
    attachedObjects: [
        //This QTimer instance controls when the carousel scrolls and also has 
        // logic to transition from the last list element back to the first
        QTimer {
            id: intervalTimer
            property int index: 0
            interval: banner.refreshRate * 1000 //Multiply by 1000 to convert from ms to s
            
            //Whenever the timeout occurs we increment the index and move the list
            onTimeout: {
                intervalTimer.index = ++ intervalTimer.index % adsDataModel.childCount("")
                if (intervalTimer.index == 0) {
                    //If the index is 0 it means the list has ended, so we use a fade to 
                    // help the transition occur more smoothly
                    fadeOut.play();
                } else {
                    //Scroll to the next item in the list using the default animation
                    adCarousel.scrollToItem([ intervalTimer.index ], ScrollAnimation.Default);
                }
            }
        },
        
        //The next two animations perform a fade transition so we can scroll back to the start
        // of the list without a harsh native effect
        FadeTransition {
            id: fadeOut
            target: adCarousel
            fromOpacity: 1.0
            toOpacity: 0.0
            duration: 200
            onEnded: {
                adCarousel.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.None);
                fadeIn.play();
            }
        },
        FadeTransition {
            id: fadeIn
            target: adCarousel
            fromOpacity: 0.0
            toOpacity: 1.0
            duration: 200
        }
    ]
    
    //This is where the magic happens! The ListView that displays all the ads
    ListView {
        id: adCarousel
        //snapMode and flickMode aren't really used since we control the scroll,
        // but if you wanted to change the behaviour to let the user scroll these
        // should be used
        snapMode: SnapMode.LeadingEdge
        flickMode: FlickMode.SingleItem
        scrollIndicatorMode: ScrollIndicatorMode.None

        layout: StackListLayout {
            //This controls the way the items in the list are laid out
            orientation: LayoutOrientation.TopToBottom
        }
        dataModel: XmlDataModel {
            id: adsDataModel
            //To add your own ads modify the file below
            source: "asset:///models/admodel.xml"
        }
        // This sample supports 2 types of ad units:
        //	1) BlackBerryWorldAdUnit - Launches BlackBerry World to the target app
        //	2) InternetAdUnit - Launches the browser to the target URL
        // Both of the above support animated GIFs 
        listItemComponents: [
            ListItemComponent {
                type: "BlackBerryWorldAdUnit"
                BlackBerryWorldAdUnit {
                    imageSource: "images/advertimages/" + ListItemData.image
                    uri: ListItemData.uri
                    name: ListItemData.name
                    animated: parseInt(ListItemData.animated) == 1
                }
            },
            ListItemComponent {
                type: "InternetAdUnit"
                InternetAdUnit {
                    imageSource: "images/advertimages/" + ListItemData.image
                    uri: ListItemData.uri
                    name: ListItemData.name
                    animated: parseInt(ListItemData.animated) == 1
                }
            }
        ]
        onCreationCompleted: {
            //When the ListView is created we start the timer
            intervalTimer.start();
        }
    }

	//In the below section we listen for user touches and let them stay on the current ad
	// as long as they hold on it, starting the timer back up when the user stops
    onTouch: {
        if (event.touchType == TouchType.Up || event.touchType == TouchType.Cancel) {
            intervalTimer.start();
            adCarousel.touchPropagationMode = TouchPropagationMode.Full;
        } else {
            intervalTimer.stop();
            adCarousel.touchPropagationMode = TouchPropagationMode.PassThrough;
        }
    }

}
