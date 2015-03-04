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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QList>
#include <QMap>
#include "Entitlement.hpp"
#include <bb/cascades/ListView>
#include <bb/system/InvokeManager>
#include <bb/system/SystemUiResult>
#include <IModel.hpp>
#include <ModelImpl.hpp>
#include <bb/pim/account/Account>
#include <bb/pim/account/AccountService>
#include <bb/pim/account/Provider>

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
    namespace system
    {
        class InvokeManager;
    }
}

class QTranslator;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI: public QObject
{
Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI();

    Q_INVOKABLE void submitBookingRequest(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half, QString note);
    Q_INVOKABLE void cancelBookingRequest(int local_request_id, int current_status);
    Q_INVOKABLE void updateBookingRequest(int local_request_id, int current_leave_year, QDate current_from_date, QDateTime updating_from_date, QDate current_to_date, QDateTime updating_to_date,
            int current_first_day_half, bool updating_first_day_half, int current_last_day_half, bool last_day_half, int current_day_half,  QVariant day_half, QString note);
    Q_INVOKABLE void submitApprovalDecision(int task_id, int task_type, int local_request_id, int decision, int originating_op_id);
    Q_INVOKABLE void deleteApprovalTask(int task_id);
    Q_INVOKABLE void addCalendarEntry(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, int day_half, QString note);
    Q_INVOKABLE void refreshEntitlement(int leave_year);
    Q_INVOKABLE void clearProcessingStatus(int local_request_id, int leave_year);
    Q_INVOKABLE QString toLocalDateFormat(QString date_string);
    Q_INVOKABLE QString statusText(int booking_status, int last_op_status);
    Q_INVOKABLE QString statusIcon(int booking_status, int last_op_status);
    Q_INVOKABLE QString lastOpDesc(int last_op_status);
    Q_INVOKABLE QString taskStatus(int task_status, int decision);
    Q_INVOKABLE QString taskText(int task_status, int decision, QString user_id, int half_days, QString from_date);
    Q_INVOKABLE QString taskType(int task_status);
    Q_INVOKABLE QString taskIcon(int task_type, int task_status, int decision);
    Q_INVOKABLE QString taskDecision(int decision);
    Q_INVOKABLE bool sameDates(QDateTime datetime1, QDateTime datetime2);
    Q_INVOKABLE QString requestParamsInterpretation(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half);
    Q_INVOKABLE QString approvalParamsInterpretation(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half);
    Q_INVOKABLE QString datesText(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half);
    Q_INVOKABLE QString updatingText(int last_op_status, QString updating_from_date, QString updating_to_date, int updating_first_day_half, int updating_last_day_half, int updating_day_half);

    Q_INVOKABLE void stopHeadlessService();
    Q_INVOKABLE void startHeadlessService();
    Q_INVOKABLE void resetAdapterSettings();
    Q_INVOKABLE void checkAdapterSettings();
    Q_INVOKABLE void requestAdapterStatus();

    Q_INVOKABLE void clientInitiatedSync(int leave_year);

    Q_INVOKABLE bool isValidDateRange(QDateTime from_date, QDateTime to_date, QString range_name);

    Q_INVOKABLE void loadTaskData();
    Q_INVOKABLE void loadRequestData(QString leave_year);

    Q_INVOKABLE bool isValidEmailAddress(QString email_address);

    Q_PROPERTY(
            int total_entitlement
            READ totalEntitlement
            NOTIFY signalTotalEntitlementChanged)

    Q_PROPERTY(
            int current_entitlement
            READ currentEntitlement
            NOTIFY signalCurrentEntitlementChanged)

    Q_PROPERTY(
            QString adapter_name
            READ adapterName
            NOTIFY signalAdapterNameChanged)

    Q_PROPERTY(
            QString adapter_version
            READ adapterVersion
            NOTIFY signalAdapterVersionChanged)

    Q_PROPERTY(
            QString adapter_description
            READ adapterDescription
            NOTIFY signalAdapterDescriptionChanged)

    Q_PROPERTY(
            bool adapter_configured
            READ adapterConfigured
            NOTIFY signalAdapterConfiguredChanged)

    Q_PROPERTY(
            bool inbox_is_empty
            READ isInboxEmpty
            NOTIFY signalInboxEmptyChanged)

    Q_PROPERTY(
            QString setting1name
            READ setting1Name
            NOTIFY signalSetting1NameChanged)

