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

#include "AdapterImpl.hpp"

AdapterImpl* AdapterImpl::_instance;

AdapterImpl::AdapterImpl(QObject *parent)
	: QObject(parent)
	, _emailAdapter(EmailAdapter::getInstance(this))
	, _accountList(bb::pim::account::AccountService().accounts(bb::pim::account::Service::Messages))
{
	qDebug() << "AAAA AdapterImpl::AdapterImpl()" << endl;

    // initialise Email Adapter GUI accessible settings. Some of these will go when we refactor to separate P2P from back-end email
    Settings* settings = Settings::getInstance(this);
    AdapterSettings* adapter_settings = AdapterSettings::getInstance(this);

    qDebug() << "AAAA AdapterImpl initialising settings" << endl;

    // NB only dealing with P2P mode settings at present

    adapter_settings->setP2pModeEnabled(true);
    adapter_settings->setConfigured(false);

    // separate from isConfigured() to avoid race condition here
    bool state_ok=true;

    QString setting_1_name = settings->getValueFor(GUI_SETTING_1_NAME,"undefined");
    QString setting_1_value = settings->getValueFor(GUI_SETTING_1_VALUE,"undefined");

    if (setting_1_name.compare("undefined") == 0) {
        settings->saveValueFor(GUI_SETTING_1_NAME, "Email Account Name");
        settings->saveValueFor(GUI_SETTING_1_VALUE, "");
        state_ok = false;
    }

    adapter_settings->setAccountName(setting_1_value);

    QString setting_2_name = settings->getValueFor(GUI_SETTING_2_NAME,"undefined");
    QString setting_2_value = settings->getValueFor(GUI_SETTING_2_VALUE,"undefined");

    if (setting_2_name.compare("undefined") == 0) {
        settings->saveValueFor(GUI_SETTING_2_NAME, "Email Provider Name");
        settings->saveValueFor(GUI_SETTING_2_VALUE, "");
        state_ok = false;
    }

    adapter_settings->setProviderName(setting_2_value);

    QString setting_3_name = settings->getValueFor(GUI_SETTING_3_NAME,"undefined");
    QString setting_3_value = settings->getValueFor(GUI_SETTING_3_VALUE,"undefined");

    if (setting_3_name.compare("undefined") == 0) {
        settings->saveValueFor(GUI_SETTING_3_NAME, "Approver Name");
        settings->saveValueFor(GUI_SETTING_3_VALUE, "");
        state_ok = false;
    }

    adapter_settings->setP2pModeApproverName(setting_3_value);

    QString setting_4_name = settings->getValueFor(GUI_SETTING_4_NAME,"undefined");
    QString setting_4_value = settings->getValueFor(GUI_SETTING_4_VALUE,"undefined");

    if (setting_4_name.compare("undefined") == 0) {
        settings->saveValueFor(GUI_SETTING_4_NAME, "Approver Email Address");
        settings->saveValueFor(GUI_SETTING_4_VALUE, "");
        state_ok = false;
    }

    adapter_settings->setP2pModeApproverEmail(setting_4_value);

    QString setting_5_name = settings->getValueFor(GUI_SETTING_5_NAME,"undefined");
    QString setting_5_value = settings->getValueFor(GUI_SETTING_5_VALUE,"undefined");

    if (setting_5_name.compare("undefined") == 0) {
        settings->saveValueFor(GUI_SETTING_5_NAME, "User ID");
        settings->saveValueFor(GUI_SETTING_5_VALUE, "");
        state_ok = false;
    }

    adapter_settings->setP2pClientUserId(setting_5_value);

    adapter_settings->setConfigured(state_ok);

}

AdapterImpl::~AdapterImpl()
{
	qDebug() << "AAAA ~AdapterImpl()" << endl;
	_instance = NULL;
}

AdapterImpl *AdapterImpl::getInstance(QObject *parent)
{
	qDebug() << "AAAA AdapterImpl::getInstance() - _instance " << _instance << endl;
    if (_instance == 0) {
        _instance = new AdapterImpl(parent);
    }
    return _instance;
}

QString AdapterImpl::version()
{
	return ADAPTER_IMPL_VERSION;
}

QString AdapterImpl::name()
{
	return ADAPTER_IMPL_NAME;
}

QString AdapterImpl::description()
{
	return ADAPTER_IMPL_DESCRIPTION;
}

// Start -- API from IAdapter

int AdapterImpl::outSubmitBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id)
{
	qDebug() << "AAAA outSubmitBookingRequ()" << endl;

	return _emailAdapter->outSubmitBookingRequ(localId,leaveYear, fromDate, toDate, firstDayHalf, lastDayHalf, dayHalf, note, originating_op_id);
}

