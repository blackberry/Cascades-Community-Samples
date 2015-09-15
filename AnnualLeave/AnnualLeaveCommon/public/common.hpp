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

#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <ApiVersion.hpp>

static const int OP_STATUS_OK = 0;
static const int OP_STATUS_NOT_SUPPORTED = 125;

static const int STATUS_BOOKING_PENDING = 0;   // Created by user and submitted locally (into the outbound Ops Queue)
static const int STATUS_BOOKING_SUBMITTED = 1; // Acknowledgement from enterprise system has been received
static const int STATUS_BOOKING_APPROVED = 2;  // Request has been approved by the manager
static const int STATUS_BOOKING_REJECTED = 3;    // Manager has rejected the request

static const int STATUS_CANCELLATION_PENDING = 4; // User has cancelled their booking but it has not yet been submitted to the enterprise system
static const int STATUS_CANCELLATION_SUBMITTED = 5; // User has cancelled their booking and it has been submitted to the enterprise system
static const int STATUS_CANCELLATION_OK = 6; // Back end system has indicated that a cancellation request was processed successfully
static const int STATUS_CANCELLATION_REJECTED = 7; // Back end system has indicated that a cancellation request was rejected for some reason

static const int STATUS_UPDATE_PENDING = 8; // User has issued an update to their booking but it has not yet been submitted to the enterprise system
static const int STATUS_UPDATE_SUBMITTED = 9; // User has issued an update to their request and it has been submitted to the enterprise system
static const int STATUS_UPDATE_OK = 10; // Back end system has indicated that an update request was processed successfully
static const int STATUS_UPDATE_REJECTED = 11; // Back end system has indicated that an update request was rejected for some reason

static const int STATUS_COMPLETED = 126; // Generic status meaning operation was processed to completion successfully (by the back end system, typically)
static const int STATUS_FAILED = 127;    // Generic status meaning processing of the request by the back end system failed for some reason, probably a technical error

static const int TASK_STATUS_AWAITING_MANAGER  = 0;   // Approval task awaiting manager
static const int TASK_STATUS_PENDING  = 1;   // Decision made and submitted... currently sitting in the outbound queue
static const int TASK_STATUS_SUBMITTED = 2;
static const int TASK_STATUS_FAILED = 3;

static const int TASK_APPROVAL_DECISION_APPROVED = 1;
static const int TASK_APPROVAL_DECISION_REJECTED = 2;

static const char* USER_DATABASE = "annual_leave.db";
static const char* OPS_QUEUE_DATABASE = "operations_queue.db";

// Operations API object types
static const int OP_TYPE_SUBMIT_BOOKING_REQUEST = 1;
static const int OP_TYPE_SUBMIT_BOOKING_RESPONSE = 254; // response op_type is 255 minus request op_type

static const int OP_TYPE_HALF_DAYS_USED_REQUEST = 2;
static const int OP_TYPE_HALF_DAYS_USED_RESPONSE = 253;

static const int OP_TYPE_APPROVAL_OUTCOME_REQUEST = 3;
static const int OP_TYPE_APPROVAL_OUTCOME_RESPONSE = 252;

static const int OP_TYPE_CANCELLATION_REQUEST = 4;
static const int OP_TYPE_CANCELLATION_RESPONSE = 251;

static const int OP_TYPE_CANCELLATION_OUTCOME_REQUEST = 5;
static const int OP_TYPE_CANCELLATION_OUTCOME_RESPONSE = 250;

static const int OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST = 6;
static const int OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE = 249;

static const int OP_TYPE_UPDATE_BOOKING_REQUEST = 7;
static const int OP_TYPE_UPDATE_BOOKING_RESPONSE = 248;

static const int OP_TYPE_UPDATE_OUTCOME_REQUEST = 8;
static const int OP_TYPE_UPDATE_OUTCOME_RESPONSE = 247;

static const int OP_TYPE_APPROVAL_TASK_REQUEST = 9;
static const int OP_TYPE_APPROVAL_TASK_RESPONSE = 246;

