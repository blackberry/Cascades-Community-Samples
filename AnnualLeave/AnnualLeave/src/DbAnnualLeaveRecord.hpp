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

#ifndef DB_ANNUAL_LEAVE_HPP_
#define DB_ANNUAL_LEAVE_HPP_

#include <DbAnnualLeaveRecord.hpp>

class DbAnnualLeaveRecord
{

public:
    DbAnnualLeaveRecord();
    ~DbAnnualLeaveRecord();

    int alStatus() const;
    void setAlStatus(int alStatus);
    qint64 fromDateS() const;
    void setFromDateS(qint64 fromDateS);
    int halfDays() const;
    void setHalfDays(int halfDays);
    int firstDayHalf() const;
    void setFirstDayHalf(int fdh);
    int lastDayHalf() const;
    void setLastDayHalf(int ldh);
    int dayHalf() const;
    void setDayHalf(int dh);
    int leaveYear() const;
    void setLeaveYear(int leaveYear);
    int localRequestId() const;
    void setLocalRequestId(int localRequestId);
    const QString& note() const;
    void setNote(const QString& note);
    qint64 toDateS() const;
    void setToDateS(qint64 toDateS);
    qint64 updatingFromDateS() const;
    void setUpdatingFromDateS(qint64 updatingFromDateS);
    int updatingLeaveYear() const;
    void setUpdatingLeaveYear(int updatingLeaveYear);
    qint64 updatingToDateS() const;
    void setUpdatingToDateS(qint64 updatingToDateS);
    int updatingFirstDayHalf() const;
    void setUpdatingFirstDayHalf(int fdh);
    int updatingLastDayHalf() const;
    void setUpdatingLastDayHalf(int ldh);
    int updatingDayHalf() const;
    void setUpdatingDayHalf(int dh);
    int originatingOpId();
    void setOriginatingOpId(int id);

private:
    int _local_request_id;
    int _leave_year;
    int _from_date_s;
    int _to_date_s;
    int _half_days;
    int _first_day_half;
    int _last_day_half;
    int _day_half;
    int _al_status;
    QString _note;
    int _updating_leave_year;
    int _updating_from_date_s;
    int _updating_to_date_s;
    int _updating_first_day_half;
    int _updating_last_day_half;
    int _updating_day_half;
    int _originating_op_id;

};

#endif /* DB_ANNUAL_LEAVE_HPP_ */
