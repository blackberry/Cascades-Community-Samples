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
    property int p_monitoring: 1
    property int p_beat_timer_running: 0
    property string p_dev_addr
    property string p_dev_name
    property int p_prev_tab: 0
    // constants
    property int p_monitor_tab: 0
    property int p_scan_tab: 1
    property int p_history_tab: 2
    property int p_about_tab: 3
    property variant digit: [ "asset:///images/dg0.PNG", "asset:///images/dg1.PNG", "asset:///images/dg2.PNG", "asset:///images/dg3.PNG",
        "asset:///images/dg4.PNG", "asset:///images/dg5.PNG", "asset:///images/dg6.PNG", "asset:///images/dg7.PNG",
        "asset:///images/dg8.PNG", "asset:///images/dg9.PNG" ]
    property variant dash: "asset:///images/dgh.PNG"
    property variant blank: "asset:///images/dgblank.PNG"

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
        _scan.setRemoteDevice(device_addr);
        _hrm.monitorHeartRate(device_addr, device_name);
        monitor_timer.start();
        p_dev_addr = device_addr;
        p_dev_name = device_name;
        console.log("XXXX p_dev_addr=" + p_dev_addr + ",p_dev_name=" + p_dev_name);
    }
    function logHeartRate(rate) {
        console.log("XXXX QML has received rate:" + rate);
        p_monitoring = 1;
        if (p_beat_timer_running == 0) {
            beat_timer.start();
        }
        var beat_freq_ms = Math.floor((60 / rate) * 1000);
        beat_timer.time_limit = beat_freq_ms;
        console.log("XXXX heart beat timer set to " + beat_freq_ms);
        var index1 = Math.floor(rate / 100);
        var index2 = Math.floor(((rate - (index1 * 100)) / 10));
        var index3 = Math.floor(((rate - (index1 * 100) - (index2 * 10))));
        var image1_url = digit[index1];
        var image2_url = digit[index2];
        var image3_url = digit[index3];

        if (index1 === 0) {
            imgDigit1.imageSource = blank;
        } else {
            imgDigit1.imageSource = digit[index1];
        }
        if ((index1 === 0) && (index2 === 0)) {
            imgDigit2.imageSource = blank;
        } else {
            imgDigit2.imageSource = digit[index2];
        }
        imgDigit3.imageSource = digit[index3];

        // send to WebView for graphing
        webView.postMessage(rate);

        // update stats
        var min_hr = _hrm.getMinHr();
        var max_hr = _hrm.getMaxHr();
        var avg_hr = _hrm.getAvgHr();
        lblMinHr.text = "Min: " + min_hr;
        lblMaxHr.text = "Max: " + max_hr;
        lblAvgHr.text = "Avg: " + avg_hr;
    }
