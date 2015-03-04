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

#include "service.hpp"

Service::Service(bb::Application *app)
	: QObject(app)
	, _invokeManager(new InvokeManager(this))
	, _applicationSettings(Settings::getInstance(this))
	, _adapterImpl(AdapterImpl::getInstance(this))
	, _networkMonitor(NetworkMonitor::getInstance(this))
	, _inboundQueueManager(InboundQueueManager::getInstance(this))
	, _outboundQueueManager(OutboundQueueManager::getInstance(this))
	, _guiProcessRunning(false)
	, _socketListener(SocketListener::getInstance(this))
	, _ops(Operations::getInstance(this))
{
	qDebug() << "SSSS Service::Service(bb::Application *app)" << endl;

	_iAdapter = _adapterImpl;

	if (!QObject::connect(app, SIGNAL(processStateChanged(bb::ProcessState::Type)),
			             this, SLOT(onProcessStateChanged(bb::ProcessState::Type)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onProcessStateChanged" << strerror(errno) << endl;
	}

	if (!QObject::connect(app, SIGNAL(aboutToQuit()),
			             this, SLOT(onAboutToQuit()))) {
		qWarning() << "SSSS Service::Service() - connect failed - aboutToQuit" << strerror(errno) << endl;
	}

	if (!QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
                                    this, SLOT(onInvoked(const bb::system::InvokeRequest&)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onInvoked" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inApprovalOutcomeRequ(int, int, const QString &,int,int)),
			                            _inboundQueueManager, SLOT(onInApprovalOutcomeRequ(int, int, const QString &,int,int)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onApprovalOutcomeRequ" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inHalfDaysUsedRequ(int, int)),
			                            _inboundQueueManager, SLOT(onInHalfDaysUsedRequ(int, int)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onHalfDaysUsedRequ" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inCancelBookingResp(int, int)),
			                            _inboundQueueManager, SLOT(onInCancelBookingResp(int, int)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onCancelBookingResp" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inSubmitBookingResp(int, int)),
			                            _inboundQueueManager, SLOT(onInSubmitBookingResp(int, int)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onInSubmitBookingResp" << strerror(errno) << endl;
	}

    if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inClientSynchronizeResp(int)),
                                        _inboundQueueManager, SLOT(onInClientSynchronizeResp(int)))) {
        qWarning() << "SSSS Service::Service() - connect failed - onInClientSynchronizeResp" << strerror(errno) << endl;
    }


	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inAnnualEntitlementResp(int, int, int, int)),
			                            _inboundQueueManager, SLOT(onInAnnualEntitlementResp(int, int, int, int)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onAnnualEntitlementResp" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inUpdateBookingResp(int, int, qint64, qint64)),
			                            _inboundQueueManager, SLOT(onInUpdateBookingResp(int, int, qint64, qint64)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onUpdateBookingResp" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inApprovalTaskRequ(int, const QString &, int, qint64, qint64, bool, bool, int, int, int, const QString &, int)),
			                            _inboundQueueManager, SLOT(onInApprovalTaskRequ(int, const QString &, int, qint64, qint64, bool, bool, int, int, int, const QString &, int)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onUpdateBookingOutcomeRequ (BOOKING)" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inApprovalTaskRequ(int, int, int, int, qint64, qint64, qint64, qint64, bool, bool, bool, bool, int, int, const QString &, int, int)),
			                            _inboundQueueManager, SLOT(onInApprovalTaskRequ(int, int, int, int, qint64, qint64, qint64, qint64, bool, bool, bool, bool, int, int, const QString &, int, int)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onUpdateBookingOutcomeRequ (UPDATE)" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inApprovalTaskRequ(int, int, int, qint64, qint64, bool, bool, int, const QString &, const QString &, int, int)),
			                            _inboundQueueManager, SLOT(onInApprovalTaskRequ(int, int, int, qint64, qint64, bool, bool, int, const QString &, const QString &, int, int)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onUpdateBookingOutcomeRequ (CANCEL)" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inApprovalTaskOutcomeResp(int, int)),
			                            _inboundQueueManager, SLOT(onInApprovalTaskOutcomeResp(int, int)))) {
		qWarning() << "SSSS Service::Service() - connect failed - onInApprovalTaskOutcomeResp" << strerror(errno) << endl;
	}

    if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inAdapterDetailsRequ()),
                                        _inboundQueueManager, SLOT(onInAdapterDetailsRequ()))) {
        qWarning() << "SSSS Service::Service() - connect failed - onInAdapterDetailsRequ" << strerror(errno) << endl;
    } else {
        qDebug() << "SSSS  Service::Service() - connected inAdapterDetailsRequ to onInAdapterDetailsRequ OK";
    }

    if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inAdapterStatusResp(int)),
                                        _inboundQueueManager, SLOT(onInAdapterStatusResp(int)))) {
        qWarning() << "SSSS Service::Service() - connect failed - onInAdapterStatusResp" << strerror(errno) << endl;
    } else {
        qDebug() << "SSSS  Service::Service() - connected inAdapterStatusResp to onInAdapterStatusResp OK";    }

    if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(inServerSyncResultRequ(QByteArray)),
                                        _inboundQueueManager, SLOT(onInServerSyncResultRequ(QByteArray)))) {
        qWarning() << "SSSS Service::Service() - connect failed - inServerSyncResultRequ" << strerror(errno) << endl;
    } else {
        qDebug() << "SSSS  Service::Service() - connected inServerSyncResultRequ to oninServerSyncResultRequ OK";
    }


	if (!QObject::connect(_networkMonitor, SIGNAL(networkAvailable()),
                                     this, SLOT(onNetworkAvailable()))) {
		qWarning() << "SSSS Service::Service() - connect failed - onNetworkAvailable" << strerror(errno) << endl;
	}

	if (!QObject::connect(_networkMonitor, SIGNAL(networkNotAvailable()),
                                     this, SLOT(onNetworkNotAvailable()))) {
		qWarning() << "SSSS Service::Service() - connect failed - onNetworkNotAvailable" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(pauseDataFromApi()),
                                                        this, SLOT(onPauseDataFromApi()))) {
		qWarning() << "SSSS Service::Service() - connect failed - onPauseDataFromApi" << strerror(errno) << endl;
	}

	if (!QObject::connect(dynamic_cast<QObject *>(_iAdapter), SIGNAL(resumeDataFromApi()),
                                                        this, SLOT(onResumeDataFromApi()))) {
		qWarning() << "SSSS Service::Service() - connect failed - onResumeDataFromApi" << strerror(errno) << endl;
	}

	startServices();
}

