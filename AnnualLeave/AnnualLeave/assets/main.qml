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
import bb.data 1.0

TabbedPane {

    id: tabbed_pane
    objectName: "tabbed_pane"
    showTabsOnActionBar: true
    
    function getYear(year_inx) {
        // 0 means current year
        // other years have an index relative to 0 so next year is +1 and last year is -1

        var d = new Date();
        var this_year = d.getFullYear();

        return this_year + year_inx;
    }

    function onShowTab(tab_index) {
        switch (tab_index) {
            case 0:
                tabbed_pane.activeTab = tab_request_list
                break;
            case 1:
                tabbed_pane.activeTab = tab_new
                break;
            case 2:
                tabbed_pane.activeTab = tab_inbox
                break;
            case 3:
                tabbed_pane.activeTab = tab_dev
                break;
            default:
                console.log("QQQQ onShowTab(" + tab_index + ") - invalid tab index");
        }
    }

    function refreshRequests() {
        console.log("QQQQ refreshRequests");
        requestsDataModel.clear();
        app.loadRequestData(dd_leave_year.selectedValue);
        tabbed_pane.activeTab = tab_request_list
    }

    function refreshRequestsForYear(year) {
        console.log("QQQQ refreshRequests for "+year);
        requestsDataModel.clear();
        app.loadRequestData(year);
		var options = dd_leave_year.options;
        for (var i = 0; i < dd_leave_year.options.length; i ++) {
            if (dd_leave_year.options[i].value == year) {
                dd_leave_year.options[i].selected = true
            } else {
                dd_leave_year.options[i].selected = false
            }
        }

		console.log("QQQQ options="+options); 
        tabbed_pane.activeTab = tab_request_list
    }

    function loadRequests(data) {
        console.log("QQQQ loadRequests")
        console.log("QQQQ clearing data model");
        requestsDataModel.clear();
        console.log("QQQQ inserting list into data model");
        requestsDataModel.insertList(data);
        tabbed_pane.activeTab = tab_request_list
    }

    function refreshTasks() {
        console.log("QQQQ refreshTasks");
        tasksDataModel.clear();
        app.loadTaskData();
        tabbed_pane.activeTab = tab_inbox
    }

    function loadTasks(data) {
        console.log("QQQQ loadTasks" + data)
        console.log("QQQQ clearing data model");
        tasksDataModel.clear();
        console.log("QQQQ inserting list into data model");
        tasksDataModel.insertList(data);
        tabbed_pane.activeTab = tab_inbox
    }

    function entitlementMessage() {
        var total_entitlement = app.total_entitlement / 2;
        var current_entitlement = app.current_entitlement / 2;
        if (total_entitlement > -0.5) {
            return current_entitlement + " / " + total_entitlement;
        } else {
            return "unknown";
        }
    }

    Tab {
        id: tab_request_list
        // Localized text with the dynamic translation and locale updates support
        title: qsTr("Requests") + Retranslate.onLocaleOrLanguageChanged
        imageSource: "asset:///images/out_2.png"

        onTriggered: {
            console.log("QQQQ tab is loading data");
            app.loadRequestData(dd_leave_year.selectedValue);
        }
        

        Page {

            Container {
                leftPadding: 20
                rightPadding: 20
                
                Label {
                    text: qsTr("Requests and Entitlement") + Retranslate.onLocaleOrLanguageChanged
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.fontWeight: style.heading_weight
                }
                Label {
                    id: lbl_allowance
                    text: "Days available: " + entitlementMessage()
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Divider {
                }
                // populate properly from C++
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

                    onSelectedIndexChanged: {
                        app.refreshEntitlement(dd_leave_year.selectedValue);
                        app.loadRequestData(dd_leave_year.selectedValue);
                    }
                }
                Divider {
                }
                Container {
                    ListView {
                        id: al_list
                        objectName: "al_list"

                        dataModel: requestsDataModel

                        function daysText(half_days) {
                            if (half_days >= 0) {
                                var days = half_days / 2;
                                if ((half_days > 2) || (half_days < 1)) {
                                    return days + " days";
                                } else {
                                    return days + " day";
                                }
                            } else {
                                return ""
                            }
                        }

                        onTriggered: {
                            console.log("QQQQ onTriggered. indexPath:" + indexPath);
                            var selected_row = requestsDataModel.data(indexPath);

                            requestSheet.mode = requestSheet.mode_edit_request
                            requestSheet.local_request_id = selected_row.local_request_id;
                            requestSheet.from_date = selected_row.from_date
                            requestSheet.to_date = selected_row.to_date
                            requestSheet.current_leave_year = selected_row.leave_year;
                                                        requestSheet.current_from_date = selected_row.from_date;
                            requestSheet.current_to_date = selected_row.to_date;
                            
                            requestSheet.current_first_day_half = selected_row.first_day_half;
                            requestSheet.current_last_day_half = selected_row.last_day_half;
                            requestSheet.current_day_half = selected_row.day_half;
                            requestSheet.half_days = selected_row.half_days;
                            requestSheet.booking_status = selected_row.booking_status;
                            requestSheet.last_op_status = selected_row.last_op_status;
                            requestSheet.updating_text = app.updatingText(selected_row.last_op_status, selected_row.updating_from_date, selected_row.updating_to_date, selected_row.updating_first_day_half, selected_row.updating_last_day_half, selected_row.updating_day_half)
                            requestSheet.status_text = app.statusText(selected_row.booking_status, selected_row.last_op_status)
                            requestSheet.first_day_half = (selected_row.first_day_half == 1 ? true : false)
                            requestSheet.last_day_half = (selected_row.last_day_half == 1 ? true : false)
                            requestSheet.day_half = selected_row.day_half
                            requestSheet.note = selected_row.note;
                            requestSheet.connectFinished()
                            requestSheet.open();
                        }

                        listItemComponents: [
                            ListItemComponent {
                                type: "item"
                                StandardListItem {
                                    id: al_item
                                    title: app.datesText(ListItemData.from_date, ListItemData.to_date, ListItemData.first_day_half, ListItemData.last_day_half, ListItemData.first_day_half, ListItemData.day_half)
                                    description: app.statusText(ListItemData.booking_status, ListItemData.last_op_status) + " - " + al_item.ListItem.view.daysText(ListItemData.half_days)
                                    imageSource: app.statusIcon(ListItemData.booking_status, ListItemData.last_op_status)
                                    textFormat: TextFormat.Html

                                    attachedObjects: [
                                        RequestSheet {
                                            id: ctx_requestSheet
                                        }
                                    ]

                                    contextActions: [
                                        ActionSet {
                                            title: "Actions"
                                            subtitle: "Act upon the selected item"

                                            actions: [
                                                ActionItem {
                                                    title: "Edit"
                                                    imageSource: "asset:///images/al_edit.png"
                                                    onTriggered: {
                                                        ctx_requestSheet.mode = ctx_requestSheet.mode_edit_request
                                                        ctx_requestSheet.local_request_id = ListItemData.local_request_id;
                                                        ctx_requestSheet.from_date = ListItemData.from_date
                                                        ctx_requestSheet.to_date = ListItemData.to_date
                                                        ctx_requestSheet.current_leave_year = ListItemData.leave_year;
                                                        ctx_requestSheet.current_from_date = selected_row.from_date;
                                                        ctx_requestSheet.current_to_date = selected_row.to_date;                                                        
                                                        ctx_requestSheet.current_first_day_half = ListItemData.first_day_half;
                                                        ctx_requestSheet.current_last_day_half = ListItemData.last_day_half;
                                                        ctx_requestSheet.current_day_half = ListItemData.day_half;
                                                        ctx_requestSheet.day_half = ListItemData.day_half;
                                                        ctx_requestSheet.half_days = ListItemData.half_days;
                                                        ctx_requestSheet.booking_status = ListItemData.booking_status;
                                                        ctx_requestSheet.note = ListItemData.note;
                                                        ctx_requestSheet.status_text = app.statusText(ListItemData.booking_status, ListItemData.last_op_status)
                                                        ctx_requestSheet.connectFinished()
                                                        ctx_requestSheet.open();
                                                    }
                                                },
                                                ActionItem {
                                                    title: "Clear Processing Status"
                                                    imageSource: "asset:///images/clear.png"
                                                    onTriggered: {
                                                        app.clearProcessingStatus(ListItemData.local_request_id, ListItemData.leave_year)
                                                    }
                                                },
                                                ActionItem {
                                                    title: "Add to Calendar"
                                                    imageSource: "asset:///images/add_calendar.png"
                                                    onTriggered: {
                                                        app.addCalendarEntry(ListItemData.from_date, ListItemData.to_date, ListItemData.first_day_half, ListItemData.last_day_half, ListItemData.day_half, ListItemData.note)
                                                    }
                                                },
                                                DeleteActionItem {
                                                    title: "Delete/Cancel"
                                                    onTriggered: {
                                                        app.cancelBookingRequest(ListItemData.local_request_id, ListItemData.booking_status);

                                                    }
                                                }
                                            ]
                                        } // end of ActionSet
                                    ] // end of contextActions list
                                }
                            }
                        ]

                        onCreationCompleted: {
                            console.log("QQQQ onCreationCompleted for ListView - invoking loadRequestData with " + dd_leave_year.selectedValue)
                            app.loadRequestData(dd_leave_year.selectedValue);
                        }

                    } // ListView
                }
            }

            attachedObjects: [
                RequestSheet {
                    id: requestSheet
                },
                GroupDataModel {
                    id: requestsDataModel
                    sortingKeys: [ "from_date" ]
                    grouping: ItemGrouping.None
                }
            ]

        }
    } //End of first tab
    Tab { //Second tab
        id: tab_new
        title: qsTr("New") + Retranslate.onLocaleOrLanguageChanged
        imageSource: "asset:///images/new_2.png"

        onTriggered: {
            newRequestSheet.reset()
            newRequestSheet.open()
        }

        attachedObjects: [
            RequestSheet {
                id: newRequestSheet
                mode: mode_new_request
            }
        ]
    } //End of second tab
    Tab { //Third tab
        id: tab_inbox
        title: qsTr("Inbox") + Retranslate.onLocaleOrLanguageChanged
        imageSource: "asset:///images/in_2.png"
        onTriggered: {
            console.log("QQQQ tab is loading data");
            app.loadTaskData();
        }

        Page {
            Container {
                leftPadding: 20
                rightPadding: 20
                
                Style {
                    id: style                
                }
                
                Label {
                    text: qsTr("Inbox") + Retranslate.onLocaleOrLanguageChanged
                    textStyle.base: SystemDefaults.TextStyles.TitleText
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.fontWeight: style.heading_weight
                }
                Divider {
                }
                Container {
                    visible: app.inbox_is_empty
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Center
                    Label {
                        text: "Your Inbox is Empty"
                        textStyle {
                            color: style.inbox_empty_color
                        }
                    }
                }
                Container {
                    visible: ! app.inbox_is_empty

                    ListView {
                        id: task_list
                        objectName: "task_list"

                        dataModel: tasksDataModel

                        onTriggered: {
                            console.log("QQQQ onTriggered. indexPath:" + indexPath);
                            var selected_row = tasksDataModel.data(indexPath);
                            console.log("QQQQ onTriggered. Task ID:" + selected_row.task_id);
                            console.log("QQQQ onTriggered. Task ID:" + selected_row.task_type);
                            console.log("QQQQ onTriggered. Local Request ID:" + selected_row.local_request_id);
                            console.log("QQQQ onTriggered. Originating Op ID:" + selected_row.originating_op_id);
                            console.log("QQQQ onTriggered. user_id:" + selected_row.user_id);
                            console.log("QQQQ onTriggered. from_date:" + selected_row.from_date);
                            console.log("QQQQ onTriggered. to_date:" + selected_row.to_date);
                            console.log("QQQQ onTriggered. half_days:" + selected_row.half_days);
                            console.log("QQQQ onTriggered. task_status:" + selected_row.task_status);
                            console.log("QQQQ onTriggered. decision:" + selected_row.decision);
                            console.log("QQQQ onTriggered. decision_date:" + selected_row.decision_date);
                            console.log("QQQQ onTriggered. note:" + selected_row.note);

                            taskSheet.task_id = selected_row.task_id;
                            taskSheet.task_type = selected_row.task_type;
                            taskSheet.local_request_id = selected_row.local_request_id;
                            taskSheet.originating_op_id = selected_row.originating_op_id;
                            taskSheet.user_id = selected_row.user_id;
                            taskSheet.task_status = selected_row.task_status;
                            taskSheet.decision = selected_row.decision;
                            taskSheet.decision_date = selected_row.decision_date;
                            taskSheet.note = selected_row.note;
                            taskSheet.request_text = app.approvalParamsInterpretation(selected_row.from_date, selected_row.to_date, selected_row.first_day_half, selected_row.last_day_half, selected_row.day_half)
                            if (selected_row.task_type == 2) {
                                taskSheet.update_text = app.approvalParamsInterpretation(selected_row.updating_from_date, selected_row.updating_to_date, selected_row.updating_first_day_half, selected_row.updating_last_day_half, selected_row.updating_day_half)
                            } else {
                                taskSheet.update_text = "";
                            }
                            taskSheet.connectFinished()
                            taskSheet.reset();
                            taskSheet.open();
                        }

                        listItemComponents: [
                            ListItemComponent {
                                type: "item"
                                StandardListItem {
                                    id: task_item
                                    title: app.taskType(ListItemData.task_type) + " - " + app.taskDecision(ListItemData.decision)
                                    description: app.taskText(ListItemData.task_status, ListItemData.decision, ListItemData.user_id, ListItemData.half_days, ListItemData.from_date)
                                    imageSource: app.taskIcon(ListItemData.task_type, ListItemData.task_status, ListItemData.decision)

                                    attachedObjects: [
                                        TaskSheet {
                                            id: ctx_taskSheet
                                        }
                                    ]

                                    contextActions: [
                                        ActionSet {
                                            title: "Actions"
                                            subtitle: "Act upon the selected item"

                                            actions: [
                                                ActionItem {
                                                    title: "Open"
                                                    imageSource: "asset:///images/ic_open.png"
                                                    onTriggered: {
                                                        ctx_taskSheet.task_id = ListItemData.task_id;
                                                        ctx_taskSheet.task_type = ListItemData.task_type;
                                                        ctx_taskSheet.local_request_id = ListItemData.local_request_id;
                                                        ctx_taskSheet.originating_op_id = ListItemData.originating_op_id;
                                                        ctx_taskSheet.user_id = ListItemData.user_id;
                                                        ctx_taskSheet.task_status = ListItemData.task_status;
                                                        ctx_taskSheet.decision = ListItemData.decision;
                                                        ctx_taskSheet.decision_date = ListItemData.decision_date;
                                                        ctx_taskSheet.note = ListItemData.note;
                                                        ctx_taskSheet.request_text = app.approvalParamsInterpretation(ListItemData.from_date, ListItemData.to_date, ListItemData.first_day_half, ListItemData.last_day_half, ListItemData.day_half)
                                                        if (ListItemData.task_type == 2) {
                                                            ctx_taskSheet.update_text = app.approvalParamsInterpretation(ListItemData.updating_from_date, ListItemData.updating_to_date, ListItemData.updating_first_day_half, ListItemData.updating_last_day_half, ListItemData.updating_day_half)
                                                        } else {
                                                            ctx_taskSheet.update_text = "";
                                                        }
                                                        ctx_taskSheet.connectFinished()
                                                        ctx_taskSheet.reset();
                                                        ctx_taskSheet.open();
                                                    }
                                                },
                                                DeleteActionItem {
                                                    title: "Delete"
                                                    onTriggered: {
                                                        app.deleteApprovalTask(ListItemData.task_id);

                                                    }
                                                }
                                            ]
                                        } // end of ActionSet
                                    ] // end of contextActions list
                                }
                            }
                        ]

                        onCreationCompleted: {
                            app.loadTaskData();
                        }

                    } // ListView
                }
            }
        }

        attachedObjects: [
            TaskSheet {
                id: taskSheet
            },
            GroupDataModel {
                id: tasksDataModel
                sortingKeys: [ "from_date" ]
                grouping: ItemGrouping.None
            }
        ]

    } //End of third tab

    attachedObjects: [
        HelpSheet {
            id: help
        },
        SyncSheet {
            id: sync
        },
        AboutSheet {
            id: aboutInfo
        },
        Settings {
            id: settings
            objectName: "settings"
        }
    ]

    Menu.definition: MenuDefinition {
        actions: [
            ActionItem {
                id: action_help
                title: "Help"
                imageSource: "asset:///images/help.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    help.open();
                }
            },
            ActionItem {
                title: "About"
                imageSource: "images/about.png"
                
                onTriggered: {
                    aboutInfo.open();
                }
            },
            ActionItem {
                id: action_sync
                title: "Sync"
                imageSource: "asset:///images/sync.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    sync.open();
                }
            },
            ActionItem {
                id: action_settings
                title: "Settings"
                imageSource: "asset:///images/al_settings.png"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    settings.open();
                }
            }
         ]
    }
}
