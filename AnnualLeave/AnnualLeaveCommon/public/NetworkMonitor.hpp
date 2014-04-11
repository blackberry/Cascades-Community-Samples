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

#ifndef NETWORKMONITOR_HPP_
#define NETWORKMONITOR_HPP_

#include <Common.hpp>

#include <QObject>
#include <bb/core/AbstractBpsEventHandler.hpp>
#include <bps/netstatus.h>
#include <errno.h>

using namespace bb;

class Q_DECL_EXPORT NetworkMonitor
	: public QObject
	, public bb::AbstractBpsEventHandler
{
	Q_OBJECT

public:
	virtual ~NetworkMonitor();
	static NetworkMonitor *getInstance(QObject *parent=NULL);
	void start();
	void stop();

signals:
	void networkAvailable();
	void networkNotAvailable();

private:
	NetworkMonitor(QObject *parent=NULL);
	static NetworkMonitor* _instance;
    bool _initialised;
    bool _networkWasAvailable;
    void initialise();
    void terminate();
    void event(bps_event_t *event);
    void handleNetStatusEvent(bps_event_t *event);
    bool networkAvailabile();
    void setNetworkInitialStatus();
    void setNetworkStatus();
};

#endif /* NETWORKMONITOR_HPP_ */