    Q_PROPERTY(
            QString setting1value
            READ setting1Value
            WRITE setSetting1Value
            NOTIFY signalSetting1ValueChanged)

    Q_PROPERTY(
            QString setting2name
            READ setting2Name
            NOTIFY signalSetting2NameChanged)

    Q_PROPERTY(
            QString setting2value
            READ setting2Value
            WRITE setSetting2Value
            NOTIFY signalSetting2ValueChanged)

    Q_PROPERTY(
            QString setting3name
            READ setting3Name
            NOTIFY signalSetting3NameChanged)

    Q_PROPERTY(
            QString setting3value
            READ setting3Value
            WRITE setSetting3Value
            NOTIFY signalSetting5ValueChanged)

    Q_PROPERTY(
            QString setting4name
            READ setting4Name
            NOTIFY signalSetting4NameChanged)

    Q_PROPERTY(
            QString setting4value
            READ setting4Value
            WRITE setSetting4Value
            NOTIFY signalSetting4ValueChanged)

    Q_PROPERTY(
            QString setting5name
            READ setting5Name
            NOTIFY signalSetting5NameChanged)

    Q_PROPERTY(
            QString setting5value
            READ setting5Value
            WRITE setSetting5Value
            NOTIFY signalSetting5ValueChanged)

    int totalEntitlement();
    void setTotalEntitlement(int total_entitlement);

    int currentEntitlement();
    void setCurrentEntitlement(int current_entitlement);

    QString adapterName();
    QString adapterVersion();
    QString adapterDescription();
    bool adapterConfigured();
    bool isInboxEmpty();

    QString setting1Name();
    QString setting1Value();
    void setSetting1Value(QString value);

    QString setting2Name();
    QString setting2Value();
    void setSetting2Value(QString value);

    QString setting3Name();
    QString setting3Value();
    void setSetting3Value(QString value);

    QString setting4Name();
    QString setting4Value();
    void setSetting4Value(QString value);

    QString setting5Name();
    QString setting5Value();
    void setSetting5Value(QString value);

signals:
    void signalTotalEntitlementChanged();
    void signalCurrentEntitlementChanged();
    void signalAdapterNameChanged();
    void signalAdapterVersionChanged();
    void signalAdapterDescriptionChanged();
    void signalAdapterConfiguredChanged();
    void signalInboxEmptyChanged();
    void signalSetting1NameChanged();
    void signalSetting1ValueChanged();
    void signalSetting2NameChanged();
    void signalSetting2ValueChanged();
    void signalSetting3NameChanged();
    void signalSetting3ValueChanged();
    void signalSetting4NameChanged();
    void signalSetting4ValueChanged();
    void signalSetting5NameChanged();
    void signalSetting5ValueChanged();

public slots:
    void onInvoked(const bb::system::InvokeRequest &request);

private slots:
    void onSystemLanguageChanged();
    void loadRequests(const QVariantList& data);
    void loadTasks(const QVariantList& data);
    void onCancelDialogFinished(bb::system::SystemUiResult::Type type);
    void onDeleteTaskDialogFinished(bb::system::SystemUiResult::Type type);
    void onSettingsChanged();

    void onZeroDaysWarning();
    void onUpdateReceived();
    void onSuggestShowRequestsTab();
    void onSuggestShowInboxTab();
    void onSyncComplete();
    void onOperationNotSupported();
    void onAdapterNotConfigured();

private:

    void processInboundQueue();
    void processBookingApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome);
    void processCancellationApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome);
    void processUpdateApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome);
    void showTab(int tab_index);
    void showRequests(int year);
    void requestEntitlementData(int leave_year);
    void populateEmailSettingControls();

    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
    bb::system::InvokeManager *_invokeManager;

    ModelImpl *_modelImpl;
    IModel *_iModel;

    int _local_request_id;
    int _task_id;
    int _current_status;
    bool _inbox_is_empty;

    QString _setting1_name;
    QString _setting1_value;

    QString _setting2_name;
    QString _setting2_value;

    QString _setting3_name;
    QString _setting3_value;

    QString _setting4_name;
    QString _setting4_value;

    QString _setting5_name;
    QString _setting5_value;

    QList<bb::pim::account::Account> _accountList;
    QMap<QString, QString> *_account_provider_map;
};

#endif /* ApplicationUI_HPP_ */
