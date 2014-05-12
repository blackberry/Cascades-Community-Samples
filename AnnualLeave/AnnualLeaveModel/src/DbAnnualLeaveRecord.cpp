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

#include "DbAnnualLeaveRecord.hpp"

DbAnnualLeaveRecord::DbAnnualLeaveRecord() :
  _al_status(-1)
, _from_date_s(-1)
, _to_date_s(-1)
, _half_days(-1)
, _first_day_half(-1)
, _last_day_half(-1)
, _day_half(-1)
, _leave_year(-1)
, _local_request_id(-1)
, _updating_from_date_s(-1)
, _updating_to_date_s(-1)
, _updating_leave_year(-1)
, _originating_op_id(-1)
{

}

DbAnnualLeaveRecord::~DbAnnualLeaveRecord()
{
}

int DbAnnualLeaveRecord::alStatus() const
{
    return _al_status;
}

void DbAnnualLeaveRecord::setAlStatus(int alStatus)
{
    _al_status = alStatus;
}

qint64 DbAnnualLeaveRecord::fromDateS() const
{
    return _from_date_s;
}

void DbAnnualLeaveRecord::setFromDateS(qint64 fromDateS)
{
    _from_date_s = fromDateS;
}

int DbAnnualLeaveRecord::halfDays() const
{
    return _half_days;
}

void DbAnnualLeaveRecord::setHalfDays(int halfDays)
{
    _half_days = halfDays;
}

int DbAnnualLeaveRecord::firstDayHalf() const
{
    return _first_day_half;
}

void DbAnnualLeaveRecord::setFirstDayHalf(int fdh)
{
    _first_day_half = fdh;
}

int DbAnnualLeaveRecord::lastDayHalf() const
{
    return _last_day_half;
}

void DbAnnualLeaveRecord::setLastDayHalf(int ldh)
{
    _last_day_half = ldh;
}

int DbAnnualLeaveRecord::dayHalf() const
{
    return _day_half;
}

void DbAnnualLeaveRecord::setDayHalf(int dh)
{
    _day_half = dh;
}

int DbAnnualLeaveRecord::leaveYear() const
{
    return _leave_year;
}

void DbAnnualLeaveRecord::setLeaveYear(int leaveYear)
{
    _leave_year = leaveYear;
}

int DbAnnualLeaveRecord::localRequestId() const
{
    return _local_request_id;
}

void DbAnnualLeaveRecord::setLocalRequestId(int localRequestId)
{
    _local_request_id = localRequestId;
}

const QString& DbAnnualLeaveRecord::note() const
{
    return _note;
}

void DbAnnualLeaveRecord::setNote(const QString& note)
{
    _note = note;
}

qint64 DbAnnualLeaveRecord::toDateS() const
{
    return _to_date_s;
}

void DbAnnualLeaveRecord::setToDateS(qint64 toDateS)
{
    _to_date_s = toDateS;
}

qint64 DbAnnualLeaveRecord::updatingFromDateS() const
{
    return _updating_from_date_s;
}

void DbAnnualLeaveRecord::setUpdatingFromDateS(qint64 updatingFromDateS)
{
    _updating_from_date_s = updatingFromDateS;
}

int DbAnnualLeaveRecord::updatingLeaveYear() const
{
    return _updating_leave_year;
}

void DbAnnualLeaveRecord::setUpdatingLeaveYear(int updatingLeaveYear)
{
    _updating_leave_year = updatingLeaveYear;
}

qint64 DbAnnualLeaveRecord::updatingToDateS() const
{
    return _updating_to_date_s;
}

void DbAnnualLeaveRecord::setUpdatingToDateS(qint64 updatingToDateS)
{
    _updating_to_date_s = updatingToDateS;
}

int DbAnnualLeaveRecord::updatingFirstDayHalf() const
{
    return _updating_first_day_half;
}

void DbAnnualLeaveRecord::setUpdatingFirstDayHalf(int fdh)
{
    _updating_first_day_half = fdh;
}

int DbAnnualLeaveRecord::updatingLastDayHalf() const
{
    return _updating_last_day_half;
}

void DbAnnualLeaveRecord::setUpdatingLastDayHalf(int ldh)
{
    _updating_last_day_half = ldh;
}

int DbAnnualLeaveRecord::updatingDayHalf() const
{
    return _updating_day_half;
}

void DbAnnualLeaveRecord::setUpdatingDayHalf(int dh)
{
    _updating_day_half = dh;
}

int DbAnnualLeaveRecord::originatingOpId() {
    return _originating_op_id;
}

void DbAnnualLeaveRecord::setOriginatingOpId(int op_id) {
    _originating_op_id = op_id;
}
