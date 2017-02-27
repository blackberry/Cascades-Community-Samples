import bb.cascades 1.4
import bb.system 1.2

import "functions.js" as ExtFunction

Page {
    property bool debugMode: false
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        ScrollView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.TopToBottom
                }
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    leftPadding: ui.sdu(2)
                    topPadding: ui.sdu(2)
                    rightPadding: ui.sdu(2)
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    DropDown {
                        id: dd_num_datasets
                        title: qsTr("# Datasets")
                        onCreationCompleted: {
                            var newOption;
                            for (var i = 1; i < 10; i ++) {
                                newOption = newOptionDef.createObject();
                                newOption.text = i.toString();
                                newOption.value = i;
                                if (i == 1)
                                    newOption.selected = true;
                                add(newOption);
                            }
                        }
                    }
                    DropDown {
                        id: dd_itemsPerDataset
                        title: qsTr("Items per Dataset")
                        onCreationCompleted: {
                            var newOption;
                            for (var i = 1; i <= 50; i ++) {
                                newOption = newOptionDef.createObject();
                                newOption.text = i.toString();
                                newOption.value = i;
                                if (i == 5)
                                    newOption.selected = true;
                                add(newOption);
                            }
                        }
                    }
                }
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    leftPadding: ui.sdu(2)
                    topPadding: ui.sdu(2)
                    rightPadding: ui.sdu(2)
                    layout: StackLayout {
                        orientation: LayoutOrientation.TopToBottom
                    }
                    DropDown {
                        id: dd_rangeValue
                        title: qsTr("Range of values")
                        options: [
                            Option {
                                text: "0, 1, 2, ..., 9"
                                value: {
                                    var arr = [];
                                    for (var i = 0; i < 10; i ++) arr[i] = i;
                                    return arr;
                                }
                                selected: true
                            },
                            Option {
                                text: "1, 2, 3, ..., 10"
                                value: {
                                    var arr = [];
                                    for (var i = 0; i < 10; i ++) arr[i] = i + 1;
                                    return arr;
                                }
                            },
                            Option {
                                text: "0, 1, 2, ..., 99"
                                value: {
                                    var arr = [];
                                    for (var i = 0; i < 100; i ++) arr[i] = i;
                                    return arr;
                                }
                            },
                            Option {
                                text: "1, 2, 3, ..., 100"
                                value: {
                                    var arr = [];
                                    for (var i = 0; i < 100; i ++) arr[i] = i + 1;
                                    return arr;
                                }
                            },
                            Option {
                                text: "0, 1, 2, ..., 999"
                                value: {
                                    var arr = [];
                                    for (var i = 0; i < 1000; i ++) arr[i] = i;
                                    return arr;
                                }
                            },
                            Option {
                                text: "1, 2, 3, ..., 1000"
                                value: {
                                    var arr = [];
                                    for (var i = 0; i < 1000; i ++) arr[i] = i + 1;
                                    return arr;
                                }
                            },
                            Option {
                                text: "-10, -9, .. , 0, ..., 9, 10"
                                value: {
                                    var arr = [];
                                    for (var i = 0; i <= 20; i ++) arr[i] = i - 10;
                                    return arr;
                                }
                            },
                            Option {
                                text: "-100, -9, .. , 0, ..., 9, 100"
                                value: {
                                    var arr = [];
                                    for (var i = 0; i <= 200; i ++) arr[i] = i - 100;
                                    return arr;
                                }
                            },
                            Option {
                                text: "-1000, -9, .. , 0, ..., 9, 1000"
                                value: {
                                    var arr = [];
                                    for (var i = 0; i <= 2000; i ++) arr[i] = i - 1000;
                                    return arr;
                                }
                            }
                        ]
                    }
                    DropDown {
                        id: dd_typeValue
                        title: qsTr("Labels")
                        options: [
                            Option {
                                text: qsTr("like the value")
                                value: dd_rangeValue.selectedValue
                                selected: true
                            },
                            Option {
                                text: "January, February, March, ..., December"
                                value: [ "January", "February", "March", "April", "May", "June", "July", "August",
                                    "September", "October", "November", "December" ]
                            },
                            Option {
                                text: "Monday, Tuesday, ..., Sunday"
                                value: [ "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" ]
                            },
                            Option {
                                text: "Random Countries"
                                value: [ "China", "India", "United States", "Indonesia", "Brazil", "Pakistan", "Nigeria",
                                    "Bangladesh", "Russia", "Japan", "Mexico", "Philippines", "Vietnam", "Ethiopia",
                                    "Egypt", "Germany", "Iran", "Turkey", "Democratic Republic of the Congo",
                                    "Thailand" ]
                            }
                        ]
                    }
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        ImageView {
                            imageSource: "asset:///images/pie.png"
                            verticalAlignment: VerticalAlignment.Center
                            scalingMethod: ScalingMethod.AspectFit
                            preferredHeight: ui.sdu(10)
                            preferredWidth: ui.sdu(15)
                        }
                        SegmentedControl {
                            id: sc_pieOrDoughnut
                            verticalAlignment: VerticalAlignment.Center
                            options: [
                                Option {
                                    text: qsTr("Pie")
                                    value: "pie"
                                },
                                Option {
                                    text: qsTr("Doughnut")
                                    value: "doughnut"
                                }
                            ]
                        }
                        ImageView {
                            imageSource: "asset:///images/doughnut.png"
                            verticalAlignment: VerticalAlignment.Center
                            scalingMethod: ScalingMethod.AspectFit
                            preferredHeight: ui.sdu(10)
                            preferredWidth: ui.sdu(15)
                        }
                    }
                }
                WebView {
                    id: webview
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    url: "local:///assets/html/pieDoughnut.html" + (debugMode ? "?debug=true" : "")
                    settings.zoomToFitEnabled: true
                    onMessageReceived: {
                        var risp = message.data;
                        //console.debug(risp);
                        if (risp == "SUCCESS") {
                            console.debug("----- chart created with success");
                        }
                    }

                    function createChart() {
                        /*console.debug("*** Items Per Dataset:");
                         * console.debug(JSON.stringify(dd_itemsPerDataset.selectedValue, null, 4));
                         * console.debug("*** Range Value:");
                         * console.debug(JSON.stringify(dd_rangeValue.selectedValue, null, 4));
                         * console.debug("*** Type of Value (labels):");
                         console.debug(JSON.stringify(dd_typeValue.selectedValue, null, 4));*/

                        var datasets = [];
                        for (var i = 0; i < dd_num_datasets.selectedValue; i ++) {
                            datasets[i] = {
                                label: 'DS #' + (i + 1).toString(),
                                data: ExtFunction.createRandomArray(dd_itemsPerDataset.selectedValue, dd_rangeValue.selectedValue),
                                backgroundColor: ExtFunction.createBackgroundColorArray(dd_itemsPerDataset.selectedValue, 0.6),
                                borderColor: ExtFunction.createBackgroundColorArray(dd_itemsPerDataset.selectedValue, 0.8)
                            }
                        }
                        var labels = ExtFunction.createRandomArray(dd_itemsPerDataset.selectedValue, dd_typeValue.selectedValue);

                        var type = sc_pieOrDoughnut.selectedValue;

                        var chart = {
                            'type': type,
                            'datasets': datasets,
                            'labels': labels
                        }

                        console.debug(JSON.stringify(chart, null, 4));
                        webview.postMessage(JSON.stringify(chart));
                        console.debug("----- chart data sended");
                    }
                }
            }
        }
        attachedObjects: [
            ComponentDefinition {
                id: newOptionDef
                Option {
                }
            },
            SystemToast {
                property int viewed: 1
                id: reloadWarning
                body: "Reload WebView before update the chart"
            }
        ]
    }
    actions: [
        ActionItem {
            title: "Create Chart"
            imageSource: "asset:///images/ic_wizard_1.png"
            ActionBar.placement: ActionBarPlacement.Signature
            onTriggered: {
                if(reloadWarning.viewed == 0) reloadWarning.show();
                else webview.createChart();
                reloadWarning.viewed = 0;
            }
        },
        ActionItem {
            title: "Reload WebView"
            imageSource: "asset:///images/ic_reload.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                reloadWarning.viewed = 1;
                webview.reload();
            }
        }
    ]
}
