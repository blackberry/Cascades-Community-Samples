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

#include "DeviceListing.hpp"

DeviceListing::DeviceListing(QObject *parent)
    : QObject(parent)
    , _model(new bb::cascades::GroupDataModel(QStringList() << "deviceName" << "deviceAddress" << "deviceClass" << "deviceType", this))
	, IMMEDIATE_ALERT_SERVICE_UUID("1802")
{
    _model->setSortingKeys(QStringList() << "deviceType");
    _model->setGrouping(bb::cascades::ItemGrouping::ByFullValue);

	if (!Utilities::getOSVersion().startsWith("10.0")) {
		IMMEDIATE_ALERT_SERVICE_UUID.prepend("0x");
	}
}

void DeviceListing::update()
{
    _model->clear();

    bt_remote_device_t **remoteDeviceArray;
    bt_remote_device_t *nextRemoteDevice = 0;

    remoteDeviceArray = bt_disc_retrieve_devices(BT_DISCOVERY_PREKNOWN, 0);
    if (remoteDeviceArray) {
        for (int i = 0; (nextRemoteDevice = remoteDeviceArray[i]); ++i) {
            QVariantMap map;
            char buffer[128];
            const int bufferSize = sizeof(buffer);

            if (isAnImmediateAlertDevice(nextRemoteDevice)) {
                qDebug() << "XXXX DeviceListing::update() - found an Immediate Alert device";
				bt_rdev_get_friendly_name(nextRemoteDevice, buffer, bufferSize);
				map["deviceName"] = QString::fromLatin1(buffer);
				bt_rdev_get_address(nextRemoteDevice, buffer);
				map["deviceAddress"] = QString::fromLatin1(buffer);
				map["deviceClass"] = QString::number(bt_rdev_get_device_class(nextRemoteDevice, BT_COD_DEVICECLASS));
				map["deviceType"] = tr("Paired Bluetooth Devices Capable of Immediate Alert");
				_model->insert(map);
            } else {
                qDebug() << "XXXX DeviceListing::update() - not an Immediate Alert device";
            }
        }

        bt_rdev_free_array(remoteDeviceArray);
    }
}

void DeviceListing::discover()
{
	bb::system::SystemToast toast;
    toast.setBody(tr("Searching for Bluetooth devices ... please wait until search completed ..."));
    toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast.show();

    bt_disc_start_inquiry(BT_INQUIRY_GIAC);
    delay(5);
    bt_disc_cancel_inquiry();

    toast.cancel();
    toast.setBody(tr("Search completed!"));
    toast.exec();

    update();

    bt_remote_device_t *nextRemoteDevice = 0;

    bt_remote_device_t **remoteDeviceArray = bt_disc_retrieve_devices(BT_DISCOVERY_CACHED, 0);
    if (remoteDeviceArray) {
        for (int i = 0; (nextRemoteDevice = remoteDeviceArray[i]); ++i) {
            QVariantMap map;
            char buffer[128];
            const int bufferSize = sizeof(buffer);

            if (isAnImmediateAlertDevice(nextRemoteDevice)) {
				bt_rdev_get_friendly_name(nextRemoteDevice, buffer, bufferSize);
				map["deviceName"] = QString::fromLatin1(buffer);
				bt_rdev_get_address(nextRemoteDevice, buffer);
				map["deviceAddress"] = QString::fromLatin1(buffer);
				map["deviceClass"] = QString::number(bt_rdev_get_device_class(nextRemoteDevice, BT_COD_DEVICECLASS));
				map["deviceType"] = tr("Bluetooth Devices Nearby Capable of Immediate Alert");
				_model->insert(map);
            } else {
                qDebug() << "XXXX DeviceListing::update() - not an Immediate Alert device";
            }
        }

        bt_rdev_free_array(remoteDeviceArray);
    }
}

bool DeviceListing::isAnImmediateAlertDevice(bt_remote_device_t *remoteDevice) {

    qDebug() << "XXXX DeviceListing::isAnImmediateAlertDevice() - entering";

	bool foundImmediateAlert = false;

	if (!remoteDevice) {
	    qDebug() << "XXXX DeviceListing::isAnImmediateAlertDevice() - no devices - exiting";
        return foundImmediateAlert;
	}

    const int deviceType = bt_rdev_get_type(remoteDevice);
	if ((deviceType == BT_DEVICE_TYPE_LE_PUBLIC) || (deviceType == BT_DEVICE_TYPE_LE_PRIVATE)) {
		char **servicesArray = bt_rdev_get_services_gatt(remoteDevice);
		if (servicesArray) {
			for (int i = 0; servicesArray[i]; i++) {
				if (strcasecmp(servicesArray[i], IMMEDIATE_ALERT_SERVICE_UUID.toAscii().constData()) == 0) {
					foundImmediateAlert = true;
					break;
				}
			}
			bt_rdev_free_services(servicesArray);
		}
	}
	return foundImmediateAlert;
}

bb::cascades::DataModel* DeviceListing::model() const
{
    return _model;
}
