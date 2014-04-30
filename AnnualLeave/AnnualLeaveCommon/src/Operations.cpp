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

#include <Operations.hpp>
#include <GuiIndicator.hpp>
#include <OpSubmitBookingRequ.hpp>
#include <OpSubmitBookingResp.hpp>
#include <OpCancelBookingRequ.hpp>
#include <OpCancelBookingResp.hpp>
#include <OpUpdateBookingRequ.hpp>
#include <OpUpdateBookingResp.hpp>
#include <OpHalfDaysUsedRequ.hpp>
#include <OpHalfDaysUsedResp.hpp>
#include <OpApprovalOutcomeRequ.hpp>
#include <OpApprovalOutcomeResp.hpp>
#include <OpApprovalTaskRequ.hpp>
#include <OpApprovalTaskResp.hpp>
#include <OpAdapterStatusRequ.hpp>
#include <OpAdapterStatusResp.hpp>
#include <OpClientSynchronizeRequ.hpp>
#include <OpClientSynchronizeResp.hpp>
#include <OpServerSyncResultRequ.hpp>
#include <OpServerSyncResultResp.hpp>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include "common.hpp"

Operations* Operations::_instance;

Operations::Operations(QObject *parent) :
        QObject(parent), _notify(new bb::platform::Notification(this)), _invokeManager(new bb::system::InvokeManager(this)), _guiConnected(false), _guiAppProcessRunning(false), _socketServerPath(
                QDir::tempPath().append("/").append(ANNUAL_LEAVE_HEADLESS_SOCKET_SERVER_NAME))
{
    qDebug() << "OOOO constructing Operations object";
    _sql = OpsSqlDataSource::getInstance(this);
}

Operations::~Operations()
{
}

Operations *Operations::getInstance(QObject *parent)
{
    qDebug() << "OOOO Operations::getInstance()";
    if (_instance == 0) {
        _instance = new Operations(parent);
    }
    return _instance;
}

int Operations::submit_Booking_Requ(OpSubmitBookingRequ request)
{
    qDebug() << "OOOO Operations API: submit_Booking_requ: local_request_id=" << request.localRequestId();

    int id = _sql->storeOutbound(request.opType(), request.localRequestId(), request.marshall());
    qDebug() << "OOOO Operations API: submit_Al_requ got out_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);

    return id;
}

void Operations::submit_Booking_Resp(OpSubmitBookingResp response)
{
    qDebug() << "OOOO Operations API: submit_Booking_Resp: local_request_id=" << response.localRequestId();

    int id = _sql->storeInbound(response.opType(), response.localRequestId(), response.marshall());
    qDebug() << "OOOO Operations API: submit_Booking_Resp got in_op_id=" << id;

    // create notification in the hub which when selected, launches the GUI
    if (!isAlGuiRunning()) {
        qDebug() << "OOOO creating notification for the Hub";
        notifyGuiInboundOperation();
    } else {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }
}

int Operations::cancel_Booking_Requ(OpCancelBookingRequ request)
{
    qDebug() << "OOOO Operations API: cancel_Booking_requ: local_request_id=" << request.localRequestId();

    int id = _sql->storeOutbound(request.opType(), request.localRequestId(), request.marshall());
    qDebug() << "OOOO Operations API: cancel_Booking_requ got out_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);
    return id;
}

void Operations::cancel_Booking_Resp(OpCancelBookingResp response)
{
    qDebug() << "OOOO Operations API: cancel_Booking_Resp: local_request_id=" << response.localRequestId();

    int id = _sql->storeInbound(response.opType(), response.localRequestId(), response.marshall());
    qDebug() << "OOOO Operations API: cancel_Booking_Resp got in_op_id=" << id;

    // create notification in the hub which when selected, launches the GUI
    if (isAlGuiRunning()) {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }
}

int Operations::update_Booking_Requ(OpUpdateBookingRequ request)
{
    qDebug() << "OOOO Operations API: update_Booking_requ: local_request_id=" << request.localRequestId();

    int id = _sql->storeOutbound(request.opType(), request.localRequestId(), request.marshall());
    qDebug() << "OOOO Operations API: update_Booking_requ got out_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);
    return id;
}

void Operations::update_Booking_Resp(OpUpdateBookingResp response)
{
    qDebug() << "OOOO Operations API: update_Booking_Resp: local_request_id=" << response.localRequestId();

    int id = _sql->storeInbound(response.opType(), response.localRequestId(), response.marshall());
    qDebug() << "OOOO Operations API: update_Booking_Resp got in_op_id=" << id;

    // create notification in the hub which when selected, launches the GUI
    if (isAlGuiRunning()) {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }
}

