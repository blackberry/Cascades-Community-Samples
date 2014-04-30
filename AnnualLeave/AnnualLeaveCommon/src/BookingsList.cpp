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

#include <BookingList.hpp>
#include <Booking.hpp>
#include <QDataStream>
#include <QList>

BookingList::BookingList() :
      _bookings(QList<Booking*>())
{
}

BookingList::~BookingList()
{
}

QByteArray BookingList::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);

    int i=0;
    int list_size = _bookings.size();
    stream << list_size;

    for (i=0;i < _bookings.size();i++) {
        stream << _bookings.at(i)->marshall();
    }
    return serialized_data;
}

void BookingList::unmarshall(QByteArray serialized_data)
{
    QDataStream *stream = new QDataStream(&serialized_data, QIODevice::ReadOnly);
    int list_size;
    *stream >> list_size;
    int i=0;
    for (i=0;i<list_size;i++) {
        Booking *booking = new Booking();
        booking->unmarshall(stream);
    }
}

QList <Booking*> BookingList::list()
{
    return _bookings;
}

void BookingList::addBooking(Booking *booking) {
    _bookings.append(booking);
}
