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

#include <OpApprovalTaskOutcomeRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpApprovalTaskOutcomeRequ::OpApprovalTaskOutcomeRequ() :
        _request_status(-1)
,       _originating_op_id(-1)

{
    _op_type = OP_TYPE_APPROVAL_TASK_OUTCOME_REQUEST;;
}

OpApprovalTaskOutcomeRequ::~OpApprovalTaskOutcomeRequ()
{
}

QByteArray OpApprovalTaskOutcomeRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << taskId();
    stream << taskType();
    stream << requestStatus();
    stream << _local_request_id;
    stream << originatingOpId();
    return serialized_data;
}

void OpApprovalTaskOutcomeRequ::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _task_id >> _task_type >> _request_status >> _local_request_id >> _originating_op_id;
}

int OpApprovalTaskOutcomeRequ::taskId() {
    return _task_id;
}

void OpApprovalTaskOutcomeRequ::setTaskId(int task_id) {
    _task_id = task_id;
}

int OpApprovalTaskOutcomeRequ::taskType() {
    return _task_type;
}

void OpApprovalTaskOutcomeRequ::setTaskType(int tt) {
    _task_type = tt;
}


int OpApprovalTaskOutcomeRequ::requestStatus()
{
    return _request_status;
}

void OpApprovalTaskOutcomeRequ::setRequestStatus(int status)
{
    _request_status = status;
}

int OpApprovalTaskOutcomeRequ::localRequestId() {
    return _local_request_id;
}

void OpApprovalTaskOutcomeRequ::setLocalRequestId(int local_request_id) {
    _local_request_id = local_request_id;
}


int OpApprovalTaskOutcomeRequ::originatingOpId() {
    return _originating_op_id;
}

void OpApprovalTaskOutcomeRequ::setOriginatingOpId(int op_id) {
    _originating_op_id = op_id;
}
