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

#include <OpHalfDaysUsedRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpHalfDaysUsedRequ::OpHalfDaysUsedRequ() :
        _half_days(-1)

{
    _op_type = OP_TYPE_HALF_DAYS_USED_REQUEST;
}

OpHalfDaysUsedRequ::~OpHalfDaysUsedRequ()
{
}

QByteArray OpHalfDaysUsedRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << localRequestId();
    stream << halfDays();
    return serialized_data;
}

void OpHalfDaysUsedRequ::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _local_request_id >> _half_days;
}

int OpHalfDaysUsedRequ::localRequestId() {
    return _local_request_id;
}

void OpHalfDaysUsedRequ::setLocalRequestId(int local_request_id) {
    _local_request_id = local_request_id;
}

int OpHalfDaysUsedRequ::halfDays()
{
    return _half_days;
}

void OpHalfDaysUsedRequ::setHalfDays(int half_days)
{
    _half_days = half_days;
}
