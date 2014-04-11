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

#include "applicationui.hpp"
#include "UserSqlDataSource.hpp"
#include "Operations.hpp"
#include "GuiIndicator.hpp"
#include "OpSubmitBookingRequ.hpp"
#include "OpSubmitBookingResp.hpp"
#include "OpAdapterDetailsRequ.hpp"
#include "OpAdapterDetailsResp.hpp"
#include "common.hpp"
#include "Utilities.hpp"
#include "Settings.hpp"
#include "GuiIndicator.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/SystemDialog>
#include <QtNetwork/QLocalSocket>

#include <QTimer>

#include <bb/pim/calendar/CalendarService>
#include <bb/pim/calendar/CalendarEvent>
#include <bb/pim/calendar/CalendarFolder>

using namespace bb::cascades;
using namespace bb::pim::calendar;

static AbstractPane *_root = 0;

ApplicationUI::ApplicationUI(bb::cascades::Application *app)
	: QObject(app)
	, _invokeManager(new bb::system::InvokeManager(this))
	, _total_entitlement(-1)
	, _current_entitlement(-1)
	, _adapter_description("Unknown")
	, _adapter_configured(false)
	, _inbox_is_empty(true)
	, _adapter_name("Unknown")
	, _adapter_version("Unknown")
	, _last_adapter_configured_value(true)
	, _socket(new QLocalSocket(this))
	, _connectedToServer(false)
    , _socketRetryInterval(HEADLESS_SOCKET_SERVER_RETRY_INTERVAL_MS)
{
    qDebug() << "GGGG ApplicationUI constructor - starting";

    // getting instance of GuiIndicator ensures the indicator directory has been created before we watch it
    GuiIndicator* indicator = GuiIndicator::getInstance(this);
    if (!QObject::connect(indicator, SIGNAL(signalInboundOperation()), this, SLOT(onIndication()), Qt::QueuedConnection)) {
        qWarning() << "GGGG onIndication - connect failed " << strerror(errno) << endl;
    }

    qDebug() << "GGGG made GuiIndicator QueuedConnection";

    indicator->clearIndications();

    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));

    Settings* settings = Settings::getInstance(this);
    if (!QObject::connect(settings, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()))) {
        qWarning() << "GGGG settingsChanged - connect failed " << strerror(errno) << endl;
    }

    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    _sql = UserSqlDataSource::getInstance(this);
    _ops = Operations::getInstance(this);

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    QDeclarativeContext *root_context;
    root_context = qml->defaultDeclarativeEngine()->rootContext();
    root_context->setContextProperty("sql", _sql);
    root_context->setContextProperty("app", this);

    // Create root object for the UI
    _root = qml->createRootObject<AbstractPane>();

    connectSocket();

    // Set created root object as the application scene
    app->setScene(_root);

    int conn_res = QObject::connect(_sql, SIGNAL(signalRequestsLoaded(const QVariantList &)), this, SLOT(loadRequests(const QVariantList &)));
    conn_res = QObject::connect(_sql, SIGNAL(signalTasksLoaded(const QVariantList &)), this, SLOT(loadTasks(const QVariantList &)));
    conn_res = QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

    startHeadlessService();

    connectToHeadless();

    checkAdapterSettings();

    refreshEntitlement(QDate::currentDate().year());
    processInboundQueue();

}

ApplicationUI::~ApplicationUI() {
	if (_socket) {
		disconnectSocket();
		_connectedToServer = false;
		_socket->close();
		_socket->deleteLater();
	}
}

