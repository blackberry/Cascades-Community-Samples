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

#include "EmailAdapter.hpp"
#include "BookingList.hpp"

EmailAdapter* EmailAdapter::_instance;
Settings* EmailAdapter::_settings;
AdapterSettings* EmailAdapter::_adapter_settings;
SimpleTestCalendar* EmailAdapter::_calendar;

EmailAdapter::EmailAdapter(QObject *parent) :
        QObject(parent), _adapterImpl(NULL) // careful -- don't initialise with AdapterImpl::getInstance(this) here - recursion!!!
                , _messageService(new MessageService(this)), _accountList(bb::pim::account::AccountService().accounts(bb::pim::account::Service::Messages)), _model(new GroupDataModel(this)), _builder(
                NULL), _mapOpType(new StringMap()), _mapStatus(new StringMap()), _mapTaskType(new StringMap()), _cache(EntitlementCache::getInstance(this)), _last_configured_value(-1), _rollBack(
                RollBack::getInstance(this)), _userIdCache(UserIdCache::getInstance(this))
{
    qDebug() << "EEEE EmailAdapter::EmailAdapter(QObject *parent)" << endl;

    _adapter_settings = AdapterSettings::getInstance(this);
    _calendar = SimpleTestCalendar::getInstance(this);

    qRegisterMetaType<QList<bb::pim::message::ConversationKey> >("QList<bb::pim::message::ConversationKey>");

    setUpStringMaps();
}

EmailAdapter::~EmailAdapter()
{
    _instance = NULL;
    if (_builder) {
        delete _builder;
        _builder = NULL;
    }
    if (_mapOpType) {
        delete _mapOpType;
        _mapOpType = NULL;
    }
    if (_mapStatus) {
        delete _mapStatus;
        _mapStatus = NULL;
    }
}

EmailAdapter *EmailAdapter::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new EmailAdapter(parent);
    }
    return _instance;
}

int EmailAdapter::start()
{
    qDebug() << "EEEE EmailAdapter::start()" << endl;

    connectSettingsChangedSignal();

    connectAdapterDetailsSignal();

    logAccounts();

    onSettingsChanged();
    selectCurrentAccount();
    messagesUpdated();

    if (_adapter_settings->isConfigured()) {
        qDebug() << "EEEE -----------------------------------------------" << endl;
        qDebug() << "EEEE EmailAdapter IS CONFIGURED AND IS STARTING" << endl;
        qDebug() << "EEEE -----------------------------------------------" << endl;
        return ADAPTER_IMPL_ACCEPTED;
    } else {
        qWarning() << "EEEE -----------------------------------------------" << endl;
        qWarning() << "EEEE EmailAdapter NOT CONFIGURED AND SO CANNOT START" << endl;
        qWarning() << "EEEE -----------------------------------------------" << endl;
        qWarning() << _adapter_settings->toString();
        return ADAPTER_IMPL_NOTACCEPTED;
    }
}

void EmailAdapter::connectSettingsChangedSignal()
{

    // must connect this signal regardless of adapter configuration state otherwise we never get notified of settings changes

    _settings = Settings::getInstance(this);

    if (!QObject::connect(_settings, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()))) {
        qWarning() << "EEEE EmailAdapter::connectSettingsChangedSignal() - connect failed - onSettingsChanged" << strerror(errno) << endl;
    } else {
        qDebug() << "EEEE connected settingsChanged signal";
    }
}

void EmailAdapter::connectAdapterDetailsSignal()
{

    _adapterImpl = AdapterImpl::getInstance(this);

    if (!QObject::connect(this, SIGNAL(inAdapterDetailsRequ()), _adapterImpl, SIGNAL(inAdapterDetailsRequ()))) {
        qWarning() << "EEEE EmailAdapter::connectAdapterDetailsSignal() - connect failed - inAdapterDetailsRequ" << strerror(errno) << endl;
    } else {
        qDebug() << "EEEE connected inAdapterDetailsRequ signal";
    }
}

