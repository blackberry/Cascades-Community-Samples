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

#include <OpAnnualEntitlementResp.hpp>
#include "common.hpp"
#include <QDataStream>

OpAnnualEntitlementResp::OpAnnualEntitlementResp() :
        _request_status(-1), _op_status(0), _leave_year(-1), _total_entitlement_half_days(-1), _current_entitlement_half_days(-1)
{
    _op_type = OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE;
}

OpAnnualEntitlementResp::~OpAnnualEntitlementResp()
{
}

QByteArray OpAnnualEntitlementResp::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << opStatus();
    stream << requestStatus();
    stream << leaveYear();
    stream << totalEntitlementHalfDays();
    stream << currentEntitlementHalfDays();
    return serialized_data;
}

void OpAnnualEntitlementResp::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _op_status >> _request_status >> _leave_year >> _total_entitlement_half_days >> _current_entitlement_half_days;
}

int OpAnnualEntitlementResp::opStatus()
{
    return _op_status;
}

void OpAnnualEntitlementResp::setOpStatus(int op_status)
{
    _op_status = op_status;
}

int OpAnnualEntitlementResp::requestStatus()
{
    return _request_status;
}

void OpAnnualEntitlementResp::setRequestStatus(int request_status)
{
    _request_status = request_status;
}

int OpAnnualEntitlementResp::leaveYear()
{
    return _leave_year;
}

void OpAnnualEntitlementResp::setLeaveYear(int year)
{
    _leave_year = year;
}


int OpAnnualEntitlementResp::totalEntitlementHalfDays()
{
    return _total_entitlement_half_days;
}

void OpAnnualEntitlementResp::setTotalEntitlementHalfDays(int half_days)
{
    _total_entitlement_half_days = half_days;
}

int OpAnnualEntitlementResp::currentEntitlementHalfDays()
{
    return _current_entitlement_half_days;
}

void OpAnnualEntitlementResp::setCurrentEntitlementHalfDays(int half_days)
{
    _current_entitlement_half_days = half_days;
}