void Operations::annual_entitlement_Requ(OpAnnualEntitlementRequ request)
{
    qDebug() << "OOOO Operations API: annual_entitlement_Requ leave_year(" << request.leaveYear() << ")";

    if (!_sql->annualEntitelementRequExists(request._leave_year)) {
        int id = _sql->storeOutbound(request.opType(), -1, request.marshall());
        qDebug() << "OOOO Operations API: annual_entitlement_Requ got out_op_id=" << id;
        bb::system::InvokeRequest invoke_request;
        invoke_request.setTarget(TARGET_AL_SERVICE);
        invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
        _invokeManager->invoke(invoke_request);
    } else {
        qDebug() << "OOOO Operations API: annual_entitlement_Requ - outbound queue already contains a request for this leave_year(" << request.leaveYear() << ")";
    }
}

void Operations::annual_entitlement_Resp(OpAnnualEntitlementResp response)
{
    qDebug() << "OOOO Operations API: annual_entitlement_Resp";

    int id = _sql->storeInbound(response.opType(), -1, response.marshall());
    qDebug() << "OOOO Operations API: annual_entitlement_Resp got in_op_id=" << id;

    if (isAlGuiRunning()) {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }
}

void Operations::adapter_details_Requ(OpAdapterDetailsRequ request)
{
    qDebug() << "OOOO Operations API: adapter_details_Requ()";

    int id = _sql->storeInbound(request.opType(), -1, request.marshall());
    qDebug() << "OOOO Operations API: adapter_details_Requ got out_op_id=" << id;

    if (isAlGuiRunning()) {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }

}

void Operations::adapter_details_Resp(OpAdapterDetailsResp response)
{
    qDebug() << "OOOO Operations API: adapter_details_Resp";

    int id = _sql->storeOutbound(response.opType(), -1, response.marshall());
    qDebug() << "OOOO Operations API: adapter_details_Resp got in_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);
}

void Operations::adapter_status_Requ(OpAdapterStatusRequ request)
{
    qDebug() << "OOOO Operations API: adapter_status_Requ()";
    int id = _sql->storeOutbound(request.opType(), -1, request.marshall());
    qDebug() << "OOOO Operations API: adapter_status_Requ got in_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);
}

void Operations::adapter_status_Resp(OpAdapterStatusResp response)
{
    qDebug() << "OOOO Operations API: adapter_status_Resp";

    int id = _sql->storeInbound(response.opType(), -1, response.marshall());
    qDebug() << "OOOO Operations API: adapter_status_Resp got out_op_id=" << id;

    if (isAlGuiRunning()) {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }

}

void Operations::half_days_used_Requ(OpHalfDaysUsedRequ request)
{
    qDebug() << "OOOO Operations API: half_days_used_Requ: local_request_id=" << request.localRequestId();

    int id = _sql->storeInbound(request.opType(), request.localRequestId(), request.marshall());
    qDebug() << "OOOO Operations API: half_days_used_Requ got out_op_id=" << id;

    if (!isAlGuiRunning()) {
        qDebug() << "OOOO creating notification for the Hub";
        notifyGuiInboundOperation();
    } else {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }

}

void Operations::half_days_used_Resp(OpHalfDaysUsedResp response)
{
    qDebug() << "OOOO Operations API: half_days_used_Resp: local_request_id=" << response.localRequestId();

    int id = _sql->storeOutbound(response.opType(), response.localRequestId(), response.marshall());
    qDebug() << "OOOO Operations API: submit_Al_Resp got in_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);

}

// receiving an approval outcome from a manager, indicating what the decision was for a request you previously issued
void Operations::approval_outcome_Requ(OpApprovalOutcomeRequ request)
{
    qDebug() << "OOOO Operations API: approval_outcome_Requ: local_request_id=" << request.localRequestId() << " originating_op_id=" << request.originatingOpId();

    int id = _sql->storeInbound(request.opType(), request.localRequestId(), request.marshall());
    qDebug() << "OOOO Operations API: approval_outcome_Requ got out_op_id=" << id;

    if (!isAlGuiRunning()) {
        qDebug() << "OOOO creating notification for the Hub";
        notifyGuiInboundOperation();
    } else {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }

}

// acknowledging the approval outcome request
void Operations::approval_outcome_Resp(OpApprovalOutcomeResp response)
{
    qDebug() << "OOOO Operations API: approval_outcome_Resp: local_request_id=" << response.localRequestId();

    int id = _sql->storeOutbound(response.opType(), response.localRequestId(), response.marshall());
    qDebug() << "OOOO Operations API: approval_outcome_Resp got in_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);

}

