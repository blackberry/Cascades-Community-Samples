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

#include "WiFiDirection.hpp"
#include <QThread>

WiFiDirection::WiFiDirection()
: mDirect(new bb::device::WiFiDirect(this))
, mDataModel(new bb::cascades::GroupDataModel(QStringList() << "Name" << "HardwareAddress", this))
, mIPAddress("127.0.0.1")
, mSessionName("")
{

	//Connect all the signals to slots that we'll be using.
	QObject::connect(mDirect, SIGNAL(scanCompleted()), this, SLOT(scanCompleted()));

	QObject::connect(mDirect, SIGNAL(
			deviceFound(QString, QString, bb::device::WiFiDirectDeviceType::Type)),
			this,
			SLOT(deviceFound(QString, QString, bb::device::WiFiDirectDeviceType::Type)));

	QObject::connect(mDirect, SIGNAL(deviceLost(const QString)),
			this, SLOT(deviceLost(QString)));

	QObject::connect(mDirect, SIGNAL(sessionStarted(const QString)),
				this, SLOT(sessionStarted(QString)));

	QObject::connect(mDirect, SIGNAL(deviceConnected (const QString, const QString, bb::device::WiFiDirectDeviceType::Type, const QString, bb::device::WiFiDirectIntent::Type)),
					this, SLOT(deviceConnected (const QString, const QString, bb::device::WiFiDirectDeviceType::Type, const QString, bb::device::WiFiDirectIntent::Type)));

	QObject::connect(mDirect, SIGNAL(deviceDisconnected (const QString)),
					this, SLOT(deviceDisconnected (const QString)));
}

//Clean up the connections and WiFiDirect session.
WiFiDirection::~WiFiDirection()
{
	if (mWDClient != NULL)
	{
		qDebug() << "Disconnecting client.";
		mWDClient->doDisconnect();
	}

	if (mWDServer != NULL)
	{
		qDebug() << "Disconnecting server.";
		mWDServer->doDisconnect();
	}

	if (mSessionName.size() > 0)
	{
		qDebug() << "Stopping session.";
		mDirect->requestStopSession(mSessionName);
	}
}

//Called when the WiFiDirect session is started.
void WiFiDirection::sessionStarted(QString sessionName)
{
	qDebug() << "Session started.  Name: " << sessionName;
	mSessionName = sessionName;
}

//Called to initiate scanning.
void WiFiDirection::startScanning()
{
	qDebug() << "Started scanning...";


	//Make sure WiFi Direct isn't currently in use.
	if (bb::device::WiFiDirectConnectionState::Connected == mDirect->connectionState())
	{
		emit scanStartResult(false, "WiFi Direct or Miracast in use.  Go to Settings -> Network Connections -> Wi-Fi -> Wi-Fi Direct to view and disconnect the current active connection.");
		return;
	}

	//Close the existing session and connections if they exist.
	if (mWDClient != NULL)
	{
		qDebug() << "Disconnecting client.";
		mWDClient->doDisconnect();
	}

	if (mWDServer != NULL)
	{
		qDebug() << "Disconnecting server.";
		mWDServer->doDisconnect();
	}

	if (mSessionName.size() > 0)
	{
		qDebug() << "Stopping session.";
		mDirect->requestStopSession(mSessionName);
		mSessionName = "";
	}

	//Clear out any previous devices in the data model.
	mDataModel->clear();
	emit devicesUpdated();

	bb::device::WiFiDirectResult::Type result;

	//Request a scan for devices.  Required for both devices.  Device can only be found
	//while it is also scanning.
	result = mDirect->requestScan();

	bool success = true;
	QString resultError;

	//Check if the scan was executed successfully.
	if (bb::device::WiFiDirectResult::Success != result)
	{
		success = false;
		resultError = resultToText(result);
	}

	emit scanStartResult(success, resultError);
}

//Called when scanning is complete
void WiFiDirection::scanCompleted()
{
	qDebug() << "Scan completed!";

	emit scanComplete();
}

