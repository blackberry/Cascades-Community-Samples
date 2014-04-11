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

#include <OpAdapterStatusRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpAdapterStatusRequ::OpAdapterStatusRequ()
{
    _op_type = OP_TYPE_ADAPTER_STATUS_REQUEST;
}

OpAdapterStatusRequ::~OpAdapterStatusRequ()
{
}

QByteArray OpAdapterStatusRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    return serialized_data;
}

void OpAdapterStatusRequ::unmarshall(QByteArray serialized_data)
{
}
