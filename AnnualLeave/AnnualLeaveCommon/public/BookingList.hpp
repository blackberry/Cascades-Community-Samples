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

#ifndef BOOKING_LIST_HPP_
#define BOOKING_LIST_HPP_

#include <QList>
#include "Booking.hpp"

class Q_DECL_EXPORT BookingList {

public:
	BookingList();
	~BookingList();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

	QList <Booking*> list();

	QList <Booking*> _bookings;

	void addBooking(Booking *booking);
};

#endif /* BOOKING_LIST_HPP_ */