static const int OP_TYPE_APPROVAL_TASK_OUTCOME_REQUEST = 10;
static const int OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE = 245;

static const int OP_TYPE_ADAPTER_DETAILS_REQUEST = 11;
static const int OP_TYPE_ADAPTER_DETAILS_RESPONSE = 244;

static const int OP_TYPE_ADAPTER_STATUS_REQUEST = 12;
static const int OP_TYPE_ADAPTER_STATUS_RESPONSE = 243;

static const int OP_TYPE_CLIENT_SYNCHRONIZE_REQUEST = 13;
static const int OP_TYPE_CLIENT_SYNCHRONIZE_RESPONSE = 242;

static const int OP_TYPE_SERVER_SYNC_RESULT_REQUEST = 14;
static const int OP_TYPE_SERVER_SYNC_RESULT_RESPONSE = 241;


// invocation framework targets and actions

#define ANNUAL_LEAVE_ACTION_SYSTEM_STARTED "bb.action.system.STARTED"

static const char* TARGET_AL_SERVICE = "com.example.AnnualLeaveService";
static const char* TARGET_AL_GUI = "com.example.AnnualLeave";

static const char* TARGET_AL_ACTION_OUTBOUND_QUEUE = "com.example.annual_leave.OUTBOUND_QUEUE_DATA";
static const char* TARGET_AL_ACTION_INBOUND_QUEUE = "com.example.annual_leave.INBOUND_QUEUE_DATA";
static const char* TARGET_AL_ACTION_START = "bb.action.START";
static const char* TARGET_AL_ACTION_STOP_SERVICE = "com.example.annual_leave.STOP_SERVICE";
static const char* TARGET_AL_ACTION_RESET_ADAPTER = "com.example.annual_leave.RESET_ADAPTER";

// Main UI tabs

static const int TAB_REQUESTS = 0;
static const int TAB_NEW = 1;
static const int TAB_INBOX = 2;
static const int TAB_DEV = 3;

// Settings

static const char* SETTINGS_KEY_SIMULATION_ENABLED = "SIMULATION_ENABLED";
static const char* SETTINGS_KEY_SERVICE_VERBOSE = "SERVICE_VERBOSE";

static const char* SETTINGS_KEY_ADAPTER_CONFIGURED = "ADAPTER-CONFIGURED";

static const char* GUI_SETTING_1_NAME  = "GUI_SETTING_1_NAME";
static const char* GUI_SETTING_1_VALUE = "GUI_SETTING_1_VALUE";

static const char* GUI_SETTING_2_NAME  = "GUI_SETTING_2_NAME";
static const char* GUI_SETTING_2_VALUE = "GUI_SETTING_2_VALUE";

static const char* GUI_SETTING_3_NAME  = "GUI_SETTING_3_NAME";
static const char* GUI_SETTING_3_VALUE = "GUI_SETTING_3_VALUE";

static const char* GUI_SETTING_4_NAME  = "GUI_SETTING_4_NAME";
static const char* GUI_SETTING_4_VALUE = "GUI_SETTING_4_VALUE";

static const char* GUI_SETTING_5_NAME  = "GUI_SETTING_5_NAME";
static const char* GUI_SETTING_5_VALUE = "GUI_SETTING_5_VALUE";

static const char* GUI_SETTING_6_NAME  = "GUI_SETTING_6_NAME";
static const char* GUI_SETTING_6_VALUE = "GUI_SETTING_6_VALUE";

// approval task types

static const int TASK_TYPE_BOOKING_APPROVAL = 0;
static const int TASK_TYPE_CANCEL_APPROVAL = 1;
static const int TASK_TYPE_UPDATE_APPROVAL = 2;

// Various

static const int DAY_HALF_MORNING = 1;
static const int DAY_HALF_AFTERNOON = 2;

static const char* INDICATOR_DIR = "/indications";

static const char* ANNUAL_LEAVE_HEADLESS_SOCKET_SERVER_NAME = "AnnualLeaveServiceSocket";

#endif /* COMMON_HPP_ */
