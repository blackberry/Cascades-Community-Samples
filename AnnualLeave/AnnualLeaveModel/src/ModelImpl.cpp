#include "ModelImpl.hpp"
#include "UserSqlDataSource.hpp"
#include "GuiIndicator.hpp"
#include "Settings.hpp"
#include <bb/pim/calendar/CalendarService>
#include <bb/pim/calendar/CalendarEvent>
#include <bb/pim/calendar/CalendarFolder>

ModelImpl *ModelImpl::_instance;

using namespace bb::pim::calendar;

ModelImpl::ModelImpl(QObject *parent) :
        QObject(parent)
, _last_adapter_configured_value(true)
, _invokeManager(new bb::system::InvokeManager(this))
, _adapter_name(QString(""))
, _adapter_version(QString(""))
, _adapter_description(QString(""))
, _adapter_configured(false)
, _total_entitlement(-1)
, _current_entitlement(-1)
, _socket(new QLocalSocket(this))
, _connectedToServer(false)
, _socketRetryInterval(HEADLESS_SOCKET_SERVER_RETRY_INTERVAL_MS)
{
    qDebug() << "MMMM ModelImpl::AdapterImpl()" << endl;

    _sql = UserSqlDataSource::getInstance(this);
    _ops = Operations::getInstance(this);

    int conn_res = QObject::connect(_sql, SIGNAL(signalRequestsLoaded(const QVariantList &)), this, SIGNAL(signalRequestsLoaded(const QVariantList &)));
    conn_res = QObject::connect(_sql, SIGNAL(signalTasksLoaded(const QVariantList &)), this, SIGNAL(signalTasksLoaded(const QVariantList &)));

    // getting instance of GuiIndicator ensures the indicator directory has been created before we watch it
    GuiIndicator* indicator = GuiIndicator::getInstance(this);
    if (!QObject::connect(indicator, SIGNAL(signalInboundOperation()), this, SLOT(onIndication()), Qt::QueuedConnection)) {
        qWarning() << "MMMM onIndication - connect failed " << strerror(errno) << endl;
    }

    indicator->clearIndications();
    startHeadlessService();
    connectToHeadless();
    connectSocket();

}

ModelImpl::~ModelImpl()
{
    qDebug() << "MMMM ~ModelImpl()" << endl;
    _instance = NULL;

    if (_socket) {
        disconnectSocket();
        _connectedToServer = false;
        _socket->close();
        _socket->deleteLater();
    }

}

ModelImpl *ModelImpl::getInstance(QObject *parent)
{
    qDebug() << "MMMM ModelImpl::getInstance() - _instance " << _instance << endl;
    if (_instance == 0) {
        _instance = new ModelImpl(parent);
    }
    return _instance;
}

// Start -- API from IModel

