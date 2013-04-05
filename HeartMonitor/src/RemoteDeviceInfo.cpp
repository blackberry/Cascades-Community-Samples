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

#include "RemoteDeviceInfo.hpp"

#include <btapi/btdevice.h>
#include <btapi/btspp.h>

RemoteDeviceInfo::RemoteDeviceInfo(QObject *parent)
    : QObject(parent)
    , _model(new bb::cascades::GroupDataModel(QStringList() << "uuid" << "serviceDescription" << "address" << "serviceType", this))
{
    _model->setSortingKeys(QStringList() << "serviceType");
    _model->setGrouping(bb::cascades::ItemGrouping::ByFullValue);
}

void RemoteDeviceInfo::update(const QString &deviceAddress)
{
    bt_remote_device_t *remoteDevice = bt_rdev_get_device(deviceAddress.toAscii());

    qDebug() << "YYYY RemoteDeviceInfo::update : deviceAddress" << deviceAddress;

    if (!remoteDevice)
        return;

    bool ok = false;
    char buffer[128];
    const int bufferSize = sizeof(buffer);

    const QString unknown = tr("Unknown");
    const QString notAvailable = tr("N/A");

    ok = (bt_rdev_get_friendly_name(remoteDevice, buffer, bufferSize) == 0);
    _name = (ok ? QString::fromLatin1(buffer) : unknown);

    qDebug() << "YYYY RemoteDeviceInfo::update: _name" << _name;

    _address = deviceAddress;

    const int deviceClass = bt_rdev_get_device_class(remoteDevice, BT_COD_DEVICECLASS);
    if (deviceClass >= 0) {
        _deviceClass.sprintf("0x%x", deviceClass);
    } else {
        _deviceClass = unknown;
    }
    _deviceClassInt = deviceClass;

    const int deviceType = bt_rdev_get_type(remoteDevice);
    _deviceType = ((deviceType == BT_DEVICE_TYPE_LE_PUBLIC || deviceType == BT_DEVICE_TYPE_LE_PUBLIC) ? tr("Low energy") : tr("Regular"));
    _deviceTypeInt = deviceType;

    _encrypted = ((bt_rdev_is_encrypted(remoteDevice) >= 0) ? tr("true") : tr("false"));
    _encryptedBool = bt_rdev_is_encrypted(remoteDevice);

    bool paired = false;
    ok = (bt_rdev_is_paired(remoteDevice, &paired) == 0);
    _paired = (ok ? (paired ? tr("true") : tr("false")) : unknown);
    _pairedBool = paired;

    _trusted = (bt_rdev_is_trusted(remoteDevice) ? tr("true") : tr("false"));

    int rssi = 0;
    ok = (bt_rdev_get_rssi(remoteDevice, &rssi) == 0);
    _rssi = (ok ? QString::number(rssi) : unknown);

    uint16_t minConnIvl, maxConnIvl, latency, superTmo, appearance;
    ok = (bt_rdev_get_le_conn_params(remoteDevice, &minConnIvl, &maxConnIvl, &latency, &superTmo) == 0);

    _minimumConnectionInterval = (ok ? QString::number(minConnIvl) : notAvailable);
    _maximumConnectionInterval = (ok ? QString::number(maxConnIvl) : notAvailable);
    _latency = (ok ? QString::number(latency) : notAvailable);
    _supervisoryTimeout = (ok ? QString::number(superTmo) : notAvailable);

    uint8_t flags, connectable;
    ok = (bt_rdev_get_le_info(remoteDevice, &appearance, &flags, &connectable) == 0);

    _appearance = (ok ? QString::number(appearance) : notAvailable);
    _flags = (ok ? QString::number(flags) : notAvailable);
    _connectable = (ok ? QString::number(connectable) : notAvailable);

    _model->clear();

    char **servicesArray = bt_rdev_get_services(remoteDevice);
    if (servicesArray) {
        for (int i = 0; servicesArray[i]; i++) {
            QVariantMap map;

            const QString uuid = QString::fromLatin1(servicesArray[i]);

            map["uuid"] = uuid;
            map["address"] = _address;
            map["serviceType"] = tr("Bluetooth BR/EDR Services");
            map["serviceDescription"] = serviceDescription(uuid);

            _model->insert(map);
        }

        bt_rdev_free_services(servicesArray);
    }

    if (deviceType == BT_DEVICE_TYPE_LE_PUBLIC || deviceType == BT_DEVICE_TYPE_LE_PRIVATE) {
        servicesArray = bt_rdev_get_services_gatt(remoteDevice);
        if (servicesArray) {
            for (int i = 0; servicesArray[i]; i++) {
                QVariantMap map;

                map["uuid"] = QString::fromLatin1(servicesArray[i]);
                map["address"] = _address;
                map["serviceType"] = tr("Bluetooth LE GATT Services");
                map["serviceDescription"] = serviceDescription(QString::fromLatin1(servicesArray[i]));

                _model->insert(map);
            }

            bt_rdev_free_services(servicesArray);
        }
    }

    bt_rdev_free(remoteDevice);

    emit changed();
}

void RemoteDeviceInfo::reset() {
	_model->clear();

	_name = "";
    _address = "";
    _deviceClass = "";
    _deviceType = "";
    _encrypted = "";
    _paired = "";
    _trusted = "";
    _rssi = "";
    _minimumConnectionInterval = "";
    _maximumConnectionInterval = "";
    _latency = "";
    _supervisoryTimeout = "";
    _appearance = "";
    _flags = "";
    _connectable = "";

    emit changed();
}

