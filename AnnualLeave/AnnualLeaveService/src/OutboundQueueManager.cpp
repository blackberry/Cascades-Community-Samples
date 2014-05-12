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

#include <OutboundQueueManager.hpp>

OutboundQueueManager* OutboundQueueManager::_instance;

OutboundQueueManager::OutboundQueueManager(QObject *parent)
	: QObject(parent)
	, _initialised(false)
	, _paused(true)
	, _adapterImpl(AdapterImpl::getInstance(this))
	, _ops(Operations::getInstance(this))
	, _dumpQueueEnabled(false)
	, _settings(Settings::getInstance(this))
{
	_iAdapter = _adapterImpl;

    if (!QObject::connect(_settings, SIGNAL(settingsChanged()),
    		                   this, SLOT(onSettingsChanged()))) {
        qWarning() << "SSSS OUT OutboundQueueManager::OutboundQueueManager() - connect failed - onSettingsChanged" << strerror(errno) << endl;
    }
}

OutboundQueueManager::~OutboundQueueManager()
{
	_instance = NULL;
	delete _adapterImpl;
}

OutboundQueueManager *OutboundQueueManager::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new OutboundQueueManager(parent);
    }
    return _instance;
}

int OutboundQueueManager::start()
{
	_initialised = true;
	return EOK;
}

int OutboundQueueManager::stop()
{
	_initialised = false;
	return EOK;
}

int OutboundQueueManager::pause()
{
	_paused = true;
	return EOK;
}

int OutboundQueueManager::resume()
{
	_paused = false;
	processQueue();
	return EOK;
}

