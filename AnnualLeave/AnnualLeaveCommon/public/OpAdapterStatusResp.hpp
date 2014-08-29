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

#ifndef ADAPTER_STATUS_RESP_
#define ADAPTER_STATUS_RESP_
#include "OperationAl.hpp"

class Q_DECL_EXPORT OpAdapterStatusResp : public OperationAl {

public:
    OpAdapterStatusResp();
	~OpAdapterStatusResp();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

    int opStatus();
    void setOpStatus(int op_status);

    int _op_status;

};

#endif /* ADAPTER_STATUS_RESP_ */
