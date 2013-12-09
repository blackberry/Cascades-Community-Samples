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

#include "ServicesManager.hpp"
#include "CharacteristicsManager.hpp"

ServicesManager* ServicesManager::_instance;
QString ServicesManager::KEY_SERVICE_UUID = "service_uuid";
QString ServicesManager::KEY_SERVICE_DESCRIPTION = "service_description";
QString ServicesManager::KEY_SERVICE_ICON = "service_icon";
QString ServicesManager::KEY_SERVICE_BTLE = "service_btle";

QString ServicesManager::DEFAULT_SERVICE_ICON = "images/bluetooth.png";

ServicesManager::ServicesManager(QObject *parent)
	: QObject(parent)
	, _peripheralAddress(QString(""))
	, _peripheralName(QString(""))
	, _numberOfServices(0)
	, _peripheralPaired(false)
	, _peripheralKnown(false)
	, _peripheralIndex(0)
{
	qRegisterMetaType<ServiceList_t>("ServiceList");

	createWellKnownServiceList();

	QObject::connect(parent, SIGNAL(deviceSelected(QVariant,QVariant)),
					   this,   SLOT(deviceSelected(QVariant,QVariant)));
}

ServicesManager::~ServicesManager()
{
	_instance = 0;
}

ServicesManager* ServicesManager::getInstance(QObject *parent)
{
	if (_instance == 0) {
		_instance = new ServicesManager(parent);
	}

	return _instance;
}

void ServicesManager::enumerateServices(bt_remote_device_t *remoteDevice)
{
    qDebug() << "XXXX ServicesManager::enumerateServices() - entering" << endl;

	if (!remoteDevice) {
	    qDebug() << "XXXX ServicesManager::enumerateServices() - bad device - exiting" << endl;
	    return;
	}

	resetServices();

	int rc = 0;
	char btAddress[18];

	rc = bt_rdev_get_address(remoteDevice, btAddress);
	if (rc == EOK) {
	    qDebug() << "XXXX ServicesManager::enumerateServices() - setting address : " << btAddress << endl;
		setPeripheralAddress(btAddress);
	} else {
	    qDebug() << "XXXX ServicesManager::enumerateServices() - bt_rdev_get_address() - error: " << strerror(errno) << endl;
	}

	char btName[256];
	rc = bt_rdev_get_remote_name(remoteDevice, btName, sizeof(btName));
	if (rc == EOK) {
	    qDebug() << "XXXX ServicesManager::enumerateServices() - setting name : " << btName << endl;
		setPeripheralName(btName);
	} else {
	    qDebug() << "XXXX ServicesManager::enumerateServices() - bt_rdev_get_remote_name() - error: " << strerror(errno) << endl;
	}

	_peripheralKnown = false;
	_peripheralPaired = false;

	bt_rdev_is_known(remoteDevice, &_peripheralKnown);
	bt_rdev_is_paired(remoteDevice, &_peripheralPaired);

    qDebug() << "XXXX ServicesManager::enumerateServices() - _peripheralKnown: " << _peripheralKnown <<  ", _peripheralPaired: " << _peripheralPaired << endl;

	pairDeviceIfRequired(remoteDevice);

	bt_rdev_is_known(remoteDevice, &_peripheralKnown);
	bt_rdev_is_paired(remoteDevice, &_peripheralPaired);

    qDebug() << "XXXX ServicesManager::enumerateServices() - _peripheralKnown: " << _peripheralKnown <<  ", _peripheralPaired: " << _peripheralPaired << endl;

	// Let DataContainer know that paired or known status may have changed as a result

	DataContainer::getInstance()->setKnown(_peripheralIndex, _peripheralKnown);
	DataContainer::getInstance()->setPaired(_peripheralIndex, _peripheralPaired);

	int numberOfServices = 0;
    const int deviceType = bt_rdev_get_type(remoteDevice);

    if ((deviceType == BT_DEVICE_TYPE_LE_PUBLIC) || (deviceType == BT_DEVICE_TYPE_LE_PRIVATE)) {
	    qDebug() << "XXXX ServicesManager::enumerateServices() - suitable BTLE device type" << endl;
		char **servicesArray = bt_rdev_get_services_gatt(remoteDevice);
		if (servicesArray) {
			for (int i = 0; servicesArray[i]; i++) {
				numberOfServices++;
				addService(QString(servicesArray[i]));
			    qDebug() << "XXXX ServicesManager::enumerateServices() - adding service : " << servicesArray[i] << endl;
			}
			bt_rdev_free_services(servicesArray);
		} else {
		    qDebug() << "XXXX ServicesManager::enumerateServices() - unable to get service list - errno : " << strerror(errno) << endl;
		}
	} else {
	    qDebug() << "XXXX ServicesManager::enumerateServices() - unsuitable device type" << endl;
	}

	qDebug() << "XXXX ServicesManager::enumerateServices() - emitting signal" << endl;

	_numberOfServices = numberOfServices;

	emit setServiceCount(QVariant(_numberOfServices));
    emit servicesChanged();
}

