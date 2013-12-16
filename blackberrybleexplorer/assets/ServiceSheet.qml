/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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

/*
 * Nomenclature:
 * icon - the visual representation of a slidable or selectable "thing"
 * item - the thing itself, usually modelled in C++ and having an index which is its position in an underlying list
 * centre_offset - one of three icon positions on the screen, relative to the centre position which is position 0 plus two off screen positions, -2 (left of screen) and +2 (right of screen)
 */

import bb.cascades 1.2
import bb.displayInfo 1.0
import CustomTimer 1.0


Sheet {
    id: root

	Page {
	    
	    Container {
	        id: mainPage
	        objectName: "mainPage"
	        
	        property int centre_x: 640
	        property int centre_y: 384
	        property int page_width: 0
	        property int page_height: 0
	        property int service_count: 0
	        property int central_item_inx: -9 // index to the data model of the item which is represented by the central icon on screen. -9 to ensure onCentral_item_inxChanged is triggered even if we have no items
	        property int right: 1
	        property int left: 2
            property string currentPeripheralName: ""
            property string currentPeripherAddress: ""
            
	        onCreationCompleted: {
	            page_height = displayInfo.pixelSize.height;
	            page_width = displayInfo.pixelSize.width;
	            centre_y = displayInfo.pixelSize.width / 2;
	            // this may look the wrong way round but... the app runs in landscape orientation only so it's correct
	            centre_x = displayInfo.pixelSize.height / 2;
	            centre_y = displayInfo.pixelSize.width / 2;
	            console.log("CCCC set centre_x=" + centre_x + " and centre_y=" + centre_y);
	            smgr.setServiceCount.connect(setServiceCount);
                smgr.peripheralAddressChanged.connect(peripheralAddressChanged);
                smgr.peripheralNameChanged.connect(peripheralNameChanged);
	        }
            
            function peripheralAddressChanged(address) {
                console.log("CCCC ServiceSheet.qml peripheralAddressChanged");
                currentPeripherAddress = address;
            }
            
            function peripheralNameChanged(name) {
                console.log("CCCC ServiceSheet.qml peripheralNameChanged");
                currentPeripheralName = name;
            }
            
	        onCentral_item_inxChanged: {
	            if (service_count > 0) {
	                labels.attr1_text = smgr.getServiceName(mainPage.central_item_inx);
	                labels.attr2_text = smgr.getServiceUuid(mainPage.central_item_inx);
	            } else {
	                labels.attr1_text = "No items in list";
	                labels.attr2_text = "";
	                attr2_timer.start();
	            }
	        }
	        
	        function setServiceCount(count) {
	            console.log("CCCC service_count=" + count);
	            service_count = count;
	            
	            arrow_left.visible = false;
	            arrow_right.visible = false;
	            
	            service_left_offscreen.visible = true
	            service_icon0.visible = true
	            service_icon1.visible = true
	            service_icon2.visible = true
	            service_right_offscreen.visible = true
	            
	            service_left_offscreen.setImage("asset:///images/service_orange_3.png");
	            service_icon0.setImage("asset:///images/service_orange_3.png");
	            service_icon1.setImage("asset:///images/service_orange_3.png");
	            service_icon2.setImage("asset:///images/service_orange_3.png");
	            service_right_offscreen.setImage("asset:///images/service_orange_3.png");
	            
	            if (count == 0) {
	                // <blank> | <blank> | <blank>
	                central_item_inx = -1;
	                service_left_offscreen.visible = false
	                service_icon0.visible = false
	                service_icon1.visible = false
	                service_icon2.visible = false
	                service_right_offscreen.visible = false
	            }
	            if (count == 1) {
	                // <blank> | <0> | <blank>
	                central_item_inx = 0;
	                service_left_offscreen.visible = false
	                service_icon0.visible = false
	                service_icon1.visible = true
	                service_icon2.visible = false
	                service_right_offscreen.visible = false
	            }
	            if (count == 2) {
	                // <1> | <0> | <blank>
	                central_item_inx = 0;
	                arrow_left.visible = false;
	                arrow_right.visible = true;
	                service_left_offscreen.setImage("asset:///images/blank.png");
	                service_right_offscreen.setImage("asset:///images/blank.png");
	                service_icon2.setImage("asset:///images/blank.png");
	            }
	            if (count > 2) {
	                // <2> | <1> | <0>
	                central_item_inx = 1;
	                arrow_left.visible = false;
	                arrow_right.visible = true;
	            }
	            
	            if (count > 3) {
	                console.log("CCCC we have more than three items!");
	            }
	            
	            resetIconPositions(right);
	            setArrowVisibility();
	            
	            labels.attr1_text = "Found "+service_count+" services";
	            attr1_timer.start();
	            labels.attr2_text = "Determining details";
	            attr2_timer.start();
	        }
	        
	        function setArrowVisibility() {
	            var arrow_left_visible = false;
	            var arrow_right_visible = false;
	            if (service_count > 1) {
	                if (central_item_inx > 0) {
	                    arrow_left_visible = true;
	                }
	                if ((central_item_inx + 1) < service_count) {
	                    arrow_right_visible = true;
	                }
	            }
	            arrow_left.visible = arrow_left_visible;
	            arrow_right.visible = arrow_right_visible;
	        }
	        
	        function resetIconPositions(direction) {
	            // put the ItemService components back in their default positions but switch off implicit animation so that the "movement" is not animated and therefore not visible to the user
	            
	            console.log("CCCC ++++ RESET ICON POSITIONS")
	            service_left_offscreen.disableTranslationAnimation();
	            service_icon0.disableTranslationAnimation();
	            service_icon1.disableTranslationAnimation();
	            service_icon2.disableTranslationAnimation();
	            service_right_offscreen.disableTranslationAnimation();
	            
	            if (service_count == 2) {
	                if (direction == left) {
	                    service_icon0.setImage("asset:///images/blank.png");
	                    service_icon2.setImage("asset:///images/service_orange_3.png");
	                } else {
	                    service_icon0.setImage("asset:///images/service_orange_3.png");
	                    service_icon2.setImage("asset:///images/blank.png");
	                }
	            }
	            
	            service_left_offscreen.setOffset(-2);
	            service_icon0.setOffset(-1);
	            service_icon1.setOffset(0);
	            service_icon2.setOffset(1);
	            service_right_offscreen.setOffset(2)
	            
	            service_left_offscreen.enableTranslationAnimation();
	            service_icon0.enableTranslationAnimation();
	            service_icon1.enableTranslationAnimation();
	            service_icon2.enableTranslationAnimation();
	            service_right_offscreen.enableTranslationAnimation();
	            
	            console.log("CCCC ---- RESET ICON POSITIONS")
	        }
	        
	        function slideItemsLeft() {
	            console.log("CCCC handling request to slide left");
	            console.log("CCCC central_item_inx=" + central_item_inx);
	            
	            if (arrow_left.visible == false) {
	                return;
	            }
	            
	            console.log("CCCC sliding left");
	            
	            resetIconPositions(left);
	            service_icon0.setOffset(-2)
	            service_icon1.setOffset(-1)
	            service_icon2.setOffset(0)
	            
	            // if the central item has index > 1 then there's at least one item to the right "off screen" we can slide into view
	            if (central_item_inx > 1) {
	                console.log("CCCC can slide off screen right into view");
	                service_right_offscreen.setOffset(1)
	            } else {
	                console.log("CCCC Not sliding off screen right into view so we leave an empty space");
	            }
	            
	            central_item_inx = central_item_inx - 1;
	            setArrowVisibility();
	            
	            if (service_count > 1) {
	                if (central_item_inx > 0) {
	                    service_icon2
	                }
	            }
	            
	            console.log("CCCC central_item_inx=" + central_item_inx);
	        
	        }
	        
	        function slideItemsRight() {
	            
	            console.log("CCCC handling request to slide right");
	            console.log("CCCC central_item_inx=" + central_item_inx);
	            
	            if (arrow_right.visible == false) {
	                return;
	            }
	            
	            console.log("CCCC sliding right");
	            
	            // reset positions ready to animate right
	            resetIconPositions(right);
	            service_icon0.setOffset(0);
	            service_icon1.setOffset(1);
	            service_icon2.setOffset(2); // off screen
	            
	            // if the central item has index at least 2 less than the highest index then there's at least one item to the left "off screen" we can slide into view
	            if ((service_count - central_item_inx) > 2) {
	                console.log("CCCC must slide off screen left into view");
	                service_left_offscreen.setOffset(-1) // on screen
	            } else {
	                console.log("CCCC Not sliding off screen left into view so we leave an empty space");
	            
	            }
	            
	            central_item_inx = central_item_inx + 1;
	            setArrowVisibility();
	            console.log("CCCC central_item_inx=" + central_item_inx);
	        }
	        
	        function centralItemSelected() {
	            console.log("CCCC centralItemSelected: selected item has item index " + central_item_inx);
	            if (central_item_inx > -1) {
                    smgr.selectService(smgr.getServiceUuid(central_item_inx));
                    charSheet.open();
                    charSheet.setPeekEnabled(false);
	            }
	        }
	        
	        background: back.imagePaint
	        attachedObjects: [
	            ImagePaintDefinition {
	                id: back
	                repeatPattern: RepeatPattern.Fill
	                imageSource: "asset:///images/bg.png"
	            },
	            DisplayInfo {
	                id: displayInfo
	            }
	        ]
	        
	        function startActivityIndicator() {
	            activityIndicator.start();
	        }
	        
	        function stopActivityIndicator() {
	            activityIndicator.stop();
	        }
	        
	        ActivityIndicator {
	            id: activityIndicator
	            preferredWidth: 300
	            horizontalAlignment: HorizontalAlignment.Center
	            verticalAlignment: VerticalAlignment.Center
	        }
	        
	        // make the container of ItemServices float under the TouchHandler
	        layout: DockLayout {
	        }
	        ServicesTopLabels {
	            id: top_label
	        }
	        Container {
	            id: item_container
	            layout: AbsoluteLayout {
	            }
	            verticalAlignment: VerticalAlignment.Fill
	            horizontalAlignment: HorizontalAlignment.Fill
	            ItemService {
	                id: service_left_offscreen
	                service_identifier: 1
	                visible: true
	                centre_offset: -2
	            }
	            ItemService {
	                id: service_icon0
	                service_identifier: 2
	                visible: true
	                centre_offset: -1
	            }
	            ItemService {
	                id: service_icon1
	                service_identifier: 3
	                visible: true
	                centre_offset: 0
	            }
	            ItemService {
	                id: service_icon2
	                service_identifier: 4
	                visible: true
	                centre_offset: 1
	            }
	            ItemService {
	                id: service_right_offscreen
	                service_identifier: 5
	                visible: true
	                centre_offset: 2
	            }
	        }
	        Container {
	            id: arrow_left
	            leftPadding: 5
	            horizontalAlignment: HorizontalAlignment.Left
	            verticalAlignment: VerticalAlignment.Center
	            onTouch: {
	                if (event.isUp()) {
	                    slideItemsLeft();
	                }
	            }
	            ImageView {
	                id: leftMore
	                imageSource: "asset:///images/wedge_left_orange.png"
	            }
	        }
	        Container {
	            id: arrow_right
	            rightPadding: 5
	            horizontalAlignment: HorizontalAlignment.Right
	            verticalAlignment: VerticalAlignment.Center
	            onTouch: {
	                if (event.isUp()) {
	                    slideItemsRight();
	                }
	            }
	            ImageView {
	                id: rightMore
	                imageSource: "asset:///images/wedge_right_orange.png"
	            }
	        }
	        Labels {
	            id: labels
                text_color: Color.create("#ffa500");
	        }
	        Timer {
	            id: attr1_timer
	            time_limit: 3000
	            onTimeout: {
	                labels.attr1_text = smgr.getServiceName(mainPage.central_item_inx);
	                attr1_timer.stop();
	            }
	        }
	        Timer {
	            id: attr2_timer
	            time_limit: 5000
	            onTimeout: {
	                labels.attr2_text = smgr.getServiceUuid(mainPage.central_item_inx);
	                attr2_timer.stop();
	            }
	        }
	        
	        TouchHandler {
	            verticalAlignment: VerticalAlignment.Fill
	            horizontalAlignment: HorizontalAlignment.Fill
	        }
	    }
	    
	    attachedObjects: [
	        CharacteristicSheet {
	            id: charSheet
	        }
	    ]
	    
        actions: [
            ActionItem {
                id: action_close
                title: "Close"
                imageSource: "asset:///images/previous.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    root.close();
                }
            }
        ]
	}
}