Service::~Service()
{
	_networkMonitor->stop();
	_inboundQueueManager->stop();
	_outboundQueueManager->stop();
	_adapterImpl->stop();
}

void Service::onInvoked(const bb::system::InvokeRequest & request)
{
	if (request.action().compare(TARGET_AL_ACTION_OUTBOUND_QUEUE) == 0) {
	    qDebug() << "SSSS received notification that the outbound queue contains operations to process" << endl;

	    _outboundQueueManager->processQueue();

	} else if (request.action().compare(TARGET_AL_ACTION_STOP_SERVICE) == 0) {
	    qDebug() << "SSSS received instruction to stop the service application" << endl;

		_networkMonitor->stop();
		_inboundQueueManager->stop();
		_outboundQueueManager->stop();
		_adapterImpl->stop();
	    QCoreApplication::quit();

	} else if (request.action().compare(TARGET_AL_ACTION_RESET_ADAPTER) == 0) {
		qDebug() << "SSSS received instruction to reset the adapter" << endl;

		_adapterImpl->reset();

	} else {
		qWarning() << "SSSS received invocation request we don't handle:" << request.action() << endl;
	}
}

void Service::onProcessStateChanged(bb::ProcessState::Type processState)
{
	switch (processState) {
		case ProcessState::Foreground:
			qDebug() << "SSSS Process state changes to Foreground" << endl;
			_guiProcessRunning = true;
			break;
		case ProcessState::Background:
			qDebug() << "SSSS Process state changes to Background" << endl;
			_guiProcessRunning = false;
			break;
		case ProcessState::Stopping:
			qDebug() << "SSSS Process state changes to Stopping" << endl;
			_guiProcessRunning = false;
			break;
		case ProcessState::Unknown:
			qDebug() << "SSSS Process state changes to Unknown" << endl;
			_guiProcessRunning = false;
			break;
		default:
			qDebug() << "SSSS Process state changes to unspecified value" << processState << endl;
			break;
	}
	_ops->setGuiAppProcessRunning(_guiProcessRunning);
}

void Service::onAboutToQuit()
{
	qDebug() << "SSSS Service about to quit" << endl;

	stopServices();
}

void Service::startServices()
{
	qDebug() << "SSSS Service starting services" << endl;

	_adapterImpl->start();
	_outboundQueueManager->start();
	_inboundQueueManager->start();
	_networkMonitor->start();
}

void Service::stopServices()
{
	qDebug() << "SSSS Service stopping services" << endl;

	_networkMonitor->stop();
	_inboundQueueManager->stop();
	_outboundQueueManager->stop();
	_adapterImpl->stop();
}

void Service::onNetworkAvailable()
{
	qDebug() << "SSSS onNetworkAvailable()" << endl;

	_iAdapter->networkAvailable();
	_outboundQueueManager->resume();
}

void Service::onNetworkNotAvailable()
{
	qDebug() << "SSSS onNetworkNotAvailable()" << endl;

	_outboundQueueManager->pause();
	_iAdapter->networkNotAvailable();
}

void Service::onPauseDataFromApi()
{
	qDebug() << "SSSS onPauseDataFromApi()" << endl;

	_outboundQueueManager->pause();
}

void Service::onResumeDataFromApi()
{
	qDebug() << "SSSS onResumeDataFromApi()" << endl;

	_outboundQueueManager->resume();
}