void ApplicationUI::onInvoked(const bb::system::InvokeRequest &request)
{

    qDebug() << "GGGG entering ApplicationUI::onInvoked(const InvokeRequest &request): " << request.action() << endl;

    connectToHeadless();

    if (request.action().compare(TARGET_AL_ACTION_INBOUND_QUEUE) == 0) {
        qDebug() << "GGGG TARGET_AL_ACTION_INBOUND_QUEUE action" << endl;
        processInboundQueue();

    } else {
        qDebug() << "GGGG not doing anything special with requested action" << endl;
    }

}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("VacationRequestPrototype_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::loadRequests(const QVariantList& data)
{
    qDebug() << "GGGG loadRequests in ApplicationUI:" << data;
    Q_UNUSED(data);
    QMetaObject::invokeMethod(_root, "loadRequests", Q_ARG(QVariant, QVariant(data)));
}

void ApplicationUI::loadTasks(const QVariantList& data)
{
    qDebug() << "GGGG loadTasks in ApplicationUI:" << data;
    Q_UNUSED(data);
    QMetaObject::invokeMethod(_root, "loadTasks", Q_ARG(QVariant, QVariant(data)));
}

bool ApplicationUI::isValidDateRange(QDateTime from_date, QDateTime to_date, QString range_name)
{

    if (to_date < from_date) {
        Utilities::alert(QString("Error: %1: 'To Date' must not be before 'From Date'").arg(range_name));
        return false;
    }

    if (to_date.date().year() != from_date.date().year()) {
        Utilities::alert(QString("Error: %1: 'From Date' and 'To Date' must be in the same year").arg(range_name));
        return false;
    }

    return true;

}

void ApplicationUI::submitBookingRequest(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half, QString note)
{

    qDebug() << "GGGG ApplicationUI::submitBookingRequest: from_date=" << from_date.toString() << " to_date=" << to_date.toString() << " first_day_half=" << first_day_half << " last_day_half="
            << last_day_half << " day_half=" << day_half;

    // validation

    if (to_date < from_date) {
        Utilities::alert("Error: 'To Date' must not be before 'From Date'");
        return;
    }

    if (to_date.date().year() != from_date.date().year()) {
        Utilities::alert("Error: 'From Date' and 'To Date' must be in the same year");
        return;
    }

    // store in user database and receive local_request_id in response

    int half_of_the_day = 0;
    if (sameDates(from_date, to_date)) {
        half_of_the_day = day_half.toInt();
    }

    int leave_year = to_date.date().year();
    int fdate = from_date.toMSecsSinceEpoch() / 1000;
    int tdate = to_date.toMSecsSinceEpoch() / 1000;

    // make sure "half day bookings" indicated with both first_day_half and last_day_half set to true
    if (fdate == tdate) {
        if (first_day_half) {
            last_day_half = true;
        }
    } else {
        if (first_day_half) {
            half_of_the_day = DAY_HALF_AFTERNOON;
        } else {
            if (last_day_half) {
                half_of_the_day = DAY_HALF_MORNING;
            }
        }
    }

    //TODO what if there's an error?
    int local_request_id = _sql->storeRequest(fdate, tdate, note, leave_year, first_day_half, last_day_half, half_of_the_day);

    qDebug() << "GGGG stored request and got ID:" << local_request_id;

    // submit request using the operations API
    OpSubmitBookingRequ request;
    request.setLocalRequestId(local_request_id);
    request.setLeaveYear(leave_year);
    request.setFromDate(from_date.toMSecsSinceEpoch());
    request.setToDate(to_date.toMSecsSinceEpoch());
    request.setFirstDayHalf(first_day_half);
    request.setLastDayHalf(last_day_half);
    request.setDayHalf(half_of_the_day);
    request.setNote(note);

    int originating_op_id = _ops->submit_Booking_Requ(request);
    _sql->setOriginatingOpId(local_request_id, originating_op_id);

    qDebug() << "GGGG done submitting request";

    showRequests(leave_year);
}

void ApplicationUI::cancelBookingRequest(int local_request_id, int current_status)
{
    qDebug() << "GGGG ApplicationUI::cancelBookingRequest(" << local_request_id << "," << current_status + ")";

    _local_request_id = local_request_id;
    _current_status = current_status;

    // confirm

    bb::system::SystemDialog *dial = new bb::system::SystemDialog("Confirm", "Cancel", this);
    dial->setTitle("Confirm Action");

    if (current_status == STATUS_BOOKING_REJECTED || current_status == STATUS_FAILED || current_status == STATUS_CANCELLATION_OK) {
        dial->setBody("Please confirm deletion request");
    } else {
        if (current_status == STATUS_CANCELLATION_PENDING || current_status == STATUS_CANCELLATION_SUBMITTED) {
            dial->setBody("Item is already being cancelled - Please confirm deletion");
        } else {
            dial->setBody("Please confirm cancellation request");
        }
    }
    dial->setDismissAutomatically(true);
    bool success = connect(dial, SIGNAL(finished(bb::system::SystemUiResult::Type)), this, SLOT(onCancelDialogFinished(bb::system::SystemUiResult::Type)));

    if (success) {
        dial->show();
    } else {
        dial->deleteLater();
        Utilities::alert("Error relating to system dialog");
        return;
    }

}

void ApplicationUI::deleteApprovalTask(int task_id)
{
    qDebug() << "GGGG ApplicationUI::deleteApprovalTask(" << task_id + ")";

    _task_id = task_id;

    // confirm

    bb::system::SystemDialog *dial = new bb::system::SystemDialog("Confirm", "Cancel", this);
    dial->setTitle("Confirm Action");
    dial->setBody("Please confirm deletion request");
    dial->setDismissAutomatically(true);
    bool success = connect(dial, SIGNAL(finished(bb::system::SystemUiResult::Type)), this, SLOT(onDeleteTaskDialogFinished(bb::system::SystemUiResult::Type)));

    if (success) {
        dial->show();
    } else {
        dial->deleteLater();
        Utilities::alert("Error relating to system dialog");
        return;
    }
}

void ApplicationUI::updateBookingRequest(int local_request_id, int current_leave_year, QDate current_from_date, QDateTime updating_from_date, QDate current_to_date, QDateTime updating_to_date,
        int current_first_day_half, bool updating_first_day_half, int current_last_day_half, bool updating_last_day_half, int current_day_half, QVariant updating_day_half, QString note)
{

    qDebug() << "GGGG ApplicationUI::updateBookingRequest: local_request_id=" << local_request_id << " current_leave_year=" << current_leave_year << " current_from_date=" << current_from_date
            << " updating_from_date=" << updating_from_date << " current_to_date=" << current_to_date << " updating_to_date=" << updating_to_date;

    qDebug() << "GGGG ApplicationUI::updateBookingRequest:  " << " current_first_day_half=" << current_first_day_half << "updating_first_day_half=" << updating_first_day_half;
    qDebug() << "GGGG ApplicationUI::updateBookingRequest:  " << " current_last_day_half=" << current_last_day_half << " updating_last_day_half=" << updating_last_day_half;
    qDebug() << "GGGG ApplicationUI::updateBookingRequest:  " << " current_day_half=" << current_day_half << " updating_day_half=" << updating_day_half << " note=" << note;

    // validation

    if (updating_to_date < updating_from_date) {
        Utilities::alert("Error: 'To Date' must not be before 'From Date'");
        return;
    }

    if (updating_to_date.date().year() != updating_from_date.date().year()) {
        Utilities::alert("Error: 'From Date' and 'To Date' must be in the same year");
        return;
    }

    int updating_leave_year = updating_to_date.date().year();
    int updating_fdate = updating_from_date.toMSecsSinceEpoch() / 1000;
    int updating_tdate = updating_to_date.toMSecsSinceEpoch() / 1000;

    int day_half_indicator = 0;
    if (sameDates(updating_from_date, updating_to_date)) {
        if (updating_first_day_half) {
            day_half_indicator = updating_day_half.toInt();
            updating_last_day_half = true;
        }
    }

    // submit request using the operations API
    OpUpdateBookingRequ request;
    request.setLocalRequestId(local_request_id);

    request.setOldLeaveYear(current_leave_year);
    request.setNewLeaveYear(updating_leave_year);

    request.setOldFromDate(QDateTime(current_from_date).toMSecsSinceEpoch());
    request.setNewFromDate(updating_from_date.toMSecsSinceEpoch());

    request.setOldToDate(QDateTime(current_to_date).toMSecsSinceEpoch());
    request.setNewToDate(updating_to_date.toMSecsSinceEpoch());

    request.setOldDayHalf(current_day_half);
    request.setNewDayHalf(day_half_indicator);

    request.setOldFirstDayHalf(current_first_day_half);
    request.setNewFirstDayHalf(updating_first_day_half);

    request.setOldLastDayHalf(current_last_day_half);
    request.setNewLastDayHalf(updating_last_day_half);

    request.setNote(note);

    int originating_op_id = _ops->update_Booking_Requ(request);

    _sql->updateRequest(local_request_id, originating_op_id, current_leave_year, updating_leave_year, updating_fdate, updating_tdate, updating_first_day_half, updating_last_day_half,
            day_half_indicator, note);

    qDebug() << "GGGG updated request in database";

    qDebug() << "GGGG done submitting request";

    showTab(TAB_REQUESTS);
}

void ApplicationUI::onCancelDialogFinished(bb::system::SystemUiResult::Type type)
{
    qDebug() << QString("GGGG onCancelDialogFinished");
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        qDebug() << QString("GGGG onCancelDialogFinished confirmed");

        if (_current_status == STATUS_BOOKING_REJECTED || _current_status == STATUS_FAILED || _current_status == STATUS_CANCELLATION_OK) {
            _sql->deleteRequest(_local_request_id);
            qDebug() << "GGGG done deleting request";
            showTab(TAB_REQUESTS);
        } else {
            if (_current_status == STATUS_BOOKING_PENDING) {
                _sql->deleteRequest(_local_request_id);
                _ops->deleteOutboundOp(_local_request_id, OP_TYPE_SUBMIT_BOOKING_REQUEST);
                qDebug() << "GGGG done deleting request";
                showTab(TAB_REQUESTS);
            } else {
                if (_current_status == STATUS_CANCELLATION_PENDING) {
                    _sql->deleteRequest(_local_request_id);
                    _ops->deleteOutboundOp(_local_request_id, OP_TYPE_CANCELLATION_REQUEST);
                    qDebug() << "GGGG done deleting request";
                    showTab(TAB_REQUESTS);
                } else {
                    _sql->cancelRequest(_local_request_id);
                    DbAnnualLeaveRecord db_record = _sql->getAnnualLeaveRequest(_local_request_id);
                    OpCancelBookingRequ request;
                    request.setLocalRequestId(_local_request_id);
                    request.setLeaveYear(db_record.leaveYear());
                    request.setFromDate(db_record.fromDateS() * 1000);
                    request.setToDate(db_record.toDateS() * 1000);
                    request.setDayHalf(db_record.dayHalf());
                    request.setFirstDayHalf(db_record.firstDayHalf());
                    request.setLastDayHalf(db_record.lastDayHalf());
                    request.setNote(db_record.note());
                    int originating_op_id = _ops->cancel_Booking_Requ(request);
                    _sql->setOriginatingOpId(_local_request_id, originating_op_id);
                    qDebug() << "GGGG done submitting cancellation request with leave_year=" << request.leaveYear() << " from_date=" << request.fromDate() << " to_date=" << request.toDate();
                    showTab(TAB_REQUESTS);
                }
            }
        }

    }

}

