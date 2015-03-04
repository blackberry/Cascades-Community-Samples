import bb.cascades 1.2

Container {

    property int local_request_id: -1
    property alias from_date: dp_from_date.value
    property alias to_date: dp_to_date.value
    property int current_leave_year
    property variant current_from_date
    property variant current_to_date
    property variant current_first_day_half
    property variant current_last_day_half
    property variant current_day_half
    property int half_days
    property int booking_status
    property int last_op_status
    property int mode_new_request: 1
    property int mode_edit_request: 2
    property int mode
    property alias note: ta_note.text
    property variant status_text
    property alias first_day_half: cb_first_day_half.checked
    property alias last_day_half: cb_last_day_half.checked
    property int day_half
    property variant updating_text

    signal finished()

    function daysUsedText(half_days) {
        if (half_days > 0) {
            return (half_days / 2);
        } else {
            return "TBC"
        }
    }

    onDay_halfChanged: {
        hd_morning.selected = false;
        hd_afternoon.selected = false;
        if (day_half == 1) {
            hd_morning.selected = true;
            return;
        }
        if (day_half == 2) {
            hd_afternoon.selected = true;
        }
    }

    Style {
        id: style
    }

    ScrollView {

        Container {

            Container {
                Container {
                    topPadding: 20
                    DateTimePicker {
                        id: dp_from_date
                        title: "From:"
                        mode: DateTimePickerMode.Date
                        onValueChanged: {
                            if (dp_from_date.value > dp_to_date.value) {
                                dp_to_date.value = dp_from_date.value
                            }
                            if (app.sameDates(dp_from_date.value, dp_to_date.value)) {
                                cb_last_day_half.visible = false;
                                cb_last_day_half.visible = false;
                                cb_first_day_half.visible = true;
                            } else {
                                cb_last_day_half.visible = true;
                                cb_last_day_half.visible = true;
                                cb_first_day_half.visible = true;
                            }
                        }
                    }
                    Container {
                        leftPadding: 22
                        CheckBox {
                            id: cb_first_day_half
                            checked: false
                            text: app.sameDates(dp_from_date.value, dp_to_date.value) ? "Half Day Only" : "Start with half day"
                            onCheckedChanged: {
                                if (checked) {
                                    // default to morning if not already set
                                    if (app.sameDates(dp_from_date.value, dp_to_date.value)) {
                                        if (day_half == 0) {
                                            day_half = 1;
                                        }
                                    } else {
                                        cb_last_day_half.checked = false;
                                    }
                                }
                            }
                        }
                        RadioGroup {
                            id: rg_day_half
                            visible: app.sameDates(dp_from_date.value, dp_to_date.value) && cb_first_day_half.checked ? true : false
                            Option {
                                id: hd_morning
                                text: "Morning"
                                value: 1
                                selected: (day_half == 1) ? true : false
                            }
                            Option {
                                id: hd_afternoon
                                text: "Afternoon"
                                value: 2
                                selected: (day_half == 2) ? true : false
                            }
                            onSelectedValueChanged: {
                                if (selectedValue != undefined) {
                                    day_half = selectedValue
                                }
                            }
                        }
                    }
                }
                Container {
                    topPadding: 20
                    DateTimePicker {
                        id: dp_to_date
                        title: "To:"
                        mode: DateTimePickerMode.Date
                        onValueChanged: {
                            if (app.sameDates(dp_from_date.value, dp_to_date.value)) {
                                cb_last_day_half.visible = false;
                                cb_last_day_half.visible = false;
                                cb_first_day_half.visible = true;
                            } else {
                                cb_last_day_half.visible = true;
                                cb_last_day_half.visible = true;
                                cb_first_day_half.visible = true;
                            }
                        }
                    }
                    Container {
                        leftPadding: 22
                        CheckBox {
                            id: cb_last_day_half
                            visible: app.sameDates(dp_from_date.value, dp_to_date.value) ? false : true
                            checked: false
                            text: "End with half day"
                            onCheckedChanged: {
                                if (checked) {
                                    cb_first_day_half.checked = false;
                                }
                            }
                        }
                    }
                }
                Divider {

                }
                Container {
                    leftPadding: 22
                    topPadding: 20
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Label {
                        id: lbl_days_used_label
                        text: "Days used:"
                        textStyle.fontWeight: style.request_label_style
                        visible: mode == mode_edit_request ? true : false
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 80
                        }
                    }
                    Label {
                        id: lbl_days_used_value
                        visible: mode == mode_edit_request ? true : false
                        text: daysUsedText(half_days)
                        textStyle {
                            textAlign: TextAlign.Right
                        }
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                        horizontalAlignment: HorizontalAlignment.Right
                    }
                }
                Container {
                    topPadding: 20
                    leftPadding: 22
                    Label {
                        id: lbl_note
                        text: "Note"
                        textStyle.fontWeight: style.request_label_style
                    }
                    TextArea {
                        id: ta_note

                    }
                }

                Container {
                    topPadding: 20
                    leftPadding: 22
                    Label {
                        text: "Details"
                        textStyle {
                            fontWeight: style.request_label_style
                        }
                    }
                    Label {
                        text: app.requestParamsInterpretation(dp_from_date.value, dp_to_date.value, cb_first_day_half.checked, cb_last_day_half.checked, rg_day_half.selectedValue)
                        textStyle {
                            color: style.details_text_color
                        }
                        multiline: true
                    }
                }
                Container {
                    topPadding: 20
                    leftPadding: 22
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Label {
                        visible: mode == mode_edit_request ? true : false
                        text: "Status"
                        textStyle {
                            fontWeight: style.request_label_style
                        }
                    }
                }
                Container {
                    leftPadding: 22
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    ImageView {
                        id: status_icon
                        visible: mode == mode_edit_request ? true : false
                        imageSource: app.statusIcon(booking_status, last_op_status)
                    }
                    Label {
                        id: lbl_status_text
                        visible: mode == mode_edit_request ? true : false
                        text: status_text + "\n" + updating_text
                        multiline: true
                    }
                }
            }
        }
    }
}