void ModelImpl::submitBookingRequest(int leave_year, QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half, QString note)
{

    // store in user database and receive local_request_id in response

    int half_of_the_day = 0;
    if (Utilities::sameDates(from_date, to_date)) {
        half_of_the_day = day_half.toInt();
    }

    int fdate_s = Utilities::toDateOnlyS(from_date);
    int tdate_s = Utilities::toDateOnlyS(to_date);

    qDebug() << "MMMM ApplicationUI::submitBookingRequest:  fdate_s=" << fdate_s << " tdate_s=" << tdate_s;

    // make sure "half day bookings" indicated with both first_day_half and last_day_half set to true
    if (fdate_s == tdate_s) {
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

    int local_request_id = _sql->storeRequest(fdate_s, tdate_s, note, leave_year, first_day_half, last_day_half, half_of_the_day, -1);

    qDebug() << "MMMM stored request and got ID:" << local_request_id;

    // submit request using the operations API
    OpSubmitBookingRequ request;
    request.setLocalRequestId(local_request_id);
    request.setLeaveYear(leave_year);
    request.setFromDate(fdate_s);
    request.setToDate(tdate_s);
    request.setFirstDayHalf(first_day_half);
    request.setLastDayHalf(last_day_half);
    request.setDayHalf(half_of_the_day);
    request.setNote(note);

    int originating_op_id = _ops->submit_Booking_Requ(request);
    _sql->setOriginatingOpId(local_request_id, originating_op_id);

}

void ModelImpl::updateBookingRequest(int local_request_id, int current_leave_year, QDate current_from_date, QDateTime updating_from_date, QDate current_to_date, QDateTime updating_to_date,
        int current_first_day_half, bool updating_first_day_half, int current_last_day_half, bool updating_last_day_half, int current_day_half, QVariant updating_day_half, QString note)
{

    int updating_leave_year = updating_to_date.date().year();
    int updating_fdate_s = Utilities::toDateOnlyS(updating_from_date);
    int updating_tdate_s = Utilities::toDateOnlyS(updating_to_date);

    int day_half_indicator = 0;
    if (Utilities::sameDates(updating_from_date, updating_to_date)) {
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

    request.setOldFromDate(Utilities::toSeconds(current_from_date));
    request.setNewFromDate(Utilities::toDateOnlyS(updating_from_date));

    request.setOldToDate(Utilities::toSeconds(current_to_date));
    request.setNewToDate(Utilities::toDateOnlyS(updating_to_date));

    request.setOldDayHalf(current_day_half);
    request.setNewDayHalf(day_half_indicator);

    request.setOldFirstDayHalf(current_first_day_half);
    request.setNewFirstDayHalf(updating_first_day_half);

    request.setOldLastDayHalf(current_last_day_half);
    request.setNewLastDayHalf(updating_last_day_half);

    request.setNote(note);

    int originating_op_id = _ops->update_Booking_Requ(request);

    _sql->updateRequest(local_request_id, originating_op_id, current_leave_year, updating_leave_year, updating_fdate_s, updating_tdate_s, updating_first_day_half, updating_last_day_half,
            day_half_indicator, note);

    qDebug() << "MMMM updated request in database";

}

void ModelImpl::cancelBookingRequest(int status, int local_request_id)
{
    if (status == STATUS_BOOKING_REJECTED || status == STATUS_FAILED || status == STATUS_CANCELLATION_OK) {
        _sql->deleteRequest(local_request_id);
        qDebug() << "MMMM done deleting request";
    } else {
        if (status == STATUS_BOOKING_PENDING) {
            _sql->deleteRequest(local_request_id);
            _ops->deleteOutboundOp(local_request_id, OP_TYPE_SUBMIT_BOOKING_REQUEST);
            qDebug() << "MMMM done deleting request";
        } else {
            if (status == STATUS_CANCELLATION_PENDING) {
                _sql->deleteRequest(local_request_id);
                _ops->deleteOutboundOp(local_request_id, OP_TYPE_CANCELLATION_REQUEST);
                qDebug() << "MMMM done deleting request";
            } else {
                _sql->cancelRequest(local_request_id);
                DbAnnualLeaveRecord db_record = _sql->getAnnualLeaveRequest(local_request_id);
                OpCancelBookingRequ request;
                request.setLocalRequestId(local_request_id);
                request.setLeaveYear(db_record.leaveYear());
                request.setFromDate(db_record.fromDateS());
                request.setToDate(db_record.toDateS());
                request.setDayHalf(db_record.dayHalf());
                request.setFirstDayHalf(db_record.firstDayHalf());
                request.setLastDayHalf(db_record.lastDayHalf());
                request.setNote(db_record.note());
                int originating_op_id = _ops->cancel_Booking_Requ(request);
                _sql->setOriginatingOpId(local_request_id, originating_op_id);
                qDebug() << "MMMM done submitting cancellation request with leave_year=" << request.leaveYear() << " from_date=" << request.fromDate() << " to_date=" << request.toDate();
            }
        }
    }

}

void ModelImpl::submitApprovalDecision(int task_id, int task_type, int local_request_id, int decision, int originating_op_id)
{
    qDebug() << "MMMM submitApprovalDecision task_id=" << task_id << " task_type=" << task_type << " local_request_id=" << local_request_id << " decision=" << decision << " originating_op_id="
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
                    qDebug() << "MMMM ERROR: submitApprovalDecision task_id=" << task_id << " invalid decision=" << decision;
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
                    qDebug() << "MMMM ERROR: submitApprovalDecision task_id=" << task_id << " invalid decision=" << decision;
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
                    qDebug() << "MMMM ERROR: submitApprovalDecision task_id=" << task_id << " invalid decision=" << decision;
                    return;

            }
            break;
    }
    _sql->updateApprovalTask(task_id, decision);
    _ops->approval_task_outcome_Requ(request);
    qDebug() << "MMMM submitApprovalDecision:done";

}

void ModelImpl::deleteApprovalTask(int task_id)
{
    _sql->deleteApprovalTask(task_id);
}