int AdapterImpl::outHalfDaysUsedResp(int localId, int opStatus)
{
	qDebug() << "AAAA outHalfDaysUsedResp()" << endl;

	return _emailAdapter->outHalfDaysUsedResp(localId, opStatus);
}

int AdapterImpl::outApprovalOutcomeResp(int localId, int opStatus)
{
	qDebug() << "AAAA outApprovalOutcomeResp()" << endl;

	return _emailAdapter->outApprovalOutcomeResp(localId, opStatus);
}

int AdapterImpl::outCancelBookingRequ(int localId, int leaveYear, qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf, int dayHalf, const QString &note, int originating_op_id)
{
	qDebug() << "AAAA outCancelBookingRequ()" << endl;

	return _emailAdapter->outCancelBookingRequ(localId,leaveYear, fromDate, toDate, firstDayHalf, lastDayHalf, dayHalf, note,originating_op_id);
}

int AdapterImpl::outAnnualEntitlementRequ(int year)
{
	qDebug() << "AAAA outAnnualEntitlementRequ() for year " << year << endl;

	return _emailAdapter->outAnnualEntitlementRequ(year);
}

int AdapterImpl::outUpdateBookingRequ(int localId, int oldYear, int newYear, qint64 oldFromDate, qint64 newFromDate,
					  qint64 oldToDate, qint64 newToDate,
					  bool oldFirstDayHalf, bool newFirstDayHalf,
					  bool oldLastDayHalf, bool newLastDayHalf,
					  int oldDayHalf, int newDayHalf, const QString &note, int originating_op_id)
{
	qDebug() << "AAAA outUpdateBookingRequ()" << endl;

	return _emailAdapter->outUpdateBookingRequ(localId,
											oldYear, newYear,
											oldFromDate, newFromDate,
											oldToDate, newToDate,
										    oldFirstDayHalf, newFirstDayHalf,
										    oldLastDayHalf, newLastDayHalf,
										    oldDayHalf, newDayHalf,
										    note,
										    originating_op_id);
}

int AdapterImpl::outApprovalTaskResp(int taskId, int opStatus, const QString &userId)
{
	qDebug() << "AAAA outApprovalTaskResp()" << endl;

	return _emailAdapter->outApprovalTaskResp(taskId, opStatus, userId);
}

int AdapterImpl::outApprovalTaskOutcomeRequ(int taskId, int local_request_id, int requestStatus, int originating_op_id, int task_type)
{
	qDebug() << "AAAA AdapterImpl:outApprovalTaskOutcomeRequ() taskId=" << taskId << " local_request_id=" << local_request_id << " requestStatus=" << requestStatus << endl;

	return _emailAdapter->outApprovalTaskOutcomeRequ(taskId, local_request_id, requestStatus, originating_op_id, task_type);
}

int AdapterImpl::outAdapterStatusRequ()
{
    qDebug() << "AAAA AdapterImpl:outAdapterStatusRequ()" << endl;

    return _emailAdapter->outAdapterStatusRequ();
}

int AdapterImpl::outClientSynchronizeRequ(int leave_year)
{
    qDebug() << "AAAA AdapterImpl:outClientSynchronizeRequ()" << endl;

    return _emailAdapter->outClientSynchronizeRequ(leave_year);
}


int AdapterImpl::outServerSyncResultResp(int op_id, int op_status) {
    qDebug() << "AAAA outServerSyncResultResp()" << endl;
    return _emailAdapter->outServerSyncResultResp(op_id, op_status);
}

int AdapterImpl::start()
{
	qDebug() << "AAAA start()" << endl;

	return _emailAdapter->start();
}

int AdapterImpl::pause()
{
	qDebug() << "AAAA pause()" << endl;

	return _emailAdapter->pause();
}

int AdapterImpl::resume()
{
	qDebug() << "AAAA resume()" << endl;

	return _emailAdapter->resume();
}

int AdapterImpl::stop()
{
	qDebug() << "AAAA stop()" << endl;

	return _emailAdapter->start();
}

int AdapterImpl::reset()
{
	qDebug() << "AAAA reset()" << endl;

	return _emailAdapter->reset();
}

int AdapterImpl::networkAvailable()
{
	qDebug() << "AAAA networkAvailable()" << endl;

	return _emailAdapter->networkAvailable();
}

int AdapterImpl::networkNotAvailable()
{
	qDebug() << "AAAA networkNotAvailable()" << endl;

	return _emailAdapter->networkNotAvailable();
}

bool AdapterImpl::isConfigured() {
    AdapterSettings* adapter_settings = AdapterSettings::getInstance(this);
    return adapter_settings->isConfigured();
}

// End -- API from IAdapter