void ApplicationUI::onDeleteTaskDialogFinished(bb::system::SystemUiResult::Type type)
{
    qDebug() << QString("GGGG onDeleteTaskDialogFinished");
    if (type == bb::system::SystemUiResult::ConfirmButtonSelection) {
        qDebug() << QString("GGGG onDeleteTaskDialogFinished confirmed");
        _sql->deleteApprovalTask(_task_id);
        qDebug() << "GGGG done deleting task";
        showTab(TAB_INBOX);
        if (isInboxEmpty()) {
            emit signalInboxEmptyChanged();
        }
    }

}

void ApplicationUI::processInboundQueue()
{
    qDebug() << "GGGG processInboundQueue";
    QList<OperationAl*> ops = _ops->inboundQueueOps();
    int num_ops = ops.size();

    qDebug() << "GGGG got " << num_ops << " operations from the inbound queue";

    OpSubmitBookingResp* op_submit_resp;

    OpHalfDaysUsedRequ* op_half_days_used_requ;
    OpHalfDaysUsedResp op_half_days_used_resp;

    OpApprovalOutcomeRequ* op_approval_outcome_requ;

    OpCancelBookingResp* op_cancel_resp;

    OpAnnualEntitlementResp* op_entitlement_resp;

    OpUpdateBookingResp* op_update_resp;

    OpApprovalTaskRequ* op_approval_task_requ;
    OpApprovalTaskResp op_approval_task_resp;

    OpApprovalTaskOutcomeResp* op_approval_task_outcome_resp;

    OpAdapterDetailsRequ* op_adapter_details_requ;
    OpAdapterDetailsResp op_adapter_details_resp;

    OpAdapterStatusResp* op_adapter_status_resp;

    DbAnnualLeaveRecord record;

    bool toast_reqd = false;
    bool zero_days_warning = false;
    bool show_tab_requests = false;
    bool show_tab_inbox = false;

    for (int i = 0; i < num_ops; i++) {

        int task_id = -1;
        int task_status = 0;
        int task_type = -1;
        int current_status = -1;
        int last_op_status = -1;
        int op_type = ops.at(i)->opType();
        int op_id = ops.at(i)->opId();
        int leave_year = -1;
        int total_entitlement = -1;
        int current_entitlement = -1;
        int update_count = 0;
        int fdate = 0;
        int tdate = 0;
        int first_day_half = 0;
        int last_day_half = 0;
        int day_half = 0;
        int new_fdate = 0;
        int new_tdate = 0;

        QDateTime from_year_date;

        qDebug() << "GGGG processInboundQueue got op_id=" << op_id << " op_type=" << op_type;
        switch (op_type) {
            case OP_TYPE_SUBMIT_BOOKING_RESPONSE:
                op_submit_resp = dynamic_cast<OpSubmitBookingResp*>(ops.at(i));
                qDebug() << "GGGG OP_TYPE_SUBMIT_BOOKING_RESPONSE op_id=" << op_submit_resp->opId() << " local_request_id=" << op_submit_resp->localRequestId() << " op_status="
                        << op_submit_resp->opStatus() << " request_status=" << op_submit_resp->requestStatus();
                if (op_submit_resp->opStatus() == OP_STATUS_OK) {
                    _sql->updateRequestStatus(op_submit_resp->localRequestId(), op_submit_resp->requestStatus());
                } else {
                }
                toast_reqd = true;
                show_tab_requests = true;
                break;
            case OP_TYPE_HALF_DAYS_USED_REQUEST:
                op_half_days_used_requ = dynamic_cast<OpHalfDaysUsedRequ*>(ops.at(i));
                qDebug() << "GGGG OP_TYPE_HALF_DAYS_USED_REQUEST op_id=" << op_half_days_used_requ->opId() << " op_type=" << op_type << " local_request_id=" << op_half_days_used_requ->localRequestId()
                        << " half_days=" << op_half_days_used_requ->halfDays();
                _sql->updateHalfDays(op_half_days_used_requ->localRequestId(), op_half_days_used_requ->halfDays());
                op_half_days_used_resp.setLocalRequestId(op_half_days_used_requ->localRequestId());
                op_half_days_used_resp.setOpStatus(OP_STATUS_OK);
                _ops->half_days_used_Resp(op_half_days_used_resp);
                record = _sql->getAnnualLeaveRequest(op_half_days_used_requ->localRequestId());
                requestEntitlementData(record.leaveYear());
                if (op_half_days_used_requ->halfDays() == 0) {
                    zero_days_warning = true;
                }
                break;
            case OP_TYPE_APPROVAL_OUTCOME_REQUEST:
                op_approval_outcome_requ = dynamic_cast<OpApprovalOutcomeRequ*>(ops.at(i));
                task_type = op_approval_outcome_requ->taskType();
                qDebug() << "GGGG OP_TYPE_APPROVAL_OUTCOME_REQUEST op_id=" << op_approval_outcome_requ->opId() << " op_type=" << op_type << " local_request_id="
                        << op_approval_outcome_requ->localRequestId() << " request_status=" << op_approval_outcome_requ->requestStatus() << " task_type=" << task_type << " originating_op_id="
                        << op_approval_outcome_requ->originatingOpId() << " half_days=" << op_approval_outcome_requ->halfDays();
                switch (task_type) {
                    case TASK_TYPE_BOOKING_APPROVAL:
                        processBookingApprovalOutcome(op_approval_outcome_requ);
                        break;
                    case TASK_TYPE_CANCEL_APPROVAL:
                        processCancellationApprovalOutcome(op_approval_outcome_requ);
                        break;
                    case TASK_TYPE_UPDATE_APPROVAL:
                        processUpdateApprovalOutcome(op_approval_outcome_requ);
                        break;
                }
                toast_reqd = true;
                record = _sql->getAnnualLeaveRequest(op_approval_outcome_requ->localRequestId());
                requestEntitlementData(record.leaveYear());
                show_tab_requests = true;
                break;
            case OP_TYPE_APPROVAL_TASK_REQUEST:
                op_approval_task_requ = dynamic_cast<OpApprovalTaskRequ*>(ops.at(i));
                qDebug() << "GGGG OP_TYPE_APPROVAL_TASK_REQUEST op_id=" << op_approval_task_requ->opId() << " op_type=" << op_type << " user_id=" << op_approval_task_requ->userId();

                fdate = op_approval_task_requ->fromDate() / 1000;
                tdate = op_approval_task_requ->toDate() / 1000;
                new_fdate = op_approval_task_requ->newFromDate() / 1000;
                new_tdate = op_approval_task_requ->newToDate() / 1000;

                task_id = _sql->storeApprovalTask(op_approval_task_requ->userId(), op_approval_task_requ->taskType(), fdate, tdate, op_approval_task_requ->leaveYear(),
                        op_approval_task_requ->firstDayHalf(), op_approval_task_requ->lastDayHalf(), op_approval_task_requ->dayHalf(), op_approval_task_requ->halfDays(),
                        op_approval_task_requ->localRequestId(), op_approval_task_requ->note(), new_fdate, new_tdate, op_approval_task_requ->newLeaveYear(), op_approval_task_requ->newFirstDayHalf(),
                        op_approval_task_requ->newLastDayHalf(), op_approval_task_requ->newDayHalf(), op_approval_task_requ->halfDays(), op_approval_task_requ->originatingOpId());

                op_approval_task_resp.setOpStatus(OP_STATUS_OK);
                op_approval_task_resp.setTaskId(task_id);
                op_approval_task_resp.setUserId(op_approval_task_requ->userId());

                _ops->approval_task_Resp(op_approval_task_resp);
                toast_reqd = true;
                _inbox_is_empty = false;
                emit signalInboxEmptyChanged();
                show_tab_inbox = true;
                break;
            case OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE:
                op_approval_task_outcome_resp = dynamic_cast<OpApprovalTaskOutcomeResp*>(ops.at(i));
                qDebug() << "GGGG OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE op_id=" << op_approval_task_outcome_resp->opId() << " task_id=" << op_approval_task_outcome_resp->taskId() << " op_status="
                        << op_approval_task_outcome_resp->opStatus();
                if (op_approval_task_outcome_resp->opStatus() == OP_STATUS_OK) {
                    if (op_approval_task_outcome_resp->opStatus() == 0) {
                        task_status = TASK_STATUS_SUBMITTED;
                    }
                    _sql->updateApprovalTaskProcessingStatus(op_approval_task_outcome_resp->taskId(), task_status);
                } else {
                    qDebug() << "GGGG op_status indicates processing the operation request failed: " << op_approval_task_outcome_resp->opStatus();
                }
                toast_reqd = true;
                show_tab_inbox = true;
                leave_year = _sql->getApprovalTaskYear(op_approval_task_outcome_resp->taskId());
                requestEntitlementData(leave_year);
                break;
            case OP_TYPE_CANCELLATION_RESPONSE:
                op_cancel_resp = dynamic_cast<OpCancelBookingResp*>(ops.at(i));
                qDebug() << "GGGG OP_TYPE_CANCELLATION_RESPONSE op_id=" << op_cancel_resp->opId() << " local_request_id=" << op_cancel_resp->localRequestId() << " op_status="
                        << op_cancel_resp->opStatus() << " request_status=" << op_cancel_resp->requestStatus();
                if (op_cancel_resp->opStatus() == OP_STATUS_OK) {
                    _sql->updateRequestStatus(op_cancel_resp->localRequestId(), op_cancel_resp->requestStatus());
                } else {
                    qDebug() << "GGGG op_status indicates processing the operation request failed: " << op_cancel_resp->opStatus();
                }
                toast_reqd = true;
                show_tab_requests = true;
                record = _sql->getAnnualLeaveRequest(op_cancel_resp->localRequestId());
                requestEntitlementData(record.leaveYear());
                break;
            case OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE:
                op_entitlement_resp = dynamic_cast<OpAnnualEntitlementResp*>(ops.at(i));
                leave_year = op_entitlement_resp->leaveYear();
                total_entitlement = op_entitlement_resp->totalEntitlementHalfDays();
                current_entitlement = op_entitlement_resp->currentEntitlementHalfDays();
                qDebug() << "GGGG OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE op_id=" << op_entitlement_resp->opId() << " op_status=" << op_entitlement_resp->opStatus() << " request_status="
                        << op_entitlement_resp->requestStatus() << " leave_year=" << leave_year << " total_entitlement=" << total_entitlement << " current_entitlement=" << current_entitlement;
                if (op_entitlement_resp->opStatus() == OP_STATUS_OK && op_entitlement_resp->requestStatus() == STATUS_COMPLETED && leave_year > -1) {
                    _sql->updateEntitlement(leave_year, total_entitlement, current_entitlement);
                    setTotalEntitlement(total_entitlement);
                    setCurrentEntitlement(current_entitlement);
                } else {
                    qDebug() << "GGGG op_status and/or request_status indicates processing the operation request failed: op_status=" << op_entitlement_resp->opStatus() << ",request_status="
                            << op_entitlement_resp->requestStatus();
                }
                break;
            case OP_TYPE_UPDATE_BOOKING_RESPONSE:
                op_update_resp = dynamic_cast<OpUpdateBookingResp*>(ops.at(i));
                qDebug() << "GGGG OP_TYPE_UPDATE_BOOKING_RESPONSE op_id=" << op_update_resp->opId() << " local_request_id=" << op_update_resp->localRequestId() << " op_status="
                        << op_update_resp->opStatus() << " request_status=" << op_update_resp->requestStatus();
                if (op_update_resp->opStatus() == OP_STATUS_OK) {
                    _sql->updateRequestStatus(op_update_resp->localRequestId(), op_update_resp->requestStatus());
                } else {
                    qDebug() << "GGGG op_status indicates processing the operation request failed: " << op_update_resp->opStatus();
                }
                toast_reqd = true;
                show_tab_requests = true;
                break;
            case OP_TYPE_ADAPTER_DETAILS_REQUEST:
                op_adapter_details_requ = dynamic_cast<OpAdapterDetailsRequ*>(ops.at(i));
                qDebug() << "GGGG OP_TYPE_ADAPTER_DETAILS_REQUEST op_id=" << op_adapter_details_requ->opId();
                _adapter_name = op_adapter_details_requ->adapterName();
                _adapter_version = op_adapter_details_requ->adapterVersion();
                _adapter_description = op_adapter_details_requ->adapterDescription();
                _adapter_configured = op_adapter_details_requ->isConfigured();
                qDebug() << "GGGG OP_TYPE_ADAPTER_DETAILS_REQUEST adapter_name=" << _adapter_name << " adapter_version=" << _adapter_version << " adapter_description=" << _adapter_description
                        << " _adapter_configured=" << _adapter_configured << " _last_adapter_configured_value=" << _last_adapter_configured_value;
                emit signalAdapterNameChanged();
                emit signalAdapterVersionChanged();
                emit signalAdapterDescriptionChanged();
                emit signalAdapterConfiguredChanged();
                op_adapter_details_resp.setOpStatus(OP_STATUS_OK);
                _ops->adapter_details_Resp(op_adapter_details_resp);
                if (_adapter_configured) {
                    requestEntitlementData(QDate::currentDate().year());
                } else {
                    if (_last_adapter_configured_value != _adapter_configured) {
                        Utilities::toast("Please check application settings - Swipe down to access");
                    }
                }
                _last_adapter_configured_value = _adapter_configured;
                break;
            case OP_TYPE_ADAPTER_STATUS_RESPONSE:
                op_adapter_status_resp = dynamic_cast<OpAdapterStatusResp*>(ops.at(i));
                qDebug() << "GGGG OP_TYPE_ADAPTER_STATUS_RESPONSE op_id=" << op_adapter_status_resp->opId();
                if (op_adapter_status_resp->opStatus() != OP_STATUS_OK) {
                    qDebug() << "GGGG op_status indicates processing the operation request failed: " << op_adapter_status_resp->opStatus();
                }
                break;
            default:
                qDebug() << "GGGG processInboundQueue - invalid op_type:" << op_type;
                break;
        }
        qDebug() << "GGGG deleting inbound operation " << op_id;
        _ops->deleteInboundOp(op_id);

    }

    if (zero_days_warning) {
        Utilities::toast("WARNING: your request was for zero days in real terms");
    }

    if (toast_reqd) {
        Utilities::toast("An update was received");
    }

    if (show_tab_requests) {
        showTab(TAB_REQUESTS);
    }

    if (show_tab_inbox) {
        showTab(TAB_INBOX);
    }

    qDebug() << "GGGG done processInboundQueue";

}

