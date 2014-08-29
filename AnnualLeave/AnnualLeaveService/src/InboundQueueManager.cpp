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

#include <InboundQueueManager.hpp>
#include <QDate>

InboundQueueManager* InboundQueueManager::_instance;

InboundQueueManager::InboundQueueManager(QObject *parent)
	: QObject(parent)
	, _initialised(false)
	, _adapterImpl(AdapterImpl::getInstance(this))
	, _ops(Operations::getInstance(this))
	, _dumpQueueEnabled(false)
	, _settings(Settings::getInstance(this))
{
    _iAdapter = _adapterImpl;

    if (!QObject::connect(_settings, SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()))) {
        qWarning() << "SSSS IN InboundQueueManager::InboundQueueManager() - connect failed - onSettingsChanged" << strerror(errno) << endl;
    }
}

InboundQueueManager::~InboundQueueManager()
{
    _instance = NULL;
    delete _adapterImpl;
}

InboundQueueManager *InboundQueueManager::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new InboundQueueManager(parent);
    }
    return _instance;
}

int InboundQueueManager::start()
{
    // TODO: may need some initialising here -- placeholder for the moment
    return EOK;
}

int InboundQueueManager::stop()
{
    // TODO: may need some finalising here -- placeholder for the moment
    return EOK;
}

void InboundQueueManager::onInSubmitBookingResp(int localId, int status)
{
    qDebug() << "SSSS onInSubmitBookingResp";
    OpSubmitBookingResp response;
    dumpQueue();
    response.setOpStatus(OP_STATUS_OK);
    response.setRequestStatus(status);
    response.setLocalRequestId(localId);
    _ops->submit_Booking_Resp(response);

    dumpQueue();
}

void InboundQueueManager::onInApprovalOutcomeRequ(int localId, int status, const QString &note, int originating_op_id, int half_days)
{
    Q_UNUSED(note)

    qDebug() << "SSSS onInApprovalOutcomeRequ localId=" << localId << " status=" << status << " originating_op_id" << originating_op_id << " half_days=" << half_days;
    dumpQueue();

    int task_type = -1;
    OpApprovalOutcomeRequ request;
    request.setLocalRequestId(localId);
    request.setRequestStatus(status);
    request.setOriginatingOpId(originating_op_id);
    request.setHalfDays(half_days);
    switch (status) {
        case STATUS_BOOKING_APPROVED:
            task_type = TASK_TYPE_BOOKING_APPROVAL;
            break;
        case STATUS_BOOKING_REJECTED:
            task_type = TASK_TYPE_BOOKING_APPROVAL;
            break;
        case STATUS_CANCELLATION_OK:
            task_type = TASK_TYPE_CANCEL_APPROVAL;
            break;
        case STATUS_CANCELLATION_REJECTED:
            task_type = TASK_TYPE_CANCEL_APPROVAL;
            break;
        case STATUS_UPDATE_OK:
            task_type = TASK_TYPE_UPDATE_APPROVAL;
            break;
        case STATUS_UPDATE_REJECTED:
            task_type = TASK_TYPE_UPDATE_APPROVAL;
            break;
    }
    request.setTaskType(task_type);
    _ops->approval_outcome_Requ(request);

    dumpQueue();
}

void InboundQueueManager::onInHalfDaysUsedRequ(int localId, int used)
{
    qDebug() << "SSSS onInHalfDaysUsedRequ";
    dumpQueue();

    OpHalfDaysUsedRequ request;
    request.setLocalRequestId(localId);
    request.setHalfDays(used);
    _ops->half_days_used_Requ(request);

    dumpQueue();
}

void InboundQueueManager::onInCancelBookingResp(int localId, int status)
{
    qDebug() << "SSSS onInCancelBookingResp";
    OpCancelBookingResp response;

    dumpQueue();

    response.setOpStatus(OP_STATUS_OK);
    response.setRequestStatus(status);
    response.setLocalRequestId(localId);
    _ops->cancel_Booking_Resp(response);

    dumpQueue();
}

void InboundQueueManager::onInAnnualEntitlementResp(int status, int year, int total, int current)
{
    qDebug() << "SSSS onInAnnualEntitlementResp";
    OpAnnualEntitlementResp response;

    dumpQueue();

    response.setOpStatus(OP_STATUS_OK);
    response.setRequestStatus(status);
    response.setLeaveYear(year);
    response.setCurrentEntitlementHalfDays(current);
    response.setTotalEntitlementHalfDays(total);
    _ops->annual_entitlement_Resp(response);

    dumpQueue();
}

