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

#ifndef APPROVAL_TASK_REQU_HPP_
#define APPROVAL_TASK_REQU_HPP_
#include "OperationAl.hpp"
#include <QString>

class Q_DECL_EXPORT OpApprovalTaskRequ : public OperationAl {

public:
	OpApprovalTaskRequ();
	~OpApprovalTaskRequ();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

	QString userId();
	void setUserId(QString user_id);

	int taskType();
	void setTaskType(int tt);

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

    int localRequestId();
    void setLocalRequestId(int local_request_id);

    int originatingOpId();
    void setOriginatingOpId(int id);

    int newLeaveYear();
    void setNewLeaveYear(int year);
    qint64 newFromDate();
    void setNewFromDate(qint64 fdate);
    qint64 newToDate();
    void setNewToDate(qint64 tdate);
    bool newFirstDayHalf();
    void setNewFirstDayHalf(bool is_half);
    bool newLastDayHalf();
    void setNewLastDayHalf(bool is_half);
    int newDayHalf();
    void setNewDayHalf(int half_of_day);

    QString _user_id;
    int _task_type;
    int _leave_year;
    qint64 _from_date;
    qint64 _to_date;
    bool _first_day_half;
    bool _last_day_half;
    int _day_half;
    int _half_days;
    QString _note;
    int _local_request_id;

    // if the task_type is TASK_TYPE_UPDATE_APPROVAL then these fields will also be populated
    int _new_leave_year;
    qint64 _new_from_date;
    qint64 _new_to_date;
    bool _new_first_day_half;
    bool _new_last_day_half;
    int _new_day_half;

    // for all types, we including the op ID of the original operation that requested the change to be approved
    int _originating_op_id;

};

#endif /* APPROVAL_TASK_REQU_HPP_ */