void ApplicationUI::processBookingApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome)
{
    qDebug() << "GGGG processBookingApprovalOutcome";
    OpApprovalOutcomeResp op_approval_outcome_resp;

    int update_count = _sql->countBookingUpdates(approval_outcome->localRequestId(), approval_outcome->originatingOpId());

    op_approval_outcome_resp.setLocalRequestId(approval_outcome->localRequestId());
    op_approval_outcome_resp.setOpStatus(OP_STATUS_OK);

    if (update_count == 1) {
        _sql->updateRequestStatus(approval_outcome->localRequestId(), approval_outcome->requestStatus());
    } else {
        qDebug() << "GGGG approval outcome ignored as original booking has been updated again since this approval was requested";
    }
    _ops->approval_outcome_Resp(op_approval_outcome_resp);

}

void ApplicationUI::processCancellationApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome)
{
    qDebug() << "GGGG processCancellationApprovalOutcome";
    OpApprovalOutcomeResp op_approval_outcome_resp;
    DbAnnualLeaveRecord record;

    int last_op_status;

    int update_count = _sql->countBookingUpdates(approval_outcome->localRequestId(), approval_outcome->originatingOpId());

    if (update_count == 1) {
        last_op_status = _sql->getLastOpStatus(approval_outcome->localRequestId());
        if (last_op_status == STATUS_CANCELLATION_SUBMITTED) {
            if (approval_outcome->requestStatus() == STATUS_CANCELLATION_OK) {
                // update status to CANCELLED - User can delete completely if they want to
                _sql->updateRequestStatus(approval_outcome->localRequestId(), approval_outcome->requestStatus());
            } else {
                if (approval_outcome->requestStatus() == STATUS_CANCELLATION_REJECTED) {
                    _sql->updateRequestStatus(approval_outcome->localRequestId(), approval_outcome->requestStatus());
                }
            }
        } else {
            qDebug() << "GGGG ignoring OP_TYPE_CANCELLATION_OUTCOME_REQUEST since target request does not have appropriate current status:" << last_op_status;
        }
    } else {
        qDebug() << "GGGG approval outcome ignored as original booking has been updated again since this approval was requested";
    }

    op_approval_outcome_resp.setLocalRequestId(approval_outcome->localRequestId());
    op_approval_outcome_resp.setOpStatus(OP_STATUS_OK);
    _ops->approval_outcome_Resp(op_approval_outcome_resp);
}