void InboundQueueManager::onInUpdateBookingResp(int localId, int status, qint64 newFromDate, qint64 newToDate)
{
    Q_UNUSED(newFromDate)
    Q_UNUSED(newToDate)

    qDebug() << "SSSS onInUpdateBookingResp";
    OpUpdateBookingResp response;

    dumpQueue();

    response.setOpStatus(OP_STATUS_OK);
    response.setRequestStatus(status);
    response.setLocalRequestId(localId);
    _ops->update_Booking_Resp(response);

    dumpQueue();
}


void InboundQueueManager::onInApprovalTaskRequ(int taskType, const QString &userId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, int half_days, int local_request_id, const QString &note, int originating_op_id)
{
    qDebug() << "SSSS IN onInApprovalTaskRequ() [BOOKING]";

    qDebug() << "SSSS IN onInApprovalTaskRequ() - taskType:" << taskType << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - userId:" << userId << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - leaveYear:" << leaveYear << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - fromDate:" << fromDate << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - toDate:" << toDate << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - firstDayHalf:" << firstDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - lastDayHalf:" << lastDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - dayHalf:" << dayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - half_days:" << half_days << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - local_request_id:" << local_request_id << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - note:" << note << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - originating_op_id:" << originating_op_id << endl;

    dumpQueue();

    OpApprovalTaskRequ request;

    if (taskType == TASK_TYPE_BOOKING_APPROVAL) {

    	request.setTaskType(taskType);
        request.setLeaveYear(leaveYear);
        request.setFromDate(fromDate);
        request.setToDate(toDate);
        request.setFirstDayHalf(firstDayHalf);
        request.setLastDayHalf(lastDayHalf);
        request.setDayHalf(dayHalf);
        request.setHalfDays(half_days);
        request.setUserId(userId);
        request.setLocalRequestId(local_request_id);
        request.setNote(note);
        request.setOriginatingOpId(originating_op_id);
        _ops->approval_task_Requ(request);
    }

    dumpQueue();
}

void InboundQueueManager::onInApprovalTaskRequ(int taskType, int localId, int oldYear, int newYear, qint64 oldFromDate, qint64 newFromDate,
                          qint64 oldToDate, qint64 newToDate, bool oldFirstDayHalf, bool newFirstDayHalf,
                          bool oldLastDayHalf, bool newLastDayHalf, int oldDayHalf, int newDayHalf, const QString &userId, int halfDays, int originating_op_id)
{
    qDebug() << "SSSS IN onInApprovalTaskRequ() [UPDATE]";

    qDebug() << "SSSS IN onInApprovalTaskRequ() - taskType:" << taskType << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - oldYear:" << oldYear << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - newYear:" << newYear << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - oldFromDate:" << oldFromDate << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - newFromDate:" << newFromDate << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - oldToDate:" << oldToDate << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - newToDate:" << newToDate << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - oldFirstDayHalf:" << oldFirstDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - newFirstDayHalf:" << newFirstDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - oldLastDayHalf:" << oldLastDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - newLastDayHalf:" << newLastDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - oldDayHalf:" << oldDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - newDayHalf:" << newDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - localId:" << localId << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - userId:" << userId << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - halfDays:" << halfDays << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - originating_op_id:" << originating_op_id << endl;

    dumpQueue();

    OpApprovalTaskRequ request;

    if (taskType == TASK_TYPE_UPDATE_APPROVAL) {

    	request.setTaskType(taskType);
    	request.setLeaveYear(oldYear);
    	request.setFromDate(oldFromDate);
    	request.setToDate(oldToDate);
    	request.setFirstDayHalf(oldFirstDayHalf);
    	request.setLastDayHalf(oldLastDayHalf);
    	request.setDayHalf(oldDayHalf);
    	request.setLocalRequestId(localId);
        request.setUserId(userId);
        request.setHalfDays(halfDays);

        request.setNewLeaveYear(newYear);
    	request.setNewFromDate(newFromDate);
    	request.setNewToDate(newToDate);
    	request.setNewFirstDayHalf(newFirstDayHalf);
    	request.setNewLastDayHalf(newLastDayHalf);
    	request.setNewDayHalf(newDayHalf);

        request.setOriginatingOpId(originating_op_id);

    	_ops->approval_task_Requ(request);
    }

    dumpQueue();
}

