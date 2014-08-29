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

#ifndef UPDATE_BOOKING_REQU_HPP_
#define UPDATE_BOOKING_REQU_HPP_
#include "OperationAl.hpp"
#include <QString>

class Q_DECL_EXPORT OpUpdateBookingRequ: public OperationAl
{

public:
    OpUpdateBookingRequ();
    ~OpUpdateBookingRequ();

    QByteArray marshall();
    void unmarshall(QByteArray serialized_data);

    int localRequestId();
    void setLocalRequestId(int local_request_id);

    int oldLeaveYear();
    void setOldLeaveYear(int year);

    int newLeaveYear();
    void setNewLeaveYear(int year);

    qint64 oldFromDate();
    void setOldFromDate(qint64 fdate);

    qint64 newFromDate();
    void setNewFromDate(qint64 fdate);

    qint64 oldToDate();
    void setOldToDate(qint64 tdate);

    qint64 newToDate();
    void setNewToDate(qint64 tdate);

    bool oldFirstDayHalf();
    void setOldFirstDayHalf(bool is_half);

    bool newFirstDayHalf();
    void setNewFirstDayHalf(bool is_half);

    bool oldLastDayHalf();
    void setOldLastDayHalf(bool is_half);

    bool newLastDayHalf();
    void setNewLastDayHalf(bool is_half);

    int oldDayHalf();
    void setOldDayHalf(int half_of_day);

    int newDayHalf();
    void setNewDayHalf(int half_of_day);

    QString note();
    void setNote(QString note);

    // public so that we can easily use the >> operator when unmarshalling
    int _local_request_id;
    int _old_leave_year;
    int _new_leave_year;
    qint64 _old_from_date;
    qint64 _old_to_date;
    bool _old_first_day_half;
    bool _old_last_day_half;
    int _old_day_half;
    qint64 _new_from_date;
    qint64 _new_to_date;
    bool _new_first_day_half;
    bool _new_last_day_half;
    int _new_day_half;
    QString _note;

};

#endif /* UPDATE_BOOKING_REQU_HPP_ */