void ModelImpl::addCalendarEntry(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, int day_half, QString note)
{

    qDebug() << "MMMM addCalendarEntry: from_date=" << from_date << " to_date=" << to_date << " first_day_half=" << first_day_half << " last_day_half=" << last_day_half << " day_half=" << day_half;

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

void ModelImpl::refreshEntitlement(int leave_year)
{
    qDebug() << "MMMM refreshEntitlement:" << leave_year;
    Entitlement entitlement = _sql->getEntitlement(leave_year);

    setTotalEntitlement(entitlement.totalEntitlementHalfDays());
    setCurrentEntitlement(entitlement.currentEntitlementHalfDays());

    // if we don't have this data cached for the given year, ask the back end for up to date entitlement figures
    if (entitlement.totalEntitlementHalfDays() == -1) {
        requestEntitlementData(leave_year);
    }
}

void ModelImpl::setServiceVerbose(bool state)
{

}

void ModelImpl::clearProcessingStatus(int local_request_id, int leave_year)
{
    qDebug() << "MMMM clearProcessingStatus: local_request_id=" << local_request_id << " leave_year=" << leave_year;
    _sql->clearProcessingStatus(local_request_id, leave_year);
}

QString ModelImpl::statusText(int booking_status, int last_op_status)
{
    return QString("");
}

QString ModelImpl::statusIcon(int booking_status, int last_op_status)
{
    return QString("");
}

QString ModelImpl::lastOpDesc(int last_op_status)
{
    return QString("");
}

QString ModelImpl::taskStatus(int task_status, int decision)
{
    return QString("");
}

QString ModelImpl::taskText(int task_status, int decision, QString user_id, int half_days, QString from_date)
{
    return QString("");
}

QString ModelImpl::taskType(int task_status)
{
    return QString("");
}

QString ModelImpl::taskIcon(int task_type, int task_status, int decision)
{
    return QString("");
}

QString ModelImpl::taskDecision(int decision)
{
    return QString("");
}

QString ModelImpl::requestParamsInterpretation(QDateTime from_date, QDateTime to_date, bool first_day_half, bool last_day_half, QVariant day_half)
{
    qDebug() << "MMMM requestParamsInterpretation: from_date=" << from_date.date() << " to_date=" << to_date.date() << " first_day_half=" << first_day_half << " last_day_half=" << last_day_half
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

QString ModelImpl::approvalParamsInterpretation(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half)
{
    qDebug() << "MMMM approvalParamsInterpretation: from_date=" << from_date << " to_date=" << to_date << " first_day_half=" << first_day_half << " last_day_half=" << last_day_half << " day_half="
            << day_half;

    return interpret(Utilities::toLocalDateFormat(from_date), Utilities::toLocalDateFormat(to_date), first_day_half, last_day_half, day_half);
}

QString ModelImpl::datesText(QString from_date, QString to_date, int first_day_half, int last_day_half, int day_half)
{
    return QString("");
}

QString ModelImpl::updatingText(int last_op_status, QString updating_from_date, QString updating_to_date, int updating_first_day_half, int updating_last_day_half, int updating_day_half)
{
    qDebug() << "MMMM updatingText: last_op_status=" << last_op_status << " updating_from_date=" << updating_from_date << " updating_to_date=" << updating_to_date << " updating_first_day_half="
            << updating_first_day_half << " updating_last_day_half=" << updating_last_day_half << " updating_day_half=" << updating_day_half;

    QString text("");

    if (last_op_status != STATUS_UPDATE_PENDING && last_op_status != STATUS_UPDATE_SUBMITTED) {
        return text;
    }

    text.append("Updating to ").append(interpret(Utilities::toLocalDateFormat(updating_from_date), Utilities::toLocalDateFormat(updating_to_date), (updating_first_day_half == 1), (updating_last_day_half == 1), updating_day_half));

    return text;
}

void ModelImpl::stopHeadlessService()
{
    qDebug() << "MMMM stopHeadlessService";
    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_STOP_SERVICE);
    _invokeManager->invoke(invoke_request);
}

void ModelImpl::startHeadlessService()
{
    qDebug() << "MMMM startHeadlessService";
    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_START);
    _invokeManager->invoke(invoke_request);
}

void ModelImpl::resetAdapterSettings()
{
    qDebug() << "MMMM resetAdapterSettings";

    bb::system::InvokeRequest invoke_request;
    invoke_request.setTarget(TARGET_AL_SERVICE);
    invoke_request.setAction(TARGET_AL_ACTION_RESET_ADAPTER);
    _invokeManager->invoke(invoke_request);
}

void ModelImpl::checkAdapterSettings()
{
    Settings* settings = Settings::getInstance(this);
    QString configured = settings->getValueFor(SETTINGS_KEY_ADAPTER_CONFIGURED, "0");
    (configured.compare("1") == 0) ? _adapter_configured = true : _adapter_configured = false;
    _last_adapter_configured_value = _adapter_configured;
    emit adapterConfiguredChanged();
    qDebug() << "MMMM configured setting=" << configured << ",_adapter_configured=" << _adapter_configured << ",_last_adapter_configured_value=" << _last_adapter_configured_value;
    if (!_adapter_configured) {
        Utilities::toast("Please check application settings - Swipe down to access");
    } else {
        // it's configured but still its details are "Unknown" so request an update from the Adapter
        if (_adapter_name.compare("Unknown") == 0) {
            requestAdapterStatus();
        }
    }
}

