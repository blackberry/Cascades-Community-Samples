/* Copyright (c) 2013 Research In Motion Limited.
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
#ifndef BLUETOOTHLE_HPP_
#define BLUETOOTHLE_HPP_

#include <QtCore/QObject>

#include <bb/cascades/Application>

#include <btapi/btdevice.h>
#include <btapi/btgatt.h>
#include <btapi/btle.h>
#include <btapi/btspp.h>

#include "LocalDeviceInfo.hpp"
#include "RemoteDeviceInfo.hpp"
#include "DeviceListing.hpp"
#include "FindMeService.hpp"

class BluetoothLe : public QObject {

	Q_OBJECT

    Q_PROPERTY(bool bluetoothActive READ bluetoothActive NOTIFY bluetoothActiveChanged)
    Q_PROPERTY(bool discoverableActive READ discoverableActive NOTIFY discoverableActiveChanged)
    Q_PROPERTY(DeviceListing* deviceListing READ deviceListing CONSTANT)
    Q_PROPERTY(LocalDeviceInfo* localDeviceInfo READ localDeviceInfo CONSTANT)
    Q_PROPERTY(RemoteDeviceInfo* remoteDeviceInfo READ remoteDeviceInfo CONSTANT)
    Q_PROPERTY(bool remoteDeviceSelected READ remoteDeviceSelected NOTIFY remoteDeviceSelectedChanged)
    Q_PROPERTY(bool discoverableActive READ discoverableActive NOTIFY discoverableActiveChanged)
    Q_PROPERTY(bool serviceIsInitialised READ serviceIsInitialised NOTIFY serviceIsInitialisedChanged)
    Q_PROPERTY(bool findMeServiceIsConnected READ findMeServiceIsConnected NOTIFY findMeServiceIsConnectedChanged)
    Q_PROPERTY(bool deviceIsPaired READ deviceIsPaired NOTIFY deviceIsPairedChanged)

public:
	BluetoothLe(QObject *obj);
	virtual ~BluetoothLe();
	void emitBtEventSignal(int event, const QString &btAddr, const QString &eventData);

private:
	LocalDeviceInfo *_localDeviceInfo;
	DeviceListing *_deviceListing;
	RemoteDeviceInfo *_remoteDeviceInfo;
	FindMeService *_findMeService;
    QString btEventName(int event);
    bool bluetoothActive() const;
    bool discoverableActive() const;

    DeviceListing *deviceListing() const;
    LocalDeviceInfo *localDeviceInfo() const;
    RemoteDeviceInfo *remoteDeviceInfo() const;
    FindMeService *findMeService() const;
    bool remoteDeviceSelected() const;
    bool serviceIsInitialised() const;
    bool findMeServiceIsConnected() const;

    bool deviceIsPaired() const;

public slots:
    void toggleBluetoothActive();
    void toggleDiscoverableActive();
    void setRemoteDevice(const QString &address);
    void initialiseService();
    void deInitialiseService();
    void connectToImmediateAlertService();
    void disconnnectFromImmediateAlertService();
    void sendAlertOff();
    void sendAlertMild();
    void sendAlertHigh();

    void pairDevice();

private slots:
	void handleBtEventSignal(int event, const QString &btAddr, const QString &eventData);
    void serviceStatusChanged();
    void findMeServiceConnected();
    void findMeServiceDisconnected();

signals:
	void message(const QVariant &text);
	void btEventMessage(const QVariant &text);
    void btEventSignal(int event, const QString &btAddr, const QString &eventData);
    void bluetoothActiveChanged();
    void discoverableActiveChanged();
    void remoteDeviceSelectedChanged();
    void serviceIsInitialisedChanged();
    void findMeServiceIsConnectedChanged();

    void deviceIsPairedChanged();
};

#endif /* BLUETOOTHLE_HPP_ */
