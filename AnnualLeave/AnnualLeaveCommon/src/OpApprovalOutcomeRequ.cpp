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

#include <OpApprovalOutcomeRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpApprovalOutcomeRequ::OpApprovalOutcomeRequ() :
        _task_type(0)
,       _local_request_id(0)
,       _request_status(-1)
,       _originating_op_id(-1)
,       _half_days(-1)

{
    _op_type = OP_TYPE_APPROVAL_OUTCOME_REQUEST;
}

OpApprovalOutcomeRequ::~OpApprovalOutcomeRequ()
{
}

QByteArray OpApprovalOutcomeRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << taskType();
    stream << localRequestId();
    stream << requestStatus();
    stream << originatingOpId();
    stream << halfDays();
    return serialized_data;
}

void OpApprovalOutcomeRequ::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _task_type >> _local_request_id >> _request_status >> _originating_op_id >> _half_days;
}

int OpApprovalOutcomeRequ::taskType() {
    return _task_type;
}

void OpApprovalOutcomeRequ::setTaskType(int tt) {
    _task_type = tt;
}

int OpApprovalOutcomeRequ::localRequestId() {
    return _local_request_id;
}

void OpApprovalOutcomeRequ::setLocalRequestId(int local_request_id) {
    _local_request_id = local_request_id;
}

int OpApprovalOutcomeRequ::requestStatus()
{
    return _request_status;
}

void OpApprovalOutcomeRequ::setRequestStatus(int status)
{
    _request_status = status;
}


int OpApprovalOutcomeRequ::originatingOpId() {
    return _originating_op_id;
}

void OpApprovalOutcomeRequ::setOriginatingOpId(int op_id) {
    _originating_op_id = op_id;
}

int OpApprovalOutcomeRequ::halfDays()
{
    return _half_days;
}

void OpApprovalOutcomeRequ::setHalfDays(int half_days)
{
    _half_days = half_days;
}
