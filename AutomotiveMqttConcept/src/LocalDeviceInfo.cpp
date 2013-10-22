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

#include "LocalDeviceInfo.hpp"

#include <btapi/btdevice.h>
#include <btapi/btspp.h>

LocalDeviceInfo::LocalDeviceInfo(QObject *parent)
    : QObject(parent)
{
}

void LocalDeviceInfo::update()
{
    char buffer[128];
    const int bufferSize = sizeof(buffer);

    bool ok = false;

    const QString unknown = tr("Unknown");
    const QString notAvailable = tr("N/A");

    ok = (bt_ldev_get_friendly_name(buffer, bufferSize) == 0);
    _name = (ok ? QString::fromLatin1(buffer) : unknown);

    ok = (bt_ldev_get_address(buffer) == 0);
    _address = (ok ? QString::fromLatin1(buffer) : unknown);

    const int code = bt_ldev_get_device_class(BT_COD_DEVICECLASS);
    if (code >= 0) {
        _deviceClass.sprintf("0x%x", code);
    } else {
        _deviceClass = unknown;
    }

    _discoverable = ((bt_ldev_get_discoverable() == BT_DISCOVERABLE_GIAC) ? tr("true") : tr("false"));
    _enabled = (bt_ldev_get_power() ? tr("true") : tr("false"));

    const int deviceType = bt_ldev_get_type();
    _deviceType = ((deviceType == BT_DEVICE_TYPE_LE_PUBLIC || deviceType == BT_DEVICE_TYPE_LE_PUBLIC) ?  tr("Low energy") : tr("Regular"));

    ok = (bt_ldev_get_property(BT_PROP_API_VERSION, buffer, bufferSize) == 0);
    _apiVersion = (ok ? QString::fromLatin1(buffer) : notAvailable);

    ok = (bt_ldev_get_property(BT_PROP_MASTER_SLAVE_SWITCH, buffer, bufferSize) == 0);
    _masterSlaveAllowed = (ok ? QString::fromLatin1(buffer) : notAvailable);

    ok = (bt_ldev_get_property(BT_PROP_MAX_CONNECTED_DEVICES, buffer, bufferSize) == 0);
    _maximumConnections = (ok ? QString::fromLatin1(buffer) : notAvailable);

    ok = (bt_ldev_get_property(BT_PROP_MAX_L2CAP_RCVMTU, buffer, bufferSize) == 0);
    _maximumL2capMtu = (ok ? QString::fromLatin1(buffer) : notAvailable);

    ok = (bt_ldev_get_property(BT_PROP_MAX_RFCOMM_RCVMTU, buffer, bufferSize) == 0);
    _maximumRfcommMtu = (ok ? QString::fromLatin1(buffer) : notAvailable);

    ok = (bt_ldev_get_property(BT_PROP_CONNECTED_INQUIRY_SCAN, buffer, bufferSize) == 0);
    _incomingInquiryScan = (ok ? QString::fromLatin1(buffer) : notAvailable);

    ok = (bt_ldev_get_property(BT_PROP_CONNECTED_PAGE_SCAN, buffer, bufferSize) == 0);
    _incomingPageScan = (ok ? QString::fromLatin1(buffer) : notAvailable);

    ok = (bt_ldev_get_property(BT_PROP_CONNECTED_INQUIRY, buffer, bufferSize) == 0);
    _outgoingInquiryScan = (ok ? QString::fromLatin1(buffer) : notAvailable);

    ok = (bt_ldev_get_property(BT_PROP_CONNECTED_PAGE, buffer, bufferSize) == 0);
    _outgoingPageScan = (ok ? QString::fromLatin1(buffer) : notAvailable);

    emit changed();
}

QString LocalDeviceInfo::name() const
{
    return _name;
}

QString LocalDeviceInfo::address() const
{
    return _address;
}

QString LocalDeviceInfo::deviceClass() const
{
    return _deviceClass;
}

QString LocalDeviceInfo::discoverable() const
{
    return _discoverable;
}

QString LocalDeviceInfo::enabled() const
{
    return _enabled;
}

QString LocalDeviceInfo::deviceType() const
{
    return _deviceType;
}

QString LocalDeviceInfo::apiVersion() const
{
    return _apiVersion;
}

QString LocalDeviceInfo::masterSlaveAllowed() const
{
    return _masterSlaveAllowed;
}

QString LocalDeviceInfo::maximumConnections() const
{
    return _maximumConnections;
}

QString LocalDeviceInfo::maximumL2capMtu() const
{
    return _maximumL2capMtu;
}

QString LocalDeviceInfo::maximumRfcommMtu() const
{
    return _maximumRfcommMtu;
}

QString LocalDeviceInfo::incomingInquiryScan() const
{
    return _incomingInquiryScan;
}

QString LocalDeviceInfo::incomingPageScan() const
{
    return _incomingPageScan;
}

QString LocalDeviceInfo::outgoingInquiryScan() const
{
    return _outgoingInquiryScan;
}

QString LocalDeviceInfo::outgoingPageScan() const
{
    return _outgoingPageScan;
}