//    onCreationCompleted: {
//        console.log("YYYY current device name:" + _hrdc.getCurrentDeviceName());
//        console.log("YYYY current device addr:" + _hrdc.getCurrentDeviceAddr());
//        if (_hrdc.getCurrentDeviceAddr() != "") {
//            p_prev_tab = p_monitor_tab;
//            tabbedPane.activeTab = monitor_tab;
//        } else {
//            tabbedPane.activeTab = scan_tab;
//            _scan.deviceListing.discover();
//            beat_timer.stop();
//            p_prev_tab = p_scan_tab;
//        }
//    }
    onActiveTabChanged: {
    }
    Tab {
        id: monitor_tab
        title: "Monitor"
        imageSource: "asset:///images/monitor.png"
        onTriggered: {
            beat_timer.start();
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                Label {
                    id: lblHeading
                    text: qsTr("Heart Rate Monitor")
                    textStyle {
                        base: SystemDefaults.TextStyles.BigText
                        color: Color.Green
                    }
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 25
                    }
                    id: heart_rate
                    objectName: "heart_rate"
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    visible: true
                    LedDigit {
                        id: imgDigit1
                        objectName: "imgDigit1"
                        imageSource: "asset:///images/dgh.PNG"
                    }
                    LedDigit {
                        id: imgDigit2
                        objectName: "imgDigit2"
                        imageSource: "asset:///images/dgh.PNG"
                    }
                    LedDigit {
                        id: imgDigit3
                        objectName: "imgDigit3"
                        imageSource: "asset:///images/dgh.PNG"
                    }
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 10
                    }
                    id: stats
                    objectName: "stats"
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    visible: true
                    Label {
                        id: lblMinHr
                        text: ""
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblAvgHr
                        text: ""
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                    Label {
                        id: lblMaxHr
                        text: ""
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            color: Color.Green
                            fontFamily: "Courier"
                            lineHeight: 1.1
                        }
                        horizontalAlignment: HorizontalAlignment.Center
                    }
                }
                WebView {
                    id: webView
                    url: "local:///assets/webview/heart_rate.html"
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 65
                    }
                }
                Label {
                    id: lblMonitorMessage
                    text: ""
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Green
                        fontFamily: "Courier"
                        lineHeight: 1.1
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 10
                    }
                }
                Timer {
                    id: monitor_timer
                    time_limit: 5000
                    onTimeout: {
                        lblMonitorMessage.text = ""
                    }
                }
                Timer {
                    id: beat_timer
                    time_limit: 1000
                    onTimeout: {
                        imgDigit1.tweak();
                        imgDigit2.tweak();
                        imgDigit3.tweak();
                    }
                }
            }

            // Actions are associated with the History function and will be reinstated in a later release when session history can be saved and restored
            //            actions: [
            //                ActionItem {
            //                    id: toggle_hr
            //                    title: qsTr("Stop and Save")
            //                    imageSource: "asset:///images/save.png"
            //                    onTriggered: {
            //                        if (p_monitoring == 1) {
            //                            beat_timer.stop();
            //                            _hrm.stopMonitoringHeartRate();
            //                            _hrm.saveSession("test");
            //                            toggle_hr.title = "Start"
            //                            p_monitoring = 0;
            //                        } else {
            //                            beat_timer.start();
            //                            _hrm.monitorHeartRate(p_dev_addr, p_dev_name);
            //                            toggle_hr.title = "Stop and Save";
            //                            p_monitoring = 1;
            //                        }
            //                    }
            //                }
            //            ]
        }
    }
    Tab {
        id: scan_tab
        title: "Scan"
        imageSource: "asset:///images/scan.png"
        onTriggered: {
            console.log("QQQQ scan_tab onTriggered");
            _hrm.scanForDevices();
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
                        color: Color.Green
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
                        color: Color.Green
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

    // The History feature is a work in progress and will be completed in a subsequent release

    //    Tab {
    //        id: history_tab
    //        title: "History"
    //        imageSource: "asset:///images/history.png"
    //        onTriggered: {
    //            _hrm.stopMonitoringHeartRate();
    //            beat_timer.stop();
    //            scan_timer.stop();
    //            console.log("XXXX requesting loadSavedSessionList");
    //            _sessions.loadSavedSessionList();
    //            p_prev_tab = p_history_tab;
    //        }
    //        Page {
    //            actionBarVisibility: ChromeVisibility.Visible
    //            Container {
    //                layout: StackLayout {
    //                }
    //                verticalAlignment: VerticalAlignment.Fill
    //                Label {
    //                    id: lblHeading_history
    //                    text: qsTr("History")
    //                    textStyle {
    //                        base: SystemDefaults.TextStyles.BigText
    //                        color: Color.Green
    //                    }
    //                    verticalAlignment: VerticalAlignment.Center
    //                    horizontalAlignment: HorizontalAlignment.Center
    //                }
    //                Container {
    //                    topPadding: 20
    //                    leftPadding: 20
    //                    rightPadding: 20
    //                    bottomPadding: 20
    //                    ListView {
    //                        dataModel: _sessions.model
    //                        listItemComponents: [
    //                            ListItemComponent {
    //                                type: "listItem"
    //                                StandardListItem {
    //                                    title: ListItemData.session_name
    //                                }
    //                            },
    //                            ListItemComponent {
    //                                type: "listItem"
    //                                StandardListItem {
    //                                    title: ListItemData.session_datetime
    //                                }
    //                            }
    //                        ]
    //                        onTriggered: {
    //                        }
    //                        function itemType(data, indexPath) {
    //                            if (indexPath.length == 1) {
    //                                return "header";
    //                            } else {
    //                                return "listItem";
    //                            }
    //                        }
    //                    }
    //                }
    //                Label {
    //                    id: lblHistoryMessage
    //                    textStyle {
    //                        base: SystemDefaults.TextStyles.BodyText
    //                        color: Color.Green
    //                        fontFamily: "Courier"
    //                        lineHeight: 1.1
    //                    }
    //                }
    //                Timer {
    //                    id: history_timer
    //                    time_limit: 5000
    //                    onTimeout: {
    //                        lblHistoryMessage.text = ""
    //                    }
    //                }
    //            }
    //        }
    //    }
    Tab {
        id: about_tab
        title: "About"
        imageSource: "asset:///images/about.png"
        onTriggered: {
            beat_timer.stop();
            scan_timer.stop();
            p_prev_tab = p_about_tab;
        }
        Page {
            actionBarVisibility: ChromeVisibility.Visible
            Container {
                layout: StackLayout {
                }
                verticalAlignment: VerticalAlignment.Fill
                Label {
                    id: lblHeading_about
                    text: qsTr("About HeartMonitor")
                    textStyle {
                        base: SystemDefaults.TextStyles.BigText
                        color: Color.Green
                    }
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: lblHeading_version
                    text: qsTr("V1.0.4")
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    id: lblBody_about
                    text: qsTr("The HeartMonitor application demonstrates how to acquire and use data transmitted by a Bluetooth 4.0 LE heart monitor device")
                    multiline: true
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Green
                        fontFamily: "Courier"
                        lineHeight: 1.1
                    }
                }
                Label {
                    id: lblAuthors
                    text: qsTr("Authors:")
                    multiline: true
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Green
                        fontFamily: "Courier"
                        lineHeight: 1.1
                    }
                }
                Label {
                    id: lblAuthors1
                    text: qsTr("Martin Woolley (@mdwrim)")
                    multiline: true
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Green
                        fontFamily: "Courier"
                        lineHeight: 1.1
                    }
                }
                Label {
                    id: lblAuthors2
                    text: qsTr("John Murray (@jcmrim)")
                    multiline: true
                    textStyle {
                        base: SystemDefaults.TextStyles.BodyText
                        color: Color.Green
                        fontFamily: "Courier"
                        lineHeight: 1.1
                    }
                }
            }
        }
    }
}
