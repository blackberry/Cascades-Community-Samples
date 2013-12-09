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
#ifndef CHARACTERISTICSMANAGER_H
#define CHARACTERISTICSMANAGER_H

#include <stdint.h>
#include <errno.h>

#include <QObject>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <bb/cascades/GroupDataModel>
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>

#include <btapi/btgatt.h>
#include <btapi/btdevice.h>

#include "Types.hpp"
#include "ServicesManager.hpp"

typedef GenericList_t CharacteristicsList_t;
typedef GenericListItem_t CharacteristicsItem_t;

typedef GenericList_t DescriptorList_t;
typedef GenericListItem_t DescriptorItem_t;

using namespace bb::cascades;

class CharacteristicsManager : public QObject
{
	Q_OBJECT

    Q_PROPERTY(QString       serviceUuid
    		   READ          serviceUuid
    		   WRITE         setServiceUuid
    		   NOTIFY        serviceUuidChanged)

    Q_PROPERTY(QString       serviceDescription
    		   READ          serviceDescription
               WRITE         setServiceDescription
               NOTIFY        serviceDescriptionChanged)

	Q_PROPERTY(DataModel*    model
			   READ          model
			   CONSTANT)

public:
	static CharacteristicsManager* getInstance(QObject *parent = 0);

    QString serviceUuid() const;
    QString serviceDescription() const;

    void setServiceUuid(const QString &uuid);
    void setServiceDescription(const QString &description);
    void reset();
	void addCharacteristic(const QString &uuid, uint16_t handle, uint16_t valueHandle, bt_gatt_char_prop_mask properties);

	QString characteristicDescription(const QString &uuid);
	QString descriptorDescription(const QString &uuid);
	bool isWellKnownCharacteristic(const QString &uuid);
	bool isWellKnownDescriptor(const QString &uuid);
	bool matchesWellKnownUuid(const QString &wellKnownUuid, const QString &uuidToCheck);

    static QString KEY_CHARACTERISTIC_UUID;
    static QString KEY_CHARACTERISTIC_HANDLE;
    static QString KEY_CHARACTERISTIC_VALUEHANDLE;
    static QString KEY_CHARACTERISTIC_HEX_VALUE;
    static QString KEY_CHARACTERISTIC_DESCRIPTION;
    static QString KEY_CHARACTERISTIC_PROP_BROADCAST;
    static QString KEY_CHARACTERISTIC_PROP_READ;
    static QString KEY_CHARACTERISTIC_PROP_WRITE_NORESP;
    static QString KEY_CHARACTERISTIC_PROP_WRITE;
    static QString KEY_CHARACTERISTIC_PROP_NOTIFY;
    static QString KEY_CHARACTERISTIC_PROP_INDICATE;
    static QString KEY_CHARACTERISTIC_PROP_WRITE_SIGNED;
    static QString KEY_CHARACTERISTIC_PROP_EXT_PROP;

    static QString KEY_DESCRIPTOR_UUID;
	static QString KEY_DESCRIPTOR_DESCRIPTION;

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

	DataModel* model() const;

private:
    CharacteristicsManager(QObject *parent = 0);
	virtual ~CharacteristicsManager();

	void createWellKnownCharacteristicsList();
	void createWellKnownDescriptorList();
	void initialiseGatt();
	void terminateGatt();
	void connectToSelectedService(const QString &serviceUuid);
	void disconnectFromSelectedService();

	static CharacteristicsManager* _instance;

	QString _serviceUuid;
    QString _serviceDescription;
    CharacteristicsList_t _wellKnownCharacteristics;
    DescriptorList_t _wellKnownDescriptors;
    GroupDataModel* _model;
    int _selectedServiceInstance;
    QString getCharacteristicHexValue(uint16_t handle);

signals:
	void serviceUuidChanged();
	void serviceDescriptionChanged();
	void characteristicSelected(const QString &characteristicUuid, const QVariant indexPath);
	void selectedServiceDisconnected();
	void selectedServiceUpdated();
	void selectedServiceConnected();
	void scanStarted(const QString &name, const QString &service);
	void scanStopped();
	void connectionError(const QString &uuid);

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

public slots:
	void selectCharacteristic(const QString &characteristicUuid, const QVariant indexPath);

private slots:
	void serviceSelected(const QString &serviceUuid);
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
};

#endif // ifndef CHARACTERISTICSMANAGER_H
