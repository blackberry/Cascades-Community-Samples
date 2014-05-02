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

#ifndef BOOKING_HPP_
#define BOOKING_HPP_

class Q_DECL_EXPORT Booking {

public:
	Booking();
	~Booking();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;
    void unmarshall(QDataStream *stream) ;

	int leaveYear();
	void setLeaveYear(int year);

	qint64 fromDate();
	void setFromDate(qint64 fdate);

	qint64 toDate();
	void setToDate(qint64 tdate);

    bool firstDayHalf();
    void setFirstDayHalf(bool is_half);

    bool lastDayHalf();
    void setLastDayHalf(bool is_half);

    int dayHalf();
    void setDayHalf(int half_of_day);

    int halfDays();
    void setHalfDays(int half_days);

	QString note();
	void setNote(QString note);

	int _leave_year;
	qint64 _from_date;
	qint64 _to_date;
	bool _first_day_half;
	bool _last_day_half;
	int _day_half;
    int _half_days;
	QString _note;

};

#endif /* BOOKING_ */
