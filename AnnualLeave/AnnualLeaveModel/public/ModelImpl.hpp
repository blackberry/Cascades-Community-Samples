#ifndef MODELIMPL_HPP_
#define MODELIMPL_HPP_

#define HEADLESS_SOCKET_SERVER_RETRY_INTERVAL_MS (2*1000)
#define HEADLESS_SOCKET_SERVER_MAX_RETRY_INTERVAL_MS (30*1000)

#include <IModel.hpp>
#include <common.hpp>

#include <QObject>
#include <QDebug>
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
#include "Entitlement.hpp"
#include "Operations.hpp"
#include "Utilities.hpp"
#include <bb/pim/calendar/CalendarService>
#include <bb/pim/calendar/CalendarEvent>
#include <bb/pim/calendar/CalendarFolder>

class UserSqlDataSource;

class Q_DECL_EXPORT ModelImpl
	: public QObject
	, public IModel
{
	Q_OBJECT
	Q_INTERFACES(IModel)

public:
	virtual ~ModelImpl();
	static ModelImpl *getInstance(QObject *parent);

    void submitBookingRequest(int leave_year, QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half, QString note);
    void cancelBookingRequest(int status, int local_request_id);
    void updateBookingRequest(int local_request_id, int current_leave_year, QDate current_from_date, QDateTime updating_from_date, QDate current_to_date, QDateTime updating_to_date,
            int current_first_day_half, bool updating_first_day_half, int current_last_day_half, bool last_day_half, int current_day_half,  QVariant day_half, QString note);
    void submitApprovalDecision(int task_id, int task_type, int local_request_id, int decision, int originating_op_id);
    void deleteApprovalTask(int task_id);
    void addCalendarEntry(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, int day_half, QString note);
    void refreshEntitlement(int leave_year);
    void setServiceVerbose(bool state);
    void clearProcessingStatus(int local_request_id, int leave_year);
    QString statusText(int booking_status, int last_op_status);
    QString statusIcon(int booking_status, int last_op_status);
    QString lastOpDesc(int last_op_status);
    QString taskStatus(int task_status, int decision);
    QString taskText(int task_status, int decision, QString user_id, int half_days, QString from_date);
    QString taskType(int task_status);
    QString taskIcon(int task_type, int task_status, int decision);
    QString taskDecision(int decision);
    QString requestParamsInterpretation(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half);
    QString approvalParamsInterpretation(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half);
    QString datesText(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half);
    QString updatingText(int last_op_status, QString updating_from_date, QString updating_to_date, int updating_first_day_half, int updating_last_day_half, int updating_day_half);

    void stopHeadlessService();
    void startHeadlessService();
    void resetAdapterSettings();
    void checkAdapterSettings();
    void requestAdapterStatus();
    void clientInitiatedSync(int leave_year);
    bool isValidDateRange(QDateTime from_date, QDateTime to_date, QString range_name);
    int totalEntitlement();
    void setTotalEntitlement(int total_entitlement);
    int currentEntitlement();
    void setCurrentEntitlement(int current_entitlement);
    QString adapterName();
    QString adapterVersion();
    QString adapterDescription();
    bool adapterConfigured();
    bool isInboxEmpty();

    void processInboundQueue();
    void processBookingApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome);
    void processCancellationApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome);
    void processUpdateApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome);
    void showTab(int tab_index);
    void showRequests(int year);
    QString interpret(QString from_date, QString to_date, bool first_day_half, bool last_day_half, int day_half);
    void requestEntitlementData(int leave_year);

    void connectToHeadless();
    void disconnectFromHeadless();
    void sendToHeadless(const QVariant &text);
    void triggerRetryConnection();

    void loadTaskData();
    void loadRequestData(QString leave_year);

signals:
    void zeroDaysWarning();
    void updateReceived();
    void suggestShowRequestsTab();
    void suggestShowInboxTab();
    void syncComplete();
    void operationNotSupported();
    void adapterNotConfigured();
    void inboxEmptyChanged();
    void adapterNameChanged();
    void adapterVersionChanged();
    void adapterDescriptionChanged();
    void adapterConfiguredChanged();
    void totalEntitlementChanged();
    void currentEntitlementChanged();
    void signalRequestsLoaded(const QVariantList &data);
    void signalTasksLoaded(const QVariantList &data);

private:
	ModelImpl(QObject *parent=NULL);
	static ModelImpl *_instance;

	int  processSyncData(BookingList *bookings);
    void disconnectSocket();
    void connectSocket();

    UserSqlDataSource* _sql;
    Operations* _ops;

    QString _adapter_name;
    QString _adapter_version;
    QString _adapter_description;
    bool _adapter_configured;
    bool _last_adapter_configured_value;
    bool _inbox_is_empty;
    int _total_entitlement;
    int _current_entitlement;
    bb::system::InvokeManager *_invokeManager;
    QLocalSocket *_socket;
    bool _connectedToServer;
    int _socketRetryInterval;


private slots:
    void onIndication();
    void onConnected();
    void onDisconnected();
    void onError(QLocalSocket::LocalSocketError socketError);
    void onStateChanged(QLocalSocket::LocalSocketState socketState);
    void onReadyRead();
    void onTimeout();

};

#endif /* MODELIMPL_HPP_ */
