/*
 * Copyright (c) 2014 BlackBerry Limited.
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

import bb.system 1.0
import bb.cascades 1.0

Page {
    titleBar: TitleBar {
        title: qsTr("Chart Sample")
    }
    property variant valueData 
    property int valore_history: 0
    property variant connectWater:0
    property variant connectSparky:0
    property string sparky_String: ""
    property string titoloGraph : qsTr("Show Chart")
    actions:[
        ActionItem {
            id: actAggAcqua 
            title: qsTr("Add")
            imageSource: "asset:///images/ic_add.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                myDialogAddWater.show()
            }
            attachedObjects: [
                SystemPrompt {
                    id: myDialogAddWater
                    title: qsTr("Chart Sample")
                    body: qsTr("Insert Value - ONLY NUMBERS")
                    modality: SystemUiModality.Application
                    inputField.inputMode: SystemUiInputMode.NumbersAndPunctuation
                    confirmButton.label: qsTr("Ok")
                    confirmButton.enabled: true
                    cancelButton.label: qsTr("Cancel")
                    cancelButton.enabled: true
                    onFinished: {
                        if (myDialogAddWater.result == SystemUiResult.ConfirmButtonSelection){
                            var dd = new Date().getDate()
                            if (dd < 10) {dd = "0" + dd}
                            var mm = new Date().getMonth() + 1;
                            if (mm < 10) {mm = "0" + mm}
                            var aaaa = new Date().getFullYear()
                            var hours = new Date().getHours()
                            if (hours < 10) {hours = "0" + hours}
                            var minutes = new Date().getMinutes()
                            if (minutes < 10) {minutes = "0" + minutes}
                            var seconds = new Date().getSeconds()
                            if (seconds < 10) {seconds = "0" + seconds}
                            var timeWater =  aaaa + "" + mm + "" + dd + "" + hours + "" + minutes +""+ seconds
                            var ml = inputFieldTextEntry()
                            _app.createRecord(String(timeWater),String(ml))
                            listWater.dataModel.clear()
                            valore_history = 0
                            _app.readWater("SELECT * FROM water ORDER BY rowID ASC")
                        }                                   
                    }
                }
            ]
        },
        ActionItem {
            id: actGraph
            title: titoloGraph
            imageSource: "asset:///images/ic_chart.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                if (valore_history > 1) { 
                    if (actGraph.title == "Show Chart") {
                        listWater.dataModel.clear()
                        _app.readWater("SELECT * FROM water ORDER BY rowID ASC") 
                        waterWebView.evaluateJavaScript(sparky_String)
                        contWebView.visible = true
                        actGraph.title = qsTr("Hide Chart")
                        actAggAcqua.enabled = false
                    }
                    else {
                        contWebView.visible = false
                        actGraph.title = qsTr("Show Chart")
                        actAggAcqua.enabled = true
                    }
                }
                else {
                    var mess = qsTr("Data is not sufficient to generate the chart.")
                    _app.showToast(mess)
                }
            }
            
        }
    ]
    function onGotSparkyWater(stringa) {
        sparky_String = stringa
    }
    function onGotWater(row) {
        listWater.visible = true
        valore_history = valore_history + 1
        listWater.dataModel.insert(row);
    }
    onCreationCompleted: {
        if (connectSparky == 0){
            _app.gotSparky.connect(onGotSparkyWater)
            connectSparky = 1 
        }
        if (connectWater == 0){ 
            _app.gotWater.connect(onGotWater);
            connectWater = 1
        }
        _app.readWater("SELECT * FROM water ORDER BY rowID ASC")
    }
    Container {
        Container {
            id: contWebView
            visible: false
            Divider {
            
            }
            WebView {
                id: waterWebView
                objectName: "waterWebView"
                url: "local:///assets/webview/index.html"
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
            }
            Divider {
            
            }
        }
        ListView {
            id: listWater
            visible: false
            dataModel: GroupDataModel {
                grouping: ItemGrouping.None
                sortingKeys: ["data"]         
            }
            onSelectionChanged: {              
                var selectedItem = dataModel.data(indexPath);
                valueData = selectedItem.data
            }
            // These actions will be shown in the Context Menu this list item is
            // long-pressed
            contextActions: [
                ActionSet {
                    DeleteActionItem {
                        title: qsTr("Delete Item")
                        onTriggered: {
                            if (contWebView.visible == true) {
                                var avviso = qsTr("First close the chart.")
                                _app.showToast(avviso)
                            }
                            else {
                                myQmlDeleteWater.show() 
                            }                             
                        }
                        attachedObjects: [
                            SystemDialog {
                                id: myQmlDeleteWater
                                title: qsTr("Chart Sample")
                                body: qsTr("Remove Item?")
                                confirmButton.label: qsTr("Ok")
                                confirmButton.enabled: true
                                cancelButton.label: qsTr("Cancel")
                                cancelButton.enabled: true
                                onFinished: {
                                    if (myQmlDeleteWater.result == SystemUiResult.ConfirmButtonSelection){
                                        _app.deleteRecord(valueData)
                                        listWater.dataModel.clear()
                                        valore_history = 0 
                                        listWater.visible = false
                                        _app.readWater("SELECT * FROM water ORDER BY rowID ASC")
                                    }                                   
                                }
                            }
                        ]
                    }
                }
            ]
            overlapTouchPolicy: OverlapTouchPolicy.Allow
            listItemComponents: [
                ListItemComponent {
                    type: "item"
                    Container {
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.TopToBottom
                            
                            }
                            leftPadding: 10
                            Label {
                                text: qsTr("Data : ") +ListItemData.data
                                textStyle.fontWeight: FontWeight.Bold
                                multiline: true
                            }
                            Label {
                                text: qsTr("Quantity entered: ") + ListItemData.quantity
                                multiline: true
                            } 
                        }   
                        Divider {
                        
                        }           
                    }
                }
            ]
        }
    }
}
