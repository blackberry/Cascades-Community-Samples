/* Copyright (c) 2013 Research In Motion Limited.
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
import CustomTimer 1.0

TabbedPane {
    // used to tab selections as there are some conditional behaviours that depends on which tab we came "from"
    showTabsOnActionBar: true
    id: tabbedPane
    property int p_chart: 1 // 1=wheel, 2=crank
    property int p_last_wheel: 0
    property int p_last_crank: 0
    property int p_wheel_zero_count: 0
    property int p_crank_zero_count: 0
    property int p_wheel_max_zeroes_in_row: 10
    property int p_crank_max_zeroes_in_row: 10
    property string p_dev_addr
    property string p_dev_name
    property bool p_logging_active: false
    // constants
    property int p_monitor_tab: 0
    property int p_scan_tab: 1
    property int p_about_tab: 2
    property variant digit_orange: [ "asset:///images/orange0.png", "asset:///images/orange1.png", "asset:///images/orange2.png",
        "asset:///images/orange3.png", "asset:///images/orange4.png", "asset:///images/orange5.png",
        "asset:///images/orange6.png", "asset:///images/orange7.png", "asset:///images/orange8.png",
        "asset:///images/orange9.png" ]
    property variant digit_bluegreen: [ "asset:///images/bluegreen0.png", "asset:///images/bluegreen1.png", "asset:///images/bluegreen2.png",
        "asset:///images/bluegreen3.png", "asset:///images/bluegreen4.png", "asset:///images/bluegreen5.png",
        "asset:///images/bluegreen6.png", "asset:///images/bluegreen7.png", "asset:///images/bluegreen8.png",
        "asset:///images/bluegreen9.png" ]

    function onShowTab(tab_index) {
        switch (tab_index) {
            case 0:
                tabbedPane.activeTab = monitor_tab
                break;
            case 1:
                tabbedPane.activeTab = scan_tab
                break;
            case 2:
                tabbedPane.activeTab = about_tab
                break;
            default:
                console.log("QQQQ onShowTab(" + tab_index + ") - invalid tab index");
        }
    }

    function startActivityIndicator() {
        activityIndicator.start();
    }

    function stopActivityIndicator() {
        activityIndicator.stop();
    }

    function monitor(device_addr, device_name) {
        console.log("XXXX device selected");
        _scan.setRemoteDevice(device_addr);
        _cscm.monitor(device_addr, device_name);
        p_dev_addr = device_addr;
        p_dev_name = device_name;
        console.log("XXXX p_dev_addr=" + p_dev_addr + ",p_dev_name=" + p_dev_name);
    }
    function chartIt(wheel, crank) {
        wheelWebView.postMessage(wheel);
        crankWebView.postMessage(crank);
    }
    function logCscData(wheel, crank) {
        console.log("XXXX:QML has received wheel and crank values::" + wheel + "," + crank);
        if (wheel == 0 && p_last_wheel != 0) {
            p_wheel_zero_count = p_wheel_zero_count + 1;
            console.log("XXXX:QML p_wheel_zero_count=" + p_wheel_zero_count);
            if (p_wheel_zero_count < p_wheel_max_zeroes_in_row) {
                wheel = p_last_wheel;
                console.log("XXXX:QML smoothing - wheel set to last +ve value " + wheel);
            } else {
                // ok, we allow the value shown to be the reported zero and reset our counter
                console.log("XXXX:QML reseting: p_last_wheel and p_wheel_zero_count set to 0");
                p_last_wheel = 0;
                p_wheel_zero_count = 0;
                wheel = 0;
            }
        }
        if (wheel > 0) {
            console.log("XXXX:QML has received a +ve wheel value:" + wheel);
            p_last_wheel = wheel;
            var wheel_index1 = Math.floor(wheel / 100);
            var wheel_index2 = Math.floor(((wheel - (wheel_index1 * 100)) / 10));
            var wheel_index3 = Math.floor(((wheel - (wheel_index1 * 100) - (wheel_index2 * 10))));
            var wheel_image1_url = digit_orange[wheel_index1];
            var wheel_image2_url = digit_orange[wheel_index2];
            var wheel_image3_url = digit_orange[wheel_index3];
            wheelImgDigit1.imageSource = wheel_image1_url;
            wheelImgDigit2.imageSource = wheel_image2_url;
            wheelImgDigit3.imageSource = wheel_image3_url;
            console.log("XXXX:QML done setting wheel digits");
        } else {
            wheelImgDigit1.imageSource = digit_orange[0];
            wheelImgDigit2.imageSource = digit_orange[0];
            wheelImgDigit3.imageSource = digit_orange[0];
        }
        if (crank == 0 && p_last_crank != 0) {
            p_crank_zero_count = p_crank_zero_count + 1;
            console.log("XXXX:QML p_crank_zero_count=" + p_crank_zero_count);
            if (p_crank_zero_count < p_crank_max_zeroes_in_row) {
                crank = p_last_crank;
                console.log("XXXX:QML smoothing - crank set to last +ve value " + crank);
            } else {
                // ok, we allow the value shown to be the reported zero and reset our counter
                console.log("XXXX:QML reseting: p_last_crank and p_crank_zero_count set to 0");
                p_last_crank = 0;
                p_crank_zero_count = 0;
                crank = 0;
            }
        }
        if (crank > 0) {
            console.log("XXXX:QML has received a +ve crank value:" + crank);
            p_last_crank = crank;
            var crank_index1 = Math.floor(crank / 100);
            var crank_index2 = Math.floor(((crank - (crank_index1 * 100)) / 10));
            var crank_index3 = Math.floor(((crank - (crank_index1 * 100) - (crank_index2 * 10))));
            console.log("XXXX:QML setting crank digits");
            var crank_image1_url = digit_bluegreen[crank_index1];
            var crank_image2_url = digit_bluegreen[crank_index2];
            var crank_image3_url = digit_bluegreen[crank_index3];
            crankImgDigit1.imageSource = digit_bluegreen[crank_index1];
            crankImgDigit2.imageSource = digit_bluegreen[crank_index2];
            crankImgDigit3.imageSource = digit_bluegreen[crank_index3];
            console.log("XXXX:QML done setting crank digits");
        } else {
            crankImgDigit1.imageSource = digit_bluegreen[0];
            crankImgDigit2.imageSource = digit_bluegreen[0];
            crankImgDigit3.imageSource = digit_bluegreen[0];
        }
        // send to WebView for graphing
        if (wheel > 0 || crank > 0) {
            chartIt(wheel, crank);
        }
        console.log("XXXX:QML updating stats");

        // update stats
        var min_wheel_rpm = _cscdc.getMinWheelRpm();
        var max_wheel_rpm = _cscdc.getMaxWheelRpm();
        var avg_wheel_rpm = _cscdc.getAvgWheelRpm();
        var min_crank_rpm = _cscdc.getMinCrankRpm();
        var max_crank_rpm = _cscdc.getMaxCrankRpm();
        var avg_crank_rpm = _cscdc.getAvgCrankRpm();
        console.log("XXXX:QML got latest stats");
        lblMinWheelRpm.text = "Min: " + min_wheel_rpm;
        lblMaxWheelRpm.text = "Max: " + max_wheel_rpm;
        lblAvgWheelRpm.text = "Avg: " + avg_wheel_rpm;
        lblMinCrankRpm.text = "Min: " + min_crank_rpm;
        lblMaxCrankRpm.text = "Max: " + max_crank_rpm;
        lblAvgCrankRpm.text = "Avg: " + avg_crank_rpm;
        console.log("XXXX:QML done updating stats");
    }
    Tab {
        id: monitor_tab
        title: "Monitor"
        imageSource: "asset:///images/monitor.png"
        onTriggered: {
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                Label {
                    id: lblHeading
                    text: qsTr("Cycling Monitor")
                    textStyle {
                        base: SystemDefaults.TextStyles.BigText
                        color: Color.Gray
                    }
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Container {
                    id: wheel_area
                    onTouch: {
                        if (event.isDown()) {
                            p_chart = 1;
                            wheel_area.background = Color.create("#484848");
                            crank_area.background = Color.create("#303030");
                            chartIt(p_last_wheel, p_last_crank);
                            crankWebView.visible = false;
                            wheelWebView.visible = true;
                        }
                    }
                    background: Color.create("#484848")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Fill
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 30
                    }
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        topPadding: 20
                        ImageView {
                            imageSource: "asset:///images/BikeWheel_orange.png"
                        }
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        id: wheel_rpm
                        objectName: "wheel_rpm"
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        visible: true
                        LedDigit {
                            id: wheelImgDigit1
                            objectName: "wheelImgDigit1"
                            imageSource: "asset:///images/orange0.png"
                        }
                        LedDigit {
                            id: wheelImgDigit2
                            objectName: "wheelImgDigit2"
                            imageSource: "asset:///images/orange0.png"
                        }
                        LedDigit {
                            id: wheelImgDigit3
                            objectName: "wheelImgDigit3"
                            imageSource: "asset:///images/orange0.png"
                        }
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        id: wheel_stats
                        objectName: "wheel_stats"
                        verticalAlignment: VerticalAlignment.Top
                        horizontalAlignment: HorizontalAlignment.Center
                        visible: true
                        Label {
                            id: lblMinWheelRpm
                            text: "Min: 0"
                            textStyle {
                                base: SystemDefaults.TextStyles.BodyText
                                color: Color.create("#ffb000")
                                fontFamily: "Courier"
                                lineHeight: 1.1
                            }
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        Label {
                            id: lblAvgWheelRpm
                            text: "Avg: 0"
                            textStyle {
                                base: SystemDefaults.TextStyles.BodyText
                                color: Color.create("#ffb000")
                                fontFamily: "Courier"
                                lineHeight: 1.1
                            }
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        Label {
                            id: lblMaxWheelRpm
                            text: "Max: 0"
                            textStyle {
                                base: SystemDefaults.TextStyles.BodyText
                                color: Color.create("#ffb000")
                                fontFamily: "Courier"
                                lineHeight: 1.1
                            }
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                    }
                }
                Container {
                    id: crank_area
                    onTouch: {
                        if (event.isDown()) {
                            p_chart = 2;
                            crank_area.background = Color.create("#484848");
                            wheel_area.background = Color.create("#303030");
                            chartIt(p_last_wheel, p_last_crank);
                            crankWebView.visible = true;
                            wheelWebView.visible = false;
                        }
                    }
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Fill
                    topMargin: 10
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 30
                    }
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        ImageView {
                            imageSource: "asset:///images/crank5_cyan.png"
                        }
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        id: crank_rpm
                        objectName: "crank_rpm"
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Center
                        visible: true
                        LedDigit {
                            id: crankImgDigit1
                            objectName: "crankImgDigit1"
                            imageSource: "asset:///images/bluegreen0.png"
                        }
                        LedDigit {
                            id: crankImgDigit2
                            objectName: "crankImgDigit2"
                            imageSource: "asset:///images/bluegreen0.png"
                        }
                        LedDigit {
                            id: crankImgDigit3
                            objectName: "crankImgDigit3"
                            imageSource: "asset:///images/bluegreen0.png"
                        }
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        id: crank_stats
                        objectName: "crank_stats"
                        verticalAlignment: VerticalAlignment.Top
                        horizontalAlignment: HorizontalAlignment.Center
                        visible: true
                        Label {
                            id: lblMinCrankRpm
                            text: "Min: 0"
                            textStyle {
                                base: SystemDefaults.TextStyles.BodyText
                                color: Color.create("#04859d")
                                fontFamily: "Courier"
                                lineHeight: 1.1
                            }
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        Label {
                            id: lblAvgCrankRpm
                            text: "Avg: 0"
                            textStyle {
                                base: SystemDefaults.TextStyles.BodyText
                                color: Color.create("#04859d")
                                fontFamily: "Courier"
                                lineHeight: 1.1
                            }
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                        Label {
                            id: lblMaxCrankRpm
                            text: "Max: 0"
                            textStyle {
                                base: SystemDefaults.TextStyles.BodyText
                                color: Color.create("#04859d")
                                fontFamily: "Courier"
                                lineHeight: 1.1
                            }
                            horizontalAlignment: HorizontalAlignment.Center
                        }
                    }
                }
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    background: Color.create("#000000")
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 40
                    }
                    leftPadding: 20
                    rightPadding: 20
                    WebView {
                        id: wheelWebView
                        url: "local:///assets/webview/csc_wheel_chart.html"
                        visible: true
                    }
                    WebView {
                        id: crankWebView
                        url: "local:///assets/webview/csc_crank_chart.html"
                        visible: false
                    }
                }
            }
        }
    }
    Tab {
        id: scan_tab
        title: "Scan"
        imageSource: "asset:///images/scan.png"
        onTriggered: {
            console.log("QQQQ scan_tab onTriggered");
            _cscm.scanForDevices();
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
                        base: SystemDefaults.TextStyles.BigText
                        color: Color.Gray
                    }
                    text: qsTr("Available Monitors")
                    verticalAlignment: VerticalAlignment.Top
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: lblSelect
                    text: "Please select a monitor"
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Gray
                        fontFamily: "Courier"
                        lineHeight: 1.1
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                }
                ActivityIndicator {
                    id: activityIndicator
                    preferredWidth: 300
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
                Container {
                    topPadding: 20
                    leftPadding: 20
                    rightPadding: 20
                    bottomPadding: 20
                    ListView {
                        dataModel: _scan.deviceListing.model
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
                            tabbedPane.activeTab = monitor_tab;
                        }
                        function itemType(data, indexPath) {
                            if (indexPath.length == 1) {
                                return "header";
                            } else {
                                return "listItem";
                            }
                        }
                    }
                }
            }
        }
    }
    Tab {
        id: settings_tab
        title: "Settings"
        onTriggered: {
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                Label {
                    id: lblSettings
                    text: qsTr("Settings")
                    textStyle {
                        base: SystemDefaults.TextStyles.BigText
                        color: Color.Gray
                    }
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                CheckBox {
                    id: logSetting
                    text: qsTr("Log Notifications")
                    checked: tabbedPane.p_logging_active
                    onCheckedChanged: {
                        tabbedPane.p_logging_active = checked;
                        _cscdc.setLogToFile(checked);
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
            aboutInfo.open();
        }
        Page {
        }
    }
    attachedObjects: [
        AboutSheet {
            id: aboutInfo
            onClosed: {
                tabbedPane.activeTab = monitor_tab;
            }
        }
    ]
}
