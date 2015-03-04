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

#ifndef APPROVAL_TASK_OUTCOME_RESP_HPP_
#define APPROVAL_TASK_OUTCOME_RESP_HPP_
#include "OperationAl.hpp"

class Q_DECL_EXPORT OpApprovalTaskOutcomeResp : public OperationAl {

public:
	OpApprovalTaskOutcomeResp();
	~OpApprovalTaskOutcomeResp();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

	int opStatus();
	void setOpStatus(int op_status);

	int taskId();
    void setTaskId(int task_id);

	// public so that we can easily use the >> operator when unmarshalling
	int _op_status;
	int _task_id; // approval task ID... manager's perspective

};

#endif /* APPROVAL_TASK_OUTCOME_RESP_HPP_ */
