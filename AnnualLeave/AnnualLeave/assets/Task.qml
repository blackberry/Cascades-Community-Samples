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

import bb.cascades 1.2

Container {

    property variant task_id: -1
    property variant task_type: 0
    property variant local_request_id: -1
    property variant originating_op_id: -1
    property variant user_id: -1
    property alias request_text: ta_request.text
    property alias update_text: ta_update.text
    property int task_status
    property int decision
    property variant decision_date
    property alias note: ta_note.text
    property int approval_choice: 2
    property alias decision_approved: rb_approved
    property alias decision_rejected: rb_rejected
    property alias decision_choices: rg_approved

    signal finished()

    function decisionText(decision, decision_date) {
        var text = "";
        switch (decision) {
            case 0:
                return "UNDECIDED";
            case 1:
                text = "APPROVED";
                break;
            case 2:
                text = "REJECTED";
                break;
            default:
                return "UNKNOWN";
        }
        return text + " on " + app.toLocalDateFormat(decision_date)
    }

    function taskTypeText() {
        switch (task_type) {
            case 0:
                return "Request"
            case 1:
                return "Cancellation"
            case 2:
                return "Update"
            default:
                return "UNKNOWN TASK TYPE"
        }
    }

    Container {

        ScrollView {

            Container {

                Style {
                    id: style
                }
                Container {
                    Container {
                        topPadding: 20
                        Container {
                            leftPadding: 22
                            topPadding: 20
                            Label {
                                id: lbl_processing_status
                                text: "Processing Status"
                                textStyle.fontWeight: style.task_label_style
                            }
                        }
                        Container {
                            leftPadding: 22
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            ImageView {
                                id: status_icon
                                imageSource: app.taskIcon(task_type, task_status, decision)
                            }
                            Label {
                                id: lbl_status_text
                                text: app.taskStatus(task_status, decision)
                                multiline: true
                            }
                        }
                    }
                    Container {
                        topPadding: 20
                        Container {
                            leftPadding: 22
                            topPadding: 20
                            Label {
                                id: lbl_decision
                                text: "Decision"
                                textStyle.fontWeight: style.task_label_style
                            }
                        }
                        Container {
                            leftPadding: 22
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Label {
                                id: lbl_decision_text
                                text: decisionText(decision, decision_date)
                            }
                        }
                    }
                    Divider {
                    }

                    Container {
                        topPadding: 10
                        Container {
                            leftPadding: 22
                            topPadding: 20
                            Label {
                                id: lbl_from
                                text: "User"
                                textStyle.fontWeight: style.task_label_style
                            }
                        }
                        Container {
                            leftPadding: 22
                            topPadding: 20
                            Label {
                                id: lbl_user_id
                                text: user_id
                            }
                        }
                        Container {
                            topPadding: 20
                            leftPadding: 22
                            Label {
                                id: lbl_request
                                text: taskTypeText()
                                textStyle.fontWeight: style.task_label_style
                            }
                            Label {
                                id: ta_original_label
                                text: "Original details:"
                                visible: (task_type == 2) ? true : false
                            }
                            Label {
                                id: ta_request
                                multiline: true
                            }
                            Label {
                                id: ta_update_label
                                text: "New requested details:"
                                visible: (task_type == 2) ? true : false
                            }
                            Label {
                                id: ta_update
                                multiline: true
                                visible: (task_type == 2) ? true : false
                            }
                        }
                        Container {
                            topPadding: 20
                            leftPadding: 22
                            Label {
                                id: lbl_note
                                text: "Note"
                                textStyle.fontWeight: style.task_label_style
                            }
                            Label {
                                id: ta_note
                                multiline: true
                            }
                        }

                        Container {
                            topPadding: 20
                            horizontalAlignment: HorizontalAlignment.Fill
                            visible: (task_status == 0) ? true : false
                            RadioGroup {
                                id: rg_approved
                                onSelectedValueChanged: {
                                    approval_choice = rg_approved.selectedIndex + 1
                                }
                                Option {
                                    id: rb_approved
                                    text: "Approved"
                                    value: 1
                                    selected: (approval_choice == 1)
                                }
                                Option {
                                    id: rb_rejected
                                    text: "Rejected"
                                    value: 2
                                    selected: (approval_choice == 2 || approval_choice == 0)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}