void InboundQueueManager::onInApprovalTaskRequ(int taskType, int localId, int leaveYear, qint64 fromDate, qint64 toDate,
		                  bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, const QString &userId, int halfDays, int originating_op_id)
{

    qDebug() << "SSSS IN onInApprovalTaskRequ() [CANCELLATION]";

    qDebug() << "SSSS IN onInApprovalTaskRequ() - taskType:" << taskType << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - localId:" << localId << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - leaveYear:" << leaveYear << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - fromDate:" << fromDate << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - toDate:" << toDate << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - firstDayHalf:" << firstDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - lastDayHalf:" << lastDayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - dayHalf:" << dayHalf << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - note:" << note << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - userId:" << userId << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - halfDays:" << halfDays << endl;
    qDebug() << "SSSS IN onInApprovalTaskRequ() - originating_op_id:" << originating_op_id << endl;

    dumpQueue();

    OpApprovalTaskRequ request;

	if (taskType == TASK_TYPE_CANCEL_APPROVAL) {

		request.setTaskType(taskType);
		request.setLocalRequestId(localId);
		request.setLeaveYear(leaveYear);
		request.setFromDate(fromDate);
		request.setToDate(toDate);
		request.setFirstDayHalf(firstDayHalf);
		request.setLastDayHalf(lastDayHalf);
		request.setDayHalf(dayHalf);
		request.setNote(note);
        request.setUserId(userId);
        request.setHalfDays(halfDays);
        request.setOriginatingOpId(originating_op_id);

        _ops->approval_task_Requ(request);
	}

    dumpQueue();
}

void InboundQueueManager::onInApprovalTaskOutcomeResp(int taskId, int opStatus)
{
    qDebug() << "SSSS onInApprovalTaskOutcomeResp";
    dumpQueue();

    OpApprovalTaskOutcomeResp response;
    response.setTaskId(taskId);
    response.setOpStatus(opStatus);
    _ops->approval_task_outcome_Resp(response);

    dumpQueue();
}

void InboundQueueManager::onInAdapterDetailsRequ()
{
    qDebug() << "SSSS onInAdapterDetailsRequ";
    dumpQueue();
    OpAdapterDetailsRequ request;
    request.setAdapterName(_iAdapter->name());
    request.setAdapterVersion(_iAdapter->version());
    request.setAdapterDescription(_iAdapter->description());
    request.setConfigured(_iAdapter->isConfigured());
    _ops->adapter_details_Requ(request);
    dumpQueue();
}

void InboundQueueManager::onInAdapterStatusResp(int opStatus)
{
    qDebug() << "SSSS onInAdapterStatusResp";
    dumpQueue();
    OpAdapterStatusResp response;
    response.setOpStatus(opStatus);
    _ops->adapter_status_Resp(response);
    dumpQueue();
}

void InboundQueueManager::onInClientSynchronizeResp(int opStatus)
{
    qDebug() << "SSSS onInClientSynchronizeResp: opstatus=" << opStatus;
    dumpQueue();
    OpClientSynchronizeResp response;
    response.setOpStatus(opStatus);
    _ops->client_synchronize_Resp(response);
    dumpQueue();
}

void InboundQueueManager::onInServerSyncResultRequ(QByteArray serialized_data)
{
    qDebug() << "SSSS onInServerSyncResultRequ";
    dumpQueue();
    OpServerSyncResultRequ request;
    request.unmarshall(serialized_data);
    _ops->server_sync_result_Requ(request);
    dumpQueue();
}

void InboundQueueManager::onSettingsChanged()
{
    qDebug() << "SSSS onSettingsChanged";
    _dumpQueueEnabled = false;
    qDebug() << "SSSS onSettingsChanged _settings:" << _settings;

    QString serviceVerbose = _settings->getValueFor(SETTINGS_KEY_SERVICE_VERBOSE, "0");
    qDebug() << "SSSS IN onSettingsChanged: SERVICE_VERBOSE=" << serviceVerbose;
    if (serviceVerbose.compare("1") == 0) {
        _dumpQueueEnabled = true;
    }
}

void InboundQueueManager::dumpQueue()
{
    if (_dumpQueueEnabled) {
        _ops->logInboundQueue();
    }
}
