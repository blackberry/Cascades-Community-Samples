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

#include <OpApprovalTaskOutcomeResp.hpp>
#include "common.hpp"
#include <QDataStream>

OpApprovalTaskOutcomeResp::OpApprovalTaskOutcomeResp() :
		_op_status(0)
{
	_op_type = OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE;
}

OpApprovalTaskOutcomeResp::~OpApprovalTaskOutcomeResp() {
}

QByteArray OpApprovalTaskOutcomeResp::marshall() {
	QByteArray serialized_data;
	QDataStream stream(&serialized_data, QIODevice::WriteOnly);
	stream << opStatus();
	stream << taskId();
	return serialized_data;
}

void OpApprovalTaskOutcomeResp::unmarshall(QByteArray serialized_data) {
	QDataStream stream(&serialized_data, QIODevice::ReadOnly);
	stream >> _op_status >> _task_id;
}

int OpApprovalTaskOutcomeResp::opStatus() {
	return _op_status;
}

void OpApprovalTaskOutcomeResp::setOpStatus(int op_status) {
	_op_status = op_status;
}

int OpApprovalTaskOutcomeResp::taskId() {
    return _task_id;
}

void OpApprovalTaskOutcomeResp::setTaskId(int task_id) {
    _task_id = task_id;
}
