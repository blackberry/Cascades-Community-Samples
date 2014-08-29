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

#ifndef INBOUNDQUEUEMANAGER_HPP_
#define INBOUNDQUEUEMANAGER_HPP_

#include <Common.hpp>
#include <AdapterImpl.hpp>
#include <OpSubmitBookingResp.hpp>
#include <Operations.hpp>
#include <Settings.hpp>

#include <QObject>
#include <QDebug>
#include <errno.h>

class InboundQueueManager
	: public QObject
{
	Q_OBJECT

public:
	virtual ~InboundQueueManager();

	static InboundQueueManager *getInstance(QObject *parent=NULL);

	int start();
	int stop();

public slots:

	void onInApprovalOutcomeRequ(int localId, int status, const QString &note, int originating_op_id, int half_days);

	void onInSubmitBookingResp(int localId, int status);

	void onInHalfDaysUsedRequ(int localId, int used);

	void onInCancelBookingResp(int localId, int status);

	void onInAnnualEntitlementResp(int status, int year, int total, int current);

	void onInUpdateBookingResp(int localId, int status, qint64 newFromDate, qint64 newToDate);

	// BOOKINGS
	void onInApprovalTaskRequ(int taskType, const QString &userId, int leaveYear, qint64 fromDate, qint64 toDate,
			                  bool firstDayHalf, bool lastDayHalf, int dayHalf, int half_days, int local_request_id, const QString &note, int originating_op_id);

	// UPDATES
    void onInApprovalTaskRequ(int taskType, int localId, int oldYear, int newYear, qint64 oldFromDate, qint64 newFromDate,
                              qint64 oldToDate, qint64 newToDate, bool oldFirstDayHalf, bool newFirstDayHalf,
                              bool oldLastDayHalf, bool newLastDayHalf, int oldDayHalf, int newDayHalf, const QString &userId, int halfDays, int originating_op_id);

    // CANCELLATIONS
    void onInApprovalTaskRequ(int taskType, int localId, int leaveYear, qint64 fromDate, qint64 toDate,
    		                  bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, const QString &userId, int halfDays, int originating_op_id);

	void onInApprovalTaskOutcomeResp(int taskId, int opStatus);

	void onInAdapterDetailsRequ();

    void onInAdapterStatusResp(int opStatus);

    void onInClientSynchronizeResp(int opStatus);

    void onInServerSyncResultRequ(QByteArray serialized_data);

private slots:
	void onSettingsChanged();

private:
	InboundQueueManager(QObject *parent=NULL);
	static InboundQueueManager* _instance;
    bool _initialised;
	AdapterImpl *_adapterImpl;
	IAdapter *_iAdapter;
	Operations *_ops;
	bool _dumpQueueEnabled;
	Settings *_settings;

	void dumpQueue();
};

#endif /* INBOUNDQUEUEMANAGER_HPP_ */