//Called when a WiFiDirect scan finds a device.
void WiFiDirection::deviceFound(QString name, QString hardwareAddress, bb::device::WiFiDirectDeviceType::Type type)
{
	qDebug() << "Device found: " << name << " + " << hardwareAddress << " type " << type;

	//Only add devices with type Telephone.
	if (bb::device::WiFiDirectDeviceType::Telephone == type)
	{
		QVariantMap map;
		map["Name"] = name;
		map["HardwareAddress"] = hardwareAddress;
		mDataModel->insert(map);
	}
}

//Called when a WiFiDirect scan removes a previously found device.
void WiFiDirection::deviceLost (QString hardwareAddress)
{
	qDebug() << "Device lost: " << hardwareAddress;
}

//Called when a connection is made over WiFiDirect.
void WiFiDirection::deviceConnected (const QString name, const QString hardwareAddress, bb::device::WiFiDirectDeviceType::Type type,
		const QString ipAddress, bb::device::WiFiDirectIntent::Type intent)
{
	qDebug() << "Device Connected: " << hardwareAddress;
	mIPAddress = ipAddress;
	emit devConnected();
}

//Called when a device disconnects from WiFiDirect.
void WiFiDirection::deviceDisconnected(const QString hardwareAddress)
{
	qDebug() << "Device Disconnected: " << hardwareAddress;

	emit devDisconnected();
}

//The data model that holds the list of devices found during the WiFiDirect scan.
bb::cascades::GroupDataModel* WiFiDirection::model() const
{
    return mDataModel;
}

//Request a connection to the device with supplied hardware address.
void WiFiDirection::requestConnect(QString hardwareAddress)
{
	qDebug() << "Connecting to: " << hardwareAddress;

	bool success = true;
	QString resultError;

	bb::device::WiFiDirectResult::Type result =
			mDirect->requestConnect(hardwareAddress, bb::device::WiFiDirectIntent::Client, bb::device::WiFiDirectWpsMethod::PushButtonConfiguration, "");

	if (bb::device::WiFiDirectResult::Success != result)
	{
		success = false;
		resultError = resultToText(result);
	}

	emit connectionAttempt(success, resultError);
}

//Called to initiate the WDClient socket connection to the remote, automatron device.
void WiFiDirection::connectToAutomatron()
{
	mDirect->requestStartSession();
	mWDClient = new WDClient(this);
	mWDClient->connectToAuto(mIPAddress);
	emit clientReady();
}

//Called to initiate the WDServer server to listen for connections.
void WiFiDirection::listenForOperator()
{
	mDirect->requestStartSession();

	mWDServer = new WDServer(this);
	mWDServer->listen();
	emit serverReady();
}

//Takes the WiFiDirectResult and returns a text description that can be shown to the user.
QString WiFiDirection::resultToText(bb::device::WiFiDirectResult::Type result)
{
	QString resultError = "Unknown error has occurred.";

	switch (result)
		{
		case bb::device::WiFiDirectResult::Success:
			break;

		case bb::device::WiFiDirectResult::InternalError:
			resultError = "Error: An unrecoverable internal error occurred.";
			break;

		case bb::device::WiFiDirectResult::Busy:
			resultError =
					"Error: Cannot scan as system is busy and being used by other WiFi direct service (eg.Miracast).";
			break;

		case bb::device::WiFiDirectResult::DeviceNotFound:
			resultError = "Error: Device with specified hardware address was not found.";
			break;

		case bb::device::WiFiDirectResult::NoNetworkConnection:
			resultError = "Error: Device was not connected to a network.";
			break;

		case bb::device::WiFiDirectResult::NoRequestToCancel:
			resultError = "Error: No active connection attempt available to cancel.";
			break;

		case bb::device::WiFiDirectResult::NotSupported:
			resultError = "Error: WiFiDirect is not supported on this device.";
			break;

		case bb::device::WiFiDirectResult::InvalidSession:
			resultError = "Error: The supplied session was not found.";
			break;
		}

	return resultError;
}

WDServer* WiFiDirection::wdServer()
{
	return mWDServer;
}

WDClient* WiFiDirection::wdClient()
{
	return mWDClient;
}
