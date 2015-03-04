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

#include <OperationAl.hpp>

OperationAl::OperationAl() :
    _op_id(-1)
,   _op_type(-1)
{

}

OperationAl::~OperationAl() {
}

int OperationAl::opId() {
    return _op_id;
}

void OperationAl::setOpId(int op_id) {
    qDebug() << "OOOO setOpId:" << op_id;
    _op_id = op_id;
}

int OperationAl::opType() {
	return _op_type;
}

void OperationAl::setOpType(int op_type) {
	_op_type = op_type;
}
