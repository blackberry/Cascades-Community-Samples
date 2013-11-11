/* Copyright (c) 2012 Research In Motion Limited.
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

#ifndef WIFIDIRECTION_HPP_
#define WIFIDIRECTION_HPP_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <bb/device/WiFiDirect>
#include <bb/device/WiFiDirectResult>
#include <bb/device/WiFiDirectDeviceType>
#include <bb/device/WiFiDirectWpsMethod>
#include <bb/device/WiFiDirectIntent>
#include <bb/device/WiFiDirectConnectionState>
#include <bb/cascades/GroupDataModel>
#include "WDServer.hpp"
#include "WDClient.hpp"

class WiFiDirection : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bb::cascades::DataModel* model READ model NOTIFY devicesUpdated)
	Q_PROPERTY(WDServer* wdServer READ wdServer NOTIFY serverReady)
	Q_PROPERTY(WDClient* wdClient READ wdClient NOTIFY clientReady)

	public:

	WiFiDirection();

	//Cleans up the connections and WiFiDirect session.
	~WiFiDirection();

	Q_SIGNALS:


	//This signal is emitted when the device list changes.
	void devicesUpdated();

	//Emitted after startting a WiFiDirect scan, and indicates whether the app was
	//able to start the scan.
	void scanStartResult(bool success, QString error);

	//Emitted once a WiFiDirect scan is complete.
	void scanComplete();

	//Provides the result of an attempt
	void connectionAttempt(bool success, QString error);

	//Emitted when a device is connected.
	void devConnected();

	//Emitted when a device is disconnected.
	void devDisconnected();

	//Emitted once the WDServer is ready to listen for incoming connections.
	void serverReady();

	//Emitted when the WDClient is connecting to the server.
	void clientReady();


	public Q_SLOTS:

	//Called when scanning is complete
	void scanCompleted();

	//Called when the WiFiDirect session is started.
	void sessionStarted(QString sessionName);

	//Called when a WiFiDirect scan finds a device.
	void deviceFound(QString name, QString hardwareAddress, bb::device::WiFiDirectDeviceType::Type type);

	//Called when a WiFiDirect scan removes a previously found device.
	void deviceLost (QString hardwareAddress);

	//Called to initiate scanning.
	void startScanning();

	//Request a connection to the device with supplied hardware address.
	void requestConnect(QString hardwareAddress);

	//Called when a connection is made over WiFiDirect.
	void deviceConnected (const QString name, const QString hardwareAddress, bb::device::WiFiDirectDeviceType::Type type,
			const QString ipAddress, bb::device::WiFiDirectIntent::Type intent);

	//Called when a device disconnects from WiFiDirect.
	void deviceDisconnected (const QString hardwareAddress);

	//Called to initiate the WDClient socket connection to the remote, automatron device.
	void connectToAutomatron();

	//Called to initiate the WDServer server to listen for connections.
	void listenForOperator();

	private:

	//The data model that holds the list of devices found during the WiFiDirect scan.
	bb::cascades::GroupDataModel* model() const;

	WDServer* wdServer();
	WDClient* wdClient();

	//Takes the WiFiDirectResult and returns a text description that can be shown to the user.
	QString resultToText(bb::device::WiFiDirectResult::Type result);

	bb::device::WiFiDirect* mDirect;
	bb::cascades::GroupDataModel* mDataModel;

	QString mSessionName;	//Name of the current WiFiDirect session.
	QString mIPAddress;
	WDClient* mWDClient;
	WDServer* mWDServer;

};


#endif /* WIFIDIRECTION_HPP_ */
