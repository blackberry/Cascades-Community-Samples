/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
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

import bb.cascades 1.4

TabbedPane {
    Tab {
        id: bar
        title: qsTr("Bar") + Retranslate.onLocaleOrLanguageChanged
        delegate: Delegate {
            source: "Bar.qml"
        }
    }
    Tab {
        id: line
        title: qsTr("Line") + Retranslate.onLocaleOrLanguageChanged
        delegate: Delegate {
            source: "Line.qml"
        }
    }
    Tab {
        id: radar
        title: qsTr("Radar") + Retranslate.onLocaleOrLanguageChanged
        delegate: Delegate {
            source: "Radar.qml"
        }
    }
    Tab {
        id: polarArea
        title: qsTr("Polar Area") + Retranslate.onLocaleOrLanguageChanged
        delegate: Delegate {
            source: "PolarArea.qml"
        }
    }
    Tab {
        id: pieDoughnut
        title: qsTr("Pie & Doughnut") + Retranslate.onLocaleOrLanguageChanged
        delegate: Delegate {
            source: "PieDoughnut.qml"
        }
    }
    Tab {
        id: bubble
        title: qsTr("Bubble") + Retranslate.onLocaleOrLanguageChanged
        delegate: Delegate {
            source: "Bubble.qml"
        }
    }
    Tab {
        id: crazyCharts
        title: qsTr("Crazy Charts") + Retranslate.onLocaleOrLanguageChanged
        delegate: Delegate {
            source: "CrazyCharts.qml"
        }
        delegateActivationPolicy: TabDelegateActivationPolicy.ActivateWhenSelected
    }
}
