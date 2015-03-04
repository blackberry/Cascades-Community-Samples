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

#ifndef APPROVAL_TASK_OUTCOME_REQU_HPP_
#define APPROVAL_TASK_OUTCOME_REQU_HPP_
#include "OperationAl.hpp"
#include <QString>

class Q_DECL_EXPORT OpApprovalTaskOutcomeRequ : public OperationAl {

public:
	OpApprovalTaskOutcomeRequ();
	~OpApprovalTaskOutcomeRequ();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

    int taskId();
    void setTaskId(int task_id);

    int taskType();
    void setTaskType(int tt);

	int requestStatus();
	void setRequestStatus(int status);

    int localRequestId();
    void setLocalRequestId(int local_request_id);

    int originatingOpId();
    void setOriginatingOpId(int id);

    int _task_id;
    int _task_type;
    int _request_status;
    int _local_request_id;
    int _originating_op_id;

};

#endif /* APPROVAL_TASK_OUTCOME_REQU_HPP_ */