int OutboundQueueManager::processQueue()
{
	qDebug() << "SSSS OUT processQueue()" << endl;

	if (_paused) {
		return EOK;
	}

	dumpQueue();

	QList<OperationAl*> listOfRequests = _ops->outboundQueueOps();
	QList<OperationAl*>::iterator curr;

	for (curr = listOfRequests.begin(); curr != listOfRequests.end(); curr++) {

		OpSubmitBookingRequ *bookingRequ;
		OpHalfDaysUsedResp *halfDayResp;
		OpApprovalOutcomeResp *approvalOutcomeResp;
		OpCancelBookingRequ *cancelBookingRequest;
		OpAnnualEntitlementRequ *entitlementRequ;
		OpUpdateBookingRequ *updateBookingRequest;
		OpApprovalTaskResp *approvalTaskResp;
		OpApprovalTaskOutcomeRequ *approvalTaskOutcomeRequ;
        OpAdapterDetailsResp *adapterDetailsResp;
        OpAdapterStatusRequ *adapterStatusRequ;
        OpClientSynchronizeRequ *clientSynchronizeRequ;
        OpServerSyncResultResp *serverSyncResultResp;

		switch ((*curr)->opType()) {
			case OP_TYPE_SUBMIT_BOOKING_REQUEST:
				bookingRequ = dynamic_cast<OpSubmitBookingRequ *>(*curr);

				qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_SUBMIT_BOOKING_REQUEST" << endl;

				if (doOutSubmitBookingRequ(bookingRequ) != ADAPTER_IMPL_ACCEPTED) {
					qDebug() << "SSSS OUT processQueue() - OP_TYPE_SUBMIT_BOOKING_REQUEST request not accepted by adapter" << endl;
					failBookingRequ(bookingRequ->localRequestId());
				}
				removeRequest(bookingRequ->opId());
				break;

            case OP_TYPE_CLIENT_SYNCHRONIZE_REQUEST:
                clientSynchronizeRequ = dynamic_cast<OpClientSynchronizeRequ *>(*curr);

                qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_CLIENT_SYNCHRONIZE_REQUEST" << endl;

                if (doOutClientSynchronizeRequ(clientSynchronizeRequ) != ADAPTER_IMPL_ACCEPTED) {
                    qDebug() << "SSSS OUT processQueue() - OP_TYPE_CLIENT_SYNCHRONIZE_REQUEST request not accepted by adapter" << endl;
                }
                removeRequest(clientSynchronizeRequ->opId());
                break;

            case OP_TYPE_HALF_DAYS_USED_RESPONSE:
				halfDayResp = dynamic_cast<OpHalfDaysUsedResp *>(*curr);

				qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_HALF_DAYS_USED_RESPONSE" << endl;

				doOutHalfDayResp(halfDayResp);
				removeRequest(halfDayResp->opId());
				break;

			case OP_TYPE_APPROVAL_OUTCOME_RESPONSE:
				approvalOutcomeResp = dynamic_cast<OpApprovalOutcomeResp *>(*curr);

				qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_APPROVAL_OUTCOME_RESPONSE" << endl;

				doOutApprovalOutcomeResp(approvalOutcomeResp);
				removeRequest(approvalOutcomeResp->opId());
				break;

			case OP_TYPE_CANCELLATION_REQUEST:
				cancelBookingRequest = dynamic_cast<OpCancelBookingRequ *>(*curr);

				qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_CANCELLATION_REQUEST" << endl;

				if (doOutCancelBookingRequ(cancelBookingRequest) != ADAPTER_IMPL_ACCEPTED) {
					qDebug() << "SSSS OUT processQueue() - OP_TYPE_CANCELLATION_REQUEST request not accepted by adapter" << endl;
					failCancelRequ(cancelBookingRequest->localRequestId());
				}
				removeRequest(cancelBookingRequest->opId());
				break;

			case OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST:
				entitlementRequ = dynamic_cast<OpAnnualEntitlementRequ *>(*curr);

				qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST" << endl;

				if (doOutEntitlementRequ(entitlementRequ) != ADAPTER_IMPL_ACCEPTED) {
					qDebug() << "SSSS OUT processQueue() - OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST request not accepted by adapter" << endl;
					failEntitlementRequ();
				}

				removeRequest(entitlementRequ->opId());
				break;

			case OP_TYPE_UPDATE_BOOKING_REQUEST:
				updateBookingRequest = dynamic_cast<OpUpdateBookingRequ *>(*curr);

				qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_UPDATE_BOOKING_REQUEST" << endl;

				if (doOutUpdateBookingRequ(updateBookingRequest) != ADAPTER_IMPL_ACCEPTED) {
					qDebug() << "SSSS OUT processQueue() - OP_TYPE_UPDATE_BOOKING_REQUEST request not accepted by adapter" << endl;
					failUpdateRequ(updateBookingRequest->localRequestId());
				}
				removeRequest(updateBookingRequest->opId());
				break;

			case OP_TYPE_APPROVAL_TASK_RESPONSE:
				approvalTaskResp = dynamic_cast<OpApprovalTaskResp *>(*curr);
				qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_APPROVAL_TASK_RESPONSE" << endl;

				doOutApprovalTaskResp(approvalTaskResp);
				removeRequest(approvalTaskResp->opId());
				break;

			case OP_TYPE_APPROVAL_TASK_OUTCOME_REQUEST:
				approvalTaskOutcomeRequ = dynamic_cast<OpApprovalTaskOutcomeRequ *>(*curr);
				qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_APPROVAL_TASK_OUTCOME_REQUEST" << endl;
                qDebug() << "SSSS OUT processQueue() - originating_op_id=" << approvalTaskOutcomeRequ->originatingOpId() << endl;

				doOutApprovalTaskOutcomeRequ(approvalTaskOutcomeRequ);
				removeRequest(approvalTaskOutcomeRequ->opId());
				break;

            case OP_TYPE_ADAPTER_DETAILS_RESPONSE:
                adapterDetailsResp = dynamic_cast<OpAdapterDetailsResp *>(*curr);

                qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_ADAPTER_DETAILS_RESPONSE" << endl;

                doOutAdapterDetailsResp(adapterDetailsResp);

                removeRequest(adapterDetailsResp->opId());
                break;

            case OP_TYPE_ADAPTER_STATUS_REQUEST:
                adapterStatusRequ = dynamic_cast<OpAdapterStatusRequ *>(*curr);
                qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_ADAPTER_STATUS_REQUEST" << endl;
                doOutAdapterStatusRequ(adapterStatusRequ);
                removeRequest(adapterStatusRequ->opId());
                break;
            case OP_TYPE_SERVER_SYNC_RESULT_RESPONSE:
                serverSyncResultResp = dynamic_cast<OpServerSyncResultResp *>(*curr);
                qDebug() << "SSSS OUT processQueue() - processing request type OP_TYPE_SERVER_SYNC_RESULT_RESPONSE" << endl;
                doOutServerSyncResultResp(serverSyncResultResp);
                removeRequest(serverSyncResultResp->opId());
                break;

			default:
				qWarning() << "SSSS OUT processQueue() - unimplemented operation  type" << (*curr)->opType() << endl;
				break;
		}
	}
	return EOK;
}

int OutboundQueueManager::doOutSubmitBookingRequ(OpSubmitBookingRequ *request)
{
	qDebug() << "SSSS OUT outSubmitBookingRequ()" << endl;

	return _iAdapter->outSubmitBookingRequ(request->localRequestId(),
										   request->leaveYear(),
			                               request->fromDate(),
			                               request->toDate(),
			                               request->firstDayHalf(),
			                               request->lastDayHalf(),
			                               request->dayHalf(),
			                               request->note(),
			                               request->opId());
}

int OutboundQueueManager::doOutHalfDayResp(OpHalfDaysUsedResp *response)
{
	qDebug() << "SSSS OUT doOutHalfDayResp()" << endl;

	return _iAdapter->outHalfDaysUsedResp(response->localRequestId(), response->opStatus());
}

int OutboundQueueManager::doOutApprovalOutcomeResp(OpApprovalOutcomeResp *response)
{
	qDebug() << "SSSS OUT doOutApprovalOutcomeResp()" << endl;

	return _iAdapter->outApprovalOutcomeResp(response->localRequestId(), response->opStatus());
}

int OutboundQueueManager::doOutServerSyncResultResp(OpServerSyncResultResp *response)
{
    qDebug() << "SSSS OUT doOutServerSyncResultResp()" << endl;

    return _iAdapter->outServerSyncResultResp(response->opId(), response->opStatus());
}