// a manager sending an indication of their decision regarding an approval task
void Operations::approval_task_outcome_Requ(OpApprovalTaskOutcomeRequ request)
{
    qDebug() << "OOOO Operations API: approval_task_outcome_Requ: local_request_id=" << request.localRequestId() << " originating_op_id=" << request.originatingOpId();

    int id = _sql->storeOutbound(request.opType(), request.localRequestId(), request.marshall());
    qDebug() << "OOOO Operations API: approval_task_outcome_Requ got out_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);

}

// ACKnowledgement of an approval_task_outcome_requ
void Operations::approval_task_outcome_Resp(OpApprovalTaskOutcomeResp response)
{
    qDebug() << "OOOO Operations API: approval_task_outcome_Resp: op_id=" << response.opId();
    int id = _sql->storeInbound(response.opType(), response.taskId(), response.marshall());
    qDebug() << "OOOO Operations API: approval_task_outcome_Resp got in_op_id=" << id;

    if (isAlGuiRunning()) {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }

}

void Operations::approval_task_Requ(OpApprovalTaskRequ request)
{
    qDebug() << "OOOO Operations API: approval_task_Requ: user_id=" << request.userId();

    int id = _sql->storeInbound(request.opType(), request.localRequestId(), request.marshall());
    qDebug() << "OOOO Operations API: approval_task_Requ got out_op_id=" << id;

    // always place in the approver's hub view
    qDebug() << "OOOO creating notification for the Hub";
    notifyTask();
}

void Operations::approval_task_Resp(OpApprovalTaskResp response)
{
    qDebug() << "OOOO Operations API: approval_task_Resp: op_status=" << response.opStatus();
    int id = _sql->storeOutbound(response.opType(), -1, response.marshall());
    qDebug() << "OOOO Operations API: approval_task_Resp got in_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);

}

void Operations::client_synchronize_Requ(OpClientSynchronizeRequ request)
{
    qDebug() << "OOOO Operations API: client_synchronize_Requ";
    int id = _sql->storeOutbound(request.opType(), -1, request.marshall());
    qDebug() << "OOOO Operations API: client_synchronize_Requ got out_op_id=" << id;
    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);
}

void Operations::client_synchronize_Resp(OpClientSynchronizeResp response)
{
    qDebug() << "OOOO Operations API: client_synchronize_Resp: op_id=" << response.opId();
    int id = _sql->storeInbound(response.opType(), -1, response.marshall());
    qDebug() << "OOOO Operations API: client_synchronize_Resp got in_op_id=" << id;

    if (isAlGuiRunning()) {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }
}

void Operations::server_sync_result_Requ(OpServerSyncResultRequ request)
{
    qDebug() << "OOOO Operations API: server_sync_result_Requ";
    int id = _sql->storeInbound(request.opType(), -1, request.marshall());
    qDebug() << "OOOO Operations API: server_sync_result_Requ got in_op_id=" << id;
}

void Operations::server_sync_result_Resp(OpServerSyncResultResp response)
{
    qDebug() << "OOOO Operations API: server_sync_result_Resp: op_status=" << response.opStatus();
    int id = _sql->storeOutbound(response.opType(), -1, response.marshall());
    qDebug() << "OOOO Operations API: server_sync_result_Resp got in_op_id=" << id;

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_OUTBOUND_QUEUE);
    _invokeManager->invoke(invoke_request);
}

