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

#ifndef OPERATIONS_HPP_
#define OPERATIONS_HPP_

#include <QObject>
#include <errno.h>
#include "OpSubmitBookingRequ.hpp"
#include "OpSubmitBookingResp.hpp"
#include "OpCancelBookingRequ.hpp"
#include "OpCancelBookingResp.hpp"
#include "OpUpdateBookingRequ.hpp"
#include "OpUpdateBookingResp.hpp"
#include "OpHalfDaysUsedRequ.hpp"
#include "OpHalfDaysUsedResp.hpp"
#include "OpApprovalOutcomeRequ.hpp"
#include "OpApprovalOutcomeResp.hpp"
#include "OpApprovalTaskOutcomeRequ.hpp"
#include "OpApprovalTaskOutcomeResp.hpp"
#include "OpApprovalTaskRequ.hpp"
#include "OpApprovalTaskResp.hpp"
#include "OpsSqlDataSource.hpp"
#include "OpAnnualEntitlementRequ.hpp"
#include "OpAnnualEntitlementResp.hpp"
#include "OpAdapterDetailsRequ.hpp"
#include "OpAdapterDetailsResp.hpp"
#include "OpAdapterStatusRequ.hpp"
#include "OpAdapterStatusResp.hpp"
#include "OpClientSynchronizeRequ.hpp"
#include "OpClientSynchronizeResp.hpp"
#include "OpServerSyncResultRequ.hpp"
#include "OpServerSyncResultResp.hpp"

#include <bb/system/InvokeManager>
#include <bb/platform/Notification>

class Q_DECL_EXPORT Operations
	: public QObject
{
	Q_OBJECT

public:
	virtual ~Operations();
	static Operations *getInstance(QObject *parent=NULL);

	int submit_Booking_Requ(OpSubmitBookingRequ request);
	void submit_Booking_Resp(OpSubmitBookingResp response);

    int cancel_Booking_Requ(OpCancelBookingRequ request);
    void cancel_Booking_Resp(OpCancelBookingResp response);

    int update_Booking_Requ(OpUpdateBookingRequ request);
    void update_Booking_Resp(OpUpdateBookingResp response);

	void half_days_used_Requ(OpHalfDaysUsedRequ request);
	void half_days_used_Resp(OpHalfDaysUsedResp response);

    void approval_outcome_Requ(OpApprovalOutcomeRequ request);
    void approval_outcome_Resp(OpApprovalOutcomeResp response);

    void annual_entitlement_Requ(OpAnnualEntitlementRequ request);
    void annual_entitlement_Resp(OpAnnualEntitlementResp response);

    void approval_task_Requ(OpApprovalTaskRequ request);
    void approval_task_Resp(OpApprovalTaskResp response);

    void approval_task_outcome_Requ(OpApprovalTaskOutcomeRequ request);
    void approval_task_outcome_Resp(OpApprovalTaskOutcomeResp response);

    void adapter_details_Requ(OpAdapterDetailsRequ request);
    void adapter_details_Resp(OpAdapterDetailsResp response);

    void adapter_status_Requ(OpAdapterStatusRequ request);
    void adapter_status_Resp(OpAdapterStatusResp response);

    void client_synchronize_Requ(OpClientSynchronizeRequ request);
    void client_synchronize_Resp(OpClientSynchronizeResp response);

    void server_sync_result_Requ(OpServerSyncResultRequ request);
    void server_sync_result_Resp(OpServerSyncResultResp response);

	void deleteInboundOp(int id);
	void deleteOutboundOp(int id);
	void deleteOutboundOp(int local_request_id, int op_type);

	void logInboundQueue();
    void logOutboundQueue();

    QList<OperationAl*> outboundQueueOps();
    QList<OperationAl*> inboundQueueOps();
	bool isGuiConnected() const;
	void setGuiConnected(bool guiConnected);
	bool isGuiAppProcessRunning() const;
	void setGuiAppProcessRunning(bool guiAppProcessRunning);
    const QString& getSocketServerPath() const;

signals:
//	void blahChanged();

private slots:
//	void onBlahChanged(const QString &blah);

private:
    Operations(QObject *parent = NULL);
    void notifyGuiInboundOperation();
    void notifyTask();
    bool isAlGuiRunning();

    static Operations* _instance;

    OpsSqlDataSource* _sql;

    bb::platform::Notification *_notify;
    bb::system::InvokeManager *_invokeManager;

    bool _guiConnected;
    bool _guiAppProcessRunning;
    QString _socketServerPath;
};

#endif /* OPERATIONS_HPP_ */
