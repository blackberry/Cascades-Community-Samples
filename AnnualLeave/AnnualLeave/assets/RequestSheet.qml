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

import bb.cascades 1.0

Sheet {
    id: request_sheet
    property alias local_request_id: edit_request.local_request_id
    property alias current_leave_year: edit_request.current_leave_year
    property alias current_from_date: edit_request.current_from_date
    property alias current_to_date: edit_request.current_to_date
    property alias current_first_day_half: edit_request.current_first_day_half
    property alias current_last_day_half: edit_request.current_last_day_half
    property alias current_day_half: edit_request.current_day_half
    property alias from_date: edit_request.from_date
    property alias to_date: edit_request.to_date
    property alias half_days: edit_request.half_days
    property alias booking_status: edit_request.booking_status
    property alias last_op_status: edit_request.last_op_status
    property alias mode: edit_request.mode
    property alias note: edit_request.note
    property alias updating_text: edit_request.updating_text
    property alias status_text: edit_request.status_text
    property alias first_day_half: edit_request.first_day_half
    property alias last_day_half: edit_request.last_day_half
    property alias day_half: edit_request.day_half
    property int mode_new_request: 1
    property int mode_edit_request: 2

    function connectFinished() {
        edit_request.finished.connect(request_sheet.close);
    }
    
    function reset() {
        from_date = new Date();
        to_date = new Date();
        first_day_half = false;
        last_day_half = false;
        day_half = 0;
        note = "";
    }

    Page {

        titleBar: TitleBar {
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Style {
                        id: style
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        //                        background: Color.Green
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
                        //                        background: Color.Red
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 60
                        }
                        Label {
                            text: qsTr("Vacation Request")
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontWeight: style.heading_weight
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        //                        background: Color.Green
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                        CustomButton {
                            id: title_btn2
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            text: (mode == mode_new_request) ? "Submit" : "Update"
                            onClicked: {
                                if (mode == mode_new_request) {
                                    var valid = app.isValidDateRange(from_date, to_date, "Requested dates");
                                    if (valid) {
                                        app.submitBookingRequest(from_date, to_date, first_day_half, last_day_half, day_half, note)
                                        close();
                                    }
                                } else {
                                    var valid = app.isValidDateRange(current_from_date, current_to_date, "Current dates");
                                    if (valid) {
                                        valid = app.isValidDateRange(from_date, to_date, "New dates");
                                    }
                                    if (valid) {
                                        app.updateBookingRequest(local_request_id, current_leave_year, current_from_date, from_date, current_to_date, to_date, current_first_day_half, first_day_half, current_last_day_half, last_day_half, current_day_half, day_half, note)
                                        close();
                                    }
                                }
                            }
                            rightPadding: 10
                            topPadding: 15
                        }
                    }
                }
            }
        }

        Request {
            id: edit_request
        }
    }

}