QList<OperationAl*> Operations::inboundQueueOps()
{
    qDebug() << "OOOO Operations::inboundQueueOps";
    QList<OperationAl*> ops_objects;
    QVariantList sql_rows = _sql->inboundQueueOps();
    qDebug() << "OOOO " << sql_rows;
    QVariantList::iterator j;
    for (j = sql_rows.begin(); j != sql_rows.end(); j++) {
        QMap<QString, QVariant> map = (*j).toMap();
        int op_id = map["in_op_id"].toInt();
        int op_type = map["op_type"].toInt();
        int local_request_id = map["local_request_id"].toInt();
        QByteArray payload = map["payload"].toByteArray();
        qDebug() << "OOOO in_op_id=" << op_id << " op_type=" << op_type << " local_request_id=" << local_request_id << " payload=" << payload.toHex();

        OpSubmitBookingResp* submit_booking_response;
        OpHalfDaysUsedRequ* half_days_used_request;
        OpApprovalOutcomeRequ* approval_outcome_request;
        OpApprovalOutcomeResp* approval_outcome_response;
        OpCancelBookingResp* cancel_booking_response;
        OpAnnualEntitlementResp* entitlement_response;
        OpUpdateBookingResp* update_booking_response;
        OpApprovalTaskRequ* approval_task_request;
        OpAdapterDetailsRequ* adapter_details_request;
        OpAdapterStatusResp* adapter_status_response;
        OpApprovalTaskOutcomeResp* approval_task_outcome_response;
        OpClientSynchronizeResp* client_synchronize_response;
        OpServerSyncResultRequ* server_sync_result_request;

        switch (op_type) {
            case OP_TYPE_SUBMIT_BOOKING_RESPONSE:
                submit_booking_response = new OpSubmitBookingResp();
                submit_booking_response->setOpId(op_id);
                submit_booking_response->setOpType(op_type);
                submit_booking_response->setLocalRequestId(local_request_id);
                submit_booking_response->unmarshall(payload);
                ops_objects.append(submit_booking_response);
                break;
            case OP_TYPE_HALF_DAYS_USED_REQUEST:
                half_days_used_request = new OpHalfDaysUsedRequ();
                half_days_used_request->setOpId(op_id);
                half_days_used_request->setOpType(op_type);
                half_days_used_request->setLocalRequestId(local_request_id);
                half_days_used_request->unmarshall(payload);
                ops_objects.append(half_days_used_request);
                break;
            case OP_TYPE_APPROVAL_OUTCOME_REQUEST:
                approval_outcome_request = new OpApprovalOutcomeRequ();
                approval_outcome_request->setOpId(op_id);
                approval_outcome_request->setOpType(op_type);
                approval_outcome_request->setLocalRequestId(local_request_id);
                approval_outcome_request->unmarshall(payload);
                ops_objects.append(approval_outcome_request);
                break;
            case OP_TYPE_APPROVAL_OUTCOME_RESPONSE:
                approval_outcome_response = new OpApprovalOutcomeResp();
                approval_outcome_response->setOpId(op_id);
                approval_outcome_response->setOpType(op_type);
                approval_outcome_response->setLocalRequestId(local_request_id);
                approval_outcome_response->unmarshall(payload);
                ops_objects.append(approval_outcome_response);
                break;
            case OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE:
                approval_task_outcome_response = new OpApprovalTaskOutcomeResp();
                approval_task_outcome_response->setOpId(op_id);
                approval_task_outcome_response->setOpType(op_type);
                approval_task_outcome_response->unmarshall(payload);
                ops_objects.append(approval_task_outcome_response);
                break;
            case OP_TYPE_APPROVAL_TASK_REQUEST:
                approval_task_request = new OpApprovalTaskRequ();
                approval_task_request->setOpId(op_id);
                approval_task_request->setOpType(op_type);
                approval_task_request->unmarshall(payload);
                ops_objects.append(approval_task_request);
                break;
            case OP_TYPE_CANCELLATION_RESPONSE:
                cancel_booking_response = new OpCancelBookingResp();
                cancel_booking_response->setOpId(op_id);
                cancel_booking_response->setOpType(op_type);
                cancel_booking_response->setLocalRequestId(local_request_id);
                cancel_booking_response->unmarshall(payload);
                ops_objects.append(cancel_booking_response);
                break;
            case OP_TYPE_UPDATE_BOOKING_RESPONSE:
                update_booking_response = new OpUpdateBookingResp();
                update_booking_response->setOpId(op_id);
                update_booking_response->setOpType(op_type);
                update_booking_response->setLocalRequestId(local_request_id);
                update_booking_response->unmarshall(payload);
                ops_objects.append(update_booking_response);
                break;
            case OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE:
                entitlement_response = new OpAnnualEntitlementResp();
                entitlement_response->setOpId(op_id);
                entitlement_response->setOpType(op_type);
                entitlement_response->unmarshall(payload);
                ops_objects.append(entitlement_response);
                break;
            case OP_TYPE_ADAPTER_DETAILS_REQUEST:
                adapter_details_request = new OpAdapterDetailsRequ();
                adapter_details_request->setOpId(op_id);
                adapter_details_request->setOpType(op_type);
                adapter_details_request->unmarshall(payload);
                ops_objects.append(adapter_details_request);
                break;
            case OP_TYPE_ADAPTER_STATUS_RESPONSE:
                adapter_status_response = new OpAdapterStatusResp();
                adapter_status_response->setOpId(op_id);
                adapter_status_response->setOpType(op_type);
                adapter_status_response->unmarshall(payload);
                ops_objects.append(adapter_status_response);
                break;
            case OP_TYPE_CLIENT_SYNCHRONIZE_RESPONSE:
                client_synchronize_response = new OpClientSynchronizeResp();
                client_synchronize_response->setOpId(op_id);
                client_synchronize_response->setOpType(op_type);
                client_synchronize_response->unmarshall(payload);
                ops_objects.append(client_synchronize_response);
                break;
            case OP_TYPE_SERVER_SYNC_RESULT_REQUEST:
                server_sync_result_request = new OpServerSyncResultRequ();
                server_sync_result_request->setOpId(op_id);
                server_sync_result_request->setOpType(op_type);
                server_sync_result_request->unmarshall(payload);
                ops_objects.append(server_sync_result_request);
                break;

            default:
                qDebug() << "OOOO ERROR - unrecognised op_type in inbound queue record: " << op_type;
                break;
        }
    }
    return ops_objects;
}

