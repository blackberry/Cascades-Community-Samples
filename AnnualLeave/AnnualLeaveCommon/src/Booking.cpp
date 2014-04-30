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

#include <Booking.hpp>
#include "common.hpp"
#include <QDataStream>

Booking::Booking()
{
}

Booking::~Booking()
{
}

QByteArray Booking::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << leaveYear();
    stream << fromDate();
    stream << toDate();
    stream << firstDayHalf();
    stream << lastDayHalf();
    stream << dayHalf();
    stream << halfDays();
    stream << note();
    return serialized_data;
}

void Booking::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _leave_year >> _from_date >> _to_date >> _first_day_half >> _last_day_half >> _day_half >> _half_days >> _note;
}

void Booking::unmarshall(QDataStream *stream) {
    *stream >> _leave_year >> _from_date >> _to_date >> _first_day_half >> _last_day_half >> _day_half >> _half_days >> _note;
}

int Booking::leaveYear()
{
    return _leave_year;
}

void Booking::setLeaveYear(int year)
{
    _leave_year = year;
}

qint64 Booking::fromDate()
{
    return _from_date;
}

void Booking::setFromDate(qint64 fdate)
{
    _from_date = fdate;
}

qint64 Booking::toDate()
{
    return _to_date;
}

void Booking::setToDate(qint64 tdate)
{
    _to_date = tdate;
}

bool Booking::firstDayHalf() {
    return _first_day_half;
}

void Booking::setFirstDayHalf(bool is_half) {
    _first_day_half = is_half;
}

bool Booking::lastDayHalf() {
    return _last_day_half;
}

void Booking::setLastDayHalf(bool is_half) {
    _last_day_half = is_half;
}

int Booking::dayHalf() {
    return _day_half;
}

void Booking::setDayHalf(int half_of_day) {
    _day_half = half_of_day;
}

int Booking::halfDays() {
    return _half_days;
}

void Booking::setHalfDays(int half_days) {
    _half_days = half_days;
}

QString Booking::note()
{
    return _note;
}

void Booking::setNote(QString note)
{
    _note = note;
}
