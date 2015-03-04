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

#ifndef SERVICE_H_
#define SERVICE_H_

#include <Settings.hpp>
#include <IAdapter.hpp>
#include <AdapterImpl.hpp>
#include <NetworkMonitor.hpp>
#include <InboundQueueManager.hpp>
#include <OutboundQueueManager.hpp>
#include <SocketListener.hpp>
#include <Operations.hpp>

#include <QObject>
#include <bb/Application>
#include <bb/system/InvokeManager>
#include <bb/core/ProcessState.hpp>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <QTimer>
#include <errno.h>

using namespace bb::platform;
using namespace bb::system;
using namespace bb;

class Service: public QObject
{
	Q_OBJECT

public:
	Service(Application * app);
	virtual ~Service();

private slots:
	void onInvoked(const bb::system::InvokeRequest &);
	void onNetworkAvailable();
	void onNetworkNotAvailable();
	void onProcessStateChanged(bb::ProcessState::Type processState);
	void onPauseDataFromApi();
	void onResumeDataFromApi();
	void onAboutToQuit();
	void stopServices();
	void startServices();

private:
	InvokeManager *_invokeManager;
	Settings *_applicationSettings;
	AdapterImpl *_adapterImpl;
	IAdapter *_iAdapter;
	NetworkMonitor *_networkMonitor;
	InboundQueueManager *_inboundQueueManager;
	OutboundQueueManager *_outboundQueueManager;
	bool _guiProcessRunning;
	SocketListener *_socketListener;
    Operations *_ops;
	void triggerNotification();
};

#endif /* SERVICE_H_ */