void ServicesManager::pairDeviceIfRequired(bt_remote_device_t *remoteDevice)
{
    qDebug() << "XXXX ServicesManager::pairDevice()" << endl;

    if (remoteDevice) {
    	if (_peripheralPaired) {
            qDebug() << "XXXX BlueToothLe::pairDevice() - device already paired" << endl;

    	} else if (_peripheralKnown) {
            qDebug() << "XXXX BlueToothLe::pairDevice() - device already known" << endl;

    	} else {
            qDebug() << "XXXX BlueToothLe::pairDevice() - need to pair device" << endl;
            errno = 0;
            if ((bt_rdev_pair(remoteDevice) == EOK)) {
                qDebug() << "XXXX BlueToothLe::pairDevice() - bt_rdev_pair() request success" << endl;
            } else {
                qDebug() << "XXXX BlueToothLe::pairDevice() - bt_rdev_pair() failed errno=(" << errno << ") " << strerror(errno) << endl;
            }
        }
    }
}

void ServicesManager::setPeripheralAddress(const QString &address)
{
	_peripheralAddress = address;
	emit peripheralAddressChanged(address);
}

void ServicesManager::setPeripheralName(const QString &name)
{
	_peripheralName = name;
	emit peripheralNameChanged(name);
}

QString ServicesManager::peripheralName() const
{
	return _peripheralName;
}

QString ServicesManager::peripheralAddress() const
{
	return _peripheralAddress;
}

ServiceList_t ServicesManager::services() const
{
	return _services;
}

void ServicesManager::addService(const QString &uuid)
{
	ServiceItem_t item;

	item[KEY_SERVICE_UUID] = uuid;
	item[KEY_SERVICE_DESCRIPTION] = serviceDescription(uuid);

	_services.append(item);
    qDebug() << "XXXX ServicesManager::addService() : " << uuid << endl;
    qDebug() << "XXXX ServicesManager::addService() : " << serviceDescription(uuid) << endl;

    emit foundService(uuid, serviceDescription(uuid));
}

void ServicesManager::resetServices()
{
	setPeripheralAddress("");
	setPeripheralName("");

	_services.clear();

	emit servicesChanged();
}


QString ServicesManager::getServiceName(int serviceIndex)
{
    if (_numberOfServices > 0 && serviceIndex >=0) {
    	ServiceItem_t serviceDetails = _services.at(serviceIndex);
        QVariant serviceName = serviceDetails.value(KEY_SERVICE_DESCRIPTION);
        return serviceName.toString();
    } else {
        return QString("");
    }
}

QString ServicesManager::getServiceUuid(int serviceIndex)
{
    if (_numberOfServices > 0 && serviceIndex >=0) {
    	ServiceItem_t serviceDetails = _services.at(serviceIndex);
        QVariant serviceUuid = serviceDetails.value(KEY_SERVICE_UUID);
        return serviceUuid.toString();
    } else {
        return QString("");
    }
}

int ServicesManager::getServiceCount()
{
	return _numberOfServices;
}

void ServicesManager::selectService(const QString &uuid)
{
	emit serviceSelected(uuid);
}

void ServicesManager::deviceSelected(const QVariant &device_index, const QVariant &deviceAddress)
{
    qDebug() << "XXXX ServicesManager::deviceSelected() : " << deviceAddress.toString() << endl;

    _peripheralIndex = device_index.toInt();

    qDebug() << "XXXX ServicesManager::deviceSelected() - device index : " << _peripheralIndex << endl;

    bt_remote_device_t *remoteDevice = bt_rdev_get_device(deviceAddress.toString().toAscii().constData());

    if (remoteDevice != NULL) {

    	enumerateServices(remoteDevice);

    } else {
        qDebug() << "XXXX ServicesManager::deviceSelected() invalid remote device" << endl;
    }
    bt_rdev_free(remoteDevice);
}

QString ServicesManager::serviceDescription(const QString &uuid)
{
	QListIterator<ServiceItem_t> sit(_wellKnownServices);

	while (sit.hasNext()) {
		ServiceItem_t si = sit.next();
		QString wkUuid = si.value(KEY_SERVICE_UUID).toString();

		if (matchesWellKnownUuid(wkUuid, uuid)) return si.value(KEY_SERVICE_DESCRIPTION).toString();
	}

	return "Proprietary Service";
}

QString ServicesManager::serviceIcon(const QString &uuid)
{
	QListIterator<ServiceItem_t> sit(_wellKnownServices);

	while (sit.hasNext()) {
		ServiceItem_t si = sit.next();
		QString wkUuid = si.value(KEY_SERVICE_UUID).toString();

		if (matchesWellKnownUuid(wkUuid, uuid)) return si.value(KEY_SERVICE_ICON).toString();
	}

	return DEFAULT_SERVICE_ICON;
}