void ApplicationUI::processUpdateApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome)
{
    qDebug() << "GGGG processUpdateApprovalOutcome";
    OpApprovalOutcomeResp op_approval_outcome_resp;

    int last_op_status = _sql->getLastOpStatus(approval_outcome->localRequestId());
    qDebug() << "GGGG processUpdateApprovalOutcome last_op_status=" << last_op_status;

    int update_count = _sql->countBookingUpdates(approval_outcome->localRequestId(), approval_outcome->originatingOpId());
    qDebug() << "GGGG processUpdateApprovalOutcome update_count=" << update_count;

    if (last_op_status == STATUS_UPDATE_SUBMITTED) {
        if (approval_outcome->requestStatus() == STATUS_UPDATE_OK) {
            if (update_count == 1) {
                // update booking
                _sql->applyBookingUpdate(approval_outcome->localRequestId(), approval_outcome->halfDays());
                qDebug() << "GGGG update was approved";
            } else {
                qDebug() << "GGGG booking record not updated since number of records that match required condition=" << update_count;
            }
        } else {
            // including count==1 in the condition ensures we're updating the status of the annual_leave_request record only if the rejection applies to the most recently submitted update request
            if (update_count == 1 && approval_outcome->requestStatus() == STATUS_UPDATE_REJECTED) {
                _sql->updateRequestStatus(approval_outcome->localRequestId(), approval_outcome->requestStatus());
                qDebug() << "GGGG update was rejected";
            } else {
                qDebug() << "GGGG booking record not updated since number of records that match required condition=" << update_count;
            }
        }
    } else {
        qDebug() << "GGGG ignoring OP_TYPE_UPDATE_OUTCOME_REQUEST since target request does not have appropriate current status:" << last_op_status;
    }
    op_approval_outcome_resp.setLocalRequestId(approval_outcome->localRequestId());
    op_approval_outcome_resp.setOpStatus(OP_STATUS_OK);
    _ops->approval_outcome_Resp(op_approval_outcome_resp);
}

void ApplicationUI::refreshEntitlement(int leave_year)
{
    qDebug() << "GGGG refreshEntitlement:" << leave_year;
    Entitlement entitlement = _sql->getEntitlement(leave_year);

    setTotalEntitlement(entitlement.totalEntitlementHalfDays());
    setCurrentEntitlement(entitlement.currentEntitlementHalfDays());

    // if we don't have this data cached for the given year, ask the back end for up to date entitlement figures
    if (entitlement.totalEntitlementHalfDays() == -1) {
        requestEntitlementData(leave_year);
    }
}

void ApplicationUI::requestEntitlementData(int leave_year)
{
    qDebug() << "GGGG requestEntitlementData:" << leave_year;
    OpAnnualEntitlementRequ request;
    request.setLeaveYear(leave_year);
    _ops->annual_entitlement_Requ(request);
}

void ApplicationUI::requestAdapterStatus()
{
    qDebug() << "GGGG ApplicationUI::requestAdapterStatus";
    OpAdapterStatusRequ request;
    _ops->adapter_status_Requ(request);
}

void ApplicationUI::showTab(int tab_index)
{
    switch (tab_index) {
        case TAB_REQUESTS:
            QMetaObject::invokeMethod(_root, "refreshRequests");
            break;
        case TAB_INBOX:
            QMetaObject::invokeMethod(_root, "refreshTasks");
            break;
    }
    QMetaObject::invokeMethod(_root, "onShowTab", Q_ARG(QVariant, tab_index));
}

void ApplicationUI::showRequests(int year)
{
    QMetaObject::invokeMethod(_root, "refreshRequestsForYear", Q_ARG(QVariant, year));
    QMetaObject::invokeMethod(_root, "onShowTab", Q_ARG(QVariant, TAB_REQUESTS));
}

void ApplicationUI::setSimulation(bool state)
{
    Settings* settings = Settings::getInstance(this);
    settings->saveValueFor(SETTINGS_KEY_SIMULATION_ENABLED, QString::number(state));
}