QList<OperationAl*> Operations::outboundQueueOps()
{
    qDebug() << "OOOO outboundQueueOps";
    QList<OperationAl*> ops_objects;
    QVariantList sql_rows = _sql->outboundQueueOps();
    qDebug() << "OOOO " << sql_rows;
    QVariantList::iterator j;
    for (j = sql_rows.begin(); j != sql_rows.end(); j++) {
        QMap<QString, QVariant> map = (*j).toMap();
        int op_id = map["out_op_id"].toInt();
        int op_type = map["op_type"].toInt();
        int local_request_id = map["local_request_id"].toInt();
        QByteArray payload = map["payload"].toByteArray();
        qDebug() << "OOOO out_op_id=" << op_id << " op_type=" << op_type << " local_request_id=" << local_request_id << " payload=" << payload.toHex();

        OpSubmitBookingRequ* submit_booking_request;
        OpHalfDaysUsedResp* half_days_used_response;
        OpApprovalOutcomeResp* approval_outcome_response;
        OpApprovalOutcomeRequ* approval_outcome_request;
        OpApprovalTaskOutcomeRequ* approval_task_outcome_request;
        OpApprovalTaskResp* approval_task_response;
        OpCancelBookingRequ* cancel_booking_request;
        OpAnnualEntitlementRequ* entitlement_request;
        OpUpdateBookingRequ* update_booking_request;
        OpAdapterDetailsResp* adapter_details_response;
        OpAdapterStatusRequ* adapter_status_request;
        OpClientSynchronizeRequ* client_synchronize_request;
        OpServerSyncResultResp* server_sync_result_response;

        switch (op_type) {
            case OP_TYPE_SUBMIT_BOOKING_REQUEST:
                submit_booking_request = new OpSubmitBookingRequ();
                submit_booking_request->setOpId(op_id);
                submit_booking_request->setOpType(op_type);
                submit_booking_request->setLocalRequestId(local_request_id);
                submit_booking_request->unmarshall(payload);
                ops_objects.append(submit_booking_request);
                break;
            case OP_TYPE_HALF_DAYS_USED_RESPONSE:
                half_days_used_response = new OpHalfDaysUsedResp();
                half_days_used_response->setOpId(op_id);
                half_days_used_response->setOpType(op_type);
                half_days_used_response->setLocalRequestId(local_request_id);
                half_days_used_response->unmarshall(payload);
                ops_objects.append(half_days_used_response);
                break;
            case OP_TYPE_APPROVAL_OUTCOME_REQUEST:
                approval_outcome_request = new OpApprovalOutcomeRequ();
                approval_outcome_request->setOpId(op_id);
                approval_outcome_request->setOpType(op_type);
                approval_outcome_request->setLocalRequestId(local_request_id);
                approval_outcome_request->unmarshall(payload);
                ops_objects.append(approval_outcome_request);
                break;
            case OP_TYPE_APPROVAL_TASK_OUTCOME_REQUEST:
                approval_task_outcome_request = new OpApprovalTaskOutcomeRequ();
                approval_task_outcome_request->setOpId(op_id);
                approval_task_outcome_request->setOpType(op_type);
                approval_task_outcome_request->setLocalRequestId(local_request_id);
                approval_task_outcome_request->unmarshall(payload);
                ops_objects.append(approval_task_outcome_request);
                break;
            case OP_TYPE_APPROVAL_TASK_RESPONSE:
                approval_task_response = new OpApprovalTaskResp();
                approval_task_response->setOpId(op_id);
                approval_task_response->setOpType(op_type);
                approval_task_response->unmarshall(payload);
                ops_objects.append(approval_task_response);
                break;
            case OP_TYPE_APPROVAL_OUTCOME_RESPONSE:
                approval_outcome_response = new OpApprovalOutcomeResp();
                approval_outcome_response->setOpId(op_id);
                approval_outcome_response->setOpType(op_type);
                approval_outcome_response->setLocalRequestId(local_request_id);
                approval_outcome_response->unmarshall(payload);
                ops_objects.append(approval_outcome_response);
                break;
            case OP_TYPE_CANCELLATION_REQUEST:
                cancel_booking_request = new OpCancelBookingRequ();
                cancel_booking_request->setOpId(op_id);
                cancel_booking_request->setOpType(op_type);
                cancel_booking_request->setLocalRequestId(local_request_id);
                cancel_booking_request->unmarshall(payload);
                ops_objects.append(cancel_booking_request);
                break;
            case OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST:
                entitlement_request = new OpAnnualEntitlementRequ();
                entitlement_request->setOpId(op_id);
                entitlement_request->setOpType(op_type);
                entitlement_request->unmarshall(payload);
                ops_objects.append(entitlement_request);
                break;
            case OP_TYPE_UPDATE_BOOKING_REQUEST:
                update_booking_request = new OpUpdateBookingRequ();
                update_booking_request->setOpId(op_id);
                update_booking_request->setOpType(op_type);
                update_booking_request->setLocalRequestId(local_request_id);
                update_booking_request->unmarshall(payload);
                ops_objects.append(update_booking_request);
                break;
            case OP_TYPE_ADAPTER_DETAILS_RESPONSE:
                adapter_details_response = new OpAdapterDetailsResp();
                adapter_details_response->setOpId(op_id);
                adapter_details_response->setOpType(op_type);
                adapter_details_response->unmarshall(payload);
                ops_objects.append(adapter_details_response);
                break;
            case OP_TYPE_ADAPTER_STATUS_REQUEST:
                adapter_status_request = new OpAdapterStatusRequ();
                adapter_status_request->setOpId(op_id);
                adapter_status_request->setOpType(op_type);
                adapter_status_request->unmarshall(payload);
                ops_objects.append(adapter_status_request);
                break;
            case OP_TYPE_CLIENT_SYNCHRONIZE_REQUEST:
                client_synchronize_request = new OpClientSynchronizeRequ();
                client_synchronize_request->setOpId(op_id);
                client_synchronize_request->setOpType(op_type);
                client_synchronize_request->unmarshall(payload);
                ops_objects.append(client_synchronize_request);
                break;
            case OP_TYPE_SERVER_SYNC_RESULT_RESPONSE:
                server_sync_result_response = new OpServerSyncResultResp();
                server_sync_result_response->setOpId(op_id);
                server_sync_result_response->setOpType(op_type);
                server_sync_result_response->unmarshall(payload);
                ops_objects.append(server_sync_result_response);
                break;
            default:
                qDebug() << "OOOO ERROR - unrecognised op_type in outbound queue record: " << op_type;
                break;
        }
    }

    return ops_objects;
}