bool ServicesManager::isWellKnownService(const QString &uuid)
{
	QListIterator<ServiceItem_t> sit(_wellKnownServices);

	while (sit.hasNext()) {
		ServiceItem_t si = sit.next();
		QString wkUuid = si.value(KEY_SERVICE_UUID).toString();

		if (matchesWellKnownUuid(wkUuid, uuid)) return true;
	}

	return false;
}

bool ServicesManager::matchesWellKnownUuid(const QString &wellKnownUuid, const QString &uuidToCheck)
{
	QString wellKnownPrepend0x(wellKnownUuid);
	wellKnownPrepend0x.prepend("0x");

	return ((uuidToCheck.compare(wellKnownUuid,      Qt::CaseInsensitive) == 0) ||
			(uuidToCheck.compare(wellKnownPrepend0x, Qt::CaseInsensitive) == 0));
}

void ServicesManager::createWellKnownServiceList()
{
	ServiceItem_t si;

	si[KEY_SERVICE_UUID] = "0001";
	si[KEY_SERVICE_DESCRIPTION] = "SDP";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "0003";
	si[KEY_SERVICE_DESCRIPTION] = "RFCOMM";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "0008";
	si[KEY_SERVICE_DESCRIPTION] = "OBEX";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "000C";
	si[KEY_SERVICE_DESCRIPTION] = "HTTP";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "0100";
	si[KEY_SERVICE_DESCRIPTION] = "L2CAP";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "000F";
	si[KEY_SERVICE_DESCRIPTION] = "BNEP";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1000";
	si[KEY_SERVICE_DESCRIPTION] = "Service Discovery";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1001";
	si[KEY_SERVICE_DESCRIPTION] = "Browse Group Descriptor";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1002";
	si[KEY_SERVICE_DESCRIPTION] = "Public Browse Group";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1101";
	si[KEY_SERVICE_DESCRIPTION] = "Serial Port";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1102";
	si[KEY_SERVICE_DESCRIPTION] = "Public Browse Group";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1105";
	si[KEY_SERVICE_DESCRIPTION] = "OBEX Object Push";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1106";
	si[KEY_SERVICE_DESCRIPTION] = "OBEX File Transfer";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1115";
	si[KEY_SERVICE_DESCRIPTION] = "Personal Area Networking";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1116";
	si[KEY_SERVICE_DESCRIPTION] = "Network Access Point";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1117";
	si[KEY_SERVICE_DESCRIPTION] = "Group Network";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = false;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1800";
	si[KEY_SERVICE_DESCRIPTION] = "Generic Access";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1801";
	si[KEY_SERVICE_DESCRIPTION] = "Generic Attribute";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1802";
	si[KEY_SERVICE_DESCRIPTION] = "ImmediateAlert";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1803";
	si[KEY_SERVICE_DESCRIPTION] = "Link Loss";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1804";
	si[KEY_SERVICE_DESCRIPTION] = "Tx Power";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1805";
	si[KEY_SERVICE_DESCRIPTION] = "Current Time Service";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1806";
	si[KEY_SERVICE_DESCRIPTION] = "Reference Time Update Service";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1807";
	si[KEY_SERVICE_DESCRIPTION] = "Next DST Change Service";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1808";
	si[KEY_SERVICE_DESCRIPTION] = "Glucose";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1809";
	si[KEY_SERVICE_DESCRIPTION] = "Health Thermometer";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "180A";
	si[KEY_SERVICE_DESCRIPTION] = "Device Information";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "180D";
	si[KEY_SERVICE_DESCRIPTION] = "Heart Rate";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "180E";
	si[KEY_SERVICE_DESCRIPTION] = "Phone Alert Status Service";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "180F";
	si[KEY_SERVICE_DESCRIPTION] = "Battery Service";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1810";
	si[KEY_SERVICE_DESCRIPTION] = "Blood Pressure";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1811";
	si[KEY_SERVICE_DESCRIPTION] = "Alert Notification Service";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1812";
	si[KEY_SERVICE_DESCRIPTION] = "Human Interface Device";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1813";
	si[KEY_SERVICE_DESCRIPTION] = "Scan Parameters";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1814";
	si[KEY_SERVICE_DESCRIPTION] = "Running Speed and Cadence";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);

	si[KEY_SERVICE_UUID] = "1816";
	si[KEY_SERVICE_DESCRIPTION] = "Cycling Speed and Cadence";
	si[KEY_SERVICE_ICON] = DEFAULT_SERVICE_ICON;
	si[KEY_SERVICE_BTLE] = true;
	_wellKnownServices.append(si);
}
