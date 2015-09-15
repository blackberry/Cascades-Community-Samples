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

#ifndef EMAILADAPTER_HPP_
#define EMAILADAPTER_HPP_

#define EMAIL_ADAPTER_SETTINGS_KEY_ACCOUNT_NAME "EMAIL-ADAPTER-ACCOUNT-NAME"
#define EMAIL_ADAPTER_SETTINGS_KEY_ACCOUNT_PROVIDER_NAME "EMAIL-ADAPTER-ACCOUNT-PROVIDER-NAME"
#define EMAIL_ADAPTER_SETTINGS_KEY_SERVICE_EMAIL "EMAIL-ADAPTER-SERVICE-EMAIL"
#define EMAIL_ADAPTER_SETTINGS_KEY_SERVICE_NAME "EMAIL-ADAPTER-SERVICE-NAME"
#define EMAIL_ADAPTER_SETTINGS_KEY_P2P_APPROVER_EMAIL "EMAIL-ADAPTER-P2P-APPROVER-EMAIL"
#define EMAIL_ADAPTER_SETTINGS_KEY_P2P_APPROVER_NAME "EMAIL-ADAPTER-P2P-APPROVER-NAME"
#define EMAIL_ADAPTER_SETTINGS_KEY_P2P_MODE_ENABLED "EMAIL-ADAPTER-P2P-MODE-ENABLED"
#define EMAIL_ADAPTER_SETTINGS_KEY_P2P_CLIENT_USERID "EMAIL-ADAPTER-P2P-CLIENT-USERID"

#define EMAIL_ADAPTER_SERVICE_UUID "A9C10242-DB4E-4657-B631-AABB1923C5C3"
#define EMAIL_ADAPTER_JSON_SECTION_START_MARKER "===91F539A3-F7BA-46FF-ADA6-62000C898BC5==="
#define EMAIL_ADAPTER_JSON_SECTION_BODY_REGEX "(.*)"
#define EMAIL_ADAPTER_JSON_SECTION_END_MARKER "---91F539A3-F7BA-46FF-ADA6-62000C898BC5---"