void ModelImpl::requestAdapterStatus()
{
    qDebug() << "MMMM ApplicationUI::requestAdapterStatus";
    OpAdapterStatusRequ request;
    _ops->adapter_status_Requ(request);
}

void ModelImpl::clientInitiatedSync(int leave_year)
{
    OpClientSynchronizeRequ request;
    request.setLeaveYear(leave_year);
    _ops->client_synchronize_Requ(request);
}

bool ModelImpl::isValidDateRange(QDateTime from_date, QDateTime to_date, QString range_name)
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

int ModelImpl::totalEntitlement()
{
    return _total_entitlement;
}

void ModelImpl::setTotalEntitlement(int total_entitlement)
{
    _total_entitlement = total_entitlement;
    emit totalEntitlementChanged();
}

int ModelImpl::currentEntitlement()
{
    return _current_entitlement;
}

void ModelImpl::setCurrentEntitlement(int current_entitlement)
{
    _current_entitlement = current_entitlement;
    emit currentEntitlementChanged();
}

QString ModelImpl::adapterName()
{
    return _adapter_name;
}

QString ModelImpl::adapterVersion()
{
    return _adapter_version;
}

QString ModelImpl::adapterDescription()
{
    return _adapter_description;
}

bool ModelImpl::adapterConfigured()
{
    return _adapter_configured;
}

bool ModelImpl::isInboxEmpty()
{
    int count = _sql->countTasks();
    _inbox_is_empty = (count == 0);
    return _inbox_is_empty;
}

