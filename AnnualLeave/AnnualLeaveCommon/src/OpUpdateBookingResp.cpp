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

#include <OpUpdateBookingResp.hpp>
#include "common.hpp"
#include <QDataStream>

OpUpdateBookingResp::OpUpdateBookingResp() :
        _request_status(-1), _op_status(0)
{
    _op_type = OP_TYPE_UPDATE_BOOKING_RESPONSE;
}

OpUpdateBookingResp::~OpUpdateBookingResp()
{
}

QByteArray OpUpdateBookingResp::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << localRequestId();
    stream << opStatus();
    stream << requestStatus();
    return serialized_data;
}

void OpUpdateBookingResp::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _local_request_id >> _op_status >> _request_status;
}

int OpUpdateBookingResp::opStatus()
{
    return _op_status;
}

void OpUpdateBookingResp::setOpStatus(int op_status)
{
    _op_status = op_status;
}

int OpUpdateBookingResp::localRequestId() {
    return _local_request_id;
}

void OpUpdateBookingResp::setLocalRequestId(int local_request_id) {
    _local_request_id = local_request_id;
}

int OpUpdateBookingResp::requestStatus()
{
    return _request_status;
}

void OpUpdateBookingResp::setRequestStatus(int request_status)
{
    _request_status = request_status;
}