void ApplicationUI::setServiceVerbose(bool state)
{
    Settings* settings = Settings::getInstance(this);
    settings->saveValueFor(SETTINGS_KEY_SERVICE_VERBOSE, QString::number(state));
}

void ApplicationUI::addCalendarEntry(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, int day_half, QString note)
{
    qDebug() << "GGGG addCalendarEntry: from_date=" << from_date << " to_date=" << to_date << " first_day_half=" << first_day_half << " last_day_half=" << last_day_half << " day_half=" << day_half;

    // Create the calendar service object
    CalendarService calendarService;

    // Retrieve the IDs for the default calendar
    QPair<AccountId, FolderId> defaultCalendar = calendarService.defaultCalendarFolder();

    // Create the calendar event and set its IDs
    CalendarEvent event;
    event.setAccountId(defaultCalendar.first);
    event.setFolderId(defaultCalendar.second);

    if (note.length() > 0) {
        event.setSubject(note);
    } else {
        event.setSubject("Holiday");
    }

    event.setStartTime(from_date);
    event.setEndTime(to_date);
    event.setAllDay(true);

    // no half days
    if (!first_day_half && !last_day_half) {
        // make sure we include the last day in the range
        to_date = to_date.addDays(1);
        event.setEndTime(to_date);
        if (calendarService.createEvent(event) == Result::Success) {
            Utilities::alert("Event added to calendar");
        } else {
            Utilities::alert("Error: Event could not be added to calendar");
        }
        return;
    }

    // just a single half day
    if (first_day_half && (from_date == to_date)) {
        event.setAllDay(false);
        if (day_half == DAY_HALF_MORNING) {
            from_date = from_date.addSecs(9 * 60 * 60);
            to_date = to_date.addSecs(12 * 60 * 60);
        } else {
            from_date = from_date.addSecs(12 * 60 * 60);
            to_date = to_date.addSecs(17 * 60 * 60);
        }
        event.setStartTime(from_date);
        event.setEndTime(to_date);
        if (calendarService.createEvent(event) == Result::Success) {
            Utilities::alert("Event added to calendar");
        } else {
            Utilities::alert("Error: Event could not be added to calendar");
        }
        return;
    }

    // starts on a half day - must be the afternoon then - deal with the first day only to begin with
    if (first_day_half) {
        event.setAllDay(false);
        QDateTime start_at = from_date.addSecs(12 * 60 * 60);
        QDateTime end_at = from_date.addSecs(17 * 60 * 60);
        event.setStartTime(start_at);
        event.setEndTime(end_at);
        if (calendarService.createEvent(event) == Result::Success) {
        } else {
            Utilities::alert("Error: Event could not be added to calendar");
            return;
        }
    }

    // ends on a half day - must be the morning then - deal with the last day only here
    if (last_day_half) {
        event.setAllDay(false);
        QDateTime start_at = to_date.addSecs(9 * 60 * 60);
        QDateTime end_at = to_date.addSecs(12 * 60 * 60);
        event.setStartTime(start_at);
        event.setEndTime(end_at);
        if (calendarService.createEvent(event) == Result::Success) {
        } else {
            Utilities::alert("Error: Event could not be added to calendar");
            return;
        }
    }

    // create full day events in between the first and last days

    if (from_date < to_date) {
        if (first_day_half) {
            from_date = from_date.addDays(1);
        }
        if (!last_day_half) {
            to_date = to_date.addDays(1);
        }
        event.setAllDay(true);
        event.setStartTime(from_date);
        event.setEndTime(to_date);
        if (calendarService.createEvent(event) == Result::Success) {
        } else {
            Utilities::alert("Error: Event could not be added to calendar");
            return;
        }
    }

    Utilities::alert("Event added to calendar");

}

int ApplicationUI::totalEntitlement()
{
    return _total_entitlement;
}

void ApplicationUI::setTotalEntitlement(int total_entitlement)
{
    _total_entitlement = total_entitlement;
    emit signalTotalEntitlementChanged();
}

int ApplicationUI::currentEntitlement()
{
    return _current_entitlement;
}

void ApplicationUI::setCurrentEntitlement(int current_entitlement)
{
    _current_entitlement = current_entitlement;
    emit signalCurrentEntitlementChanged();
}

void ApplicationUI::clearProcessingStatus(int local_request_id, int leave_year)
{
    qDebug() << "GGGG clearProcessingStatus: local_request_id=" << local_request_id << " leave_year=" << leave_year;
    _sql->clearProcessingStatus(local_request_id, leave_year);
}

QString ApplicationUI::toLocalDateFormat(QString sqlite_format_date_string)
{
    return Utilities::toLocalDateFormat(sqlite_format_date_string);
}

QString ApplicationUI::statusText(int booking_status, int last_op_status)
{
    return Utilities::statusText(booking_status, last_op_status);
}

QString ApplicationUI::lastOpDesc(int last_op_status)
{
    return Utilities::statusName(last_op_status);
}

QString ApplicationUI::statusIcon(int booking_status, int last_op_status)
{
    return Utilities::statusIcon(booking_status, last_op_status);
}

QString ApplicationUI::taskText(int task_status, int decision, QString user_id, int half_days, QString from_date)
{
    return Utilities::taskText(task_status, decision, user_id, half_days, toLocalDateFormat(from_date));
}

QString ApplicationUI::taskIcon(int task_type, int task_status, int decision)
{
    return Utilities::taskIcon(task_type, task_status, decision);
}

QString ApplicationUI::taskStatus(int task_status, int decision)
{
    return Utilities::taskStatus(task_status, decision);
}

QString ApplicationUI::taskDecision(int decision)
{
    return Utilities::taskDecision(decision);
}

bool ApplicationUI::sameDates(QDateTime datetime1, QDateTime datetime2)
{
    return Utilities::sameDates(datetime1, datetime2);

}

QString ApplicationUI::requestParamsInterpretation(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half)
{
    qDebug() << "GGGG requestParamsInterpretation: from_date=" << from_date.date() << " to_date=" << to_date.date() << " first_day_half=" << first_day_half << " last_day_half=" << last_day_half
            << " day_half=" << day_half;

    QDate start_date = from_date.date();
    QDate end_date = to_date.date();
    int half_of_day = 0;
    if (!day_half.isNull()) {
        half_of_day = day_half.toInt();
    }

    QString sdate = start_date.toString(Qt::SystemLocaleShortDate);
    QString edate = end_date.toString(Qt::SystemLocaleShortDate);

    return interpret(sdate, edate, first_day_half, last_day_half, half_of_day);
}

QString ApplicationUI::approvalParamsInterpretation(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half)
{
    qDebug() << "GGGG approvalParamsInterpretation: from_date=" << from_date << " to_date=" << to_date << " first_day_half=" << first_day_half << " last_day_half=" << last_day_half << " day_half="
            << day_half;

    return interpret(toLocalDateFormat(from_date), toLocalDateFormat(to_date), first_day_half, last_day_half, day_half);
}