void Operations::deleteInboundOp(int id)
{
    qDebug() << "OOOO deleteInboundOp(" << id << ")";
    _sql->deleteInboundOp(id);
}

void Operations::deleteOutboundOp(int id)
{
    qDebug() << "OOOO deleteOutboundOp(" << id << ")";
    _sql->deleteOutboundOp(id);
}

void Operations::deleteOutboundOp(int local_request_id, int op_type)
{
    qDebug() << "OOOO deleteOutboundOp(" << local_request_id << "," << op_type << ")";
    _sql->deleteOutboundOp(local_request_id, op_type);
}

void Operations::notifyGuiInboundOperation()
{
    qDebug() << "OOOO notifyGuiInboundOperation";
    _notify->setTitle("Annual Leave");
    _notify->setBody(QString("An update for your annual leave requests has been received"));
    bb::system::InvokeRequest request;
    request.setTarget(TARGET_AL_GUI);
    request.setAction(TARGET_AL_ACTION_INBOUND_QUEUE);
    _notify->setInvokeRequest(request);

    bb::platform::Notification::clearEffectsForAll();
    bb::platform::Notification::deleteAllFromInbox();
    qDebug() << "OOOO Posting notificaton" << endl;
    _notify->notify();
}

void Operations::notifyTask()
{
    qDebug() << "OOOO notifyTask";
    _notify->setTitle("Approval Task");
    _notify->setBody(QString("An approval task has been added to the Annual Leave application and requires your attention"));
    bb::system::InvokeRequest request;
    request.setTarget(TARGET_AL_GUI);
    request.setAction(TARGET_AL_ACTION_INBOUND_QUEUE);
    _notify->setInvokeRequest(request);

    bb::platform::Notification::clearEffectsForAll();
    bb::platform::Notification::deleteAllFromInbox();
    qDebug() << "OOOO Posting notificaton" << endl;
    _notify->notify();

    if (isAlGuiRunning()) {
        GuiIndicator* indicator = GuiIndicator::getInstance(this);
        indicator->indicateWaitingInboundOperation();
    }

}