void ModelImpl::processInboundQueue()
{

    qDebug() << "MMMM processInboundQueue";
    QList<OperationAl*> ops = _ops->inboundQueueOps();
    int num_ops = ops.size();

    qDebug() << "MMMM got " << num_ops << " operations from the inbound queue";

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

    OpClientSynchronizeResp* op_client_synchronize_resp;

    OpServerSyncResultRequ* op_server_sync_result_requ;
    OpServerSyncResultResp op_server_sync_result_resp;

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
        int sync_result = 0;

        qDebug() << "MMMM processInboundQueue got op_id=" << op_id << " op_type=" << op_type;
        switch (op_type) {
            case OP_TYPE_SUBMIT_BOOKING_RESPONSE:
                op_submit_resp = dynamic_cast<OpSubmitBookingResp*>(ops.at(i));
                qDebug() << "MMMM OP_TYPE_SUBMIT_BOOKING_RESPONSE op_id=" << op_submit_resp->opId() << " local_request_id=" << op_submit_resp->localRequestId() << " op_status="
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
                qDebug() << "MMMM OP_TYPE_HALF_DAYS_USED_REQUEST op_id=" << op_half_days_used_requ->opId() << " op_type=" << op_type << " local_request_id=" << op_half_days_used_requ->localRequestId()
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
                qDebug() << "MMMM OP_TYPE_APPROVAL_OUTCOME_REQUEST op_id=" << op_approval_outcome_requ->opId() << " op_type=" << op_type << " local_request_id="
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
                show_tab_requests = true;
                break;
            case OP_TYPE_APPROVAL_TASK_REQUEST:
                op_approval_task_requ = dynamic_cast<OpApprovalTaskRequ*>(ops.at(i));
                qDebug() << "MMMM OP_TYPE_APPROVAL_TASK_REQUEST op_id=" << op_approval_task_requ->opId() << " op_type=" << op_type << " user_id=" << op_approval_task_requ->userId();

                fdate = op_approval_task_requ->fromDate();
                tdate = op_approval_task_requ->toDate();
                new_fdate = op_approval_task_requ->newFromDate();
                new_tdate = op_approval_task_requ->newToDate();

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
                emit inboxEmptyChanged();
                emit suggestShowInboxTab();
                show_tab_inbox = true;
                break;
            case OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE:
                op_approval_task_outcome_resp = dynamic_cast<OpApprovalTaskOutcomeResp*>(ops.at(i));
                qDebug() << "MMMM OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE op_id=" << op_approval_task_outcome_resp->opId() << " task_id=" << op_approval_task_outcome_resp->taskId() << " op_status="
                        << op_approval_task_outcome_resp->opStatus();
                if (op_approval_task_outcome_resp->opStatus() == OP_STATUS_OK) {
                    if (op_approval_task_outcome_resp->opStatus() == 0) {
                        task_status = TASK_STATUS_SUBMITTED;
                    }
                    _sql->updateApprovalTaskProcessingStatus(op_approval_task_outcome_resp->taskId(), task_status);
                } else {
                    qDebug() << "MMMM op_status indicates processing the operation request failed: " << op_approval_task_outcome_resp->opStatus();
                }
                show_tab_inbox = true;
                leave_year = _sql->getApprovalTaskYear(op_approval_task_outcome_resp->taskId());
                break;
            case OP_TYPE_CANCELLATION_RESPONSE:
                op_cancel_resp = dynamic_cast<OpCancelBookingResp*>(ops.at(i));
                qDebug() << "MMMM OP_TYPE_CANCELLATION_RESPONSE op_id=" << op_cancel_resp->opId() << " local_request_id=" << op_cancel_resp->localRequestId() << " op_status="
                        << op_cancel_resp->opStatus() << " request_status=" << op_cancel_resp->requestStatus();
                if (op_cancel_resp->opStatus() == OP_STATUS_OK) {
                    _sql->updateRequestStatus(op_cancel_resp->localRequestId(), op_cancel_resp->requestStatus());
                } else {
                    qDebug() << "MMMM op_status indicates processing the operation request failed: " << op_cancel_resp->opStatus();
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
                qDebug() << "MMMM OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE op_id=" << op_entitlement_resp->opId() << " op_status=" << op_entitlement_resp->opStatus() << " request_status="
                        << op_entitlement_resp->requestStatus() << " leave_year=" << leave_year << " total_entitlement=" << total_entitlement << " current_entitlement=" << current_entitlement;
                if (op_entitlement_resp->opStatus() == OP_STATUS_OK && op_entitlement_resp->requestStatus() == STATUS_COMPLETED && leave_year > -1) {
                    _sql->updateEntitlement(leave_year, total_entitlement, current_entitlement);
                    setTotalEntitlement(total_entitlement);
                    setCurrentEntitlement(current_entitlement);
                    show_tab_requests = true;
                } else {
                    qDebug() << "MMMM op_status and/or request_status indicates processing the operation request failed: op_status=" << op_entitlement_resp->opStatus() << ",request_status="
                            << op_entitlement_resp->requestStatus();
                }
                break;
            case OP_TYPE_UPDATE_BOOKING_RESPONSE:
                op_update_resp = dynamic_cast<OpUpdateBookingResp*>(ops.at(i));
                qDebug() << "MMMM OP_TYPE_UPDATE_BOOKING_RESPONSE op_id=" << op_update_resp->opId() << " local_request_id=" << op_update_resp->localRequestId() << " op_status="
                        << op_update_resp->opStatus() << " request_status=" << op_update_resp->requestStatus();
                if (op_update_resp->opStatus() == OP_STATUS_OK) {
                    _sql->updateRequestStatus(op_update_resp->localRequestId(), op_update_resp->requestStatus());
                } else {
                    qDebug() << "MMMM op_status indicates processing the operation request failed: " << op_update_resp->opStatus();
                }
                toast_reqd = true;
                show_tab_requests = true;
                break;
            case OP_TYPE_ADAPTER_DETAILS_REQUEST:
                op_adapter_details_requ = dynamic_cast<OpAdapterDetailsRequ*>(ops.at(i));
                qDebug() << "MMMM OP_TYPE_ADAPTER_DETAILS_REQUEST op_id=" << op_adapter_details_requ->opId();
                _adapter_name = op_adapter_details_requ->adapterName();
                _adapter_version = op_adapter_details_requ->adapterVersion();
                _adapter_description = op_adapter_details_requ->adapterDescription();
                _adapter_configured = op_adapter_details_requ->isConfigured();
                qDebug() << "MMMM OP_TYPE_ADAPTER_DETAILS_REQUEST adapter_name=" << _adapter_name << " adapter_version=" << _adapter_version << " adapter_description=" << _adapter_description
                        << " _adapter_configured=" << _adapter_configured << " _last_adapter_configured_value=" << _last_adapter_configured_value;
                emit adapterNameChanged();
                emit adapterVersionChanged();
                emit adapterDescriptionChanged();
                emit adapterConfiguredChanged();
                op_adapter_details_resp.setOpStatus(OP_STATUS_OK);
                _ops->adapter_details_Resp(op_adapter_details_resp);
                if (_adapter_configured) {
                    requestEntitlementData(QDate::currentDate().year());
                } else {
                    if (_last_adapter_configured_value != _adapter_configured) {
                        emit adapterNotConfigured();
                    }
                }
                _last_adapter_configured_value = _adapter_configured;
                break;
            case OP_TYPE_ADAPTER_STATUS_RESPONSE:
                op_adapter_status_resp = dynamic_cast<OpAdapterStatusResp*>(ops.at(i));
                qDebug() << "MMMM OP_TYPE_ADAPTER_STATUS_RESPONSE op_id=" << op_adapter_status_resp->opId();
                if (op_adapter_status_resp->opStatus() != OP_STATUS_OK) {
                    qDebug() << "MMMM op_status indicates processing the operation request failed: " << op_adapter_status_resp->opStatus();
                }
                break;
            case OP_TYPE_CLIENT_SYNCHRONIZE_RESPONSE:
                op_client_synchronize_resp = dynamic_cast<OpClientSynchronizeResp*>(ops.at(i));
                qDebug() << "MMMM OP_TYPE_CLIENT_SYNCHRONIZE_RESPONSE op_id=" << op_client_synchronize_resp->opId() << " op_status=" << op_client_synchronize_resp->opStatus();
                if (op_client_synchronize_resp->opStatus() == OP_STATUS_NOT_SUPPORTED) {
                    emit operationNotSupported();
                }
                break;
            case OP_TYPE_SERVER_SYNC_RESULT_REQUEST:
                op_server_sync_result_requ = dynamic_cast<OpServerSyncResultRequ*>(ops.at(i));
                qDebug() << "MMMM OP_TYPE_SERVER_SYNC_RESULT_REQUEST op_id=" << op_server_sync_result_requ->opId();
                sync_result = processSyncData(op_server_sync_result_requ->bookings());
                op_server_sync_result_resp.setOpStatus(OP_STATUS_OK);
                _ops->server_sync_result_Resp(op_server_sync_result_resp);
                emit syncComplete();
                show_tab_requests = true;
                break;
            default:
                qDebug() << "MMMM processInboundQueue - invalid op_type:" << op_type;
                break;
        }
        qDebug() << "MMMM deleting inbound operation " << op_id;
        _ops->deleteInboundOp(op_id);

    }

    if (zero_days_warning) {
        zero_days_warning = false;
        emit zeroDaysWarning();
    }

    if (toast_reqd) {
        toast_reqd = false;
        emit updateReceived();
    }

    if (show_tab_requests) {
        show_tab_requests = false;
        emit suggestShowRequestsTab();
    }

    if (show_tab_inbox) {
        show_tab_inbox = false;
        emit suggestShowInboxTab();
    }

    qDebug() << "MMMM done processInboundQueue";

}

