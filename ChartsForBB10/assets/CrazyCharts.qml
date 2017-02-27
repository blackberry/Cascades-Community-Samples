import bb.cascades 1.4
import bb.system 1.2

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
                Header {
                    title: "Coffee Consumption: ITALY vs ENGLAND"
                }
                WebView {
                    property string base64image: ""
                    id: coffeeConsumption
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    url: "local:///assets/html/crazyCharts.html" + (debugMode ? "?debug=true" : "")
                    settings.zoomToFitEnabled: true
                    onMessageReceived: {
                        var risp = message.data;
                        //console.debug(risp);
                        console.debug("----- chart created with success");
                        //console.debug(risp);
                        base64image = risp;
                    }
                    onLoadingChanged: {
                        if (loadRequest.status == WebLoadStatus.Succeeded) {
                            createChart();
                        } else if (loadRequest.status == WebLoadStatus.Failed) {
                            //Load failed.
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
                        datasets[0] = {
                            label: "Italy Consumption",
                            data: [ "105253", "111921", "116803", "119293", "120184", "123394", "145158", "151595",
                                "155967", "165032", "177298", "175931", "200936", "201834", "202383", "213952",
                                "182497", "193576", "226551", "220412", "224672", "244936", "245374", "221291",
                                "281215", "249517", "257722", "248735", "255160", "287642", "249656", "243056",
                                "291099", "280605", "276033", "278721", "287785", "292763", "292213", "307122",
                                "312292", "305986", "326733", "321005", "335147", "337835", "342752", "350000",
                                "346000", "349000", "350000" ],
                            borderColor: "#2196f3",
                            pointRadius: 0,
                            lineTension: 0,
                            fill: false
                        };
                        datasets[1] = {
                            label: "England Consumption",
                            data: [ "55232", "68216", "76156", "75167", "55817", "80569", "79627", "93394", "102878",
                                "101156", "123355", "116115", "151621", "105486", "104699", "113226", "88803",
                                "109000", "149910", "117693", "146931", "132020", "137528", "156676", "149938",
                                "147655", "153841", "148572", "139863", "153294", "144124", "157525", "157062",
                                "171342", "130637", "158409", "151227", "162840", "139221", "135647", "134597",
                                "143054", "145520", "163650", "154467", "179141", "163416", "179000", "187000",
                                "181000", "167000" ],
                            borderColor: "#ff9800",
                            pointRadius: 0,
                            lineTension: 0,
                            fill: false
                        };

                        var labels = [ "1961", "1962", "1963", "1964", "1965", "1966", "1967", "1968", "1969", "1970", "1971",
                            "1972", "1973", "1974", "1975", "1976", "1977", "1978", "1979", "1980", "1981", "1982",
                            "1983", "1984", "1985", "1986", "1987", "1988", "1989", "1990", "1991", "1992", "1993",
                            "1994", "1995", "1996", "1997", "1998", "1999", "2000", "2001", "2002", "2003", "2004",
                            "2005", "2006", "2007", "2008", "2009", "2010", "2011" ];

                        var options = {
                            title: {
                                display: true,
                                text: 'Coffee Consumption:ITALY vs ENGLAND',
                                fontSize: 20
                            },
                            gridLines: [
                                {
                                    display: false
                                } ],
                            scales: {
                                xAxes: [
                                    {
                                        gridLines: {
                                            display: false
                                        },
                                        ticks: {
                                            beginAtZero: false,
                                            maxTicksLimit: 17
                                        }
                                    } ],
                                yAxes: [
                                    {
                                        gridLines: {
                                            display: false
                                        },
                                        ticks: {
                                            beginAtZero: false,
                                            stepSize: 50000
                                        }
                                    } ]
                            },
                            animation: {
                                duration: 0
                            },
                            tooltips: {
                                enabled: false
                            },
                            legend: {
                                display: true,
                                position: 'bottom',
                                labels: {
                                    boxWidth: 12
                                }
                            }
                        };

                        var chart = {
                            'type': 'line',
                            'datasets': datasets,
                            'options': options,
                            'labels': labels
                        }

                        console.debug(JSON.stringify(chart, null, 4));
                        postMessage(JSON.stringify(chart));
                        console.debug("----- chart data sended");
                    }
                }
                Button {
                    horizontalAlignment: HorizontalAlignment.Center
                    imageSource: "asset:///images/ic_save.png"
                    appearance: ControlAppearance.Primary
                    preferredWidth: ui.sdu(10)
                    topMargin: ui.sdu(3)
                    bottomMargin: ui.sdu(3)
                    onClicked: {
                        if (__App.FromBase64ToPNGImage(ethnicities.base64image, "CoffeeConsumption_chart")) savedToast.show();
                    }
                }
                Header {
                    title: "Electricity Production (billion kW h/yr)"
                }
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.TopToBottom
                    }
                    horizontalAlignment: HorizontalAlignment.Fill
                    WebView {
                        property string base64image: ""
                        id: electricityProduction_FossilFuels
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        url: "local:///assets/html/crazyCharts.html" + (debugMode ? "?debug=true" : "")
                        settings.zoomToFitEnabled: true
                        onMessageReceived: {
                            var risp = message.data;
                            console.debug("----- chart created with success");
                            //console.debug(risp);
                            base64image = risp;
                        }
                        onLoadingChanged: {
                            if (loadRequest.status == WebLoadStatus.Succeeded) {
                                createChart();
                            } else if (loadRequest.status == WebLoadStatus.Failed) {
                                //Load failed.
                            }
                        }

                        function createChart() {
                            var datasets = [
                                {
                                    data: [ 44.93, 135.9, 232.5, 189.9, 349.9 ],
                                    backgroundColor: [ "#7FE3A1", "#9988DF", "#FFEA8F", "#FFA08F", "#9693A1" ]
                                } ];

                            var labels = [ "France", "Spain", "United Kingdom", "Italy", "Germany" ];
                            var options = {
                                title: {
                                    display: true,
                                    text: 'Fossil Fuels',
                                    fontSize: 20
                                },
                                animation: {
                                    duration: 0
                                },
                                tooltips: {
                                    enabled: false
                                },
                                legend: {
                                    display: false
                                }
                            };

                            var chart = {
                                'type': 'doughnut',
                                'datasets': datasets,
                                'options': options,
                                'labels': labels
                            }

                            console.debug(JSON.stringify(chart, null, 4));
                            postMessage(JSON.stringify(chart));
                            console.debug("----- chart data sended");
                        }
                    }

                    Button {
                        horizontalAlignment: HorizontalAlignment.Center
                        imageSource: "asset:///images/ic_save.png"
                        appearance: ControlAppearance.Primary
                        preferredWidth: ui.sdu(10)
                        topMargin: ui.sdu(3)
                        bottomMargin: ui.sdu(3)
                        onClicked: {
                            if (__App.FromBase64ToPNGImage(ethnicities.base64image, "electricityProduction_FossilFuels_chart")) savedToast.show();
                        }
                    }
                    WebView {
                        property string base64image: ""
                        id: electricityProduction_NuclearEnergy
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        url: "local:///assets/html/crazyCharts.html" + (debugMode ? "?debug=true" : "")
                        settings.zoomToFitEnabled: true
                        onMessageReceived: {
                            var risp = message.data;
                            console.debug("----- chart created with success");
                            //console.debug(risp);
                            base64image = risp;
                        }
                        onLoadingChanged: {
                            if (loadRequest.status == WebLoadStatus.Succeeded) {
                                createChart();
                            } else if (loadRequest.status == WebLoadStatus.Failed) {
                                //Load failed.
                            }
                        }

                        function createChart() {
                            var datasets = [
                                {
                                    data: [ 403.7, 54.31, 64.13, 0, 92.14 ],
                                    backgroundColor: [ "#7FE3A1", "#9988DF", "#FFEA8F", "#FFA08F", "#9693A1" ]
                                } ];

                            var labels = [ "France", "Spain", "United Kingdom", "Italy", "Germany" ];
                            var options = {
                                title: {
                                    display: true,
                                    text: 'Nuclear Energy',
                                    fontSize: 20
                                },
                                animation: {
                                    duration: 0
                                },
                                tooltips: {
                                    enabled: false
                                },
                                legend: {
                                    display: false
                                }
                            };

                            var chart = {
                                'type': 'doughnut',
                                'datasets': datasets,
                                'options': options,
                                'labels': labels
                            }

                            console.debug(JSON.stringify(chart, null, 4));
                            postMessage(JSON.stringify(chart));
                            console.debug("----- chart data sended");
                        }
                    }

                    Button {
                        horizontalAlignment: HorizontalAlignment.Center
                        imageSource: "asset:///images/ic_save.png"
                        appearance: ControlAppearance.Primary
                        preferredWidth: ui.sdu(10)
                        topMargin: ui.sdu(3)
                        bottomMargin: ui.sdu(3)
                        onClicked: {
                            if (__App.FromBase64ToPNGImage(ethnicities.base64image, "electricityProduction_NuclearEnergy_chart")) savedToast.show();
                        }
                    }
                    WebView {
                        property string base64image: ""
                        id: electricityProduction_RenewableEnergy
                        horizontalAlignment: HorizontalAlignment.Fill
                        verticalAlignment: VerticalAlignment.Fill
                        url: "local:///assets/html/crazyCharts.html" + (debugMode ? "?debug=true" : "")
                        settings.zoomToFitEnabled: true
                        onMessageReceived: {
                            var risp = message.data;
                            console.debug("----- chart created with success");
                            //console.debug(risp);
                            base64image = risp;
                        }
                        onLoadingChanged: {
                            if (loadRequest.status == WebLoadStatus.Succeeded) {
                                createChart();
                            } else if (loadRequest.status == WebLoadStatus.Failed) {
                                //Load failed.
                            }
                        }

                        function createChart() {
                            var datasets = [
                                {
                                    data: [ 82.78, 86.76, 40.25, 91.8, 142.7 ],
                                    backgroundColor: [ "#7FE3A1", "#9988DF", "#FFEA8F", "#FFA08F", "#9693A1" ]
                                } ];

                            var labels = [ "France", "Spain", "United Kingdom", "Italy", "Germany" ];
                            var options = {
                                title: {
                                    display: true,
                                    text: 'Renewable Energy',
                                    fontSize: 20
                                },
                                animation: {
                                    duration: 0
                                },
                                tooltips: {
                                    enabled: false
                                },
                                legend: {
                                    display: false
                                }
                            };

                            var chart = {
                                'type': 'doughnut',
                                'datasets': datasets,
                                'options': options,
                                'labels': labels
                            }

                            console.debug(JSON.stringify(chart, null, 4));
                            postMessage(JSON.stringify(chart));
                            console.debug("----- chart data sended");
                        }
                    }

                    Button {
                        horizontalAlignment: HorizontalAlignment.Center
                        imageSource: "asset:///images/ic_save.png"
                        appearance: ControlAppearance.Primary
                        preferredWidth: ui.sdu(10)
                        topMargin: ui.sdu(3)
                        bottomMargin: ui.sdu(3)
                        onClicked: {
                            if (__App.FromBase64ToPNGImage(ethnicities.base64image, "electricityProduction_RenewableEnergy_chart")) savedToast.show();
                        }
                    }
                }
                Container {
                    layout: GridLayout {
                        columnCount: layUpdate.width > 1000 ? 5 : 3
                    }
                    topPadding: ui.sdu(3)
                    onCreationCompleted: {
                        var _country = [ "France", "Spain", "United Kingdom", "Italy", "Germany" ];
                        var _color = [ "#7FE3A1", "#9988DF", "#FFEA8F", "#FFA08F", "#9693A1" ]
                        for (var i = 0; i < _country.length; i ++) {
                            var obj = legendItem.createObject();
                            obj.color = _color[i];
                            obj.label = _country[i];
                            add(obj);
                        }
                    }
                    attachedObjects: [
                        ComponentDefinition {
                            id: legendItem
                            Container {
                                property string color: "#AAAAAA"
                                property string label: "Italy"
                                id: legendItem_cont
                                leftPadding: ui.sdu(1)
                                rightPadding: ui.sdu(1)
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                Container {
                                    verticalAlignment: VerticalAlignment.Center
                                    preferredHeight: ui.sdu(3)
                                    preferredWidth: ui.sdu(3)
                                    background: Color.create(legendItem_cont.color)
                                }
                                Label {
                                    verticalAlignment: VerticalAlignment.Center
                                    horizontalAlignment: HorizontalAlignment.Left
                                    text: legendItem_cont.label
                                    multiline: true
                                    textStyle.fontSize: FontSize.XSmall
                                }
                            }
                        }
                    ]
                }
                Header {
                    title: "Common food comsumption: JAPAN vs CHINA (Kg/person/yr)"
                }
                WebView {
                    property string base64image: ""
                    id: commonFoodComsumption
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    url: "local:///assets/html/crazyCharts.html" + (debugMode ? "?debug=true" : "")
                    settings.zoomToFitEnabled: true
                    onMessageReceived: {
                        var risp = message.data;
                        console.debug("----- chart created with success");
                        base64image = risp;
                    }
                    onLoadingChanged: {
                        if (loadRequest.status == WebLoadStatus.Succeeded) {
                            createChart();
                        } else if (loadRequest.status == WebLoadStatus.Failed) {
                            //Load failed.
                        }
                    }

                    function createChart() {
                        /*console.debug("*** Items Per Dataset:");
                         * console.debug(JSON.stringify(dd_itemsPerDataset.selectedValue, null, 4));
                         * console.debug("*** Range Value:");
                         * console.debug(JSON.stringify(dd_rangeValue.selectedValue, null, 4));
                         * console.debug("*** Type of Value (labels):");
                         console.debug(JSON.stringify(dd_typeValue.selectedValue, null, 4));*/

                        var datasets = [
                            {
                                label: '#1 JAPAN',
                                data: [ 1.676, 9.02, 3.7, 19.02, 71.71, 43.61, 48.9 ],
                                backgroundColor: [ 'rgba(255, 99, 132, 0.7)', 'rgba(54, 162, 235, 0.7)', 'rgba(255, 206, 86, 0.7)',
                                    'rgba(75, 192, 192, 0.7)', 'rgba(153, 102, 255, 0.7)', 'rgba(255, 159, 64, 0.7)' ],
                                borderColor: [ 'rgba(255,99,132,1)', 'rgba(54, 162, 235, 1)', 'rgba(255, 206, 86, 1)',
                                    'rgba(75, 192, 192, 1)', 'rgba(153, 102, 255, 1)', 'rgba(255, 159, 64, 1)' ],
                                borderWidth: 1
                            },
                            {
                                label: '#2 CHINA',
                                data: [ 0.126, 4.99, 0.061, 19.18, 32.74, 82.29, 65.45 ],
                                backgroundColor: [ 'rgba(255, 99, 132, 0.7)', 'rgba(54, 162, 235, 0.7)', 'rgba(255, 206, 86, 0.7)',
                                    'rgba(75, 192, 192, 0.7)', 'rgba(153, 102, 255, 0.7)', 'rgba(255, 159, 64, 0.7)' ],
                                borderColor: [ 'rgba(255,99,132,1)', 'rgba(54, 162, 235, 1)', 'rgba(255, 206, 86, 1)',
                                    'rgba(75, 192, 192, 1)', 'rgba(153, 102, 255, 1)', 'rgba(255, 159, 64, 1)' ],
                                borderWidth: 1
                            } ];
                        var labels = [ 'Beans', 'Beef', 'Coffee', 'Eggs', 'Milk', 'Rice' ];

                        var options = {
                            title: {
                                display: true,
                                text: 'Common food comsumption: JAPAN vs CHINA (Kg/person/yr)',
                                fontSize: 20
                            },
                            animation: {
                                duration: 0
                            },
                            tooltips: {
                                enabled: false
                            },
                            legend: {
                                display: true,
                                position: 'bottom',
                                labels: {
                                    boxWidth: 12
                                }
                            }
                        };
                        var chart = {
                            'type': 'bar',
                            'datasets': datasets,
                            'options': options,
                            'labels': labels
                        }

                        console.debug(JSON.stringify(chart, null, 4));
                        postMessage(JSON.stringify(chart));
                        console.debug("----- chart data sended");
                    }
                }
                Button {
                    horizontalAlignment: HorizontalAlignment.Center
                    imageSource: "asset:///images/ic_save.png"
                    appearance: ControlAppearance.Primary
                    preferredWidth: ui.sdu(10)
                    topMargin: ui.sdu(3)
                    bottomMargin: ui.sdu(3)
                    onClicked: {
                        if (__App.FromBase64ToPNGImage(ethnicities.base64image, "commonFoodComsumption_chart")) savedToast.show();
                    }
                }
                Header {
                    title: "Custom: Russia VS United States (%)"
                }
                WebView {
                    property string base64image: ""
                    id: ethnicities
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    url: "local:///assets/html/crazyCharts.html" + (debugMode ? "?debug=true" : "")
                    settings.zoomToFitEnabled: true
                    onMessageReceived: {
                        var risp = message.data;
                        console.debug("----- chart created with success");
                        //console.debug(risp);
                        base64image = risp;
                    }
                    onLoadingChanged: {
                        if (loadRequest.status == WebLoadStatus.Succeeded) {
                            createChart();
                        } else if (loadRequest.status == WebLoadStatus.Failed) {
                            //Load failed.
                        }
                    }

                    function createChart() {
                        /*console.debug("*** Items Per Dataset:");
                         * console.debug(JSON.stringify(dd_itemsPerDataset.selectedValue, null, 4));
                         * console.debug("*** Range Value:");
                         * console.debug(JSON.stringify(dd_rangeValue.selectedValue, null, 4));
                         * console.debug("*** Type of Value (labels):");
                         console.debug(JSON.stringify(dd_typeValue.selectedValue, null, 4));*/

                        var datasets = [
                            {
                                label: "Russia",
                                data: [ 79.8, 0, 3.8, 2, 14.4 ],
                                borderColor: "#082568",
                                backgroundColor: "#1240AB",
                                pointBorderColor: "#FFF",
                                pointBackgroundColor: "#082568",
                                pointBorderWidth: 1,
                                fill: false
                            },
                            {
                                label: "United States",
                                data: [ 80, 4.43, 12.85, 0, 2.72 ],
                                borderColor: "#7C0046",
                                backgroundColor: "#CD0074",
                                pointBorderColor: "#FFF",
                                pointBackgroundColor: "#7C0046",
                                pointBorderWidth: 1,
                                fill: false
                            } ];

                        var labels = [ "A", "B", "C", "D", "E", "F" ];

                        var options = {
                            title: {
                                display: true,
                                text: 'Custom: Russia VS United States (%)',
                                fontSize: 20
                            },
                            animation: {
                                duration: 0
                            },
                            tooltips: {
                                enabled: false
                            },
                            legend: {
                                display: true,
                                position: 'bottom',
                                labels: {
                                    boxWidth: 12
                                }
                            }
                        };
                        var chart = {
                            'type': 'bar',
                            'datasets': datasets,
                            'options': options,
                            'labels': labels
                        }

                        console.debug(JSON.stringify(chart, null, 4));
                        postMessage(JSON.stringify(chart));
                        console.debug("----- chart data sended");
                    }
                }

                Button {
                    horizontalAlignment: HorizontalAlignment.Center
                    imageSource: "asset:///images/ic_save.png"
                    appearance: ControlAppearance.Primary
                    preferredWidth: ui.sdu(10)
                    topMargin: ui.sdu(3)
                    bottomMargin: ui.sdu(3)
                    onClicked: {
                        if (__App.FromBase64ToPNGImage(ethnicities.base64image, "custom_chart")) savedToast.show();
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
            LayoutUpdateHandler {
                property int height: 1
                property int width: 1
                id: layUpdate
                onLayoutFrameChanged: {
                    height = layoutFrame.height;
                    width = layoutFrame.width;
                }

            },
            SystemToast {
                id: savedToast
                body: "Your chart was saved with success :)\nOpen the image gallery to view it."
            }
        ]
    }
}
