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
#ifndef FINDMESERVICE_HPP_
#define FINDMESERVICE_HPP_

#include <QObject>
#include <bb/cascades/Application>

#include <bb/system/SystemToast.hpp>

#include <btapi/btdevice.h>
#include <btapi/btgatt.h>
#include <btapi/btle.h>
#include <btapi/btspp.h>

#include <errno.h>
#include "Utilities.hpp"

typedef struct {
	int uuid;
	const char *name;
} uuidnames_t;

class FindMeService  : public QObject
{
    Q_OBJECT
    Q_ENUMS(AlertLevel)

public:
	FindMeService(QObject *parent = 0);
	virtual ~FindMeService();

	enum AlertLevel {Off, Mild, High};

	void emitGattServiceConnected(
			const QString &bdaddr, const QString &service, int instance,
			int err, uint16_t connInt, uint16_t latency,
			uint16_t superTimeout, void *userData
			);
	void emitGattServiceDisconnected(
			const QString &bdaddr, const QString &service,
			int instance, int reason, void *userData
			);
	void emitGattServiceUpdated(
			const QString &bdaddr, int instance,
			uint16_t connInt, uint16_t latency,
			uint16_t superTimeout, void *userData
			);

	//QString remoteDeviceAddress() const;

private:
    bool _serviceIsInitialised;
    QString _remoteDeviceAddress;
    QString IMMEDIATE_ALERT_SERVICE_UUID;
    const QString IMMEDIATE_ALERT_LEVEL_UUID;
    AlertLevel _alertLevel;
    int _serviceInstance;
    uint16_t _alertLevelServiceHandle;
    uint16_t _alertLevelValueHandle;

    const char *parseCharacteristicUuid(const char *uuid);
    const char *parseDescriptorUuid(const char *uuid);
    const char *parseServiceUuid(const char *uuid);
    const char *parseUuid(const char *uuid, const uuidnames_t uuidNames[]);

public slots:
	void initialiseService();
	void deInitialiseService();
    void setRemoteDevice(const QString &address);
    void setAlertLevel(const AlertLevel level);
	bool serviceIsInitialised() const;
	bool findMeServiceIsConnected() const;
	void connectToImmediateAlertService();
	void disconnectFromImmediateAlertService();
	void sendAlertRequest();

private slots:
	void handleGattServiceConnected(
			const QString &bdaddr, const QString &service, int instance,
			int err, uint16_t connInt, uint16_t latency,
			uint16_t superTimeout, void *userData
			);
	void handleGattServiceDisconnected(
			const QString &bdaddr, const QString &service,
			int instance, int reason, void *userData
			);
	void handleGattServiceUpdated(
			const QString &bdaddr, int instance,
			uint16_t connInt, uint16_t latency,
			uint16_t superTimeout, void *userData
			);

signals:
	void gattServiceConnected(
			const QString &bdaddr, const QString &service, int instance,
			int err, uint16_t connInt, uint16_t latency,
			uint16_t superTimeout, void *userData
			);
	void gattServiceDisconnected(
			const QString &bdaddr, const QString &service,
			int instance, int reason, void *userData
			);
	void gattServiceUpdated(
			const QString &bdaddr, int instance,
			uint16_t connInt, uint16_t latency,
			uint16_t superTimeout, void *userData
			);
	void serviceStatusChanged();
	void findMeServiceConnected();
	void findMeServiceDisconnected();
	void findMeServiceUpdated();

};

Q_DECLARE_METATYPE(uint16_t)

#endif /* FINDMESERVICE_HPP_ */