void ModelImpl::processBookingApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome)
{
    qDebug() << "MMMM processBookingApprovalOutcome";
    OpApprovalOutcomeResp op_approval_outcome_resp;

    int update_count = _sql->countBookingUpdates(approval_outcome->localRequestId(), approval_outcome->originatingOpId());

    op_approval_outcome_resp.setLocalRequestId(approval_outcome->localRequestId());
    op_approval_outcome_resp.setOpStatus(OP_STATUS_OK);

    if (update_count == 1) {
        _sql->updateRequestStatus(approval_outcome->localRequestId(), approval_outcome->requestStatus());
    } else {
        qDebug() << "MMMM approval outcome ignored as original booking has been updated again since this approval was requested";
    }
    _ops->approval_outcome_Resp(op_approval_outcome_resp);

    DbAnnualLeaveRecord record;
    record = _sql->getAnnualLeaveRequest(approval_outcome->localRequestId());
    requestEntitlementData(record.leaveYear());
}

void ModelImpl::processCancellationApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome)
{
    qDebug() << "MMMM processCancellationApprovalOutcome";
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
            qDebug() << "MMMM ignoring OP_TYPE_CANCELLATION_OUTCOME_REQUEST since target request does not have appropriate current status:" << last_op_status;
        }
    } else {
        qDebug() << "MMMM approval outcome ignored as original booking has been updated again since this approval was requested";
    }

    op_approval_outcome_resp.setLocalRequestId(approval_outcome->localRequestId());
    op_approval_outcome_resp.setOpStatus(OP_STATUS_OK);
    _ops->approval_outcome_Resp(op_approval_outcome_resp);

    record = _sql->getAnnualLeaveRequest(approval_outcome->localRequestId());
    requestEntitlementData(record.leaveYear());

}

