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

#include "Entitlement.hpp"

Entitlement::Entitlement() :
        _leave_year(-1), _total_entitlement_half_days(-1), _current_entitlement_half_days(-1)
{

}

Entitlement::~Entitlement()
{
}

int Entitlement::leaveYear()
{
    return _leave_year;
}

void Entitlement::setLeaveYear(int year)
{
    _leave_year = year;
}

int Entitlement::totalEntitlementHalfDays()
{
    return _total_entitlement_half_days;
}

void Entitlement::setTotalEntitlementHalfDays(int half_days)
{
    _total_entitlement_half_days = half_days;
}

int Entitlement::currentEntitlementHalfDays()
{
    return _current_entitlement_half_days;
}

void Entitlement::setCurrentEntitlementHalfDays(int half_days)
{
    _current_entitlement_half_days = half_days;
}