bool Operations::isAlGuiRunning()
{
    qDebug() << "OOOO isAlGuiRunning: isGuiConnected(), isGuiAppProcessRunning():" << isGuiConnected() << ", " << isGuiAppProcessRunning() << endl;

    return (isGuiConnected() || isGuiAppProcessRunning());
}

void Operations::logOutboundQueue()
{
    qDebug() << "OOOO logOutboundQueue";
    QList<OperationAl*> ops = outboundQueueOps();
    int num_ops = ops.size();
    qDebug() << "OOOO got " << num_ops << " operations from the outbound queue";

    OpSubmitBookingRequ* op_submit_requ;
    OpHalfDaysUsedResp* op_half_days_used_resp;
    OpApprovalOutcomeResp* op_approval_outcome_resp;
    OpCancelBookingRequ* op_cancellation_requ;
    OpAnnualEntitlementRequ* op_entitlement_requ;
    OpUpdateBookingRequ* op_update_requ;
    OpAdapterDetailsResp* op_adapter_details_resp;

    for (int i = 0; i < num_ops; i++) {
        int op_type = ops.at(i)->opType();
        qDebug() << "OOOO GUI got op_id=" << ops.at(i)->opId() << " op_type=" << op_type;
        switch (op_type) {
            case OP_TYPE_SUBMIT_BOOKING_REQUEST:
                op_submit_requ = dynamic_cast<OpSubmitBookingRequ*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_SUBMIT_BOOKING_REQUEST local_request_id=" << op_submit_requ->localRequestId() << " leave_year=" << op_submit_requ->leaveYear() << " from_date="
                        << op_submit_requ->fromDate() << " to_date=" << op_submit_requ->toDate() << " note=" << op_submit_requ->note() << " first_day_half=" << op_submit_requ->firstDayHalf()
                        << " last_day_half=" << op_submit_requ->lastDayHalf() << " day_half=" << op_submit_requ->dayHalf();
                break;
            case OP_TYPE_HALF_DAYS_USED_RESPONSE:
                op_half_days_used_resp = dynamic_cast<OpHalfDaysUsedResp*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_HALF_DAYS_USED_RESPONSE local_request_id=" << op_half_days_used_resp->localRequestId() << " op_status=" << op_half_days_used_resp->opStatus();
                break;
            case OP_TYPE_APPROVAL_OUTCOME_RESPONSE:
                op_approval_outcome_resp = dynamic_cast<OpApprovalOutcomeResp*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_APPROVAL_OUTCOME_RESPONSE local_request_id=" << op_approval_outcome_resp->localRequestId() << " op_status=" << op_approval_outcome_resp->opStatus();
                break;
            case OP_TYPE_CANCELLATION_REQUEST:
                op_cancellation_requ = dynamic_cast<OpCancelBookingRequ*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_CANCELLATION_REQUEST local_request_id=" << op_cancellation_requ->localRequestId();
                break;
            case OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST:
                op_entitlement_requ = dynamic_cast<OpAnnualEntitlementRequ*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST leave_year=" << op_entitlement_requ->leaveYear();
                break;
            case OP_TYPE_ADAPTER_DETAILS_RESPONSE:
                op_adapter_details_resp = dynamic_cast<OpAdapterDetailsResp*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_ADAPTER_DETAILS_RESPONSE";
                break;
            case OP_TYPE_UPDATE_BOOKING_REQUEST:
                op_update_requ = dynamic_cast<OpUpdateBookingRequ*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_UPDATE_BOOKING_REQUEST local_request_id=" << op_update_requ->localRequestId() << " old_leave_year=" << op_update_requ->oldLeaveYear() << " new_leave_year="
                        << op_update_requ->newLeaveYear() << " old_from_date=" << QDateTime::fromMSecsSinceEpoch(op_update_requ->oldFromDate()) << " new_from_date="
                        << QDateTime::fromMSecsSinceEpoch(op_update_requ->newFromDate()) << " old_to_date=" << QDateTime::fromMSecsSinceEpoch(op_update_requ->oldToDate()) << " new_to_date="
                        << QDateTime::fromMSecsSinceEpoch(op_update_requ->newToDate()) << " new_first_day_half=" << op_update_requ->newFirstDayHalf() << " new_last_day_half="
                        << op_update_requ->newLastDayHalf() << " new_day_half=" << op_update_requ->newDayHalf() << " note=" << op_update_requ->note();
                break;
            default:
                qDebug() << "OOOO logOutboundQueue - invalid op_type:" << op_type;
                break;
        }

    }
}