void ModelImpl::processUpdateApprovalOutcome(OpApprovalOutcomeRequ *approval_outcome)
{
    qDebug() << "MMMM processUpdateApprovalOutcome";
    OpApprovalOutcomeResp op_approval_outcome_resp;

    int last_op_status = _sql->getLastOpStatus(approval_outcome->localRequestId());
    qDebug() << "MMMM processUpdateApprovalOutcome last_op_status=" << last_op_status;

    int update_count = _sql->countBookingUpdates(approval_outcome->localRequestId(), approval_outcome->originatingOpId());
    qDebug() << "MMMM processUpdateApprovalOutcome update_count=" << update_count;

    if (last_op_status == STATUS_UPDATE_SUBMITTED) {
        if (approval_outcome->requestStatus() == STATUS_UPDATE_OK) {
            if (update_count == 1) {
                // update booking
                _sql->applyBookingUpdate(approval_outcome->localRequestId(), approval_outcome->halfDays());
                qDebug() << "MMMM update was approved";
            } else {
                qDebug() << "MMMM booking record not updated since number of records that match required condition=" << update_count;
            }
        } else {
            // including count==1 in the condition ensures we're updating the status of the annual_leave_request record only if the rejection applies to the most recently submitted update request
            if (update_count == 1 && approval_outcome->requestStatus() == STATUS_UPDATE_REJECTED) {
                _sql->updateRequestStatus(approval_outcome->localRequestId(), approval_outcome->requestStatus());
                qDebug() << "MMMM update was rejected";
            } else {
                qDebug() << "MMMM booking record not updated since number of records that match required condition=" << update_count;
            }
        }
    } else {
        qDebug() << "MMMM ignoring OP_TYPE_UPDATE_OUTCOME_REQUEST since target request does not have appropriate current status:" << last_op_status;
    }
    op_approval_outcome_resp.setLocalRequestId(approval_outcome->localRequestId());
    op_approval_outcome_resp.setOpStatus(OP_STATUS_OK);
    _ops->approval_outcome_Resp(op_approval_outcome_resp);

    DbAnnualLeaveRecord record;
    record = _sql->getAnnualLeaveRequest(approval_outcome->localRequestId());
    requestEntitlementData(record.leaveYear());

}

int ModelImpl::processSyncData(BookingList *bookings)
{
    int booking_count = bookings->list().size();
    qDebug() << "MMMM processSyncData count=" << booking_count;
    int i=0;
    int sync_year=0;
    if (booking_count > 0) {
        sync_year = bookings->list().at(0)->leaveYear();
    }
    // if enterprise system has a booking not in the local database, we must store it
    for (i=0;i<booking_count;i++) {
        Booking *b = bookings->list().at(i);
        if (_sql->bookingCount(b->fromDate(), b->toDate(), b->firstDayHalf(), b->lastDayHalf(), b->dayHalf()) == 0) {
            qDebug() << "MMMM sync process is adding a record to the local database";
            int local_request_id = _sql->storeRequest(b->fromDate(), b->toDate(),b->note(),b->leaveYear(),b->firstDayHalf(), b->lastDayHalf(), b->dayHalf(), b->halfDays());
            _sql->updateRequestStatus(local_request_id, STATUS_BOOKING_APPROVED);
        }
    }

    // if the local database contains a "final state" booking (for the sync year) that is not in the server sync result, we must delete it
    QVariant current_requests = _sql->getRequests(sync_year);
    qDebug() << "MMMM current_requests=" << current_requests;

    QVariantList records = current_requests.value<QVariantList>();
    int record_count = records.size();
    qDebug() << "MMMM record_count=" << records.size();
    i=0;
    int fdate=0;
    int tdate=0;
    bool fdh=false;
    bool ldh=false;
    int dh;
    for (i=0;i<record_count;i++) {
        QVariant row = records.at(i);
        fdate = row.toMap().value("from_date_s").toInt();
        tdate = row.toMap().value("to_date_s").toInt();
        fdh = (row.toMap().value("first_day_half").toInt() != 0);
        ldh = (row.toMap().value("last_day_half").toInt() != 0);
        dh = row.toMap().value("day_half").toInt();
        int booking_status = row.toMap().value("booking_status").toInt();
        qDebug() << QString("MMMM database contains: fdate=%1 tdate=%2 fdh=%3 ldh=%4 dh=%5").arg(fdate).arg(tdate).arg(fdh).arg(ldh).arg(dh);

        int j=0;
        bool deletion_needed=true;
        for (j=0;j<booking_count;j++) {
            deletion_needed = true;
            Booking *b = bookings->list().at(j);
            if (b->fromDate() == fdate && b->toDate() == tdate && b->firstDayHalf() == fdh && b->lastDayHalf() == ldh && b-> dayHalf() == dh) {
                if (booking_status == STATUS_BOOKING_APPROVED || booking_status == STATUS_CANCELLATION_OK || booking_status == STATUS_UPDATE_OK) {
                    deletion_needed = false;
                }
            }
        }
        if (deletion_needed) {
            int count = _sql->deleteRequest(fdate, tdate, fdh, ldh, dh);
            if (count == 1) {
                qDebug() << "MMMM sync process caused a record to be deleted from the local database";
            } else {
                qDebug() << "MMMM sync process chose not to delete a record from the local database as multiple records would have been affected";
            }
        }
    }
    return 0;
}

void ModelImpl::showTab(int tab_index)
{

}

void ModelImpl::showRequests(int year)
{

}

