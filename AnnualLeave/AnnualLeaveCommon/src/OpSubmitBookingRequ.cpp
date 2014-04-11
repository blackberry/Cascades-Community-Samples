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

#include <OpSubmitBookingRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpSubmitBookingRequ::OpSubmitBookingRequ() :
        _leave_year(-1)

{
    _op_type = OP_TYPE_SUBMIT_BOOKING_REQUEST;
}

OpSubmitBookingRequ::~OpSubmitBookingRequ()
{
}

QByteArray OpSubmitBookingRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << localRequestId();
    stream << leaveYear();
    stream << fromDate();
    stream << toDate();
    stream << firstDayHalf();
    stream << lastDayHalf();
    stream << dayHalf();
    stream << note();
    return serialized_data;
}

void OpSubmitBookingRequ::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _local_request_id >> _leave_year >> _from_date >> _to_date >> _first_day_half >> _last_day_half >> _day_half >> _note;
}

int OpSubmitBookingRequ::localRequestId() {
    return _local_request_id;
}

void OpSubmitBookingRequ::setLocalRequestId(int local_request_id) {
    _local_request_id = local_request_id;
}

int OpSubmitBookingRequ::leaveYear()
{
    return _leave_year;
}

void OpSubmitBookingRequ::setLeaveYear(int year)
{
    _leave_year = year;
}

qint64 OpSubmitBookingRequ::fromDate()
{
    return _from_date;
}

void OpSubmitBookingRequ::setFromDate(qint64 fdate)
{
    _from_date = fdate;
}

qint64 OpSubmitBookingRequ::toDate()
{
    return _to_date;
}

void OpSubmitBookingRequ::setToDate(qint64 tdate)
{
    _to_date = tdate;
}

bool OpSubmitBookingRequ::firstDayHalf() {
    return _first_day_half;
}

void OpSubmitBookingRequ::setFirstDayHalf(bool is_half) {
    _first_day_half = is_half;
}

bool OpSubmitBookingRequ::lastDayHalf() {
    return _last_day_half;
}

void OpSubmitBookingRequ::setLastDayHalf(bool is_half) {
    _last_day_half = is_half;
}

int OpSubmitBookingRequ::dayHalf() {
    return _day_half;
}

void OpSubmitBookingRequ::setDayHalf(int half_of_day) {
    _day_half = half_of_day;
}

QString OpSubmitBookingRequ::note()
{
    return _note;
}

void OpSubmitBookingRequ::setNote(QString note)
{
    _note = note;
}