void EmailAdapter::connectSignals()
{

    qWarning() << "EEEE EmailAdapter::connectSignals()" << endl;

    _adapterImpl = AdapterImpl::getInstance(this);

    if (!QObject::connect(_messageService, SIGNAL(messageAdded(bb::pim::account::AccountKey, bb::pim::message::ConversationKey, bb::pim::message::MessageKey)), this, SLOT(messagesUpdated()))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - messageAdded" << strerror(errno) << endl;
    }

    if (!QObject::connect(_messageService, SIGNAL(messageRemoved(bb::pim::account::AccountKey, bb::pim::message::ConversationKey, bb::pim::message::MessageKey, QString)), this,
            SLOT(messageRemoved(bb::pim::account::AccountKey, bb::pim::message::ConversationKey, bb::pim::message::MessageKey, QString)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - messageRemoved" << strerror(errno) << endl;
    }

    if (!QObject::connect(_messageService, SIGNAL(bodyDownloaded(bb::pim::account::AccountKey, bb::pim::message::MessageKey)), this,
            SLOT(bodyDownloaded(bb::pim::account::AccountKey, bb::pim::message::MessageKey)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - bodyDownloaded" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inApprovalOutcomeRequ(int, int, const QString &, int, int)), _adapterImpl, SIGNAL(inApprovalOutcomeRequ(int, int, const QString &, int, int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inApprovalOutcomeRequ" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inSubmitBookingResp(int, int)), _adapterImpl, SIGNAL(inSubmitBookingResp(int, int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inSubmitBookingResp" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inHalfDaysUsedRequ(int, int)), _adapterImpl, SIGNAL(inHalfDaysUsedRequ(int, int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inHalfDaysUsedRequ" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inCancelBookingResp(int, int)), _adapterImpl, SIGNAL(inCancelBookingResp(int, int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inCancelBookingResp" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inAnnualEntitlementResp(int, int, int, int)), _adapterImpl, SIGNAL(inAnnualEntitlementResp(int, int, int, int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inAnnualEntitlementResp" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inUpdateBookingResp(int, int, qint64, qint64)), _adapterImpl, SIGNAL(inUpdateBookingResp(int, int, qint64, qint64)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inUpdateBookingResp" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inApprovalTaskRequ(int, const QString &, int, qint64, qint64, bool, bool, int, int, int, const QString &,int)), _adapterImpl,
            SIGNAL(inApprovalTaskRequ(int, const QString &, int, qint64, qint64, bool, bool, int, int, int, const QString &,int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inApprovalTaskRequ (BOOKING)" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inApprovalTaskRequ(int, int, int, int, qint64, qint64, qint64, qint64, bool, bool, bool, bool, int, int, const QString &, int, int)), _adapterImpl,
            SIGNAL(inApprovalTaskRequ(int, int, int, int, qint64, qint64, qint64, qint64, bool, bool, bool, bool, int, int, const QString &, int, int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inApprovalTaskRequ (UPDATE)" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inApprovalTaskRequ(int, int, int, qint64, qint64, bool, bool, int, const QString &, const QString &, int, int)), _adapterImpl,
            SIGNAL(inApprovalTaskRequ(int, int, int, qint64, qint64, bool, bool, int, const QString &, const QString &, int, int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inApprovalTaskRequ (CANCEL)" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inApprovalTaskOutcomeResp(int, int)), _adapterImpl, SIGNAL(inApprovalTaskOutcomeResp(int, int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inApprovalTaskOutcomeResp" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inClientSynchronizeResp(int)), _adapterImpl, SIGNAL(inClientSynchronizeResp(int)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inClientSynchronizeResp" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(inServerSyncResultRequ(QByteArray)), _adapterImpl, SIGNAL(inServerSyncResultRequ(QByteArray)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - inServerSyncResultRequ" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(pauseDataFromApi()), _adapterImpl, SIGNAL(pauseDataFromApi()))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - pauseDataFromApi" << strerror(errno) << endl;
    }

    if (!QObject::connect(this, SIGNAL(resumeDataFromApi()), _adapterImpl, SIGNAL(resumeDataFromApi()))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - pauseDataFromApi" << strerror(errno) << endl;
    }

    if (!QObject::connect(_model, SIGNAL(itemAdded(QVariantList)), this, SLOT(onItemAdded(QVariantList)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - itemAdded" << strerror(errno) << endl;
    }

    if (!QObject::connect(_model, SIGNAL(itemUpdated(QVariantList)), this, SLOT(onItemUpdated(QVariantList)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - itemUpdated" << strerror(errno) << endl;
    }

    if (!QObject::connect(_model, SIGNAL(itemRemoved(QVariantList)), this, SLOT(onItemRemoved(QVariantList)))) {
        qWarning() << "EEEE EmailAdapter::start() - connect failed - itemRemoved" << strerror(errno) << endl;
    }

}

int EmailAdapter::stop()
{
    qDebug() << "EEEE stop() disconnecting signals";
    _adapterImpl = AdapterImpl::getInstance(this);

    QObject::disconnect(this, 0, 0, 0);
    qDebug() << "EEEE stop() disconnected signals connected to EmailAdapter";
    QObject::disconnect(_messageService, 0, 0, 0);
    qDebug() << "EEEE stop() disconnected signals connected to MessageService";
    QObject::disconnect(_model, 0, 0, 0);
    qDebug() << "EEEE stop() disconnected signals connected to DataModel";

    _cache->save();
    _rollBack->save();
    _userIdCache->save();

    // reinstate those signals which must be always connected
    connectSettingsChangedSignal();
    connectAdapterDetailsSignal();

    return ADAPTER_IMPL_ACCEPTED;
}

void EmailAdapter::onItemAdded(QVariantList indexPath)
{
    if (indexPath.length() != 2) {
        return;
    }

    qDebug() << "EEEE onItemAdded() - DataModel item ADDED at indexPath=" << indexPath << endl;

    QVariantMap entry = _model->data(indexPath).toMap();
    MessageKey messageId = entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong();
    bool toBeRemoved = entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED].toBool();
    bool completed = entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED].toBool();
    bool messageBeingProcessed = entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED].toBool();

    qDebug() << "EEEE onItemAdded() - DataModel item ADDED       messageId=" << messageId << endl;
    qDebug() << "EEEE onItemAdded() - DataModel item ADDED     toBeRemoved=" << toBeRemoved << endl;
    qDebug() << "EEEE onItemAdded() - DataModel item ADDED       completed=" << completed << endl;
    qDebug() << "EEEE onItemAdded() - DataModel item ADDED being processed=" << messageBeingProcessed << endl;
}

void EmailAdapter::onItemUpdated(QVariantList indexPath)
{
    if (indexPath.length() != 2) {
        return;
    }

    qDebug() << "EEEE onItemUpdated() - DataModel item UPDATED at indexPath=" << indexPath << endl;

    QVariantMap entry = _model->data(indexPath).toMap();
    MessageKey messageId = entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong();
    bool toBeRemoved = entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED].toBool();
    bool completed = entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED].toBool();
    bool messageBeingProcessed = entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED].toBool();

    qDebug() << "EEEE onItemUpdated() - DataModel item UPDATED     messageId=" << messageId << endl;
    qDebug() << "EEEE onItemUpdated() - DataModel item UPDATED   toBeRemoved=" << toBeRemoved << endl;
    qDebug() << "EEEE onItemUpdated() - DataModel item UPDATED     completed=" << completed << endl;
    qDebug() << "EEEE onItemUpdated() - DataModel item ADDED being processed=" << messageBeingProcessed << endl;
}

void EmailAdapter::onItemRemoved(QVariantList indexPath)
{
    if (indexPath.length() != 2) {
        return;
    }

    qDebug() << "EEEE onItemRemoved() - DataModel item REMOVED at indexPath=" << indexPath << endl;
}

int EmailAdapter::pause()
{
    // Opportunity to pause adapter processing at this point

    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::resume()
{
    // Opportunity to resume adapter processing at this point

    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::reset()
{
    _cache->clearCache();
    _cache->save();
    _rollBack->clear();
    _rollBack->save();
    _model->clear();
    _userIdCache->clear();
    _userIdCache->save();

    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::networkAvailable()
{
    // Opportunity to modify adapter processing at this point

    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::networkNotAvailable()
{
    // Opportunity to modify adapter processing at this point

    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outSubmitBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id)
{

    if (!_adapter_settings->isConfigured()) {
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    QVariantMap parameters;

    // Send a booking approval task request to the p2p approver

    parameters[EMAIL_ADAPTER_KEY_OPERATION] = stringForOpType(OP_TYPE_APPROVAL_TASK_REQUEST);
    parameters[EMAIL_ADAPTER_KEY_LOCAL_ID] = localId;
    parameters[EMAIL_ADAPTER_KEY_TASK_TYPE] = TASK_TYPE_BOOKING_APPROVAL;
    parameters[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID] = originating_op_id;
    parameters[EMAIL_ADAPTER_KEY_FROM_DATE] = fromDate;
    parameters[EMAIL_ADAPTER_KEY_TO_DATE] = toDate;
    parameters[EMAIL_ADAPTER_KEY_FIRST_DAY_HALF] = firstDayHalf;
    parameters[EMAIL_ADAPTER_KEY_LAST_DAY_HALF] = lastDayHalf;
    parameters[EMAIL_ADAPTER_KEY_DAY_HALF] = dayHalf;
    parameters[EMAIL_ADAPTER_KEY_NOTE] = note;
    parameters[EMAIL_ADAPTER_KEY_USER_ID] = _adapter_settings->p2pClientUserId();
    parameters[EMAIL_ADAPTER_KEY_LEAVE_YEAR] = leaveYear;

    //Normally I'd expect the back end system to do this so we do our best here
    int half_days = _calendar->calculateHalfDays(fromDate, toDate, firstDayHalf, lastDayHalf);
    parameters[EMAIL_ADAPTER_KEY_HALF_DAYS] = half_days;

    if (!_cache->isYearPresent(leaveYear)) {
        _cache->setP2pDefault(leaveYear);
    }

    _rollBack->deleteItem(localId);
    _rollBack->addItem(localId, originating_op_id, leaveYear, half_days);
    _cache->decrement(leaveYear, half_days);

    emit inHalfDaysUsedRequ(localId, half_days);

    if (constructAndSendMessage(parameters, _adapter_settings->p2pModeApproverEmail(), _adapter_settings->p2pModeApproverName())) {
        emit inSubmitBookingResp(localId, STATUS_BOOKING_SUBMITTED);
    } else {
        emit inSubmitBookingResp(localId, STATUS_FAILED);
    }
    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outClientSynchronizeRequ(int leave_year)
{
    qDebug() << "EEEE EmailAdapter:outClientSynchronizeRequ(" << leave_year << ")" << endl;
    if (!_adapter_settings->isConfigured()) {
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    // THIS ADAPTER DOES NOT SUPPORT THE SYNC OPERATIONS SO ALWAYS REPLIES WITH A NEGATIVE ACK

    qDebug() << "EEEE EmailAdapter:outClientSynchronizeRequ(): emitting inClientSynchronizeResp";
    emit inClientSynchronizeResp(OP_STATUS_NOT_SUPPORTED);

    return ADAPTER_IMPL_NOTACCEPTED;
}

int EmailAdapter::outServerSyncResultResp(int op_id, int op_status) {
    Q_UNUSED(op_id)
    Q_UNUSED(op_status)
    if (!_adapter_settings->isConfigured()) {
        return ADAPTER_IMPL_NOTACCEPTED;
    }
    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outCancelBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id)
{

    if (!_adapter_settings->isConfigured()) {
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    QVariantMap parameters;

    //Normally I'd expect the back end system to do this so we do our best here
    int half_days = _calendar->calculateHalfDays(fromDate, toDate, firstDayHalf, lastDayHalf);

    _rollBack->deleteItem(localId);
    _rollBack->addItem(localId, originating_op_id, leaveYear, half_days);

    // Send an cancellation task request to the p2p approver
    parameters[EMAIL_ADAPTER_KEY_OPERATION] = stringForOpType(OP_TYPE_APPROVAL_TASK_REQUEST);
    parameters[EMAIL_ADAPTER_KEY_TASK_TYPE] = TASK_TYPE_CANCEL_APPROVAL;
    parameters[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID] = originating_op_id;
    parameters[EMAIL_ADAPTER_KEY_LOCAL_ID] = localId;
    parameters[EMAIL_ADAPTER_KEY_LEAVE_YEAR] = leaveYear;
    parameters[EMAIL_ADAPTER_KEY_FROM_DATE] = fromDate;
    parameters[EMAIL_ADAPTER_KEY_TO_DATE] = toDate;
    parameters[EMAIL_ADAPTER_KEY_FIRST_DAY_HALF] = firstDayHalf;
    parameters[EMAIL_ADAPTER_KEY_LAST_DAY_HALF] = lastDayHalf;
    parameters[EMAIL_ADAPTER_KEY_DAY_HALF] = dayHalf;
    parameters[EMAIL_ADAPTER_KEY_NOTE] = note;
    parameters[EMAIL_ADAPTER_KEY_USER_ID] = _adapter_settings->p2pClientUserId();
    parameters[EMAIL_ADAPTER_KEY_HALF_DAYS] = half_days;

    _cache->increment(leaveYear, half_days);

    if (constructAndSendMessage(parameters, _adapter_settings->p2pModeApproverEmail(), _adapter_settings->p2pModeApproverName())) {
        emit inCancelBookingResp(localId, STATUS_CANCELLATION_SUBMITTED);
    } else {
        emit inCancelBookingResp(localId, STATUS_FAILED);
    }
    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outUpdateBookingRequ(int localId, int oldYear, int newYear, qint64 oldFromDate, qint64 newFromDate, qint64 oldToDate, qint64 newToDate, bool oldFirstDayHalf, bool newFirstDayHalf,
        bool oldLastDayHalf, bool newLastDayHalf, int oldDayHalf, int newDayHalf, const QString &note, int originating_op_id)
{
    Q_UNUSED(note)

    if (!_adapter_settings->isConfigured()) {
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    QVariantMap parameters;

    // Send an update approval task request to the p2p approver

    parameters[EMAIL_ADAPTER_KEY_OPERATION] = stringForOpType(OP_TYPE_APPROVAL_TASK_REQUEST);
    parameters[EMAIL_ADAPTER_KEY_TASK_TYPE] = TASK_TYPE_UPDATE_APPROVAL;
    parameters[EMAIL_ADAPTER_KEY_LOCAL_ID] = localId;
    parameters[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID] = originating_op_id;
    parameters[EMAIL_ADAPTER_KEY_FROM_DATE] = oldFromDate;
    parameters[EMAIL_ADAPTER_KEY_TO_DATE] = oldToDate;
    parameters[EMAIL_ADAPTER_KEY_FIRST_DAY_HALF] = oldFirstDayHalf;
    parameters[EMAIL_ADAPTER_KEY_LAST_DAY_HALF] = oldLastDayHalf;
    parameters[EMAIL_ADAPTER_KEY_DAY_HALF] = oldDayHalf;
    parameters[EMAIL_ADAPTER_KEY_LEAVE_YEAR] = oldYear;
    parameters[EMAIL_ADAPTER_KEY_NEW_FROM_DATE] = newFromDate;
    parameters[EMAIL_ADAPTER_KEY_NEW_TO_DATE] = newToDate;
    parameters[EMAIL_ADAPTER_KEY_NEW_FIRST_DAY_HALF] = newFirstDayHalf;
    parameters[EMAIL_ADAPTER_KEY_NEW_LAST_DAY_HALF] = newLastDayHalf;
    parameters[EMAIL_ADAPTER_KEY_NEW_DAY_HALF] = newDayHalf;
    parameters[EMAIL_ADAPTER_KEY_NEW_LEAVE_YEAR] = newYear;
    parameters[EMAIL_ADAPTER_KEY_USER_ID] = _adapter_settings->p2pClientUserId();

    //Normally I'd expect the back end system to do this so we do our best here

    int oldHalfDayCount = _calendar->calculateHalfDays(oldFromDate, oldToDate, oldFirstDayHalf, oldLastDayHalf);
    qDebug() << "EEEE calculated old half days for from_date=" << oldFromDate << " to_date=" << oldToDate << " first_day_half=" << oldFirstDayHalf << " last_day_half=" << oldLastDayHalf
            << " half_days=" << oldHalfDayCount;
    int newHalfDayCount = _calendar->calculateHalfDays(newFromDate, newToDate, newFirstDayHalf, newLastDayHalf);
    qDebug() << "EEEE calculated new half days for from_date=" << newFromDate << " to_date=" << newToDate << " first_day_half=" << newFirstDayHalf << " last_day_half=" << newLastDayHalf
            << " half_days=" << newHalfDayCount;

    parameters[EMAIL_ADAPTER_KEY_HALF_DAYS] = newHalfDayCount;

    if (!_cache->isYearPresent(oldYear)) {
        _cache->setP2pDefault(oldYear);
    }
    if (!_cache->isYearPresent(newYear)) {
        _cache->setP2pDefault(newYear);
    }

    _cache->increment(oldYear, oldHalfDayCount);
    _cache->decrement(newYear, newHalfDayCount);

    _rollBack->deleteItem(localId, originating_op_id);
    _rollBack->addOldItem(localId, originating_op_id, oldYear, oldHalfDayCount);
    _rollBack->addNewItem(localId, originating_op_id, newYear, newHalfDayCount);

    if (constructAndSendMessage(parameters, _adapter_settings->p2pModeApproverEmail(), _adapter_settings->p2pModeApproverName())) {
        emit inUpdateBookingResp(localId, STATUS_UPDATE_SUBMITTED, newFromDate, newToDate);
    } else {
        emit inUpdateBookingResp(localId, STATUS_FAILED, newFromDate, newToDate);
    }
    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outApprovalOutcomeResp(int localId, int opStatus)
{
    Q_UNUSED(localId)
    Q_UNUSED(opStatus)

    if (!_adapter_settings->isConfigured()) {
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outHalfDaysUsedResp(int localId, int opStatus)
{
    Q_UNUSED(localId)
    Q_UNUSED(opStatus)

    if (!_adapter_settings->isConfigured()) {
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outAnnualEntitlementRequ(int year)
{
    qDebug() << "EEEE outAnnualEntitlementRequ() for year " << year << endl;

    if (!_adapter_settings->isConfigured()) {
        qDebug() << "EEEE outAnnualEntitlementRequ rejecting request. Adapter not configured";
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    QVariantMap parameters;

    if (year == -1) {
        QList<int> years = _cache->years();
        QList<int>::iterator curr;
        for (curr = years.begin(); curr != years.end(); curr++) {
            emit inAnnualEntitlementResp(STATUS_COMPLETED, *curr, _cache->getTotal(*curr), _cache->getCurrent(*curr));
        }
    } else if (_cache->isYearPresent(year)) {
        emit inAnnualEntitlementResp(STATUS_COMPLETED, year, _cache->getTotal(year), _cache->getCurrent(year));
    } else {
        _cache->setP2pDefault(year);
        emit inAnnualEntitlementResp(STATUS_COMPLETED, year, _cache->getTotal(year), _cache->getCurrent(year));
    }
    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outApprovalTaskResp(int taskId, int opStatus, const QString &userId)
{
    Q_UNUSED(opStatus)

    if (!_adapter_settings->isConfigured()) {
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    if (_userIdCache->foundUserIdMatch(userId)) {
        qDebug() << "EEEE EmailAdapter::outApprovalTaskResp - adding to UserId Cache UserId=" << userId << ", taskId=" << taskId << endl;
        _userIdCache->updateItem(userId, taskId);
    } else {
        qWarning() << "EEEE EmailAdapter::outApprovalTaskResp - no match on UserId=" << userId << endl;
        return ADAPTER_IMPL_NOTACCEPTED;
    }
    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outApprovalTaskOutcomeRequ(int taskId, int local_request_id, int requestStatus, int originating_op_id, int task_type)
{
    qDebug() << "EEEE EmailAdapter:outApprovalTaskOutcomeRequ() taskId=" << taskId << " local_request_id=" << local_request_id << " requestStatus=" << requestStatus << " originating_op_id="
            << originating_op_id << endl;
    if (!_adapter_settings->isConfigured()) {
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    QVariantMap parameters;

    parameters[EMAIL_ADAPTER_KEY_OPERATION] = stringForOpType(OP_TYPE_APPROVAL_OUTCOME_REQUEST);
    parameters[EMAIL_ADAPTER_KEY_LOCAL_ID] = local_request_id;
    parameters[EMAIL_ADAPTER_KEY_TASK_ID] = taskId;
    parameters[EMAIL_ADAPTER_KEY_STATUS] = requestStatus;
    parameters[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID] = originating_op_id;
    parameters[EMAIL_ADAPTER_KEY_TASK_TYPE] = task_type;

    if (!_userIdCache->foundTaskIdMatch(taskId)) {
        qWarning() << "EEEE EmailAdapter::outApprovalTaskOutcomeRequ - no match on TaskId=" << taskId << endl;
        return ADAPTER_IMPL_NOTACCEPTED;
    }

    if (constructAndSendMessage(parameters, _userIdCache->emailAddress(taskId), _userIdCache->userId(taskId))) {
        emit inApprovalTaskOutcomeResp(taskId, OP_STATUS_OK);
    } else {
        emit inApprovalTaskOutcomeResp(taskId, STATUS_FAILED);
    }

    _userIdCache->removeItem(taskId);

    return ADAPTER_IMPL_ACCEPTED;
}

int EmailAdapter::outAdapterStatusRequ()
{
    qDebug() << "EEEE EmailAdapter:outAdapterStatusRequ()" << endl;

    qDebug() << "EEEE EmailAdapter:outAdapterStatusRequ(): emitting inAdapterDetailsRequ";
    emit inAdapterDetailsRequ();

    return ADAPTER_IMPL_ACCEPTED;
}

void EmailAdapter::onSettingsChanged()
{
    qDebug() << "EEEE EmailAdapter: onSettingsChanged";
    QString accountName = _settings->getValueFor(GUI_SETTING_1_VALUE, "UNSET");
    QString providerName = _settings->getValueFor(GUI_SETTING_2_VALUE, "UNSET");
    QString p2pModeApproverName = _settings->getValueFor(GUI_SETTING_3_VALUE, "UNSET");
    QString p2pModeApproverEmail = _settings->getValueFor(GUI_SETTING_4_VALUE, "UNSET");
    QString p2pClientUserId = _settings->getValueFor(GUI_SETTING_5_VALUE, "UNSET");
    QString p2pModeEnabled = _settings->getValueFor(EMAIL_ADAPTER_SETTINGS_KEY_P2P_MODE_ENABLED, "1");

    if (accountName.compare("UNSET") != 0) {
        _adapter_settings->setAccountName(accountName);
    }
    qDebug() << "EEEE email adapter using accountName " << _adapter_settings->accountName();

    if (providerName.compare("UNSET") != 0) {
        _adapter_settings->setProviderName(providerName);
    }
    qDebug() << "EEEE email adapter using providerName " << _adapter_settings->providerName();

    qDebug() << "EEEE email adapter using p2pModeEnabled " << _adapter_settings->isp2pModeEnabled();

    if (p2pModeApproverEmail.compare("UNSET") != 0) {
        _adapter_settings->setP2pModeApproverEmail(p2pModeApproverEmail);
    }
    qDebug() << "EEEE email adapter using p2pModeApproverEmail " << _adapter_settings->p2pModeApproverEmail();

    if (p2pClientUserId.compare("UNSET") != 0) {
        _adapter_settings->setP2pClientUserId(p2pClientUserId);
    }
    qDebug() << "EEEE email adapter using p2pClientUserId " << _adapter_settings->p2pClientUserId();

    if (p2pModeApproverName.compare("UNSET") != 0) {
        _adapter_settings->setP2pModeApproverName(p2pModeApproverName);
    }
    qDebug() << "EEEE email adapter using p2pModeApproverName " << _adapter_settings->p2pModeApproverName();

    // avoid unnecessarily informing GUI of adapter details if a change in settings hasn't modified the configured status
    int new_configured_value;
    if (_adapter_settings->isConfigured()) {
        new_configured_value = 1;
    } else {
        new_configured_value = 0;
    }
    if (new_configured_value != _last_configured_value) {
        if (_adapter_settings->isConfigured()) {
            qDebug() << "EEEE -----------------------------------------------" << endl;
            qDebug() << "EEEE EmailAdapter IS NOW CONFIGURED AND READY" << endl;
            qDebug() << "EEEE -----------------------------------------------" << endl;
            selectCurrentAccount();
            connectSignals();
            messagesUpdated();
        } else {
            qWarning() << "EEEE -----------------------------------------------" << endl;
            qWarning() << "EEEE EmailAdapter STILL NOT CONFIGURED AND NOT READY" << endl;
            qWarning() << "EEEE -----------------------------------------------" << endl;
            qWarning() << _adapter_settings->toString();
            stop();
        }
        qDebug() << "EEEE emitting inAdapterDetailsRequ";
        emit inAdapterDetailsRequ();
        _last_configured_value = new_configured_value;
    }
}

void EmailAdapter::logAccounts()
{
    foreach (const bb::pim::account::Account &account, _accountList){
    qDebug() << "EEEE account: displayName=" << account.displayName() << " providerName=" << account.provider().name() << endl;
}
}

void EmailAdapter::selectCurrentAccount()
{
    foreach (const bb::pim::account::Account &account, _accountList){
    qDebug() << "EEEE selectCurrentAccount: " << account.displayName() << " vs " << _adapter_settings->accountName() << " and " << account.provider().name() << " vs " << _adapter_settings->providerName();
    if ((account.displayName().compare(_adapter_settings->accountName(), Qt::CaseInsensitive) == 0) &&
            (account.provider().name().compare(_adapter_settings->providerName(), Qt::CaseInsensitive) == 0)) {
        _currentAccount = account;
        qDebug() << "EEEE selected account " << _currentAccount.id();
        continue;
    }
}
_builder = MessageBuilder::create(_currentAccount.id());
}

void EmailAdapter::messagesUpdated()
{

    qDebug() << "EEEE messagesUpdated";
    if (!_currentAccount.isValid()) {
        qWarning() << "EEEE messagesUpdated - account is NOT valid";
        return;
    }
    qDebug() << "EEEE messagesUpdated - account is valid";

    MessageSearchFilter filter;

    qDebug() << "EEEE messagesUpdated - processing message(s)";

    filter.addStatusCriteria(SearchStatusCriteria::Received);
    filter.addSearchCriteria(SearchFilterCriteria::Subject, EMAIL_ADAPTER_SERVICE_UUID);

    modelClear();

    const QList<Message> messages = _messageService->searchLocal(_currentAccount.id(), filter);

    foreach (const Message &message, messages){
    qDebug() << "EEEE EmailAdapter::messagesUpdated() - processing Id:" << message.id() << endl;
    qDebug() << "EEEE EmailAdapter::messagesUpdated() - processing Subject:" << message.subject() << endl;
    qDebug() << "EEEE EmailAdapter::messagesUpdated() - processing To:" << message.recipientAt(0).address() << endl;
    qDebug() << "EEEE EmailAdapter::messagesUpdated() - processing From:" << message.sender().address() << endl;
    qDebug() << "EEEE EmailAdapter::messagesUpdated() - processing isInbound:" << message.isInbound() << endl;
    qDebug() << "EEEE EmailAdapter::messagesUpdated() - processing status:" << message.status() << endl;

    if(addMessage(message)) {
        qDebug() << "EEEE EmailAdapter::messagesUpdated() - Added:" << message.id() << endl;
    } else {
        qDebug() << "EEEE EmailAdapter::messagesUpdated() - Not added:" << message.id() << endl;
    }
}

    processInboundMessages();
}

void EmailAdapter::modelClear()
{
    for (QVariantList indexPath = _model->first(); !indexPath.isEmpty(); indexPath = _model->after(indexPath)) {
        QVariantMap entry = _model->data(indexPath).toMap();
        bool toBeRemoved = entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED].toBool();
        if (toBeRemoved) {

            MessageKey messageId = entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong();
            bool completed = entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED].toBool();
            bool messageBeingProcessed = entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED].toBool();

            qDebug() << "EEEE modelClear() - DataModel item requesting removal        at indexPath=" << indexPath << endl;
            qDebug() << "EEEE modelClear() - DataModel item requesting removal           messageId=" << messageId << endl;
            qDebug() << "EEEE modelClear() - DataModel item requesting removal         toBeRemoved=" << toBeRemoved << endl;
            qDebug() << "EEEE modelClear() - DataModel item requesting removal           completed=" << completed << endl;
            qDebug() << "EEEE modelClear() - DataModel item requesting removal     being processed=" << messageBeingProcessed << endl;

            _model->removeAt(indexPath);

        } else {
            qDebug() << "EEEE modelClear() - DataModel item not requiring removal at indexPath=" << indexPath << endl;

        }
    }
}

bool EmailAdapter::modelInsert(QVariantMap entry)
{
    bool ok;
    bool duplicate = false;
    MessageKey insertMessageId = entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong(&ok);
    if (!ok) {
        qWarning() << "EEEE EmailAdapter::modelInsert() - improper insertMessageId" << entry[EMAIL_ADAPTER_KEY_MESSAGE_ID] << endl;
        return false;
    }
    for (QVariantList indexPath = _model->first(); !indexPath.isEmpty(); indexPath = _model->after(indexPath)) {
        QVariantMap targetEntry = _model->data(indexPath).toMap();
        MessageKey targetMessageId = targetEntry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong(&ok);
        if (!ok) {
            qWarning() << "EEEE EmailAdapter::modelInsert() - improper targetMessageId" << targetEntry[EMAIL_ADAPTER_KEY_MESSAGE_ID] << endl;
            continue;
        }
        if (targetMessageId == insertMessageId) {
            duplicate = true;
        }
    }
    if (!duplicate) {

        bool completed = entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED].toBool();
        bool toBeRemoved = entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED].toBool();
        bool messageBeingProcessed = entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED].toBool();

        qDebug() << "EEEE modelInsert() - DataModel item requesting insertion       messageId=" << insertMessageId << endl;
        qDebug() << "EEEE modelInsert() - DataModel item requesting insertion     toBeRemoved=" << toBeRemoved << endl;
        qDebug() << "EEEE modelInsert() - DataModel item requesting insertion       processed=" << completed << endl;
        qDebug() << "EEEE modelInsert() - DataModel item requesting insertion being processed=" << messageBeingProcessed << endl;

        _model->insert(entry);
        return true;
    } else {
        qDebug() << "EEEE EmailAdapter::modelInsert() - attempting to add duplicate message" << insertMessageId << endl;
        return false;
    }
}

void EmailAdapter::messageRemoved(bb::pim::account::AccountKey accountId, bb::pim::message::ConversationKey conversationId, bb::pim::message::MessageKey messageId, QString sourceId)
{
    Q_UNUSED(accountId)
    Q_UNUSED(conversationId)
    Q_UNUSED(sourceId)

    qDebug() << "EEEE EmailAdapter::messageRemoved() - message removed" << messageId << endl;

    for (QVariantList indexPath = _model->first(); !indexPath.isEmpty(); indexPath = _model->after(indexPath)) {
        bool ok;
        QVariantMap entry = _model->data(indexPath).toMap();
        MessageKey targetMessageId = entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong(&ok);
        if (!ok) {
            qWarning() << "EEEE EmailAdapter::messageRemoved() - improper MessageId" << entry[EMAIL_ADAPTER_KEY_MESSAGE_ID] << endl;
            continue;
        }
        if (targetMessageId == messageId) {
            qDebug() << "EEEE EmailAdapter::messageRemoved() - removed message matches target message --- flag to be removed in model" << messageId << endl;
            entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED] = true;

            bool completed = entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED].toBool();
            bool messageBeingProcessed = entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED].toBool();

            qDebug() << "EEEE modelClear() - DataModel item requesting updating    at indexPath=" << indexPath << endl;
            qDebug() << "EEEE modelClear() - DataModel item requesting updating       messageId=" << messageId << endl;
            qDebug() << "EEEE modelClear() - DataModel item requesting updating     toBeRemoved=" << entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED].toBool() << endl;
            qDebug() << "EEEE modelClear() - DataModel item requesting updating       completed=" << completed << endl;
            qDebug() << "EEEE modelClear() - DataModel item requesting updating being processed=" << messageBeingProcessed << endl;

            _model->updateItem(indexPath, entry);
        } else {
            qDebug() << "EEEE EmailAdapter::messageRemoved() - removed message does NOT matches target message" << messageId << endl;
        }
    }
}

void EmailAdapter::bodyDownloaded(bb::pim::account::AccountKey accountId, bb::pim::message::MessageKey messageId)
{
    qDebug() << "EEEE bodyDownloaded()";
    Message message = _messageService->message(accountId, messageId);

    if (message.isInbound()) {
        qDebug() << "EEEE bodyDownloaded() - this is an inbound message";
        QString messageBody = message.body(MessageBody::PlainText).plainText();
        if (!messageBody.isEmpty()) {
            qDebug() << "EEEE EmailAdapter::bodyDownloaded() - message body content available - Id:" << message.id() << endl;
            addMessage(message);
            messagesUpdated();
        } else {
            qDebug() << "EEEE EmailAdapter::bodyDownloaded() - message body still empty - Id:" << message.id() << endl;
            deleteMessage(message.id());
        }
    } else {
        qDebug() << "EEEE bodyDownloaded() - NOT an inbound message - ignoring";
    }
}

bool EmailAdapter::addMessage(const Message &message)
{
    qDebug() << "EEEE addMessage";
    JsonDataAccess jda;
    QString subjectField = message.subject();
    QString messageBody = message.body(MessageBody::PlainText).plainText();
    if (messageBody.isEmpty()) {
        _messageService->downloadMessage(_currentAccount.id(), message.id());
        qDebug() << "EEEE EmailAdapter::addMessage() - fetching message body" << endl;
        return false;
    }

    qDebug() << "EEEE EmailAdapter::addMessage() - Subject:" << subjectField << endl;
    qDebug() << "EEEE EmailAdapter::addMessage() - Body:" << messageBody << endl;
    qDebug() << "EEEE EmailAdapter::addMessage() - Id:" << message.id() << endl;
    qDebug() << "EEEE EmailAdapter::addMessage() - Sender Email:" << message.sender().address() << endl;
    qDebug() << "EEEE EmailAdapter::addMessage() - Sender Name:" << message.sender().name() << endl;
    qDebug() << "EEEE EmailAdapter::addMessage() - Sender Displayable Name:" << message.sender().displayableName() << endl;

    QString contentPatern;

    contentPatern += EMAIL_ADAPTER_JSON_SECTION_START_MARKER;
    contentPatern += EMAIL_ADAPTER_JSON_SECTION_BODY_REGEX;
    contentPatern += EMAIL_ADAPTER_JSON_SECTION_END_MARKER;

    QRegExp contentRx(contentPatern);

    qDebug() << "EEEE EmailAdapter::addMessage() - using RegExp Pattern:" << contentRx.pattern() << endl;
    qDebug() << "EEEE EmailAdapter::addMessage() - using RegExp Pattern Syntax:" << contentRx.patternSyntax() << endl;

    if (contentRx.indexIn(messageBody) != -1) {
        QVariant parsedPayload = jda.loadFromBuffer(contentRx.cap(1));
        if (!jda.hasError()) {
            QVariantMap entry;
            entry[EMAIL_ADAPTER_KEY_MESSAGE_ID] = message.id();
            entry[EMAIL_ADAPTER_KEY_SENDER_EMAIL] = message.sender().address();
            entry[EMAIL_ADAPTER_KEY_SENDER_NAME] = message.sender().name();
            entry[EMAIL_ADAPTER_KEY_PAYLOAD] = parsedPayload;
            entry[EMAIL_ADAPTER_KEY_TIME] = message.serverTimestamp().toString(Qt::ISODate);
            entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED] = false;
            entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED] = false;
            entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED] = false;
            if (!modelInsert(entry)) {
                return false;
            }
        } else {
            qWarning() << "EEEE EmailAdapter::addMessage() - message body JSON parse error" << contentRx.cap(1) << endl;
            return false;
        }
    } else {
        qWarning() << "EEEE EmailAdapter::addMessage() - no identifiable content in email body:" << messageBody << endl;
        qWarning() << "EEEE EmailAdapter::addMessage() - regexp error:" << contentRx.errorString() << endl;
        deleteMessage(message.id());
        return false;
    }
    return true;
}

void EmailAdapter::processInboundMessages()
{
    processJsonInboundMessages();
}

void EmailAdapter::processJsonInboundMessages()
{
    bool ok;
    for (QVariantList indexPath = _model->first(); !indexPath.isEmpty(); indexPath = _model->after(indexPath)) {

        QVariantMap entry = _model->data(indexPath).toMap();

        qDebug() << "EEEE processJsonInboundMessages - Message Id:" << entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong(&ok) << endl;
        qDebug() << "EEEE processJsonInboundMessages - Sender Email:" << entry[EMAIL_ADAPTER_KEY_SENDER_EMAIL].toString() << endl;
        qDebug() << "EEEE processJsonInboundMessages - Sender Name:" << entry[EMAIL_ADAPTER_KEY_SENDER_NAME].toString() << endl;
        qDebug() << "EEEE processJsonInboundMessages - Time:" << entry[EMAIL_ADAPTER_KEY_TIME].toString() << endl;
        qDebug() << "EEEE processJsonInboundMessages - Payload:" << entry[EMAIL_ADAPTER_KEY_PAYLOAD] << endl;
        qDebug() << "EEEE processJsonInboundMessages - Message Completed:" << entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED].toBool() << endl;
        qDebug() << "EEEE processJsonInboundMessages - Being Processed:" << entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED].toBool() << endl;
        qDebug() << "EEEE processJsonInboundMessages - To be removed:" << entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED].toBool() << endl;

        bool messageCompleted = entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED].toBool();
        bool toBeRemoved = entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED].toBool();

        if (toBeRemoved) {
            qDebug() << "EEEE processJsonInboundMessages - ignoring message - flagged to be removed - id:" << entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong(&ok) << endl;
            continue;
        }
        if (messageCompleted) {
            qDebug() << "EEEE processJsonInboundMessages - ignoring message - flagged as complete - id:" << entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong(&ok) << endl;
            continue;
        }

        // +++ GUARD +++ GUARD +++ GUARD +++ GUARD +++ GUARD +++ GUARD +++
        // START
        //
        // Need to make the test and setting of messageBeingProcessed atomic - SIGNAL() may happen between the two
        // and cause duplicate processing of events

        _mutex.lock();
        bool messageBeingProcessed = entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED].toBool();

        if (messageBeingProcessed) {
            qDebug() << "EEEE processJsonInboundMessages - ignoring message - flagged as in progress - id:" << entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong(&ok) << endl;
            _mutex.unlock();
            continue;
        } else {
            messageBeingProcessed = true;
            entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED] = messageBeingProcessed;
            _model->updateItem(indexPath, entry);
        }
        _mutex.unlock();

        // END
        // +++ GUARD +++ GUARD +++ GUARD +++ GUARD +++ GUARD +++ GUARD +++

        qDebug() << "EEEE processJsonInboundMessages - Processing Message Id:" << entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong(&ok) << endl;

        MessageKey messageId = entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong(&ok);
        if (!ok) {
            qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper MessageId" << entry[EMAIL_ADAPTER_KEY_MESSAGE_ID] << endl;
            continue;
        }

        QVariantMap payload = entry[EMAIL_ADAPTER_KEY_PAYLOAD].toMap();
        QDateTime timeStamp = QDateTime::fromString(entry[EMAIL_ADAPTER_KEY_TIME].toString(), Qt::ISODate);

        QString operation = payload[EMAIL_ADAPTER_KEY_OPERATION].toString();
        QString senderEmail = entry[EMAIL_ADAPTER_KEY_SENDER_EMAIL].toString();
        QString senderName = entry[EMAIL_ADAPTER_KEY_SENDER_NAME].toString();

        if (operation.compare(stringForOpType(OP_TYPE_APPROVAL_OUTCOME_REQUEST)) == 0) {

            int localId = payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper localId" << payload[EMAIL_ADAPTER_KEY_LOCAL_ID] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - localId *** : %1").arg(payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }
            int taskId = payload[EMAIL_ADAPTER_KEY_TASK_ID].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper taskId" << payload[EMAIL_ADAPTER_KEY_TASK_ID] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - taskId *** : %1").arg(payload[EMAIL_ADAPTER_KEY_TASK_ID].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            int task_type = payload[EMAIL_ADAPTER_KEY_TASK_TYPE].toInt(&ok);
            Q_UNUSED(task_type)
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper task_type" << payload[EMAIL_ADAPTER_KEY_TASK_TYPE] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - task_type *** : %1").arg(payload[EMAIL_ADAPTER_KEY_TASK_TYPE].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            QString requestStatusParam = payload[EMAIL_ADAPTER_KEY_STATUS].toString();

            int requestStatus = requestStatusParam.toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper requestStatus" << payload[EMAIL_ADAPTER_KEY_STATUS] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - requestId *** : %1").arg(payload[EMAIL_ADAPTER_KEY_STATUS].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            int originating_op_id = payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper originating_op_id" << payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - originating_op_id *** : %1").arg(payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            QString note = payload[EMAIL_ADAPTER_KEY_NOTE].toString();

            qDebug() << "EEEE OP_TYPE_APPROVAL_OUTCOME_REQUEST localId=" << localId << " requestStatus=" << requestStatus << " taskId=" << taskId << " originating_op_id=" << originating_op_id << endl;

            if (requestStatus == STATUS_BOOKING_APPROVED) {

                int half_days = _rollBack->getHalfDays(localId, originating_op_id);
                _rollBack->deleteItem(localId, originating_op_id);

                emit inApprovalOutcomeRequ(localId, STATUS_BOOKING_APPROVED, note, originating_op_id, half_days);

            } else if (requestStatus == STATUS_BOOKING_REJECTED) {

                int year = _rollBack->getYear(localId, originating_op_id);
                int half_days = _rollBack->getHalfDays(localId, originating_op_id);
                _cache->increment(year, half_days);
                _rollBack->deleteItem(localId, originating_op_id);

                emit inApprovalOutcomeRequ(localId, STATUS_BOOKING_REJECTED, note, originating_op_id, half_days);

            } else if (requestStatus == STATUS_CANCELLATION_OK) {

                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_OK - Entitlement check" << endl;

                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_OK - localId=" << localId << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_OK - originating_op_id=" << originating_op_id << endl;

                int half_days = _rollBack->getHalfDays(localId, originating_op_id);

                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_OK - halfDays from Rollback=" << half_days << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_OK - Deleting Rollback" << endl;

                _rollBack->deleteItem(localId, originating_op_id);

                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_OK - Approval to GUI for origOpId=" << originating_op_id << ", halfdays=" << half_days << endl;

                emit inApprovalOutcomeRequ(localId, STATUS_CANCELLATION_OK, note, originating_op_id, half_days);

            } else if (requestStatus == STATUS_CANCELLATION_REJECTED) {

                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_REJECTED - Entitlement check" << endl;

                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_REJECTED - localId=" << localId << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_REJECTED - originating_op_id=" << originating_op_id << endl;

                int year = _rollBack->getYear(localId, originating_op_id);
                int half_days = _rollBack->getHalfDays(localId, originating_op_id);

                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_REJECTED - year from Rollback=" << year << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_REJECTED - halfDays from Rollback=" << half_days << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_REJECTED - Decrementing Entitlement cache for year=" << year << ",by " << half_days << endl;

                _cache->decrement(year, half_days);

                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_REJECTED - Deleting Rollback for localId=" << localId << ", origOpId=" << originating_op_id << endl;

                _rollBack->deleteItem(localId, originating_op_id);

                qDebug() << "EEEE EmailAdapter - STATUS_CANCELLATION_REJECTED - Rejection to GUI for origOpId=" << originating_op_id << ", halfdays=" << half_days << endl;

                emit inApprovalOutcomeRequ(localId, STATUS_CANCELLATION_REJECTED, note, originating_op_id, half_days);

            } else if (requestStatus == STATUS_UPDATE_OK) {

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_OK - Entitlement check" << endl;

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_OK - localId=" << localId << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_OK - originating_op_id=" << originating_op_id << endl;

                int newHalfDays = _rollBack->getNewHalfDays(localId, originating_op_id);

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_OK - newHalfDays from Rollback=" << newHalfDays << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_OK - Deleting Rollback for localId=" << localId << ", origOpId=" << originating_op_id << endl;

                _rollBack->deleteItem(localId, originating_op_id);

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_OK - Approval to GUI for origOpId=" << originating_op_id << ", newHalfdays=" << newHalfDays << endl;

                emit inApprovalOutcomeRequ(localId, STATUS_UPDATE_OK, note, originating_op_id, newHalfDays);

            } else if (requestStatus == STATUS_UPDATE_REJECTED) {

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - Entitlement check" << endl;

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - localId=" << localId << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - originating_op_id=" << originating_op_id << endl;

                int oldYear = _rollBack->getOldYear(localId, originating_op_id);
                int oldHalfDayCount = _rollBack->getOldHalfDays(localId, originating_op_id);
                int newYear = _rollBack->getNewYear(localId, originating_op_id);
                int newHalfDayCount = _rollBack->getNewHalfDays(localId, originating_op_id);

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - old year from Rollback=" << oldYear << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - old half day count from Rollback=" << oldHalfDayCount << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - new year from Rollback=" << newYear << endl;
                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - new half day count from Rollback=" << newHalfDayCount << endl;

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - Decrementing Entitlement cache for oldYear=" << oldYear << ",by oldHalfDayCount=" << oldHalfDayCount << endl;

                _cache->decrement(oldYear, oldHalfDayCount);

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - Incrementing Entitlement cache for newYear=" << newYear << ",by newHalfDayCount=" << newHalfDayCount << endl;

                _cache->increment(newYear, newHalfDayCount);

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - Deleting Rollback for localId=" << localId << ", origOpId=" << originating_op_id << endl;

                _rollBack->deleteItem(localId, originating_op_id);

                qDebug() << "EEEE EmailAdapter - STATUS_UPDATE_REJECTED - Rejectiom to GUI for origOpId=" << originating_op_id << ", oldHalfDayCount=" << oldHalfDayCount << endl;

                emit inApprovalOutcomeRequ(localId, STATUS_UPDATE_REJECTED, note, originating_op_id, oldHalfDayCount);

            } else {

                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - unrecognised outcome" << requestStatus << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - requestStatus *** : %1").arg(requestStatus));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

        } else if (operation.compare(stringForOpType(OP_TYPE_HALF_DAYS_USED_REQUEST)) == 0) {

            int localId = payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toInt(&ok);

            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper localId" << payload[EMAIL_ADAPTER_KEY_LOCAL_ID] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - localId *** : %1").arg(payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            int used = payload[EMAIL_ADAPTER_KEY_USED].toInt(&ok);

            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper used" << payload[EMAIL_ADAPTER_KEY_USED] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - used *** : %1").arg(payload[EMAIL_ADAPTER_KEY_USED].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            emit inHalfDaysUsedRequ(localId, used);

        } else if (operation.compare(stringForOpType(OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE)) == 0) {

            int year = payload[EMAIL_ADAPTER_KEY_YEAR].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper year" << payload[EMAIL_ADAPTER_KEY_YEAR] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - year *** : %1").arg(payload[EMAIL_ADAPTER_KEY_YEAR].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }
            int total = payload[EMAIL_ADAPTER_KEY_TOTAL].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper total" << payload[EMAIL_ADAPTER_KEY_TOTAL] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - total *** : %1").arg(payload[EMAIL_ADAPTER_KEY_TOTAL].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }
            int current = payload[EMAIL_ADAPTER_KEY_CURRENT].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper current" << payload[EMAIL_ADAPTER_KEY_CURRENT] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - current *** : %1").arg(payload[EMAIL_ADAPTER_KEY_CURRENT].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            QString opStatus = payload[EMAIL_ADAPTER_KEY_OPERATION_STATUS].toString();

            if (opStatus.compare(stringForStatus(STATUS_COMPLETED)) == 0) {
                _cache->replaceOrAddItem(year, current, total);
                emit inAnnualEntitlementResp(STATUS_COMPLETED, year, total, current);
            } else if (opStatus.compare(stringForStatus(STATUS_FAILED)) == 0) {
                emit inAnnualEntitlementResp(STATUS_FAILED, year, total, current);
            } else {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - unrecognised outcome" << opStatus << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - opStatus *** : %1").arg(opStatus));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

        } else if (operation.compare(stringForOpType(OP_TYPE_APPROVAL_TASK_REQUEST)) == 0) {

            int taskType = payload[EMAIL_ADAPTER_KEY_TASK_TYPE].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper taskType" << payload[EMAIL_ADAPTER_KEY_TASK_TYPE] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - taskType *** : %1").arg(payload[EMAIL_ADAPTER_KEY_TASK_TYPE].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }
            if (taskType == TASK_TYPE_BOOKING_APPROVAL) {
                QString userId = payload[EMAIL_ADAPTER_KEY_USER_ID].toString();
                int leaveYear = payload[EMAIL_ADAPTER_KEY_LEAVE_YEAR].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper leaveYear" << payload[EMAIL_ADAPTER_KEY_LEAVE_YEAR] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - leaveYear *** : %1").arg(payload[EMAIL_ADAPTER_KEY_LEAVE_YEAR].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                qint64 fromDate = payload[EMAIL_ADAPTER_KEY_FROM_DATE].toLongLong(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper fromDate" << payload[EMAIL_ADAPTER_KEY_FROM_DATE] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - fromDate *** : %1").arg(payload[EMAIL_ADAPTER_KEY_FROM_DATE].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                qint64 toDate = payload[EMAIL_ADAPTER_KEY_TO_DATE].toLongLong(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper toDate" << payload[EMAIL_ADAPTER_KEY_TO_DATE] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - toDate *** : %1").arg(payload[EMAIL_ADAPTER_KEY_TO_DATE].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                bool firstDayHalf = payload[EMAIL_ADAPTER_KEY_FIRST_DAY_HALF].toBool();
                bool lastDayHalf = payload[EMAIL_ADAPTER_KEY_LAST_DAY_HALF].toBool();

                int dayHalf = payload[EMAIL_ADAPTER_KEY_DAY_HALF].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper dayHalf" << payload[EMAIL_ADAPTER_KEY_DAY_HALF] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - dayHalf *** : %1").arg(payload[EMAIL_ADAPTER_KEY_DAY_HALF].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                int half_days = payload[EMAIL_ADAPTER_KEY_HALF_DAYS].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper half_days value" << payload[EMAIL_ADAPTER_KEY_HALF_DAYS] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - half_days *** : %1").arg(payload[EMAIL_ADAPTER_KEY_HALF_DAYS].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                int local_request_id = payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper local-id value" << payload[EMAIL_ADAPTER_KEY_LOCAL_ID] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - local-id *** : %1").arg(payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                QString note = payload[EMAIL_ADAPTER_KEY_NOTE].toString();

                int originating_op_id = payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper originating-op-id value" << payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - originating-op-id *** : %1").arg(payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                qDebug() << "EEEE EmailAdapter received approval task: " << userId << "," << leaveYear << "," << fromDate << "," << toDate << "," << firstDayHalf << "," << lastDayHalf << ","
                        << dayHalf << "," << half_days << "," << local_request_id << "," << note << "," << originating_op_id;

                _userIdCache->add(userId, senderEmail);

                emit inApprovalTaskRequ(taskType, userId, leaveYear, fromDate, toDate, firstDayHalf, lastDayHalf, dayHalf, half_days, local_request_id, note, originating_op_id);

            } else if (taskType == TASK_TYPE_CANCEL_APPROVAL) {
                int local_request_id = payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper local-id value" << payload[EMAIL_ADAPTER_KEY_LOCAL_ID] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - local-id *** : %1").arg(payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                QString userId = payload[EMAIL_ADAPTER_KEY_USER_ID].toString();
                int leaveYear = payload[EMAIL_ADAPTER_KEY_LEAVE_YEAR].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper leaveYear" << payload[EMAIL_ADAPTER_KEY_LEAVE_YEAR] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - leaveYear *** : %1").arg(payload[EMAIL_ADAPTER_KEY_LEAVE_YEAR].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                qint64 fromDate = payload[EMAIL_ADAPTER_KEY_FROM_DATE].toLongLong(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper fromDate" << payload[EMAIL_ADAPTER_KEY_FROM_DATE] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - fromDate *** : %1").arg(payload[EMAIL_ADAPTER_KEY_FROM_DATE].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                qint64 toDate = payload[EMAIL_ADAPTER_KEY_TO_DATE].toLongLong(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper toDate" << payload[EMAIL_ADAPTER_KEY_TO_DATE] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - toDate *** : %1").arg(payload[EMAIL_ADAPTER_KEY_TO_DATE].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                bool firstDayHalf = payload[EMAIL_ADAPTER_KEY_FIRST_DAY_HALF].toBool();
                bool lastDayHalf = payload[EMAIL_ADAPTER_KEY_LAST_DAY_HALF].toBool();

                int dayHalf = payload[EMAIL_ADAPTER_KEY_DAY_HALF].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper dayHalf" << payload[EMAIL_ADAPTER_KEY_DAY_HALF] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - dayHalf *** : %1").arg(payload[EMAIL_ADAPTER_KEY_DAY_HALF].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                QString note = payload[EMAIL_ADAPTER_KEY_NOTE].toString();

                int half_days = payload[EMAIL_ADAPTER_KEY_HALF_DAYS].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper half_days value" << payload[EMAIL_ADAPTER_KEY_HALF_DAYS] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - half_days *** : %1").arg(payload[EMAIL_ADAPTER_KEY_HALF_DAYS].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                int originating_op_id = payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper originating-op-id value" << payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - originating-op-id *** : %1").arg(payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                qDebug() << "EEEE EmailAdapter::processJsonInboundMessages() - emitting inApprovalTaskRequ() for cancellation request for local_request_id=" << local_request_id;

                _userIdCache->add(userId, senderEmail);

                emit inApprovalTaskRequ(taskType, local_request_id, leaveYear, fromDate, toDate, firstDayHalf, lastDayHalf, dayHalf, note, userId, half_days, originating_op_id);

            } else if (taskType == TASK_TYPE_UPDATE_APPROVAL) {

                int oldYear = payload[EMAIL_ADAPTER_KEY_LEAVE_YEAR].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper oldYear" << payload[EMAIL_ADAPTER_KEY_LEAVE_YEAR] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - oldYear *** : %1").arg(payload[EMAIL_ADAPTER_KEY_LEAVE_YEAR].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                qint64 oldFromDate = payload[EMAIL_ADAPTER_KEY_FROM_DATE].toLongLong(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper oldFromDate" << payload[EMAIL_ADAPTER_KEY_FROM_DATE] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - oldFromDate *** : %1").arg(payload[EMAIL_ADAPTER_KEY_FROM_DATE].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                qint64 oldToDate = payload[EMAIL_ADAPTER_KEY_TO_DATE].toLongLong(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper oldToDate" << payload[EMAIL_ADAPTER_KEY_TO_DATE] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - oldToDate *** : %1").arg(payload[EMAIL_ADAPTER_KEY_TO_DATE].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                bool oldFirstDayHalf = payload[EMAIL_ADAPTER_KEY_FIRST_DAY_HALF].toBool();
                bool oldLastDayHalf = payload[EMAIL_ADAPTER_KEY_LAST_DAY_HALF].toBool();

                int oldDayHalf = payload[EMAIL_ADAPTER_KEY_DAY_HALF].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper oldDayHalf" << payload[EMAIL_ADAPTER_KEY_DAY_HALF] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - oldDayHalf *** : %1").arg(payload[EMAIL_ADAPTER_KEY_DAY_HALF].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                int newYear = payload[EMAIL_ADAPTER_KEY_NEW_LEAVE_YEAR].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper newYear" << payload[EMAIL_ADAPTER_KEY_NEW_LEAVE_YEAR] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - newYear *** : %1").arg(payload[EMAIL_ADAPTER_KEY_NEW_LEAVE_YEAR].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                qint64 newFromDate = payload[EMAIL_ADAPTER_KEY_NEW_FROM_DATE].toLongLong(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper newFromDate" << payload[EMAIL_ADAPTER_KEY_NEW_FROM_DATE] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - newFromDate *** : %1").arg(payload[EMAIL_ADAPTER_KEY_NEW_FROM_DATE].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                qint64 newToDate = payload[EMAIL_ADAPTER_KEY_NEW_TO_DATE].toLongLong(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper newToDate" << payload[EMAIL_ADAPTER_KEY_NEW_TO_DATE] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - newToDate *** : %1").arg(payload[EMAIL_ADAPTER_KEY_NEW_TO_DATE].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                bool newFirstDayHalf = payload[EMAIL_ADAPTER_KEY_NEW_FIRST_DAY_HALF].toBool();
                bool newLastDayHalf = payload[EMAIL_ADAPTER_KEY_NEW_LAST_DAY_HALF].toBool();

                int newDayHalf = payload[EMAIL_ADAPTER_KEY_NEW_DAY_HALF].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper newDayHalf" << payload[EMAIL_ADAPTER_KEY_NEW_DAY_HALF] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - newDayHalf *** : %1").arg(payload[EMAIL_ADAPTER_KEY_NEW_DAY_HALF].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                int local_request_id = payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper local-id value" << payload[EMAIL_ADAPTER_KEY_LOCAL_ID] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - local-id *** : %1").arg(payload[EMAIL_ADAPTER_KEY_LOCAL_ID].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }
                QString userId = payload[EMAIL_ADAPTER_KEY_USER_ID].toString();

                int half_days = payload[EMAIL_ADAPTER_KEY_HALF_DAYS].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper half_days value" << payload[EMAIL_ADAPTER_KEY_HALF_DAYS] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - half_days *** : %1").arg(payload[EMAIL_ADAPTER_KEY_HALF_DAYS].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                qDebug() << "EEEE EmailAdapter:: processJsonInboundMessages() - update booking, half_days=" << half_days;

                int originating_op_id = payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID].toInt(&ok);
                if (!ok) {
                    qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper originating-op-id value" << payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID] << endl;
                    replyWithErrorMessage(messageId, QString("*** Parse Error - originating-op-id *** : %1").arg(payload[EMAIL_ADAPTER_KEY_ORIGINATING_OP_ID].toString()));
                    deleteMessage(messageId, indexPath, entry);
                    continue;
                }

                _userIdCache->add(userId, senderEmail);

                emit inApprovalTaskRequ(taskType, local_request_id, oldYear, newYear, oldFromDate, newFromDate, oldToDate, newToDate, oldFirstDayHalf, newFirstDayHalf, oldLastDayHalf, newLastDayHalf,
                        oldDayHalf, newDayHalf, userId, half_days, originating_op_id);
            }

        } else if (operation.compare(stringForOpType(OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE)) == 0) {

            int taskId = payload[EMAIL_ADAPTER_KEY_TASK_ID].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper TaskId" << payload[EMAIL_ADAPTER_KEY_TASK_ID] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - taskId *** : %1").arg(payload[EMAIL_ADAPTER_KEY_TASK_ID].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            int opStatus = payload[EMAIL_ADAPTER_KEY_OPERATION_STATUS].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper opStatus" << payload[EMAIL_ADAPTER_KEY_OPERATION_STATUS] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - opStatus *** : %1").arg(payload[EMAIL_ADAPTER_KEY_OPERATION_STATUS].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            emit inApprovalTaskOutcomeResp(taskId, opStatus);

        } else if (operation.compare(stringForOpType(OP_TYPE_APPROVAL_TASK_RESPONSE)) == 0) {

            int taskId = payload[EMAIL_ADAPTER_KEY_TASK_ID].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper TaskId" << payload[EMAIL_ADAPTER_KEY_TASK_ID] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - taskId *** : %1").arg(payload[EMAIL_ADAPTER_KEY_TASK_ID].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }

            int opStatus = payload[EMAIL_ADAPTER_KEY_OPERATION_STATUS].toInt(&ok);
            if (!ok) {
                qWarning() << "EEEE EmailAdapter::processJsonInboundMessages() - improper opStatus" << payload[EMAIL_ADAPTER_KEY_OPERATION_STATUS] << endl;
                replyWithErrorMessage(messageId, QString("*** Parse Error - opStatus *** : %1").arg(payload[EMAIL_ADAPTER_KEY_OPERATION_STATUS].toString()));
                deleteMessage(messageId, indexPath, entry);
                continue;
            }
            qDebug() << "EEEE EmailAdapter received approval task response: " << taskId << opStatus;
        } else {
            qWarning() << "EEEE EmailAdapter received approval task response whilst running in Integration Mode - incorrect state/event";
        }
        deleteMessage(messageId, indexPath, entry);
    }
}

void EmailAdapter::deleteMessage(MessageKey id, QVariantList indexPath, QVariantMap entry)
{
    qWarning() << "EEEE EmailAdapter::deleteMessage(id, indexPath, entry) - deleting message:" << id << endl;

    entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED] = true;

    bool completed = entry[EMAIL_ADAPTER_KEY_MESSAGE_COMPLETED].toBool();
    bool toBeRemoved = entry[EMAIL_ADAPTER_KEY_TO_BE_REMOVED].toBool();
    MessageKey messageId = entry[EMAIL_ADAPTER_KEY_MESSAGE_ID].toLongLong();
    bool messageBeingProcessed = entry[EMAIL_ADAPTER_KEY_MESSAGE_BEING_PROCESSED].toBool();

    qDebug() << "EEEE deleteMessage() - DataModel item requesting updating for deletion    at indexPath=" << indexPath << endl;
    qDebug() << "EEEE deleteMessage() - DataModel item requesting updating for deletion     toBeRemoved=" << toBeRemoved << endl;
    qDebug() << "EEEE deleteMessage() - DataModel item requesting updating for deletion       completed=" << completed << endl;
    qDebug() << "EEEE deleteMessage() - DataModel item requesting updating for deletion       messageId=" << messageId << endl;
    qDebug() << "EEEE deleteMessage() - DataModel item requesting updating for deletion being processed=" << messageBeingProcessed << endl;

    _model->updateItem(indexPath, entry);

    qDebug() << "EEEE deleteMessage() - Message removal being requested=" << id << endl;

    _messageService->remove(_currentAccount.id(), id);
}

void EmailAdapter::deleteMessage(MessageKey id)
{
    qWarning() << "EEEE EmailAdapter::deleteMessage() - deleting message:" << id << endl;

    _messageService->remove(_currentAccount.id(), id);
}

void EmailAdapter::replyWithErrorMessage(MessageKey id, const QString &errorMessage)
{
    qWarning() << "EEEE EmailAdapter::replyWithErrorMessage() - Error Response to Message Id:" << id << "Text:" << errorMessage << endl;
    qWarning() << "EEEE EmailAdapter::replyWithErrorMessage() - Error Response suppressed in this build" << endl;

    /*
     MessageBuilder *builder = MessageBuilder::create(_currentAccount.id(), _messageService->message(_currentAccount.id(), id));

     QByteArray bodyData = QByteArray().append(errorMessage);
     builder->body(MessageBody::PlainText, bodyData);

     MessageKey messageId = _messageService->send(_currentAccount.id(), *builder);

     qDebug() << "EEEE EmailAdapter::replyWithErrorMessage() - Reply Message Id:" << messageId << endl;
     */
}

bool EmailAdapter::constructAndSendMessage(QVariantMap parameters, const QString &recipientEmail, const QString &recipientName)
{
    QVariant payload = QVariant(parameters);
    QString jsonPayload;
    JsonDataAccess jda;

    qDebug() << "EEEE EmailAdapter::constructAndSendMessage() - creating message" << endl;

    jda.saveToBuffer(payload, &jsonPayload);
    if (!jda.hasError()) {
        sendJsonMessage(jsonPayload, recipientEmail, recipientName);
        return true;
    } else {
        qWarning() << "EEEE EmailAdapter::constructAndSendMessage() - error creating payload" << parameters << endl;
        return false;
    }
}

void EmailAdapter::sendJsonMessage(const QString &jsonPayload, const QString &recipientEmail, const QString &recipientName)
{
    MessageContact recipient;

    qDebug() << "EEEE sendJsonMessage";
    qDebug() << "EEEE creating MessageContact with recipient name=" << recipientName << " and recipient email=" << recipientEmail;
    recipient = MessageContact(-1, MessageContact::To, recipientName, recipientEmail);

    QString subject = EMAIL_ADAPTER_SERVICE_UUID;
    QByteArray bodyData = QByteArray().append(EMAIL_ADAPTER_JSON_SECTION_START_MARKER).append("\n").append(jsonPayload).append(EMAIL_ADAPTER_JSON_SECTION_END_MARKER).append("\n");
    sendMessage(recipient, subject, bodyData);
}

void EmailAdapter::sendMessage(MessageContact recipient, QString subject, QByteArray body)
{
    qDebug() << "EEEE sendMessage";

    MessageBuilder *builder = MessageBuilder::create(_currentAccount.id());
    builder->subject(subject);
    builder->removeAllRecipients();
    builder->addRecipient(recipient);
    builder->body(MessageBody::PlainText, body);

    MessageKey messageId = _messageService->send(_currentAccount.id(), *builder);

    qDebug() << "EEEE EmailAdapter::sendMessage() - Message Id:" << messageId << endl;
    qDebug() << "EEEE EmailAdapter::sendMessage() - To:" << recipient.address() << endl;
    qDebug() << "EEEE EmailAdapter::sendMessage() - Subject:" << subject << endl;
    qDebug() << "EEEE EmailAdapter::sendMessage() - Body:" << body << endl;
}

const QString EmailAdapter::toIsoFormat(qint64 date) const
{
    QDateTime qDateTime;
    qDateTime.setMSecsSinceEpoch(date);
    return qDateTime.date().toString(Qt::ISODate);
}

qint64 EmailAdapter::fromIsoFormat(const QString &date) const
{
    return QDateTime::fromString(date, Qt::ISODate).toMSecsSinceEpoch();
}

void EmailAdapter::setUpStringMaps()
{
    _mapOpType->insert(OP_TYPE_SUBMIT_BOOKING_REQUEST, "OP_TYPE_SUBMIT_BOOKING_REQUEST");
    _mapOpType->insert(OP_TYPE_SUBMIT_BOOKING_RESPONSE, "OP_TYPE_SUBMIT_BOOKING_RESPONSE");
    _mapOpType->insert(OP_TYPE_HALF_DAYS_USED_REQUEST, "OP_TYPE_HALF_DAYS_USED_REQUEST");
    _mapOpType->insert(OP_TYPE_HALF_DAYS_USED_RESPONSE, "OP_TYPE_HALF_DAYS_USED_RESPONSE");
    _mapOpType->insert(OP_TYPE_APPROVAL_OUTCOME_REQUEST, "OP_TYPE_APPROVAL_OUTCOME_REQUEST");
    _mapOpType->insert(OP_TYPE_APPROVAL_OUTCOME_RESPONSE, "OP_TYPE_APPROVAL_OUTCOME_RESPONSE");
    _mapOpType->insert(OP_TYPE_CANCELLATION_REQUEST, "OP_TYPE_CANCELLATION_REQUEST");
    _mapOpType->insert(OP_TYPE_CANCELLATION_RESPONSE, "OP_TYPE_CANCELLATION_RESPONSE");
    _mapOpType->insert(OP_TYPE_CANCELLATION_OUTCOME_REQUEST, "OP_TYPE_CANCELLATION_OUTCOME_REQUEST");
    _mapOpType->insert(OP_TYPE_CANCELLATION_OUTCOME_RESPONSE, "OP_TYPE_CANCELLATION_OUTCOME_RESPONSE");
    _mapOpType->insert(OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST, "OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST");
    _mapOpType->insert(OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE, "OP_TYPE_ANNUAL_ENTITLEMENT_RESPONSE");
    _mapOpType->insert(OP_TYPE_UPDATE_BOOKING_REQUEST, "OP_TYPE_UPDATE_BOOKING_REQUEST");
    _mapOpType->insert(OP_TYPE_UPDATE_BOOKING_RESPONSE, "OP_TYPE_UPDATE_BOOKING_RESPONSE");
    _mapOpType->insert(OP_TYPE_UPDATE_OUTCOME_REQUEST, "OP_TYPE_UPDATE_OUTCOME_REQUEST");
    _mapOpType->insert(OP_TYPE_UPDATE_OUTCOME_RESPONSE, "OP_TYPE_UPDATE_OUTCOME_RESPONSE");
    _mapOpType->insert(OP_TYPE_APPROVAL_TASK_REQUEST, "OP_TYPE_APPROVAL_TASK_REQUEST");
    _mapOpType->insert(OP_TYPE_APPROVAL_TASK_RESPONSE, "OP_TYPE_APPROVAL_TASK_RESPONSE");
    _mapOpType->insert(OP_TYPE_APPROVAL_TASK_OUTCOME_REQUEST, "OP_TYPE_APPROVAL_TASK_OUTCOME_REQUEST");
    _mapOpType->insert(OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE, "OP_TYPE_APPROVAL_TASK_OUTCOME_RESPONSE");

    _mapStatus->insert(STATUS_BOOKING_PENDING, "STATUS_BOOKING_PENDING");
    _mapStatus->insert(STATUS_BOOKING_SUBMITTED, "STATUS_BOOKING_SUBMITTED");
    _mapStatus->insert(STATUS_BOOKING_APPROVED, "STATUS_BOOKING_APPROVED");
    _mapStatus->insert(STATUS_BOOKING_REJECTED, "STATUS_BOOKING_REJECTED");
    _mapStatus->insert(STATUS_CANCELLATION_PENDING, "STATUS_CANCELLATION_PENDING");
    _mapStatus->insert(STATUS_CANCELLATION_SUBMITTED, "STATUS_CANCELLATION_SUBMITTED");
    _mapStatus->insert(STATUS_CANCELLATION_OK, "STATUS_CANCELLATION_OK");
    _mapStatus->insert(STATUS_CANCELLATION_REJECTED, "STATUS_CANCELLATION_REJECTED");
    _mapStatus->insert(STATUS_UPDATE_PENDING, "STATUS_UPDATE_PENDING");
    _mapStatus->insert(STATUS_UPDATE_SUBMITTED, "STATUS_UPDATE_SUBMITTED");
    _mapStatus->insert(STATUS_UPDATE_OK, "STATUS_UPDATE_OK");
    _mapStatus->insert(STATUS_UPDATE_REJECTED, "STATUS_UPDATE_REJECTED");
    _mapStatus->insert(STATUS_COMPLETED, "STATUS_COMPLETED");
    _mapStatus->insert(STATUS_FAILED, "STATUS_FAILED");

    _mapTaskType->insert(TASK_APPROVAL_DECISION_APPROVED, "TASK_APPROVAL_DECISION_APPROVED");
    _mapTaskType->insert(TASK_APPROVAL_DECISION_REJECTED, "TASK_APPROVAL_DECISION_REJECTED");
}

QString EmailAdapter::stringForOpType(int key)
{
    if (_mapOpType->contains(key)) {
        return _mapOpType->value(key);
    } else {
        return "UNIMPLEMENTED OPERATION";
    }
}

QString EmailAdapter::stringForStatus(int key)
{
    if (_mapStatus->contains(key)) {
        return _mapStatus->value(key);
    } else {
        return "UNIMPLEMENTED STATUS";
    }
}

QString EmailAdapter::stringForTaskType(int key)
{
    if (_mapTaskType->contains(key)) {
        return _mapTaskType->value(key);
    } else {
        return "UNIMPLEMENTED STATUS";
    }
}