int OutboundQueueManager::doOutCancelBookingRequ(OpCancelBookingRequ *request)
{
	qDebug() << "SSSS OUT doOutCancelBookingRequ()" << endl;

	return _iAdapter->outCancelBookingRequ(request->localRequestId(),
										   request->leaveYear(),
			                               request->fromDate(),
			                               request->toDate(),
			                               request->firstDayHalf(),
			                               request->lastDayHalf(),
			                               request->dayHalf(),
			                               request->note(),
			                               request->opId());
}

int OutboundQueueManager::doOutEntitlementRequ(OpAnnualEntitlementRequ *request)
{
	qDebug() << "SSSS OUT doOutEntitlementRequ() for year:" << request->leaveYear() << endl;

	return _iAdapter->outAnnualEntitlementRequ(request->leaveYear());
}

int OutboundQueueManager::doOutUpdateBookingRequ(OpUpdateBookingRequ *request)
{
	qDebug() << "SSSS OUT doOutUpdateBookingRequ()" << endl;

	return _iAdapter->outUpdateBookingRequ(request->localRequestId(),
			                            request->oldLeaveYear(),
			                            request->newLeaveYear(),
			                            request->oldFromDate(),
			                            request->newFromDate(),
			                            request->oldToDate(),
			                            request->newToDate(),
			                            request->oldFirstDayHalf(),
			                            request->newFirstDayHalf(),
			                            request->oldLastDayHalf(),
			                            request->newLastDayHalf(),
			                            request->oldDayHalf(),
			                            request->newDayHalf(),
			                            request->note(),
			                            request->opId());
}

int OutboundQueueManager::doOutApprovalTaskResp(OpApprovalTaskResp *response)
{
	qDebug() << "SSSS OUT doOutApprovalTaskResp()" << endl;

	return _iAdapter->outApprovalTaskResp(response->taskId(), response->opStatus(), response->userId());
}

int OutboundQueueManager::doOutApprovalTaskOutcomeRequ(OpApprovalTaskOutcomeRequ *request)
{
	qDebug() << "SSSS OUT doOutApprovalTaskOutcomeRequ()" << endl;

	return _iAdapter->outApprovalTaskOutcomeRequ(request->taskId(), request->localRequestId(), request->requestStatus(), request->originatingOpId(), request->taskType());
}

int OutboundQueueManager::doOutAdapterDetailsResp(OpAdapterDetailsResp *response)
{
    Q_UNUSED(response);
    qDebug() << "SSSS OUT doOutAdapterDetailsRequ()" << endl;
    // there's no actual need to return this response to the adapter so.....
    return ADAPTER_IMPL_ACCEPTED;
}

int OutboundQueueManager::doOutAdapterStatusRequ(OpAdapterStatusRequ *request)
{
    qDebug() << "SSSS OUT doOutAdapterStatusRequ()" << endl;
    return _iAdapter->outAdapterStatusRequ();
}

int OutboundQueueManager::doOutClientSynchronizeRequ(OpClientSynchronizeRequ *request)
{
    qDebug() << "SSSS OUT doOutClientSynchronizeRequ()" << endl;
    return _iAdapter->outClientSynchronizeRequ(request->leaveYear());
}

void OutboundQueueManager::removeRequest(int opId)
{
	_ops->deleteOutboundOp(opId);
}

void OutboundQueueManager::failBookingRequ(int LocalId)
{
	OpSubmitBookingResp response;
	response.setLocalRequestId(LocalId);
	response.setRequestStatus(STATUS_FAILED);
	response.setOpStatus(OP_STATUS_OK);
	_ops->submit_Booking_Resp(response);
}

void OutboundQueueManager::failEntitlementRequ()
{
	OpAnnualEntitlementResp response;
	response.setRequestStatus(STATUS_FAILED);
	response.setOpStatus(OP_STATUS_OK);
	_ops->annual_entitlement_Resp(response);
}

void OutboundQueueManager::failCancelRequ(int LocalId)
{
	OpCancelBookingResp response;
	response.setLocalRequestId(LocalId);
	response.setRequestStatus(STATUS_FAILED);
	response.setOpStatus(OP_STATUS_OK);
	_ops->cancel_Booking_Resp(response);
}

void OutboundQueueManager::failUpdateRequ(int LocalId)
{
	OpUpdateBookingResp response;
	response.setLocalRequestId(LocalId);
	response.setRequestStatus(STATUS_FAILED);
	response.setOpStatus(OP_STATUS_OK);
	_ops->update_Booking_Resp(response);
}

void OutboundQueueManager::onSettingsChanged()
{
	_dumpQueueEnabled = false;
    QString serviceVerbose = _settings->getValueFor(SETTINGS_KEY_SERVICE_VERBOSE, "0");
    qDebug() << "SSSS OUT onSettingsChanged: SERVICE_VERBOSE=" << serviceVerbose;
    if (serviceVerbose.compare("1") == 0) {
    	_dumpQueueEnabled = true;
    }
}

void OutboundQueueManager::dumpQueue()
{
	if (_dumpQueueEnabled) {
		_ops->logOutboundQueue();
	}
}