QString RemoteDeviceInfo::serviceDescription(const QString &uuid) {

	qDebug() << "YYYY RemoteDeviceInfo::serviceDescription() - uuid=" << uuid;

	QRegExp rx;
	QString tempUuid;
	rx.setPattern("^(0x)?([0-9a-f]{4,4})$");
	rx.setCaseSensitivity(Qt::CaseInsensitive);

	if (rx.exactMatch(uuid)) {
		qDebug() << "YYYY RemoteDeviceInfo::serviceDescription() - uuid matched rx.cap(2)=" << rx.cap(2);
		tempUuid = rx.cap(2);
	} else {
		qDebug() << "YYYY RemoteDeviceInfo::serviceDescription() - uuid not matched=";
	}

	if (tempUuid.toUpper().startsWith("0001"))
        return tr("SDP");
    else if (tempUuid.toUpper().startsWith("0003"))
        return tr("RFCOMM");
    else if (tempUuid.toUpper().startsWith("0008"))
        return tr("OBEX");
    else if (tempUuid.toUpper().startsWith("000C"))
        return tr("HTTP");
    else if (tempUuid.toUpper().startsWith("0100"))
        return tr("L2CAP");
    else if (tempUuid.toUpper().startsWith("000F"))
        return tr("BNEP");
    else if (tempUuid.toUpper().startsWith("1000"))
        return tr("Service Discovery");
    else if (tempUuid.toUpper().startsWith("1001"))
        return tr("Browse Group Descriptor");
    else if (tempUuid.toUpper().startsWith("1002"))
        return tr("Public Browse Group");
    else if (tempUuid.toUpper().startsWith("1101"))
        return tr("Serial Port");
    else if (tempUuid.toUpper().startsWith("1102"))
        return tr("Public Browse Group");
    else if (tempUuid.toUpper().startsWith("1105"))
        return tr("OBEX Object Push");
    else if (tempUuid.toUpper().startsWith("1106"))
        return tr("OBEX File Transfer");
    else if (tempUuid.toUpper().startsWith("1115"))
        return tr("Personal Area Networking");
    else if (tempUuid.toUpper().startsWith("1116"))
        return tr("Network Access Point");
    else if (tempUuid.toUpper().startsWith("1117"))
        return tr("Group Network");
    else if (tempUuid.toUpper().startsWith("1800"))
        return tr("Generic Access");
    else if (tempUuid.toUpper().startsWith("1801"))
        return tr("Generic Attribute");
	else if (tempUuid.toUpper().startsWith("1802"))
        return tr("ImmediateAlert");
	else if (tempUuid.toUpper().startsWith("1803"))
        return tr("Link Loss");
	else if (tempUuid.toUpper().startsWith("1804"))
        return tr("Tx Power");
	else if (tempUuid.toUpper().startsWith("1805"))
        return tr("Current Time Service");
	else if (tempUuid.toUpper().startsWith("1806"))
        return tr("Reference Time Update Service");
	else if (tempUuid.toUpper().startsWith("1807"))
        return tr("Next DST Change Service");
	else if (tempUuid.toUpper().startsWith("1808"))
        return tr("Glucose");
	else if (tempUuid.toUpper().startsWith("1809"))
        return tr("Health Thermometer");
	else if (tempUuid.toUpper().startsWith("180A"))
        return tr("Device Information");
	else if (tempUuid.toUpper().startsWith("180D"))
        return tr("Heart Rate");
	else if (tempUuid.toUpper().startsWith("180E"))
        return tr(":Phone Alert Status Service");
	else if (tempUuid.toUpper().startsWith("180F"))
        return tr("Battery Service");
	else if (tempUuid.toUpper().startsWith("1810"))
        return tr("Blood Pressure");
	else if (tempUuid.toUpper().startsWith("1811"))
        return tr("Alert Notification Service");
	else if (tempUuid.toUpper().startsWith("1812"))
        return tr("Human Interface Device");
	else if (tempUuid.toUpper().startsWith("1813"))
        return tr("Scan Parameters");
	else if (tempUuid.toUpper().startsWith("1814"))
        return tr("Running Speed and Cadance");
    else
        return tr("Other");
}

bb::cascades::DataModel* RemoteDeviceInfo::model() const
{
    return _model;
}

QString RemoteDeviceInfo::name() const
{
    return _name;
}

QString RemoteDeviceInfo::address() const
{
    return _address;
}

QString RemoteDeviceInfo::deviceClass() const
{
    return _deviceClass;
}

QString RemoteDeviceInfo::deviceType() const
{
    return _deviceType;
}

QString RemoteDeviceInfo::encrypted() const
{
    return _encrypted;
}

QString RemoteDeviceInfo::paired() const
{
    return _paired;
}

QString RemoteDeviceInfo::trusted() const
{
    return _trusted;
}

QString RemoteDeviceInfo::rssi() const
{
    return _rssi;
}

QString RemoteDeviceInfo::minimumConnectionInterval() const
{
    return _minimumConnectionInterval;
}

QString RemoteDeviceInfo::maximumConnectionInterval() const
{
    return _maximumConnectionInterval;
}

QString RemoteDeviceInfo::latency() const
{
    return _latency;
}

QString RemoteDeviceInfo::supervisoryTimeout() const
{
    return _supervisoryTimeout;
}

QString RemoteDeviceInfo::appearance() const
{
    return _appearance;
}

QString RemoteDeviceInfo::flags() const
{
    return _flags;
}

QString RemoteDeviceInfo::connectable() const
{
    return _connectable;
}

int RemoteDeviceInfo::deviceClassInt() const
{
    return _deviceClassInt;
}

int RemoteDeviceInfo::deviceTypeInt() const
{
    return _deviceTypeInt;
}

bool RemoteDeviceInfo::encryptedBool() const
{
    return _encryptedBool;
}

bool RemoteDeviceInfo::pairedBool() const
{
    return _pairedBool;
}

