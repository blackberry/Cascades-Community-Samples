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

#ifndef ADAPTERIMPL_HPP_
#define ADAPTERIMPL_HPP_

#include <IAdapter.hpp>
#include <common.hpp>
#include <EmailAdapter.hpp>

#include <QObject>
#include <errno.h>
#include <bb/pim/account/AccountService>
#include <bb/pim/account/Account>
#include <bb/pim/account/Provider>

#define ADAPTER_IMPL_VERSION "Beta 0.0.3.4"
#define ADAPTER_IMPL_DESCRIPTION "A simple adapter that uses email as a transport to operate in peer-to-peer mode."
#define ADAPTER_IMPL_NAME "EmailAdapter"

class EmailAdapter;

class Q_DECL_EXPORT AdapterImpl
	: public QObject
	, public IAdapter
{
	Q_OBJECT
	Q_INTERFACES(IAdapter)

public:
	virtual ~AdapterImpl();
	static AdapterImpl *getInstance(QObject *parent);

    int start();
    int pause();
    int resume();
    int stop();
    int reset();

    QString version();     // returns a version identifier
    QString name();        // returns a short name for the adapter e.g. the type (“EmailP2P”)
    QString description(); // returns longer text description
    bool isConfigured();

    int networkAvailable();
    int networkNotAvailable();

	int outSubmitBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id);

    int outApprovalOutcomeResp(int localId, int opStatus);

    int outHalfDaysUsedResp(int localId, int opStatus);

	int outCancelBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id);

	int outAnnualEntitlementRequ(int year);

	int outUpdateBookingRequ(int localId, int oldYear, int newYear, qint64 oldFromDate, qint64 newFromDate,
						  qint64 oldToDate, qint64 newToDate,
						  bool oldFirstDayHalf, bool newFirstDayHalf,
						  bool oldLastDayHalf, bool newLastDayHalf,
						  int oldDayHalf, int newDayHalf, const QString &note, int originating_op_id);

	int outApprovalTaskResp(int taskId, int opStatus, const QString &userId);

	int outApprovalTaskOutcomeRequ(int taskId, int local_request_id, int requestStatus, int originating_op_id, int task_type);

	int outAdapterStatusRequ();

	int outClientSynchronizeRequ(int leave_year);

	int outServerSyncResultResp(int op_id, int op_status);

signals:
	void pauseDataFromApi();
	void resumeDataFromApi();

	void inSubmitBookingResp(int localId, int status);

	void inApprovalOutcomeRequ(int localId, int status, const QString &note, int originating_op_id, int half_days);

	void inHalfDaysUsedRequ(int localId, int used);

	void inCancelBookingResp(int localId, int status);

	void inAnnualEntitlementResp(int status, int year, int total, int current);

	void inUpdateBookingResp(int localId, int status, qint64 newFromDate, qint64 newToDate);

	void inApprovalTaskRequ(int taskType, const QString &userId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf,
						    bool lastDayHalf, int dayHalf, int half_days, int local_request_id, const QString &note, int originating_op_id);
    void inApprovalTaskRequ(int taskType, int localId, int oldYear, int newYear, qint64 oldFromDate, qint64 newFromDate,
                          qint64 oldToDate, qint64 newToDate, bool oldFirstDayHalf, bool newFirstDayHalf,
                          bool oldLastDayHalf, bool newLastDayHalf, int oldDayHalf, int newDayHalf, const QString &userId, int halfDays, int originating_op_id);
    void inApprovalTaskRequ(int taskType, int localId, int leaveYear, qint64 fromDate, qint64 toDate,
    		                bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, const QString &userId, int halfDays, int originating_op_id);

	void inApprovalTaskOutcomeResp(int taskId, int opStatus);
    void inAdapterDetailsRequ();
    void inAdapterStatusResp(int opStatus);
    void inClientSynchronizeResp(int opStatus);
    void inServerSyncResultRequ(QByteArray serialized_data);

private:
	AdapterImpl(QObject *parent=NULL);
	static AdapterImpl* _instance;
	EmailAdapter *_emailAdapter;
    QList<bb::pim::account::Account> _accountList;
};

#endif /* ADAPTERIMPL_HPP_ */