#define EMAIL_ADAPTER_KEY_OPERATION "OPERATION"
#define EMAIL_ADAPTER_KEY_LOCAL_ID "LOCAL-ID"
#define EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID "ORIGINATING-OP-ID"
#define EMAIL_ADAPTER_KEY_TASK_TYPE "TASK-TYPE"
#define EMAIL_ADAPTER_KEY_LEAVE_YEAR "LEAVE-YEAR"
#define EMAIL_ADAPTER_KEY_FROM_DATE "FROM-DATE"
#define EMAIL_ADAPTER_KEY_TO_DATE "TO-DATE"
#define EMAIL_ADAPTER_KEY_FIRST_DAY_HALF "FIRST-DAY-HALF"
#define EMAIL_ADAPTER_KEY_LAST_DAY_HALF "LAST-DAY-HALF"
#define EMAIL_ADAPTER_KEY_DAY_HALF "DAY-HALF"
#define EMAIL_ADAPTER_KEY_HALF_DAYS "HALF-DAYS"
#define EMAIL_ADAPTER_KEY_NOTE "NOTE"
#define EMAIL_ADAPTER_KEY_OPERATION_STATUS "OPERATION-STATUS"
#define EMAIL_ADAPTER_KEY_NEW_YEAR "NEW-YEAR"
#define EMAIL_ADAPTER_KEY_OLD_YEAR "OLD-YEAR"
#define EMAIL_ADAPTER_KEY_NEW_FROM_DATE "NEW-FROM-DATE"
#define EMAIL_ADAPTER_KEY_OLD_FROM_DATE "OLD-FROM-DATE"
#define EMAIL_ADAPTER_KEY_NEW_TO_DATE "NEW-TO-DATE"
#define EMAIL_ADAPTER_KEY_OLD_TO_DATE "OLD-TO-DATE"
#define EMAIL_ADAPTER_KEY_NEW_FIRST_DAY_HALF "NEW-FIRST-DAY-HALF"
#define EMAIL_ADAPTER_KEY_OLD_FIRST_DAY_HALF "OLD-FIRST-DAY-HALF"
#define EMAIL_ADAPTER_KEY_NEW_LAST_DAY_HALF "NEW-LAST-DAY-HALF"
#define EMAIL_ADAPTER_KEY_OLD_LAST_DAY_HALF "OLD-LAST-DAY-HALF"
#define EMAIL_ADAPTER_KEY_NEW_DAY_HALF "NEW-DAY-HALF"
#define EMAIL_ADAPTER_KEY_OLD_DAY_HALF "OLD-DAY-HALF"
#define EMAIL_ADAPTER_KEY_NEW_LEAVE_YEAR "NEW-LEAVE-YEAR"
#define EMAIL_ADAPTER_KEY_OUTCOME "OUTCOME"
#define EMAIL_ADAPTER_KEY_YEAR "YEAR"
#define EMAIL_ADAPTER_KEY_TOTAL "TOTAL"
#define EMAIL_ADAPTER_KEY_CURRENT "CURRENT"
#define EMAIL_ADAPTER_KEY_USED "USED"
#define EMAIL_ADAPTER_KEY_USER_ID "USER-ID"
#define EMAIL_ADAPTER_KEY_TASK_ID "TASK-ID"
#define EMAIL_ADAPTER_KEY_MESSAGE_ID "MESSAGE-ID"
#define EMAIL_ADAPTER_KEY_SENDER_EMAIL "SENDER-EMAIL"
#define EMAIL_ADAPTER_KEY_SENDER_NAME "SENDER-NAME"
#define EMAIL_ADAPTER_KEY_PAYLOAD "PAYLOAD"
#define EMAIL_ADAPTER_KEY_TIME "TIME"
#define EMAIL_ADAPTER_KEY_TO_BE_REMOVED "TO-BE-REMOVED"
#define EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED "MESSAGE-COMPLETED"
#define EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED "MESSAGE-BEING-PROCESSED"
#define EMAIL_ADAPTER_KEY_BOOKING_REQ "BOOKING-REQ"
#define EMAIL_ADAPTER_KEY_APPROVAL_OUTCOME_ACK "APPROVAL-OUTCOME-ACK"
#define EMAIL_ADAPTER_KEY_APPROVAL_OUTCOME_REQ "APPROVAL-OUTCOME-REQ"
#define EMAIL_ADAPTER_KEY_USED_DAYS_ACK "USED-DAYS-ACK"
#define EMAIL_ADAPTER_KEY_USED_DAYS_REQ "USED-DAYS-REQ"
#define EMAIL_ADAPTER_KEY_CANCEL_REQ "CANCEL-REQ"
#define EMAIL_ADAPTER_KEY_CANCEL_OUTCOME_ACK "CANCEL-OUTCOME-ACK"
#define EMAIL_ADAPTER_KEY_CANCEL_OUTCOME_REQ "CANCEL-OUTCOME-REQ"
#define EMAIL_ADAPTER_KEY_ENTITLEMENT_REQ "ENTITLEMENT-REQ"
#define EMAIL_ADAPTER_KEY_ENTITLEMENT_ACK "ENTITLEMENT-ACK"
#define EMAIL_ADAPTER_KEY_BOOKING_UPDATE_REQ "BOOKING-UPDATE-REQ"
#define EMAIL_ADAPTER_KEY_UPDATE_OUTCOME_ACK "UPDATE-OUTCOME-ACK"
#define EMAIL_ADAPTER_KEY_UPDATE_OUTCOME_REQ "UPDATE-OUTCOME-REQ"
#define EMAIL_ADAPTER_KEY_APPROVAL_TASK_ACK "APPROVAL-TASK-ACK"
#define EMAIL_ADAPTER_KEY_APPROVAL_TASK_REQ "APPROVAL-TASK-REQ"
#define EMAIL_ADAPTER_KEY_APPROVAL_TASK_OUTCOME_REQ "APPR-TASK-OUTCOME-REQ"
#define EMAIL_ADAPTER_KEY_APPROVAL_TASK_OUTCOME_ACK "APPR-TASK-OUTCOME-ACK"
#define EMAIL_ADAPTER_KEY_STATUS "STATUS"
#define EMAIL_ADAPTER_KEY_APPROVED "APPROVED"
#define EMAIL_ADAPTER_KEY_REJECTED "REJECTED"
#define EMAIL_ADAPTER_KEY_COMPLETED "COMPLETED"
#define EMAIL_ADAPTER_KEY_FAILED "FAILED"
#define EMAIL_ADAPTER_KEY_OK "OK"
#define EMAIL_ADAPTER_KEY_UNRECOGNISED_OUTCOME "*** UNRECOGNISED OUTCOME ***"
#define EMAIL_ADAPTER_KEY_UNRECOGNISED_OPERATION "*** UNRECOGNISED OPERATION ***"


// "<OPERATION> "
#define EMAIL_ADAPTER_REGEX_OPERATION "^\\s*(\\S+)\\s+"

#include "AdapterImpl.hpp"
#include "AdapterSettings.hpp"
#include "SimpleTestCalendar.hpp"
#include <common.hpp>
#include "Settings.hpp"
#include "EntitlementCache.hpp"
#include "RollBack.hpp"
#include "UserIdCache.hpp"

