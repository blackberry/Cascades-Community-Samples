/* Copyright (c) 2012 BlackBerry Limited.
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
import bb.multimedia 1.0
import CustomTimer 1.0

TabbedPane {
    id: tabbedPane
    objectName: "tabbed_pane"
    showTabsOnActionBar: true
    property int p_image_inx: 6
    property int p_max_inx: 6
    property int p_inx_delta: 1
    property int p_alert_state: 1
    property string p_dev_addr
    property string p_dev_name
    property int p_monitoring: 0
    property int p_proximity_band: 0
    property variant proximity_band_labels: [ "Close by", "at a middle distance", "Far away" ]
    property variant img_proximity: [ "asset:///images/near.png", "asset:///images/medium.png", "asset:///images/far.png" ]
    property variant img_default: "asset:///images/unknown.png"
    property variant img_mamma: "asset:///images/mamma_chicken.png"
    property variant img_panic: "asset:///images/panic_chicken.png"
    property variant img_chick: "asset:///images/ickle_chicken.png"
    
    function monitor(device_addr, device_name) {
        console.log("XXXX monitor called");
        _bt.setRemoteDevice(device_addr);
        _bcg.monitorProximity(device_addr, device_name);
        p_dev_addr = device_addr;
        p_dev_name = device_name;
        console.log("XXXX p_dev_addr=" + p_dev_addr + ",p_dev_name=" + p_dev_name);
    }
    
    function onRssi(proximity_indicator, rssi) {
        console.log("XXXX onRssi called: rssi=" + rssi + ",proximity_band=" + proximity_indicator);
        p_proximity_band = proximity_indicator;
        if (p_monitoring == 1) {
            setMessage("Signal Strength: " + rssi + " (" + proximity_band_labels[p_proximity_band] + ")");
            levelIndicator.value = rssi
        }
    }
    
    function onProximityChange(proximity_indicator) {
        console.log("XXXX onProximityChange called:" + proximity_indicator);
        if (p_monitoring == 1) {
            p_proximity_band = proximity_indicator;
            img_chicken.imageSource = img_proximity[proximity_indicator];
            if (proximity_indicator == 2) {
                squark();
            }
        }
    }
    
    function linkEstablished() {
        console.log("XXXX linkEstablished");
        p_monitoring = 1;
        img_chicken.imageSource = img_proximity[1];
        find_tab.enabled = true;
    }
    
    function lostLink() {
        console.log("XXXX lostLink");
        p_monitoring = 0;
        img_chicken.imageSource = img_default;
        find_tab.enabled = false;
        squark();
    }
    
    function squark() {
        if (audioPlayer.play() != MediaError.None) {
            console.log("XXXX error attempting to play sound");
        }
    }
    
    function setMessage(message) {
        console.log("XXXX " + message);
        proximity_message.text = message;
    }
    
    function enableProximityVisualisation() {
        console.log("XXXX enableProximityVisualisation()");
        levelIndicator.visible = true;
    }
    
    function disableProximityVisualisation() {
        console.log("XXXX disableProximityVisualisation()");
        levelIndicator.visible = false;
        setMessage("No longer connected");
    }

    function sensitivity_text(sensitivity_value) {
        if (sensitivity_value < 4) {
            return "LOW";
        }
        if (sensitivity_value < 7) {
            return "MEDIUM";
        }
        return "HIGH";                
    }
    
    Tab {
        id: chicken_tab
        imageSource: "asset:///images/monitor.png"
        title: "Monitor"
        Page {
            Container {
                layout: StackLayout {
                }
                Label {
                    text: qsTr("Bluetooth Chicken Guardian")
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                ImageView {
                    id: img_chicken
                    imageSource: img_default
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    bottomPadding: 20
                    Label {
                        id: proximity_message
                        text: (p_monitoring == 1) ? "Ready" : "Not connected to a proximity sensor"
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    ProgressIndicator {
                        id: levelIndicator
                        visible: (p_monitoring == 1) ? true : false
                        fromValue: -150
                        toValue: 10
                        value: 0
                        state: ProgressIndicatorState.Progress
                    }
                
                }
                
                attachedObjects: [
                    MediaPlayer {
                        id: audioPlayer
                        sourceUrl: "asset:///sounds/chicken.wav"
                    }
                ]
            }
        }
    }
    Tab {
        id: scan_tab
        title: "Scan"
        imageSource: "asset:///images/scan.png"
        onTriggered: {
            console.log("XXXX calling discover()");
            activityIndicator.start();
            _bt.deviceListing.discover();
            _bt.deviceListing.update();
            activityIndicator.stop();
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    id: lblHeading_scan
                    textStyle {
                        base: SystemDefaults.TextStyles.TitleText
                    }
                    text: qsTr("Chickens in Range")
                    verticalAlignment: VerticalAlignment.Top
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: lblSelect
                    text: "Please select a chicken to monitor"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        fontFamily: "Courier"
                        lineHeight: 1.1
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Container {
                    layout: DockLayout {
                    }
                    topPadding: 20
                    leftPadding: 20
                    rightPadding: 20
                    bottomPadding: 20
                    ListView {
                        dataModel: _bt.deviceListing.model
                        listItemComponents: [
                            ListItemComponent {
                                type: "listItem"
                                StandardListItem {
                                    title: ListItemData.deviceName
                                }
                            }
                        ]
                        onTriggered: {
                            var selectedItem = dataModel.data(indexPath);
                            monitor(selectedItem.deviceAddress, selectedItem.deviceName);
                            tabbedPane.activeTab = chicken_tab;
                        }
                        function itemType(data, indexPath) {
                            if (indexPath.length == 1) {
                                return "header";
                            } else {
                                return "listItem";
                            }
                        }
                    }
                    ActivityIndicator {
                        id: activityIndicator
                        preferredWidth: 300
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }
    }
    Tab {
        id: find_tab
        enabled: false
        title: "Panic"
        imageSource: "asset:///images/emergency.png"
        onTriggered: {
            console.log("XXXX calling alertHigh()");
            _bt.sendAlertHigh();
            panicing_chicken.play();
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                Container {
                    layout: StackLayout {
                    }
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    Label {
                        id: lblHeading_find
                        textStyle {
                            base: SystemDefaults.TextStyles.TitleText
                        }
                        text: qsTr("Lost Your Chicken?")
                        verticalAlignment: VerticalAlignment.Top
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Button {
                        id: btn_alert_level
                        text: "Stop that horrible noise!"
                        topMargin: 100
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        onClicked: {
                            if (p_alert_state == 1) {
                                p_alert_state = 0;
                                btn_alert_level.text = "Squark chicken, squark!"
                                console.log("XXXX calling sendAlertOff()");
                                _bt.sendAlertOff();
                                panicing_chicken.stop();
                                tabbedPane.activeTab = chicken_tab;
                            } else {
                                p_alert_state = 1;
                                btn_alert_level.text = "Stop that horrible noise!"
                            }
                        }
                    }
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    topPadding: 100
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Bottom
                    Container {
                        id: panic_chicken
                        
                        layout: DockLayout {
                        }
                        ImageView {
                            id: chicken_image
                            imageSource: img_panic
                        }
                        animations: [
                            SequentialAnimation {
                                id: panicing_chicken
                                animations: [
                                    TranslateTransition {
                                        fromX: 0
                                        toX: 700
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 700
                                        toX: 0
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 0
                                        toX: 700
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 700
                                        toX: 0
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 0
                                        toX: 700
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 700
                                        toX: 0
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 0
                                        toX: 700
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 700
                                        toX: 0
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 0
                                        toX: 700
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 700
                                        toX: 0
                                        toY: 0
                                        duration: 500
                                        easingCurve: StockCurve.SineIn
                                    },
                                    TranslateTransition {
                                        fromX: 0
                                        toX: 300
                                        toY: 0
                                        duration: 250
                                        easingCurve: StockCurve.SineIn
                                    },
                                    RotateTransition {
                                        toAngleZ: 360
                                        duration: 1000
                                    }
                                ]
                            }
                        ]
                        
                        // This is the implicit-animation controller for the chicken.
                        // We need this to set the chicken to a new position before showing it without
                        // triggering implicit animations.
                        attachedObjects: [
                            ImplicitAnimationController {
                                id: offScreenController
                                propertyName: "translationX"
                                enabled: false
                            }
                        ]
                    
                    }
                }
            }
        }
    }
    Tab {
        id: settings_tab
        title: "Settings"
        imageSource: "asset:///images/settings.png"
        onTriggered: {
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    id: lblSettingsHeading
                    text: qsTr("Settings")
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: lblRssiNear
                    text: qsTr("Near: min. signal")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                TextField {
                    id: tfRssiNear
                    text: _bcg.getRssiNear()
                    inputMode: TextFieldInputMode.NumbersAndPunctuation
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                }
                Label {
                    id: lblRssiMedium
                    text: qsTr("Medium: min. signal")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                TextField {
                    id: tfRssiMedium
                    text: _bcg.getRssiMedium()
                    inputMode: TextFieldInputMode.NumbersAndPunctuation
                    input {
                        flags: TextInputFlag.AutoCapitalizationOff
                    }
                }
                Divider {
                
                }
                Label {
                    id: lblSensitivity
                    text: qsTr("RSSI Fluctuation Sensitivity: "+Math.round(slider_sensitivity.immediateValue)+" ("+sensitivity_text(slider_sensitivity.immediateValue)+")")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Slider {
                    id: slider_sensitivity
                    fromValue: 1
                    toValue: 10
                    value: _bcg.getRssiFluctuationSensitivity()
                }
                Divider {
                
                }
                Button {
                    text: "Save"
                    horizontalAlignment: HorizontalAlignment.Fill
                    onClicked: {
                        _bcg.saveSettings(tfRssiNear.text, tfRssiMedium.text,Math.round(slider_sensitivity.immediateValue));
                        lblSettingsMessage.text = "Settings have been saved";
                        settings_timer.start();
                    }
                }
                Label {
                    id: lblSettingsMessage
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Timer {
                    id: settings_timer
                    time_limit: 5000
                    onTimeout: {
                        lblSettingsMessage.text = ""
                    }
                }
            }
        }
    }
    Tab {
        id: about_tab
        title: "About"
        imageSource: "asset:///images/about.png"
        onTriggered: {
            console.log("XXXX calling enterChicken");
            mamma_chicken.enterChicken(img_mamma, 0, 5000);
            ickle_chicken_1.enterChicken(img_chick, 0, 3000);
            ickle_chicken_2.enterChicken(img_chick, 0, 4000);
            ickle_chicken_3.enterChicken(img_chick, 0, 1000);
            ickle_chicken_4.enterChicken(img_chick, 0, 2000);
            ickle_chicken_5.enterChicken(img_chick, 0, 4500);
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Center
                Container {
                    layout: StackLayout {
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    Label {
                        id: lblHeading_about
                        text: qsTr("About BtChickenGuardian")
                        textStyle.base: SystemDefaults.TextStyles.TitleText
                        textStyle.textAlign: TextAlign.Center
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblHeading_version
                        text: qsTr("V1.1.1 Q10")
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblBody_about1
                        text: qsTr("Works with the Bluetooth Proximity Profile")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.SmallText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblBody_about2
                        text: qsTr("Use it to keep an eye on *any* precious thing")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.SmallText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblBody_about3
                        text: qsTr("It doesn't have to be a chicken")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.SmallText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblAuthors
                        text: qsTr("Authors:")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.SmallText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblAuthors1
                        text: qsTr("Martin Woolley (@mdwrim)")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.SmallText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblAuthors2
                        text: qsTr("John Murray (@jcmrim)")
                        multiline: true
                        textStyle {
                            base: SystemDefaults.TextStyles.SmallText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                            textAlign: TextAlign.Center
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    topPadding: 100
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Bottom
                    Chicken {
                        id: mamma_chicken
                        visible: true
                    }
                    Chicken {
                        id: ickle_chicken_1
                        visible: true
                    }
                    Chicken {
                        id: ickle_chicken_2
                        visible: true
                    }
                    Chicken {
                        id: ickle_chicken_3
                        visible: true
                    }
                    Chicken {
                        id: ickle_chicken_4
                        visible: true
                    }
                    Chicken {
                        id: ickle_chicken_5
                        visible: true
                    }
                }
            }
        }
    }
}
