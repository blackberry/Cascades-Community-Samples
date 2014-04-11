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

#ifndef OPERATION_AL_HPP
#define OPERATION_AL_HPP

#include <QByteArray>

class Q_DECL_EXPORT OperationAl {

public:
	OperationAl();
	virtual ~OperationAl() = 0;

	int opId();
	void setOpId(int id);

	int opType();
	void setOpType(int op_type);

	// note we serialize/deserialize all attributes *except* for the opId and opType which are both persisted separately in standard form
	virtual QByteArray marshall() = 0;
	virtual void unmarshall(QByteArray serialized_data) = 0;

	// public so that we can easily use the >> operator when unmarshalling
	int _op_id;
	int _op_type;

};

#endif /* OPERATION_AL_HPP */