QString ApplicationUI::interpret(QString from_date, QString to_date, bool first_day_half, bool last_day_half, int day_half)
{
    QString interpretation("");
    if (from_date.compare(to_date) == 0) {
        interpretation.append(from_date);
        if (first_day_half) {
            if (day_half == DAY_HALF_MORNING) {
                interpretation.append(" in the morning ");
            } else if (day_half == DAY_HALF_AFTERNOON) {
                interpretation.append(" in the afternoon ");

            }
        }
        interpretation.append(" only");
    } else {
        interpretation.append(from_date).append(" to ").append(to_date).append(" inclusive");
        if (first_day_half) {
            interpretation.append(", starting in the afternoon");
        } else {
            if (last_day_half) {
                interpretation.append(", ending in the morning");
            }
        }
    }
    return interpretation;
}

QString ApplicationUI::taskType(int task_status)
{
    switch (task_status) {
        case TASK_TYPE_BOOKING_APPROVAL:
            return "Booking";
            break;
        case TASK_TYPE_CANCEL_APPROVAL:
            return "Cancellation";
            break;
        case TASK_TYPE_UPDATE_APPROVAL:
            return "Update";
            break;
        default:
            return "UNKNOWN TASK TYPE";
    }
}

QString ApplicationUI::datesText(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half)
{

    qDebug() << "GGGG datesText: from_date=" << from_date << " to_date=" << to_date;

    QString text("");

    if (from_date.compare(to_date) != 0) {
        return QString("%1 to %2").arg(toLocalDateFormat(from_date)).arg(toLocalDateFormat(to_date));
    } else {
        return toLocalDateFormat(from_date);
    }

}

QString ApplicationUI::updatingText(int last_op_status, QString updating_from_date, QString updating_to_date, int updating_first_day_half, int updating_last_day_half, int updating_day_half)
{
    qDebug() << "GGGG updatingText: last_op_status=" << last_op_status << " updating_from_date=" << updating_from_date << " updating_to_date=" << updating_to_date << " updating_first_day_half="
            << updating_first_day_half << " updating_last_day_half=" << updating_last_day_half << " updating_day_half=" << updating_day_half;

    QString text("");

    if (last_op_status != STATUS_UPDATE_PENDING && last_op_status != STATUS_UPDATE_SUBMITTED) {
        return text;
    }

    text.append("Updating to ").append(
            interpret(toLocalDateFormat(updating_from_date), toLocalDateFormat(updating_to_date), (updating_first_day_half == 1), (updating_last_day_half == 1), updating_day_half));

    return text;

}

void ApplicationUI::stopHeadlessService()
{
    qDebug() << "GGGG stopHeadlessService";

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_STOP_SERVICE);
    _invokeManager->invoke(invoke_request);
}

void ApplicationUI::startHeadlessService()
{
    qDebug() << "GGGG startHeadlessService";

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_START);
    _invokeManager->invoke(invoke_request);
}

void ApplicationUI::resetAdapterSettings()
{
    qDebug() << "GGGG resetAdapterSettings";

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_RESET_ADAPTER);
    _invokeManager->invoke(invoke_request);
}

void ApplicationUI::submitApprovalDecision(int task_id, int task_type, int local_request_id, int decision, int originating_op_id)
{
    qDebug() << "GGGG submitApprovalDecision task_id=" << task_id << " task_type=" << task_type << " local_request_id=" << local_request_id << " decision=" << decision << " originating_op_id="
            << originating_op_id;
    OpApprovalTaskOutcomeRequ request;
    request.setLocalRequestId(local_request_id);
    request.setTaskId(task_id);
    request.setOriginatingOpId(originating_op_id);
    request.setTaskType(task_type);
    switch (task_type) {
        case (TASK_TYPE_BOOKING_APPROVAL):
            switch (decision) {
                case TASK_APPROVAL_DECISION_APPROVED:
                    request.setRequestStatus(STATUS_BOOKING_APPROVED);
                    break;
                case TASK_APPROVAL_DECISION_REJECTED:
                    request.setRequestStatus(STATUS_BOOKING_REJECTED);
                    break;
                default:
                    qDebug() << "GGGG ERROR: submitApprovalDecision task_id=" << task_id << " invalid decision=" << decision;
                    return;
            }
            break;
        case (TASK_TYPE_CANCEL_APPROVAL):
            switch (decision) {
                case TASK_APPROVAL_DECISION_APPROVED:
                    request.setRequestStatus(STATUS_CANCELLATION_OK);
                    break;
                case TASK_APPROVAL_DECISION_REJECTED:
                    request.setRequestStatus(STATUS_CANCELLATION_REJECTED);
                    break;
                default:
                    qDebug() << "GGGG ERROR: submitApprovalDecision task_id=" << task_id << " invalid decision=" << decision;
                    return;
            }
            break;

        case (TASK_TYPE_UPDATE_APPROVAL):
            switch (decision) {
                case TASK_APPROVAL_DECISION_APPROVED:
                    request.setRequestStatus(STATUS_UPDATE_OK);
                    break;
                case TASK_APPROVAL_DECISION_REJECTED:
                    request.setRequestStatus(STATUS_UPDATE_REJECTED);
                    break;
                default:
                    qDebug() << "GGGG ERROR: submitApprovalDecision task_id=" << task_id << " invalid decision=" << decision;
                    return;

            }
            break;
    }
    _sql->updateApprovalTask(task_id, decision);
    _ops->approval_task_outcome_Requ(request);
    qDebug() << "GGGG submitApprovalDecision:done";

}

QString ApplicationUI::adapterName()
{
    return _adapter_name;
}

QString ApplicationUI::adapterVersion()
{
    return _adapter_version;
}

QString ApplicationUI::adapterDescription()
{
    return _adapter_description;
}

bool ApplicationUI::adapterConfigured()
{
    return _adapter_configured;
}

QString ApplicationUI::setting1Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting1_name = settings->getValueFor(QString(GUI_SETTING_1_NAME), "");
    return _setting1_name;
}

QString ApplicationUI::setting1Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting1_value = settings->getValueFor(QString(GUI_SETTING_1_VALUE), "");
    return _setting1_value;
}

void ApplicationUI::setSetting1Value(QString setting_value)
{
    qDebug() << "GGGG setting value 1:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting1_value = setting_value;
    settings->saveValueFor(GUI_SETTING_1_VALUE, setting_value);
    emit signalSetting1ValueChanged();
}

QString ApplicationUI::setting2Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting2_name = settings->getValueFor(QString(GUI_SETTING_2_NAME), "");
    return _setting2_name;
}

QString ApplicationUI::setting2Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting2_value = settings->getValueFor(QString(GUI_SETTING_2_VALUE), "");
    return _setting2_value;
}

void ApplicationUI::setSetting2Value(QString setting_value)
{
    qDebug() << "GGGG setting value 2:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting2_value = setting_value;
    settings->saveValueFor(GUI_SETTING_2_VALUE, setting_value);
    emit signalSetting2ValueChanged();
}

QString ApplicationUI::setting3Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting3_name = settings->getValueFor(QString(GUI_SETTING_3_NAME), "");
    return _setting3_name;
}

QString ApplicationUI::setting3Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting3_value = settings->getValueFor(QString(GUI_SETTING_3_VALUE), "");
    return _setting3_value;
}

void ApplicationUI::setSetting3Value(QString setting_value)
{
    qDebug() << "GGGG setting value 3:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting3_value = setting_value;
    settings->saveValueFor(GUI_SETTING_3_VALUE, setting_value);
    emit signalSetting3ValueChanged();
}

QString ApplicationUI::setting4Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting4_name = settings->getValueFor(QString(GUI_SETTING_4_NAME), "");
    return _setting4_name;
}