QString ModelImpl::interpret(QString from_date, QString to_date, bool first_day_half, bool last_day_half, int day_half)
{

    qDebug() << "MMMM interpret: " << from_date << " " << to_date << " " << first_day_half << " " << last_day_half << " " << day_half;

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

void ModelImpl::requestEntitlementData(int leave_year)
{
    qDebug() << "MMMM requestEntitlementData:" << leave_year;
    OpAnnualEntitlementRequ request;
    request.setLeaveYear(leave_year);
    _ops->annual_entitlement_Requ(request);
}

void ModelImpl::connectToHeadless()
{
    if (!_connectedToServer) {
        qDebug() << "MMMM Connecting to Headless" << endl;
        qDebug() << "MMMM ApplicationUI::connectToHeadless() - _ops->getSocketServerPath() " << _ops->getSocketServerPath() << endl;
        _socket->connectToServer(_ops->getSocketServerPath());
    } else {
        qDebug() << "MMMM Server connected - unable to connect" << endl;
    }

}

void ModelImpl::disconnectFromHeadless()
{
    if (_connectedToServer) {
        qDebug() << "MMMM Disconnecting from Headless" << endl;
        _socket->disconnectFromServer();
    }
}

void ModelImpl::sendToHeadless(const QVariant &text)
{
    if (_connectedToServer) {
        qDebug() << "MMMM sending to headless " << text.toString() << endl;
        _socket->write(text.toByteArray());
        _socket->flush();
    } else {
        qDebug() << "MMMM Server not connected unable to send " << text.toString() << endl;
    }
}

void ModelImpl::connectSocket() {
    qDebug() << "MMMM Connecting socket SIGNAL()s" << endl;
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

void ModelImpl::disconnectSocket() {
    qDebug() << "MMMM Disconnecting socket SIGNAL()s" << endl;
    QObject::disconnect(_socket, SIGNAL(connected()),
                           this, SLOT(onConnected()));

    QObject::disconnect(_socket, SIGNAL(disconnected()),
                           this, SLOT(onDisconnected()));

    QObject::disconnect(_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
                           this, SLOT(onError(QLocalSocket::LocalSocketError)));

    QObject::disconnect(_socket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
                           this, SLOT(onStateChanged(QLocalSocket::LocalSocketState)));
}

void ModelImpl::onConnected() {
    qDebug() << "MMMM Connected to server" << endl;
    _connectedToServer = true;
    sendToHeadless("*** Message from GUI: Connected to Server ***");
}

void ModelImpl::onDisconnected() {
    qDebug() << "MMMM Disconnected from server" << endl;
    _connectedToServer = false;
    _socketRetryInterval = HEADLESS_SOCKET_SERVER_RETRY_INTERVAL_MS;
    _socket->close();
}

void ModelImpl::onError(QLocalSocket::LocalSocketError socketError) {
    qDebug() << "MMMM Socket error from server" << socketError << endl;
    if (!_connectedToServer) {
        qDebug() << "MMMM Socket not connected so retrying" << endl;
        triggerRetryConnection();
    }
}

void ModelImpl::triggerRetryConnection() {
    qDebug() << "MMMM Socket retry scheduled for " << _socketRetryInterval << "mseconds" << endl;
    // Timeout is to give time for Headless socket server to start
    QTimer::singleShot(_socketRetryInterval, this, SLOT(onTimeout()));
}

void ModelImpl::onTimeout() {
    qDebug() << "MMMM retry timeout expiry - retrying connection to socket server" << endl;
    _socketRetryInterval *= 2;
    if (_socketRetryInterval > HEADLESS_SOCKET_SERVER_MAX_RETRY_INTERVAL_MS) {
        _socketRetryInterval = HEADLESS_SOCKET_SERVER_MAX_RETRY_INTERVAL_MS;
    }
    connectToHeadless();
}

void ModelImpl::onStateChanged(QLocalSocket::LocalSocketState socketState) {
    qDebug() << "MMMM Socket state changed" << socketState << endl;
}

void ModelImpl::onReadyRead() {
    QByteArray readData = _socket->readAll();
    qDebug() << "MMMM Read: " << readData.constData() << endl;
}


void ModelImpl::onIndication()
{
    qDebug() << "MMMM [" << QThread::currentThreadId() << "] Model has received an indication of inbound messages to process";
    // delete contents of the indicator directory now so that we receive indications for any subsequent incoming messages
    GuiIndicator* indicator = GuiIndicator::getInstance(this);
    indicator->clearIndications();
    processInboundQueue();
}

void ModelImpl::loadTaskData() {
    _sql->loadTaskData();
}

void ModelImpl::loadRequestData(QString leave_year) {
    _sql->loadRequestData(leave_year);
}
