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

#ifndef IADAPTER_HPP_
#define IADAPTER_HPP_

#include <ApiVersion.hpp>

#include <QObject>

#define ADAPTER_IMPL_ACCEPTED (0)
#define ADAPTER_IMPL_NOTACCEPTED (1)

class Q_DECL_EXPORT IAdapter {

public:
    virtual ~IAdapter() {};

    virtual int start() = 0;
    virtual int pause() = 0;
    virtual int resume() = 0;
    virtual int stop() = 0;
    virtual int reset() = 0;

    virtual QString version() = 0;     // returns a version identifier
    virtual QString name() = 0;        // returns a short name for the adapter e.g. the type (“EmailP2P”)
    virtual QString description() = 0; // returns longer text description

    virtual bool isConfigured() = 0; // indicates whether or not the adapter has its required settings

    virtual int networkAvailable() = 0;
    virtual int networkNotAvailable() = 0;
/*
 * NOTE
 *
 * Not all in/out method combinations have been implemented in this API yet
 * Only those that were necessary to implement this simple model
 *
 * Unimplemented yet valid ones are commented out for future use
 *
 */
	virtual int outSubmitBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id) = 0;
//	virtual int outSubmitBookingResp(...) = 0; // Not yet implemented

    virtual int outApprovalOutcomeResp(int localId, int opStatus) = 0;
//	virtual int outApprovalOutcomeRequ(...) = 0; // Not yet implemented

//	virtual int outHalfDaysUsedRequ(...) = 0;  // Not yet implemented
    virtual int outHalfDaysUsedResp(int localId, int opStatus) = 0;

	virtual int outCancelBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id) = 0;
//	virtual int outCancelBookingResp(...) = 0; // Not yet implemented

	virtual int outAnnualEntitlementRequ(int year) = 0;
//	virtual int outAnnualEntitlementResp(...) = 0; // Not yet implemented

	virtual int outUpdateBookingRequ(int localId, int oldYear, int newYear, qint64 oldFromDate, qint64 newFromDate,
			                      qint64 oldToDate, qint64 newToDate,
			                      bool oldFirstDayHalf, bool newFirstDayHalf,
			                      bool oldLastDayHalf, bool newLastDayHalf,
			                      int oldDayHalf, int newDayHalf, const QString &note, int originating_op_id) = 0;
//	virtual int outUpdateBookingResp(...) = 0; // Not yet implemented

//	virtual int outApprovalTaskRequ(...) = 0;  // Not yet implemented
	virtual int outApprovalTaskResp(int taskId, int opStatus, const QString &userId) = 0;

	virtual int outApprovalTaskOutcomeRequ(int taskId, int local_request_id, int requestStatus, int originating_op_id, int task_type) = 0;
//	virtual int outApprovalTaskOutcomeResp(...) = 0;  // Not yet implemented

    virtual int outAdapterStatusRequ() = 0;

    virtual int outClientSynchronizeRequ(int leave_year) = 0;

    virtual int outServerSyncResultResp(int op_id, int op_status) = 0;
/*
 * NOTE
 *
 * Not all in/out signal combinations have been implemented in this API yet
 * Only those that were necessary to implement this simple model
 *
 * Unimplemented yet valid ones are commented out for future use
 *
 */
signals:
	virtual void pauseDataFromApi() = 0;
	virtual void resumeDataFromApi() = 0;

//	virtual void inSubmitBookingRequ(...) = 0;  // Not yet implemented
	virtual void inSubmitBookingResp(int localId, int status) = 0;

	virtual void inApprovalOutcomeRequ(int localId, int status, const QString &note, int originating_op_id, int half_days) = 0;
//  virtual void inApprovalOutcomeResp(...) = 0; // Not yet implemented

	virtual void inHalfDaysUsedRequ(int localId, int used) = 0;
//  virtual void inHalfDaysUsedResp(...) = 0; // Not yet implemented

//	virtual void inCancelBookingResp(...) = 0; // Not yet implemented
	virtual void inCancelBookingResp(int localId, int status) = 0;

//	virtual void inAnnualEntitlementRequ(...) = 0;
	virtual void inAnnualEntitlementResp(int status, int year, int total, int current) = 0;

//	virtual void inUpdateBookingRequ(...) = 0;  // Not yet implemented
	virtual void inUpdateBookingResp(int localId, int status, qint64 newFromDate, qint64 newToDate) = 0;

	virtual void inApprovalTaskRequ(int taskType, const QString &userId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf,
			                      bool lastDayHalf, int dayHalf, int half_days, int local_request_id, const QString &note, int originating_op_id) = 0;
    virtual void inApprovalTaskRequ(int taskType, int localId, int oldYear, int newYear, qint64 oldFromDate, qint64 newFromDate,
                          qint64 oldToDate, qint64 newToDate, bool oldFirstDayHalf, bool newFirstDayHalf,
                          bool oldLastDayHalf, bool newLastDayHalf, int oldDayHalf, int newDayHalf, const QString &userId, int halfDays, int originating_op_id) = 0;
    virtual void inApprovalTaskRequ(int taskType, int localId, int leaveYear, qint64 fromDate, qint64 toDate,
    		                bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, const QString &userId, int halfDays, int originating_op_id) = 0;
//	virtual void inApprovalTaskResp(...) = 0; // Not yet implemented

//	virtual void inApprovalTaskOutcomeRequ(...) = 0;  // Not yet implemented
	virtual void inApprovalTaskOutcomeResp(int taskId, int opStatus) = 0;

	virtual void inAdapterDetailsRequ() = 0;
    virtual void inAdapterStatusResp(int opStatus) = 0;
};

Q_DECLARE_INTERFACE(IAdapter, "com.example.annualleaveAdapter/1.0")

#endif /* IADAPTER_HPP_ */
