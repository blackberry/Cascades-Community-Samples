#ifndef IMODEL_HPP_
#define IMODEL_HPP_

#include <ApiVersion.hpp>

#include <QObject>
#include <QTime>

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
class Q_DECL_EXPORT IModel {

public:
    virtual ~IModel() {};
    virtual void submitBookingRequest(int leave_year, QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half, QString note) = 0;
    virtual void cancelBookingRequest(int status, int local_request_id) = 0;
    virtual void updateBookingRequest(int local_request_id, int current_leave_year, QDate current_from_date, QDateTime updating_from_date, QDate current_to_date, QDateTime updating_to_date,
            int current_first_day_half, bool updating_first_day_half, int current_last_day_half, bool last_day_half, int current_day_half,  QVariant day_half, QString note) = 0;
    virtual void submitApprovalDecision(int task_id, int task_type, int local_request_id, int decision, int originating_op_id) = 0;
    virtual void deleteApprovalTask(int task_id) = 0;
    virtual void addCalendarEntry(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, int day_half, QString note) = 0;
    virtual void refreshEntitlement(int leave_year) = 0;
    virtual void setServiceVerbose(bool state) = 0;
    virtual void clearProcessingStatus(int local_request_id, int leave_year) = 0;
    virtual QString statusText(int booking_status, int last_op_status) = 0;
    virtual QString statusIcon(int booking_status, int last_op_status) = 0;
    virtual QString lastOpDesc(int last_op_status) = 0;
    virtual QString taskStatus(int task_status, int decision) = 0;
    virtual QString taskText(int task_status, int decision, QString user_id, int half_days, QString from_date) = 0;
    virtual QString taskType(int task_status) = 0;
    virtual QString taskIcon(int task_type, int task_status, int decision) = 0;
    virtual QString taskDecision(int decision) = 0;
    virtual QString requestParamsInterpretation(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half) = 0;
    virtual QString approvalParamsInterpretation(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half) = 0;
    virtual QString datesText(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half) = 0;
    virtual QString updatingText(int last_op_status, QString updating_from_date, QString updating_to_date, int updating_first_day_half, int updating_last_day_half, int updating_day_half) = 0;

    virtual void stopHeadlessService() = 0;
    virtual void startHeadlessService() = 0;
    virtual void resetAdapterSettings() = 0;
    virtual void checkAdapterSettings() = 0;
    virtual void requestAdapterStatus() = 0;
    virtual void clientInitiatedSync(int leave_year) = 0;
    virtual bool isValidDateRange(QDateTime from_date, QDateTime to_date, QString range_name) = 0;
    virtual int totalEntitlement() = 0;
    virtual void setTotalEntitlement(int total_entitlement) = 0;
    virtual int currentEntitlement() = 0;
    virtual void setCurrentEntitlement(int current_entitlement) = 0;
    virtual QString adapterName() = 0;
    virtual QString adapterVersion() = 0;
    virtual QString adapterDescription() = 0;
    virtual bool adapterConfigured() = 0;
    virtual bool isInboxEmpty() = 0;

    virtual void processInboundQueue() = 0;
    virtual void processBookingApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome) = 0;
    virtual void processCancellationApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome) = 0;
    virtual void processUpdateApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome) = 0;
    virtual void showTab(int tab_index) = 0;
    virtual void showRequests(int year) = 0;
    virtual QString interpret(QString from_date, QString to_date, bool first_day_half, bool last_day_half, int day_half) = 0;
    virtual void requestEntitlementData(int leave_year) = 0;
    virtual void connectToHeadless() = 0;
    virtual void disconnectFromHeadless() = 0;
    virtual void sendToHeadless(const QVariant &text) = 0;
    virtual void disconnectSocket() = 0;
    virtual void connectSocket() = 0;
    virtual void triggerRetryConnection() = 0;
    virtual void loadTaskData() = 0;
    virtual void loadRequestData(QString leave_year) = 0;

signals:
	virtual void zeroDaysWarning() = 0;
	virtual void updateReceived() = 0;
    virtual void suggestShowRequestsTab() = 0;
    virtual void suggestShowInboxTab() = 0;
    virtual void syncComplete() = 0;
    virtual void operationNotSupported() = 0;
    virtual void adapterNotConfigured() = 0;
    virtual void inboxEmptyChanged() = 0;
    virtual void adapterNameChanged() = 0;
    virtual void adapterVersionChanged() = 0;
    virtual void adapterDescriptionChanged() = 0;
    virtual void adapterConfiguredChanged() = 0;
    virtual void totalEntitlementChanged() = 0;
    virtual void currentEntitlementChanged() = 0;
    virtual void signalRequestsLoaded(const QVariantList &data) = 0;
    virtual void signalTasksLoaded(const QVariantList &data) = 0;

};

Q_DECLARE_INTERFACE(IModel, "com.example.annualleaveModel/1.0")

#endif /* IMODEL_HPP_ */
