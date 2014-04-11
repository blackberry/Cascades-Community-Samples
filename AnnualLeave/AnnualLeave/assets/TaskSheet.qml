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
    id: task_sheet

    property alias task_id: edit_task.task_id
    property alias task_type: edit_task.task_type
    property alias local_request_id: edit_task.local_request_id
    property alias originating_op_id: edit_task.originating_op_id
    property alias user_id: edit_task.user_id
    property alias request_text: edit_task.request_text
    property alias update_text: edit_task.update_text
    property alias task_status: edit_task.task_status
    property alias decision: edit_task.decision
    property alias decision_date: edit_task.decision_date
    property alias note: edit_task.note
    property alias approval_choice: edit_task.approval_choice

    function connectFinished() {
        edit_task.finished.connect(task_sheet.close);
    }

    function reset() {
        if (decision == 0) {
            console.log("Reseting TaskSheet");
            edit_task.decision_choices.selectedIndex = 1;
        }
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
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 60
                        }
                        Label {
                            text: qsTr("Approval Task")
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontWeight: style.heading_weight
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        visible: (task_status == 0 && approval_choice > 0) ? true : false
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                        CustomButton {
                            id: title_btn2
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Fill
                            text: "Submit"
                            onClicked: {
                                app.submitApprovalDecision(task_id, task_type, local_request_id, approval_choice, originating_op_id)
                                task_sheet.close();
                            }
                            rightPadding: 10
                            topPadding: 15
                        }
                    }
                    Container {
                        verticalAlignment: VerticalAlignment.Fill
                        visible: (task_status != 0 || approval_choice == 0) ? true : false
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 20
                        }
                    }
                }
            }
        }

        Task {
            id: edit_task
        }
    }
}
