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

#include <OpUpdateBookingRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpUpdateBookingRequ::OpUpdateBookingRequ() :
        _old_leave_year(-1)
,       _new_leave_year(-1)
,       _old_from_date(-1)
,       _new_from_date(-1)
,       _old_to_date(-1)
,       _new_to_date(-1)
,       _old_first_day_half(-1)
,       _new_first_day_half(-1)
,       _old_last_day_half(-1)
,       _new_last_day_half(-1)
,       _old_day_half(-1)
,       _new_day_half(-1)
{
    _op_type = OP_TYPE_UPDATE_BOOKING_REQUEST;
}

OpUpdateBookingRequ::~OpUpdateBookingRequ()
{
}

QByteArray OpUpdateBookingRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << localRequestId();
    stream << oldLeaveYear();
    stream << newLeaveYear();
    stream << oldFromDate();
    stream << newFromDate();
    stream << oldToDate();
    stream << newToDate();
    stream << oldFirstDayHalf();
    stream << newFirstDayHalf();
    stream << oldLastDayHalf();
    stream << newLastDayHalf();
    stream << oldDayHalf();
    stream << newDayHalf();
    stream << note();
    return serialized_data;
}

void OpUpdateBookingRequ::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _local_request_id >> _old_leave_year  >> _new_leave_year >> _old_from_date >> _new_from_date >> _old_to_date >> _new_to_date
    >> _old_first_day_half >> _new_first_day_half >> _old_last_day_half >> _new_last_day_half >> _old_day_half >> _new_day_half >> _note;
}

int OpUpdateBookingRequ::localRequestId() {
    return _local_request_id;
}

void OpUpdateBookingRequ::setLocalRequestId(int local_request_id) {
    _local_request_id = local_request_id;
}

int OpUpdateBookingRequ::oldLeaveYear()
{
    return _old_leave_year;
}

void OpUpdateBookingRequ::setOldLeaveYear(int year)
{
    _old_leave_year = year;
}

int OpUpdateBookingRequ::newLeaveYear()
{
    return _new_leave_year;
}

void OpUpdateBookingRequ::setNewLeaveYear(int year)
{
    _new_leave_year = year;
}

qint64 OpUpdateBookingRequ::oldFromDate()
{
    return _old_from_date;
}

void OpUpdateBookingRequ::setOldFromDate(qint64 fdate)
{
    _old_from_date = fdate;
}

qint64 OpUpdateBookingRequ::newFromDate()
{
    return _new_from_date;
}

void OpUpdateBookingRequ::setNewFromDate(qint64 fdate)
{
    _new_from_date = fdate;
}

qint64 OpUpdateBookingRequ::oldToDate()
{
    return _old_to_date;
}

void OpUpdateBookingRequ::setOldToDate(qint64 tdate)
{
    _old_to_date = tdate;
}

qint64 OpUpdateBookingRequ::newToDate()
{
    return _new_to_date;
}

void OpUpdateBookingRequ::setNewToDate(qint64 tdate)
{
    _new_to_date = tdate;
}


bool OpUpdateBookingRequ::oldFirstDayHalf() {
    return _old_first_day_half;
}

void OpUpdateBookingRequ::setOldFirstDayHalf(bool is_half) {
    _old_first_day_half = is_half;
}

bool OpUpdateBookingRequ::newFirstDayHalf() {
    return _new_first_day_half;
}

void OpUpdateBookingRequ::setNewFirstDayHalf(bool is_half) {
    _new_first_day_half = is_half;
}

bool OpUpdateBookingRequ::oldLastDayHalf() {
    return _old_last_day_half;
}

void OpUpdateBookingRequ::setOldLastDayHalf(bool is_half) {
    _old_last_day_half = is_half;
}

bool OpUpdateBookingRequ::newLastDayHalf() {
    return _new_last_day_half;
}

void OpUpdateBookingRequ::setNewLastDayHalf(bool is_half) {
    _new_last_day_half = is_half;
}

int OpUpdateBookingRequ::oldDayHalf() {
    return _old_day_half;
}

void OpUpdateBookingRequ::setOldDayHalf(int half_of_day) {
    _old_day_half = half_of_day;
}

int OpUpdateBookingRequ::newDayHalf() {
    return _new_day_half;
}

void OpUpdateBookingRequ::setNewDayHalf(int half_of_day) {
    _new_day_half = half_of_day;
}

QString OpUpdateBookingRequ::note()
{
    return _note;
}

void OpUpdateBookingRequ::setNote(QString note)
{
    _note = note;
}