QString ApplicationUI::setting4Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting4_value = settings->getValueFor(QString(GUI_SETTING_4_VALUE), "");
    return _setting4_value;
}

void ApplicationUI::setSetting4Value(QString setting_value)
{
    qDebug() << "GGGG setting value 4:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting4_value = setting_value;
    settings->saveValueFor(GUI_SETTING_4_VALUE, setting_value);
    emit signalSetting4ValueChanged();
}

QString ApplicationUI::setting5Name()
{
    Settings* settings = Settings::getInstance(this);
    _setting5_name = settings->getValueFor(QString(GUI_SETTING_5_NAME), "");
    return _setting5_name;
}

QString ApplicationUI::setting5Value()
{
    Settings* settings = Settings::getInstance(this);
    _setting5_value = settings->getValueFor(QString(GUI_SETTING_5_VALUE), "");
    return _setting5_value;
}

void ApplicationUI::setSetting5Value(QString setting_value)
{
    qDebug() << "GGGG setting value 5:" << setting_value;
    Settings* settings = Settings::getInstance(this);
    _setting5_value = setting_value;
    settings->saveValueFor(GUI_SETTING_5_VALUE, setting_value);
    emit signalSetting5ValueChanged();
}

void ApplicationUI::onSettingsChanged()
{
    qDebug() << "GGGG onSettingsChanged";
    emit signalSetting1NameChanged();
    emit signalSetting1ValueChanged();
    emit signalSetting2NameChanged();
    emit signalSetting2ValueChanged();
    emit signalSetting3NameChanged();
    emit signalSetting3ValueChanged();
    emit signalSetting4NameChanged();
    emit signalSetting4ValueChanged();
    emit signalSetting5NameChanged();
    emit signalSetting5ValueChanged();
}

void ApplicationUI::onIndication()
{
    qDebug() << "GGGG [" << QThread::currentThreadId() << "] GUI has received an indication of inbound messages to process";
    // delete contents of the indicator directory now so that we receive indications for any subsequent incoming messages
    GuiIndicator* indicator = GuiIndicator::getInstance(this);
    indicator->clearIndications();
    processInboundQueue();
}

bool ApplicationUI::isInboxEmpty()
{
    int count = _sql->countTasks();
    _inbox_is_empty = (count == 0);
    return _inbox_is_empty;
}

void ApplicationUI::checkAdapterSettings()
{
    Settings* settings = Settings::getInstance(this);
    QString configured = settings->getValueFor(SETTINGS_KEY_ADAPTER_CONFIGURED, "0");
    (configured.compare("1") == 0) ? _adapter_configured = true : _adapter_configured = false;
    _last_adapter_configured_value = _adapter_configured;
    emit signalAdapterConfiguredChanged();

    qDebug() << "GGGG configured setting=" << configured << ",_adapter_configured=" << _adapter_configured << ",_last_adapter_configured_value=" << _last_adapter_configured_value;
    if (!_adapter_configured) {
        Utilities::toast("Please check application settings - Swipe down to access");
    } else {
        // it's configured but still its details are "Unknown" so request an update from the Adapter

        if (_adapter_name.compare("Unknown") == 0) {
            requestAdapterStatus();
        }
    }
}

void ApplicationUI::connectSocket() {
	qDebug() << "GGGG Connecting socket SIGNAL()s" << endl;
	QObject::connect(_socket, SIGNAL(connected()),
						this, SLOT(onConnected()));

	QObject::connect(_socket, SIGNAL(disconnected()),
						this, SLOT(onDisconnected()));

	QObject::connect(_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
						this, SLOT(onError(QLocalSocket::LocalSocketError)));

	QObject::connect(_socket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
						this, SLOT(onStateChanged(QLocalSocket::LocalSocketState)));

	QObject::connect(_socket, SIGNAL(readyRead()),
						this, SLOT(onReadyRead()));
}

void ApplicationUI::disconnectSocket() {
	qDebug() << "GGGG Disconnecting socket SIGNAL()s" << endl;
	QObject::disconnect(_socket, SIGNAL(connected()),
			               this, SLOT(onConnected()));

	QObject::disconnect(_socket, SIGNAL(disconnected()),
			               this, SLOT(onDisconnected()));

	QObject::disconnect(_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
			               this, SLOT(onError(QLocalSocket::LocalSocketError)));

	QObject::disconnect(_socket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
			               this, SLOT(onStateChanged(QLocalSocket::LocalSocketState)));
}

void ApplicationUI::onConnected() {
	qDebug() << "GGGG Connected to server" << endl;
	_connectedToServer = true;
	sendToHeadless("*** Message from GUI: Connected to Server ***");
}

void ApplicationUI::onDisconnected() {
	qDebug() << "GGGG Disconnected from server" << endl;
	_connectedToServer = false;
	_socketRetryInterval = HEADLESS_SOCKET_SERVER_RETRY_INTERVAL_MS;
	_socket->close();
}

void ApplicationUI::onError(QLocalSocket::LocalSocketError socketError) {
	qDebug() << "GGGG Socket error from server" << socketError << endl;
    if (!_connectedToServer) {
        qDebug() << "GGGG Socket not connected so retrying" << endl;
        triggerRetryConnection();
    }
}

void ApplicationUI::triggerRetryConnection() {
    qDebug() << "GGGG Socket retry scheduled for " << _socketRetryInterval << "mseconds" << endl;
    // Timeout is to give time for Headless socket server to start
    QTimer::singleShot(_socketRetryInterval, this, SLOT(onTimeout()));
}

void ApplicationUI::onTimeout() {
    qDebug() << "GGGG retry timeout expiry - retrying connection to socket server" << endl;
    _socketRetryInterval *= 2;
    if (_socketRetryInterval > HEADLESS_SOCKET_SERVER_MAX_RETRY_INTERVAL_MS) {
        _socketRetryInterval = HEADLESS_SOCKET_SERVER_MAX_RETRY_INTERVAL_MS;
    }
    connectToHeadless();
}

void ApplicationUI::onStateChanged(QLocalSocket::LocalSocketState socketState) {
	qDebug() << "GGGG Socket state changed" << socketState << endl;
}

void ApplicationUI::connectToHeadless() {
	if (!_connectedToServer) {
		qDebug() << "GGGG Connecting to Headless" << endl;
		_socket->connectToServer(ANNUAL_LEAVE_HEADLESS_SOCKET_SERVER_NAME);
	} else {
		qDebug() << "GGGG Server connected - unable to connect" << endl;
	}
}

void ApplicationUI::disconnectFromHeadless() {
	if (_connectedToServer) {
		qDebug() << "GGGG Disconnecting from Headless" << endl;
		_socket->disconnectFromServer();
	}
}

void ApplicationUI::sendToHeadless(const QVariant &text) {
	if (_connectedToServer) {
		qDebug() << "GGGG sending to headless " << text.toString() << endl;
		_socket->write(text.toByteArray());
		_socket->flush();
	} else {
		qDebug() << "GGGG Server not connected unable to send " << text.toString() << endl;
	}
}

void ApplicationUI::onReadyRead() {
	QByteArray readData = _socket->readAll();
	qDebug() << "GGGG Read: " << readData.constData() << endl;
}