void Operations::logInboundQueue()
{
    qDebug() << "OOOO logInboundQueue";
    QList<OperationAl*> ops = inboundQueueOps();
    int num_ops = ops.size();
    qDebug() << "OOOO got " << num_ops << " operations from the inbound queue";

    OpSubmitBookingResp* op_submit_resp;
    OpHalfDaysUsedRequ* op_half_days_used_requ;
    OpApprovalOutcomeRequ* op_approval_outcome_requ;
    OpCancelBookingResp* op_cancellation_resp;
    OpAnnualEntitlementResp* op_entitlement_resp;
    OpUpdateBookingResp* op_update_resp;
    OpAdapterDetailsRequ* op_adapter_details_requ;

    for (int i = 0; i < num_ops; i++) {
        int op_type = ops.at(i)->opType();
        qDebug() << "OOOO GUI got op_id=" << ops.at(i)->opId() << " op_type=" << op_type;
        switch (op_type) {
            case OP_TYPE_SUBMIT_BOOKING_RESPONSE:
                op_submit_resp = dynamic_cast<OpSubmitBookingResp*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_SUBMIT_BOOKING_RESPONSE op_id=" << op_submit_resp->opId() << " local_request_id=" << op_submit_resp->localRequestId() << " request_status="
                        << op_submit_resp->requestStatus();
                break;
            case OP_TYPE_HALF_DAYS_USED_REQUEST:
                op_half_days_used_requ = dynamic_cast<OpHalfDaysUsedRequ*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_HALF_DAYS_USED_REQUEST op_id=" << op_half_days_used_requ->opId() << " local_request_id=" << op_half_days_used_requ->localRequestId() << " half_days="
                        << op_half_days_used_requ->halfDays();
                break;
            case OP_TYPE_APPROVAL_OUTCOME_REQUEST:
                op_approval_outcome_requ = dynamic_cast<OpApprovalOutcomeRequ*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_APPROVAL_OUTCOME_REQUEST op_id=" << op_approval_outcome_requ->opId() << " local_request_id=" << op_approval_outcome_requ->localRequestId()
                        << " request_status=" << op_approval_outcome_requ->requestStatus();
                break;
            case OP_TYPE_CANCELLATION_RESPONSE:
                op_cancellation_resp = dynamic_cast<OpCancelBookingResp*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_CANCELLATION_RESPONSE op_id=" << op_cancellation_resp->opId() << " local_request_id=" << op_cancellation_resp->localRequestId() << " request_status="
                        << op_cancellation_resp->requestStatus();
                break;
            case OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE:
                op_entitlement_resp = dynamic_cast<OpAnnualEntitlementResp*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE op_id=" << op_entitlement_resp->opId() << " request_status=" << op_entitlement_resp->requestStatus() << " leave_year="
                        << op_entitlement_resp->leaveYear() << " total_entitlement=" << op_entitlement_resp->totalEntitlementHalfDays() << " current_entitlement="
                        << op_entitlement_resp->currentEntitlementHalfDays();
                break;
            case OP_TYPE_UPDATE_BOOKING_RESPONSE:
                op_update_resp = dynamic_cast<OpUpdateBookingResp*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_UPDATE_BOOKING_RESPONSE op_id=" << op_update_resp->opId() << " local_request_id=" << op_update_resp->localRequestId() << " request_status="
                        << op_update_resp->requestStatus();
                break;
            case OP_TYPE_ADAPTER_DETAILS_REQUEST:
                op_adapter_details_requ = dynamic_cast<OpAdapterDetailsRequ*>(ops.at(i));
                qDebug() << "OOOO OP_TYPE_ADAPTER_DETAILS_REQUEST op_id=" << op_adapter_details_requ->opId() << " adapter_name=" << op_adapter_details_requ->adapterName() << " adapter_version="
                        << op_adapter_details_requ->adapterVersion() << " adapter_description=" << op_adapter_details_requ->adapterDescription() << " configured="
                        << op_adapter_details_requ->isConfigured();
                break;
            default:
                qDebug() << "OOOO logOutboundQueue - invalid op_type:" << op_type;
                break;
        }

    }
}

bool Operations::isGuiConnected() const
{
    return _guiConnected;
}

void Operations::setGuiConnected(bool guiConnected)
{
    _guiConnected = guiConnected;
}

bool Operations::isGuiAppProcessRunning() const
{
    return _guiAppProcessRunning;
}

void Operations::setGuiAppProcessRunning(bool guiAppProcessRunning)
{
    _guiAppProcessRunning = guiAppProcessRunning;
}

const QString& Operations::getSocketServerPath() const
{
    return _socketServerPath;
}
