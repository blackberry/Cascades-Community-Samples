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

#include <OpApprovalTaskRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpApprovalTaskRequ::OpApprovalTaskRequ() :
        _user_id("")
,       _task_type(0)
,       _leave_year(-1)
,       _from_date(-1)
,       _to_date(-1)
,       _first_day_half(-1)
,       _last_day_half(-1)
,       _day_half(-1)
,       _half_days(-1)
,       _note("")
,       _local_request_id(0)
,       _new_leave_year(-1)
,       _new_from_date(0)
,       _new_to_date(0)
,       _new_first_day_half(0)
,       _new_last_day_half(0)
,       _new_day_half(0)
,       _originating_op_id(-1)
{
    _op_type = OP_TYPE_APPROVAL_TASK_REQUEST;
}

OpApprovalTaskRequ::~OpApprovalTaskRequ()
{
}

QByteArray OpApprovalTaskRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << userId();
    stream << taskType();
    stream << leaveYear();
    stream << fromDate();
    stream << toDate();
    stream << firstDayHalf();
    stream << lastDayHalf();
    stream << dayHalf();
    stream << halfDays();
    stream << note();
    stream << localRequestId();
    stream << newLeaveYear();
    stream << newFromDate();
    stream << newToDate();
    stream << newFirstDayHalf();
    stream << newLastDayHalf();
    stream << newDayHalf();
    stream << originatingOpId();

    return serialized_data;
}

void OpApprovalTaskRequ::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _user_id >> _task_type >> _leave_year >> _from_date >> _to_date >> _first_day_half >> _last_day_half >> _day_half >> _half_days >> _note >> _local_request_id
    >> _new_leave_year >> _new_from_date >> _new_to_date  >> _new_first_day_half >> _new_last_day_half >> _new_day_half >> _originating_op_id;
}

QString OpApprovalTaskRequ::userId() {
    return _user_id;
}

void OpApprovalTaskRequ::setUserId(QString user_id) {
    _user_id = user_id;
}
int OpApprovalTaskRequ::taskType() {
    return _task_type;
}

void OpApprovalTaskRequ::setTaskType(int tt) {
    _task_type = tt;
}

int OpApprovalTaskRequ::leaveYear()
{
    return _leave_year;
}

void OpApprovalTaskRequ::setLeaveYear(int year)
{
    _leave_year = year;
}

qint64 OpApprovalTaskRequ::fromDate()
{
    return _from_date;
}

void OpApprovalTaskRequ::setFromDate(qint64 fdate)
{
    _from_date = fdate;
}

qint64 OpApprovalTaskRequ::toDate()
{
    return _to_date;
}

void OpApprovalTaskRequ::setToDate(qint64 tdate)
{
    _to_date = tdate;
}

bool OpApprovalTaskRequ::firstDayHalf() {
    return _first_day_half;
}

void OpApprovalTaskRequ::setFirstDayHalf(bool is_half) {
    _first_day_half = is_half;
}

bool OpApprovalTaskRequ::lastDayHalf() {
    return _last_day_half;
}

void OpApprovalTaskRequ::setLastDayHalf(bool is_half) {
    _last_day_half = is_half;
}

int OpApprovalTaskRequ::dayHalf() {
    return _day_half;
}

void OpApprovalTaskRequ::setDayHalf(int half_of_day) {
    _day_half = half_of_day;
}

int OpApprovalTaskRequ::halfDays()
{
    return _half_days;
}

void OpApprovalTaskRequ::setHalfDays(int half_days)
{
    _half_days = half_days;
}


QString OpApprovalTaskRequ::note()
{
    return _note;
}

void OpApprovalTaskRequ::setNote(QString note)
{
    _note = note;
}

int OpApprovalTaskRequ::localRequestId() {
    return _local_request_id;
}

void OpApprovalTaskRequ::setLocalRequestId(int local_request_id) {
    _local_request_id = local_request_id;
}


int OpApprovalTaskRequ::newLeaveYear()
{
    return _new_leave_year;
}

void OpApprovalTaskRequ::setNewLeaveYear(int year)
{
    _new_leave_year = year;
}

qint64 OpApprovalTaskRequ::newFromDate()
{
    return _new_from_date;
}

void OpApprovalTaskRequ::setNewFromDate(qint64 fdate)
{
    _new_from_date = fdate;
}

qint64 OpApprovalTaskRequ::newToDate()
{
    return _new_to_date;
}

void OpApprovalTaskRequ::setNewToDate(qint64 tdate)
{
    _new_to_date = tdate;
}

bool OpApprovalTaskRequ::newFirstDayHalf() {
    return _new_first_day_half;
}

void OpApprovalTaskRequ::setNewFirstDayHalf(bool is_half) {
    _new_first_day_half = is_half;
}

bool OpApprovalTaskRequ::newLastDayHalf() {
    return _new_last_day_half;
}

void OpApprovalTaskRequ::setNewLastDayHalf(bool is_half) {
    _new_last_day_half = is_half;
}

int OpApprovalTaskRequ::newDayHalf() {
    return _new_day_half;
}

void OpApprovalTaskRequ::setNewDayHalf(int half_of_day) {
    _new_day_half = half_of_day;
}

int OpApprovalTaskRequ::originatingOpId() {
    return _originating_op_id;
}

void OpApprovalTaskRequ::setOriginatingOpId(int op_id) {
    _originating_op_id = op_id;
}
