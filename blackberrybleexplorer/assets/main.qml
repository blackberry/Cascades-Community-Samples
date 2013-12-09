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


Page {

    Container {
        id: mainPage
        objectName: "mainPage"

        property int centre_x: 640
        property int centre_y: 384
        property int page_width: 0
        property int page_height: 0
        property int device_count: 0
        property int central_item_inx: -9 // index to the data model of the item which is represented by the central icon on screen. -9 to ensure onCentral_item_inxChanged is triggered even if we have no items
        property int right: 1
        property int left: 2

        onCreationCompleted: {
            page_height = displayInfo.pixelSize.height;
            page_width = displayInfo.pixelSize.width;
            centre_y = displayInfo.pixelSize.width / 2;
            // this may look the wrong way round but... the app runs in landscape orientation only so it's correct
            centre_x = displayInfo.pixelSize.height / 2;
            centre_y = displayInfo.pixelSize.width / 2;
            console.log("QQQQ set centre_x=" + centre_x + " and centre_y=" + centre_y);
        }

        onCentral_item_inxChanged: {
            if (device_count > 0) {
                labels.attr1_text = data.getDeviceName(mainPage.central_item_inx);
                labels.attr2_text = data.getDeviceAddr(mainPage.central_item_inx);
                labels.attr3_text = deviceRelationship();
            } else {
                labels.attr1_text = "No items in list";
                labels.attr2_text = "Drag down for menu";
                labels.attr3_text = "";
                attr2_timer.start();
            }
        }
        
        function deviceRelationship() {
            var relationship = "unknown";
            if (data.isKnown(mainPage.central_item_inx)) {
                relationship = "known";
                console.log("QQQQ device is known");
            }
            if (data.isPaired(mainPage.central_item_inx)) {
                relationship = "paired";
                console.log("QQQQ device is paired");
            }
            console.log("QQQQ relationship with device is "+relationship);
            return relationship;
        }

        function setDeviceCount(count) {
            console.log("QQQQ device_count=" + count);
            device_count = count;

            arrow_left.visible = false;
            arrow_right.visible = false;

            device_left_offscreen.visible = true
            device_icon0.visible = true
            device_icon1.visible = true
            device_icon2.visible = true
            device_right_offscreen.visible = true

            device_left_offscreen.setImage("asset:///images/bluetooth.png");
            device_icon0.setImage("asset:///images/bluetooth.png");
            device_icon1.setImage("asset:///images/bluetooth.png");
            device_icon2.setImage("asset:///images/bluetooth.png");
            device_right_offscreen.setImage("asset:///images/bluetooth.png");

            if (count == 0) {
                // <blank> | <blank> | <blank>
                central_item_inx = -1;
                device_left_offscreen.visible = false
                device_icon0.visible = false
                device_icon1.visible = false
                device_icon2.visible = false
                device_right_offscreen.visible = false
            }
            if (count == 1) {
                // <blank> | <0> | <blank>
                central_item_inx = 0;
                device_left_offscreen.visible = false
                device_icon0.visible = false
                device_icon1.visible = true
                device_icon2.visible = false
                device_right_offscreen.visible = false
            }
            if (count == 2) {
                // <1> | <0> | <blank>
                central_item_inx = 0;
                arrow_left.visible = false;
                arrow_right.visible = true;
                device_left_offscreen.setImage("asset:///images/blank.png");
                device_right_offscreen.setImage("asset:///images/blank.png");
                device_icon2.setImage("asset:///images/blank.png");
            }
            if (count > 2) {
                // <2> | <1> | <0>
                central_item_inx = 1;
                arrow_left.visible = false;
                arrow_right.visible = true;
            }

            if (count > 3) {
                console.log("QQQQ we have more than three items!");
            }

            resetIconPositions(right);
            setArrowVisibility();

            labels.attr1_text = "Found "+device_count+" devices";
            attr1_timer.start();
            labels.attr2_text = "Drag down for menu";
            attr2_timer.start();
        }

        function setArrowVisibility() {
            var arrow_left_visible = false;
            var arrow_right_visible = false;
            if (device_count > 1) {
                if (central_item_inx > 0) {
                    arrow_left_visible = true;
                }
                if ((central_item_inx + 1) < device_count) {
                    arrow_right_visible = true;
                }
            }
            arrow_left.visible = arrow_left_visible;
            arrow_right.visible = arrow_right_visible;
        }

        function resetIconPositions(direction) {
            // put the ItemDevice components back in their default positions but switch off implicit animation so that the "movement" is not animated and therefore not visible to the user

            console.log("QQQQ ++++ RESET ICON POSITIONS")
            device_left_offscreen.disableTranslationAnimation();
            device_icon0.disableTranslationAnimation();
            device_icon1.disableTranslationAnimation();
            device_icon2.disableTranslationAnimation();
            device_right_offscreen.disableTranslationAnimation();

            if (device_count == 2) {
                if (direction == left) {
                    device_icon0.setImage("asset:///images/blank.png");
                    device_icon2.setImage("asset:///images/bluetooth.png");
                } else {
                    device_icon0.setImage("asset:///images/bluetooth.png");
                    device_icon2.setImage("asset:///images/blank.png");
                }
            }

            device_left_offscreen.setOffset(-2);
            device_icon0.setOffset(-1);
            device_icon1.setOffset(0);
            device_icon2.setOffset(1);
            device_right_offscreen.setOffset(2)

            device_left_offscreen.enableTranslationAnimation();
            device_icon0.enableTranslationAnimation();
            device_icon1.enableTranslationAnimation();
            device_icon2.enableTranslationAnimation();
            device_right_offscreen.enableTranslationAnimation();

            console.log("QQQQ ---- RESET ICON POSITIONS")
        }

        function slideItemsLeft() {
            console.log("QQQQ handling request to slide left");
            console.log("QQQQ central_item_inx=" + central_item_inx);

            if (arrow_left.visible == false) {
                return;
            }

            console.log("QQQQ sliding left");

            resetIconPositions(left);
            device_icon0.setOffset(-2)
            device_icon1.setOffset(-1)
            device_icon2.setOffset(0)

            // if the central item has index > 1 then there's at least one item to the right "off screen" we can slide into view
            if (central_item_inx > 1) {
                console.log("QQQQ can slide off screen right into view");
                device_right_offscreen.setOffset(1)
            } else {
                console.log("QQQQ Not sliding off screen right into view so we leave an empty space");
            }

            central_item_inx = central_item_inx - 1;
            setArrowVisibility();

            if (device_count > 1) {
                if (central_item_inx > 0) {
                    device_icon2
                }
            }

            console.log("QQQQ central_item_inx=" + central_item_inx);

        }

        function slideItemsRight() {

            console.log("QQQQ handling request to slide right");
            console.log("QQQQ central_item_inx=" + central_item_inx);

            if (arrow_right.visible == false) {
                return;
            }

            console.log("QQQQ sliding right");

            // reset positions ready to animate right
            resetIconPositions(right);
            device_icon0.setOffset(0);
            device_icon1.setOffset(1);
            device_icon2.setOffset(2); // off screen

            // if the central item has index at least 2 less than the highest index then there's at least one item to the left "off screen" we can slide into view
            if ((device_count - central_item_inx) > 2) {
                console.log("QQQQ must slide off screen left into view");
                device_left_offscreen.setOffset(-1) // on screen
            } else {
                console.log("QQQQ Not sliding off screen left into view so we leave an empty space");

            }

            central_item_inx = central_item_inx + 1;
            setArrowVisibility();
            console.log("QQQQ central_item_inx=" + central_item_inx);
        }

        function centralItemSelected() {
            console.log("QQQQ centralItemSelected: selected item has item index " + central_item_inx);
            if (central_item_inx > -1) {
                app.deviceSelected(central_item_inx);
                serviceInfo.open();
                serviceInfo.setPeekEnabled(false);
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

        // make the container of ItemDevices float under the TouchHandler
        layout: DockLayout {
        }
        Label {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Top
            textStyle {
                fontWeight: FontWeight.Bold
                fontSize: FontSize.Large
                color: Color.create("#0000FF"); 
            }
            text: "Bluetooth Low Energy Devices"
        }
        Container {
            id: item_container
            layout: AbsoluteLayout {
            }
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            ItemDevice {
                id: device_left_offscreen
                device_identifier: 1
                visible: true
                centre_offset: -2
            }
            ItemDevice {
                id: device_icon0
                device_identifier: 2
                visible: true
                centre_offset: -1
            }
            ItemDevice {
                id: device_icon1
                device_identifier: 3
                visible: true
                centre_offset: 0
            }
            ItemDevice {
                id: device_icon2
                device_identifier: 4
                visible: true
                centre_offset: 1
            }
            ItemDevice {
                id: device_right_offscreen
                device_identifier: 5
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
                imageSource: "asset:///images/wedge_left_blue.png"
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
                imageSource: "asset:///images/wedge_right_blue.png"
            }
        }
        Labels {
            id: labels
            text_color: Color.create("#0a3d91");
            attr3_visible: true
        }
        Timer {
            id: attr1_timer
            time_limit: 3000
            onTimeout: {
                labels.attr1_text = data.getDeviceName(mainPage.central_item_inx);
                stop();
            }
        }
        Timer {
            id: attr2_timer
            time_limit: 5000
            onTimeout: {
                labels.attr2_text = data.getDeviceAddr(mainPage.central_item_inx);
                labels.attr3_text = mainPage.deviceRelationship();
                stop();
            }
        }
        TouchHandler {
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
        }
    }

    Menu.definition: MenuDefinition {
        actions: [
            ActionItem {
                id: action_search
                title: "Search"
                imageSource: "asset:///images/bt_scan.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    mainPage.startActivityIndicator();
                    app.findBleDevices();
                }
            },
            ActionItem {
                title: "About"
                imageSource: "images/about.png"

                onTriggered: {
                    aboutInfo.open();
                }
            }
        ]
    }

    attachedObjects: [
        AboutSheet {
            id: aboutInfo
        },
        ServiceSheet {
            id: serviceInfo
        }
    ]
}
