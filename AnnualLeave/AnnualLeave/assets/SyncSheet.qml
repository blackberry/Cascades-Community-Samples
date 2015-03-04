/* Copyright (c) 2014 BlackBerry Limited.
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

Sheet {
    id: root

    function getYear(year_inx) {
        // 0 means current year
        // other years have an index relative to 0 so next year is +1 and last year is -1
        var d = new Date();
        var this_year = d.getFullYear();

        return this_year + year_inx;
    }

    Page {
        actionBarVisibility: ChromeVisibility.Visible

        titleBar: TitleBar {
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                        CustomButton {
                            id: title_btn1
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            text: "Back"
                            onClicked: {
                                close();
                            }
                            leftPadding: 10
                            topPadding: 15
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Center
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 60
                        }
                        Label {
                            text: qsTr("Sync")
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontWeight: style.heading_weight
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                        CustomButton {
                            id: title_btn2
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            text: "Sync"
                            onClicked: {
                                app.clientInitiatedSync(dd_leave_year.selectedValue)
                                close();
                            }
                            rightPadding: 10
                            topPadding: 15
                        }
                    }
                }
            }
        }

        Container {
            layout: StackLayout {
            }
            leftPadding: 20
            rightPadding: 20
            verticalAlignment: VerticalAlignment.Fill

            Container {

                Style {
                    id: style
                }
                Label {
                    id: lbl_sync0
                    text: qsTr("Select the year to request synchronization for")
                    horizontalAlignment: HorizontalAlignment.Center;
                    multiline: true
                    textStyle {
                        fontWeight: style.help_label_style
                    }
                }

                Label {
                    id: lbl_sync1
                    text: qsTr("then touch the Sync button")
                    horizontalAlignment: HorizontalAlignment.Center;
                    multiline: true
                    textStyle {
                        fontWeight: style.help_label_style
                    }
                }

                DropDown {
                    id: dd_leave_year
                    title: "Year"
                    Option {
                        text: getYear(+ 1)
                        value: getYear(+ 1)
                        selected: false
                    }
                    Option {
                        text: getYear(0)
                        value: getYear(0)
                        selected: true
                    }
                    Option {
                        text: getYear(-1)
                        value: getYear(-1)
                        selected: false
                    }
                    Option {
                        text: getYear(-2)
                        value: getYear(-2)
                        selected: false
                    }
                    Option {
                        text: getYear(-3)
                        value: getYear(-3)
                        selected: false
                    }
                    Option {
                        text: getYear(-4)
                        value: getYear(-4)
                        selected: false
                    }
                }

            }
        }

    }
}
