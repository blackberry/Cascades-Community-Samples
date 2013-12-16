/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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
#ifndef SERVICESMANAGER_H
#define SERVICESMANAGER_H

#include <stdint.h>
#include <errno.h>

#include <QObject>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QVariant>

#include <btapi/btdevice.h>

#include "DataContainer.hpp"

#include "Types.hpp"

typedef GenericList_t ServiceList_t;
typedef GenericListItem_t ServiceItem_t;

class ServicesManager : public QObject
{
	Q_OBJECT

    Q_PROPERTY(QString       peripheralAddress
    		   READ          peripheralAddress
    		   WRITE         setPeripheralAddress
    		   NOTIFY        peripheralAddressChanged)

    Q_PROPERTY(QString       peripheralName
    		   READ          peripheralName
               WRITE         setPeripheralName
               NOTIFY        peripheralNameChanged)

    Q_PROPERTY(ServiceList_t services
		       READ          services
			   NOTIFY        servicesChanged)

public:
	static ServicesManager* getInstance(QObject *parent = 0);

    QString peripheralName() const;
    QString peripheralAddress() const;
    ServiceList_t services() const;

	void setPeripheralAddress(const QString &address);
	void setPeripheralName(const QString &name);
	void addService(const QString &uuid);
	void resetServices();
	QString serviceDescription(const QString &uuid);
	QString serviceIcon(const QString &uuid);
	bool isWellKnownService(const QString &uuid);
	bool matchesWellKnownUuid(const QString &wellKnownUuid, const QString &uuidToCheck);

	static QString KEY_SERVICE_UUID;
	static QString KEY_SERVICE_DESCRIPTION;
	static QString KEY_SERVICE_ICON;
	static QString KEY_SERVICE_BTLE;

	Q_INVOKABLE	QString getServiceName(int serviceIndex);
	Q_INVOKABLE	QString getServiceUuid(int serviceIndex);
	Q_INVOKABLE int getServiceCount();

private:
	ServicesManager(QObject *parent = 0);
	virtual ~ServicesManager();
	void createWellKnownServiceList();

	void enumerateServices(bt_remote_device_t *remoteDevice);
	void pairDeviceIfRequired(bt_remote_device_t *remoteDevice);

	static QString DEFAULT_SERVICE_ICON;
	static ServicesManager *_instance;

	QString _peripheralAddress;
    QString _peripheralName;
    ServiceList_t _services;
    ServiceList_t _wellKnownServices;
    int _numberOfServices;
    bool _peripheralPaired;
    bool _peripheralKnown;
    int _peripheralIndex;

signals:
	void peripheralAddressChanged(const QString &address);
	void peripheralNameChanged(const QString &name);
	void servicesChanged();
	void serviceSelected(const QString &uuid);
	void foundService(const QString &uuid, const QString &description);
    void setServiceCount(QVariant count);

public slots:
    void selectService(const QString &uuid);
    void deviceSelected(const QVariant &device_index, const QVariant &deviceAddress);
};

#endif // ifndef SERVICESMANAGER_H