#include <QObject>
#include <QDebug>
#include <errno.h>
#include <unistd.h>
#include <QRegExp>

#include <bb/pim/account/AccountService>
#include <bb/pim/account/Account>
#include <bb/pim/account/Provider>
#include <bb/pim/message/MessageService>
#include <bb/cascades/GroupDataModel>
#include <bb/pim/message/MessageSearchFilter>
#include <bb/pim/message/MessageBuilder>
#include <bb/data/JsonDataAccess>

using namespace bb::cascades;
using namespace bb::pim::message;
using namespace bb::cascades;
using namespace bb::data;

typedef QMap<int, QString> StringMap;

Q_DECLARE_METATYPE(QList<bb::pim::message::ConversationKey>)

class AdapterImpl;

class Q_DECL_EXPORT EmailAdapter
	: public QObject
{
	Q_OBJECT

public:
	virtual ~EmailAdapter();
	static EmailAdapter *getInstance(QObject *parent);

	int start();
	int stop();
	int pause();
	int resume();
	int reset();

    int networkAvailable();
    int networkNotAvailable();

    int outSubmitBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id);
    int outCancelBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id);
    int outUpdateBookingRequ(int localId,
                          int oldYear, int newYear,
                          qint64 oldFromDate, qint64 newFromDate,
                          qint64 oldToDate, qint64 newToDate,
                          bool oldFirstDayHalf, bool newFirstDayHalf,
                          bool oldLastDayHalf, bool newLastDayHalf,
                          int oldDayHalf, int newDayHalf,
                          const QString &note, int originating_op_id);

	int outApprovalOutcomeResp(int localId, int opStatus);

	int outHalfDaysUsedResp(int localId, int opStatus);


	int outAnnualEntitlementRequ(int year);

	int outApprovalTaskResp(int taskId, int opStatus, const QString &userId);

	int outApprovalTaskOutcomeRequ(int taskId, int local_request_id, int requestStatus, int originating_op_id, int task_type);

	int outAdapterStatusRequ();

	int outClientSynchronizeRequ(int leave_year);

	int outServerSyncResultResp(int op_id, int op_status);

signals:
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
	void pauseDataFromApi();
	void resumeDataFromApi();

private slots:
	void messagesUpdated();
	void onSettingsChanged();
	void bodyDownloaded(bb::pim::account::AccountKey accountId, bb::pim::message::MessageKey messageId);
	void messageRemoved(bb::pim::account::AccountKey accountId, bb::pim::message::ConversationKey conversationId,
	                    bb::pim::message::MessageKey messageId, QString sourceId);

    void onItemAdded(QVariantList indexPath);
    void onItemUpdated(QVariantList indexPath);
    void onItemRemoved(QVariantList indexPath);

private:
	EmailAdapter(QObject *parent=NULL);
	static EmailAdapter* _instance;
    static Settings* _settings;
    static AdapterSettings* _adapter_settings;
    static SimpleTestCalendar* _calendar;
	AdapterImpl *_adapterImpl;
	MessageService *_messageService;
	bb::pim::account::Account _currentAccount;
    QList<bb::pim::account::Account> _accountList;
    GroupDataModel *_model;
    MessageBuilder *_builder;
	StringMap *_mapOpType;
	StringMap *_mapStatus;
	StringMap *_mapTaskType;
	EntitlementCache *_cache;
	int _last_configured_value;
	RollBack *_rollBack;
	UserIdCache *_userIdCache;
    QMutex _mutex;

	void connectSignals();
	void connectSettingsChangedSignal();
	void connectAdapterDetailsSignal();
    void selectCurrentAccount();
    void processInboundMessages();
    void processJsonInboundMessages();
    void deleteMessage(MessageKey id, QVariantList indexPath, QVariantMap entry);
    void deleteMessage(MessageKey id);
    void sendJsonMessage(const QString &jsonPayload, const QString &recipientEmail, const QString &recipientName);
    void sendMessage(MessageContact recipient, QString subject, QByteArray body);
    const QString toIsoFormat(qint64 date) const;
    qint64 fromIsoFormat(const QString &date) const;
    void setUpStringMaps();
    QString stringForOpType(int key);
    QString stringForStatus(int key);
    QString stringForTaskType(int key);
    bool constructAndSendMessage(QVariantMap parameters, const QString &recipientEmail, const QString &recipientName);
    bool addMessage(const Message &message);
    void replyWithErrorMessage(MessageKey id, const QString &errorMessage);
    void logAccounts();
    void modelClear();
    bool modelInsert(QVariantMap entry);
};

#endif /* EMAILADAPTER_HPP_ */
