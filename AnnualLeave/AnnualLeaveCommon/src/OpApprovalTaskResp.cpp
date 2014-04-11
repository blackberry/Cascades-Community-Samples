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

#include <OpApprovalTaskResp.hpp>
#include "common.hpp"
#include <QDataStream>

OpApprovalTaskResp::OpApprovalTaskResp() :
        _op_status(0)
,       _task_id(-1)
{
    _op_type = OP_TYPE_APPROVAL_TASK_RESPONSE;
}

OpApprovalTaskResp::~OpApprovalTaskResp()
{
}

QByteArray OpApprovalTaskResp::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << opStatus();
    stream << taskId();
    stream << userId();
    return serialized_data;
}

void OpApprovalTaskResp::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _op_status >> _task_id >> _user_id;
}

int OpApprovalTaskResp::taskId() {
    return _task_id;
}

void OpApprovalTaskResp::setTaskId(int task_id) {
    _task_id = task_id;
}

int OpApprovalTaskResp::opStatus()
{
    return _op_status;
}

void OpApprovalTaskResp::setOpStatus(int op_status)
{
    _op_status = op_status;
}

QString OpApprovalTaskResp::userId()
{
	return _user_id;
}

void OpApprovalTaskResp::setUserId(QString user_id)
{
	_user_id = user_id;
}
