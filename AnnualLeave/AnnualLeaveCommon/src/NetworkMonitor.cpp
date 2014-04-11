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

#include <NetworkMonitor.hpp>

NetworkMonitor* NetworkMonitor::_instance;

NetworkMonitor::NetworkMonitor(QObject *parent)
	: QObject(parent)
	, _initialised(false)
	, _networkWasAvailable(true)
{
}

NetworkMonitor::~NetworkMonitor() {}

NetworkMonitor *NetworkMonitor::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new NetworkMonitor(parent);
    }
    return _instance;
}

void NetworkMonitor::start() {
	initialise();
	setNetworkInitialStatus();
}

void NetworkMonitor::stop() {
	terminate();
}

void NetworkMonitor::initialise() {

	qDebug() << "CCCC NetworkMonitor::initialise() starting..." << endl;

	bool initialised = true;

	if (bps_initialize() == BPS_FAILURE) {
		initialised = false;
		qDebug() << "CCCC Error: BPS failed to initialise" << strerror(errno) << endl;
	}

	if (netstatus_request_events(0) == BPS_FAILURE) {
		initialised = false;
		qDebug() << "CCCC Error: Failed to request Netstatus BPS events" << strerror(errno) << endl;
		bps_shutdown();
	} else {
		qDebug() << "CCCC Registered for Netstatus BPS events OK" << endl;
		subscribe(netstatus_get_domain());
	}
	_initialised = initialised;
}

void NetworkMonitor::terminate() {
	qDebug() << "CCCC NetworkMonitor::terminate entered ..." << endl;
	netstatus_stop_events(0);
	unsubscribe(netstatus_get_domain());
	bps_shutdown();
}

void NetworkMonitor::event(bps_event_t *event) {
	qDebug() << "CCCC NetworkMonitor::event() entered ..." << endl;

	int domain = bps_event_get_domain(event);
	if ((netstatus_get_domain() == domain) &&
		(bps_event_get_code(event) == NETSTATUS_INFO)) {
			handleNetStatusEvent(event);
	}
}

void NetworkMonitor::handleNetStatusEvent(bps_event_t *event)
{
	Q_UNUSED(event)

	qDebug() << "CCCC NetworkMonitor::handleNetStatusEvent() entered ..." << endl;

	setNetworkStatus();
}

void NetworkMonitor::setNetworkInitialStatus()
{
	if (networkAvailabile()) {
		qDebug() << "CCCC Network Initially Available" << endl;
		_networkWasAvailable = true;
		emit networkAvailable();
	} else {
		qDebug() << "CCCC Network Initially Not Available" << endl;
		_networkWasAvailable = false;
		emit networkNotAvailable();
	}
}

void NetworkMonitor::setNetworkStatus()
{
	if (networkAvailabile()) {
		qDebug() << "CCCC Network Available" << endl;
		if (!_networkWasAvailable) {
			qDebug() << "CCCC Network changed to Available" << endl;
			_networkWasAvailable = true;
			emit networkAvailable();
		}
	} else {
		qDebug() << "CCCC Network Not Available" << endl;
		if (_networkWasAvailable) {
			qDebug() << "CCCC Network changed to Not Available" << endl;
			_networkWasAvailable = false;
			emit networkNotAvailable();
		}
	}
}

bool NetworkMonitor::networkAvailabile()
{
	netstatus_info_t *info;
	bool available = false;

    if (netstatus_get_info(&info) == BPS_SUCCESS) {
    	if (netstatus_info_get_availability(info)) {
        	available = true;
    	}
        netstatus_free_info(&info);
    }
    return available;
}
