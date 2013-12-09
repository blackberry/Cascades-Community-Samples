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

#include "DevicesManager.hpp"
#include "DataContainer.hpp"
#include "RemoteDeviceInfo.hpp"
#include <btapi/btdevice.h>

DevicesManager* DevicesManager::_instance;
static bool bt_initialised = false;

QString btEventName(int event)
{
    switch (event) {
        case BT_EVT_ACCESS_CHANGED:
            return QString("BT_EVT_ACCESS_CHANGED");
        case BT_EVT_RADIO_SHUTDOWN:
            return QString("BT_EVT_RADIO_SHUTDOWN");
        case BT_EVT_RADIO_INIT:
            return QString("BT_EVT_RADIO_INIT");
        case BT_EVT_CONFIRM_NUMERIC_REQUEST:
            return QString("BT_EVT_CONFIRM_NUMERIC_REQUEST");
        case BT_EVT_PAIRING_COMPLETE:
            return QString("BT_EVT_PAIRING_COMPLETE");
        case BT_EVT_DEVICE_ADDED:
            return QString("BT_EVT_DEVICE_ADDED");
        case BT_EVT_DEVICE_DELETED:
            return QString("BT_EVT_DEVICE_DELETED");
        case BT_EVT_SERVICE_CONNECTED:
            return QString("BT_EVT_SERVICE_CONNECTED");
        case BT_EVT_SERVICE_DISCONNECTED:
            return QString("BT_EVT_SERVICE_DISCONNECTED");
        case BT_EVT_LE_DEVICE_CONNECTED:
            return QString("BT_EVT_LE_DEVICE_CONNECTED");
        case BT_EVT_LE_DEVICE_DISCONNECTED:
            return QString("BT_EVT_LE_DEVICE_DISCONNECTED");
        case BT_EVT_LE_NAME_UPDATED:
            return QString("BT_EVT_LE_NAME_UPDATED");
        case BT_EVT_LE_GATT_SERVICES_UPDATED:
            return QString("BT_EVT_LE_GATT_SERVICES_UPDATED");
        case BT_EVT_FAULT:
            return QString("BT_EVT_FAULT");
        case BT_EVT_UNDEFINED_EVENT:
            return QString("BT_EVT_UNDEFINED_EVENT");
        default:
            return QString("UNKNOWN EVENT:%1").arg(event);
    }
}

void btEvent(const int event, const char *bt_addr, const char *event_data)
{

    Q_UNUSED(bt_addr)

    QString event_name = btEventName(event);
    qDebug() << "XXXX bluetooth event:" << event_name;

    if (event_data != NULL) {
        QString event_data_str = QString(*event_data);
        qDebug() << "XXXX BT event_data=" << event_data_str;
    } else {
        qDebug() << "XXXX BT event_data=NULL";
    }

}

DevicesManager::DevicesManager(QObject *parent) :
        QObject(parent), _remoteDeviceInfo(new RemoteDeviceInfo(this))
{
}

DevicesManager::~DevicesManager()
{
    qDebug() << "XXXX DevicesManager::~DevicesManager";
    _instance = 0;
}

DevicesManager* DevicesManager::getInstance(QObject *parent)
{
    qDebug() << "XXXX DevicesManager::getInstance(QObject *parent)";

    if (_instance == 0) {
        _instance = new DevicesManager(parent);
    }

    return _instance;
}

DevicesManager* DevicesManager::getDevicesManager()
{
    qDebug() << "XXXX DevicesManager::getDevicesManager()";
    return _instance;
}

void DevicesManager::findBleDevices()
{
    emit startedScanningForDevices();

    bb::system::SystemToast toast;
    toast.setBody("Searching for Bluetooth LE devices ... please wait until search has completed ...");
    toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast.show();

    if (!bt_initialised) {

        // Initialise the Bluetooth device and allocate the required resources for the library. Specify a call back function for Bluetooth events.
        bt_device_init(btEvent);
        // make sure the Bluetooth radio is switched on
        if (!bt_ldev_get_power()) {
            bt_ldev_set_power(true);
        }
        bt_initialised = true;
    }

    DataContainer *dc = DataContainer::getInstance();
    dc->clearDeviceList();
    // note that this is a blocking call. For each device discovered however, a call back is made to btEvent with event type BT_EVT_DEVICE_ADDED
    bt_disc_start_inquiry(BT_INQUIRY_GIAC);

    bt_remote_device_t **remoteDeviceArray = 0;
    bt_remote_device_t *remoteDevice = 0;

    remoteDeviceArray = bt_disc_retrieve_devices(BT_DISCOVERY_ALL, 0);

    int device_count;
    device_count=0;

    if (remoteDeviceArray) {
        for (int i = 0; (remoteDevice = remoteDeviceArray[i]); ++i) {
            const int deviceType = bt_rdev_get_type(remoteDevice);
            if ((deviceType == BT_DEVICE_TYPE_LE_PUBLIC) || (deviceType == BT_DEVICE_TYPE_LE_PRIVATE)) {
                device_count++;
                DevicesManager::getDevicesManager()->extractAndStoreBleDeviceAttributes(remoteDevice);
            }
            bt_rdev_free(remoteDevice);
        }
//        qDebug() << "YYYY DevicesManager::findBleDevices() - freeing buffer";
//        if (remoteDeviceArray) {
//            bt_rdev_free_array(remoteDeviceArray);
//        }
    }

    qDebug() << "XXXX number of devices found=" << device_count;
    dc->setDeviceCount(device_count);

    emit setDeviceCount(QVariant(device_count));
    emit finishedScanningForDevices();

}

void DevicesManager::extractAndStoreBleDeviceAttributes(bt_remote_device_t *remoteDevice)
{

    qDebug() << "XXXX DevicesManager::extractAndStoreBleDeviceAttributes";

    char buffer[128];
    bt_rdev_get_address(remoteDevice, buffer);
    _remoteDeviceInfo->populateWithDeviceAttributes(QString::fromLatin1(buffer));

    DataContainer::getInstance()->addDevice(_remoteDeviceInfo->property("name").toString().toLatin1().data(), _remoteDeviceInfo->property("address").toString().toLatin1().data(),
            _remoteDeviceInfo->property("deviceClassInt").toBool(), _remoteDeviceInfo->property("deviceTypeBool").toBool(), _remoteDeviceInfo->property("pairedBool").toBool(),
            _remoteDeviceInfo->property("encryptedBool").toBool(),
            _remoteDeviceInfo->property("knownBool").toBool()
            );

    qDebug() << "XXXX stored details of device " << _remoteDeviceInfo->name() << "(" << _remoteDeviceInfo->address() << ")";

}

void DevicesManager::selectRemoteDevice(const QString &address)
{
    qDebug() << "YYYY DevicesManager::selectRemoteDevice() " << address;
    _remoteDeviceInfo->populateWithDeviceAttributes(address);
}
