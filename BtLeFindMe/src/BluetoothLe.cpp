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
#include "BluetoothLe.hpp"

static BluetoothLe *btLeInstance = 0;

void bluetoothEvent(const int event, const char *btAddr, const char *eventData) {
	if (btLeInstance) {
        btLeInstance->emitBtEventSignal(event, btAddr, eventData);
    }
}

BluetoothLe::BluetoothLe(QObject *obj)
	: QObject(obj)
	, _localDeviceInfo(new LocalDeviceInfo(this))
	, _deviceListing(new DeviceListing(this))
	, _remoteDeviceInfo(new RemoteDeviceInfo(this))
	, _findMeService(new FindMeService(this))
{
	qDebug() << "XXXX BlueToothLe::BlueToothLe(QObject *obj)";

	btLeInstance = this;

	QObject::connect(this, SIGNAL(btEventSignal(int, QString, QString)),
                     this,   SLOT(handleBtEventSignal(int, QString, QString)));

	QObject::connect(_findMeService, SIGNAL(serviceStatusChanged()),
			                   this,   SLOT(serviceStatusChanged()));

	QObject::connect(_findMeService, SIGNAL(findMeServiceConnected()),
			                   this,   SLOT(findMeServiceConnected()));

	QObject::connect(_findMeService, SIGNAL(findMeServiceDisconnected()),
			                   this,   SLOT(findMeServiceDisconnected()));

	bt_device_init(bluetoothEvent);

	_deviceListing->update();
	_localDeviceInfo->update();
}

BluetoothLe::~BluetoothLe() {
	qDebug() << "XXXX BlueToothLe::~BlueToothLe()";

    bt_device_deinit();
}

void BluetoothLe::serviceStatusChanged() {
	emit serviceIsInitialisedChanged();
}

void BluetoothLe::findMeServiceConnected() {
	emit findMeServiceIsConnectedChanged();
}

void BluetoothLe::findMeServiceDisconnected() {
	emit findMeServiceIsConnectedChanged();
}

void BluetoothLe::handleBtEventSignal(int event, const QString &btAddr, const QString &eventData) {

	QString eventName = btEventName(event);

	qDebug() << "XXXX event=" << event << " : " << eventName;
	qDebug() << "XXXX bt_addr=" << QString(btAddr);
	qDebug() << "XXXX event_data="   << QString(eventData);

	emit btEventMessage(QString("Event %1 : %2").arg(event).arg(eventName));
	emit btEventMessage(QString("Address %1").arg(btAddr));
	emit btEventMessage(QString("Data %1").arg(eventData));

	switch (event) {
        case BT_EVT_RADIO_SHUTDOWN:
        case BT_EVT_RADIO_INIT:
        case BT_EVT_ACCESS_CHANGED:
            emit bluetoothActiveChanged();
            emit discoverableActiveChanged();
            _localDeviceInfo->update();
            break;
        case BT_EVT_PAIRING_COMPLETE:
        	emit deviceIsPairedChanged();
            _remoteDeviceInfo->update(btAddr);
            break;
        default:
        	qDebug() << "XXXX event ignored=" << QString(event);
            break;
    }
}

void BluetoothLe::emitBtEventSignal(int event, const QString &btAddr, const QString &eventData) {
    emit btEventSignal(event, btAddr, eventData);
}

QString BluetoothLe::btEventName(int event) {
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
	case BT_EVT_FAULT:
		return QString("BT_EVT_FAULT");
	case BT_EVT_UNDEFINED_EVENT:
		return QString("BT_EVT_UNDEFINED_EVENT");
	default:
		return QString("UNKNOWN EVENT:%1").arg(event);
	}
}

bool BluetoothLe::bluetoothActive() const
{
    return bt_ldev_get_power();
}

bool BluetoothLe::discoverableActive() const
{
    return (bt_ldev_get_discoverable() == BT_DISCOVERABLE_GIAC);
}

bool BluetoothLe::remoteDeviceSelected() const {
	qDebug() << "XXXX BluetoothLe::remoteDeviceSelected() - name=" << _remoteDeviceInfo->property("name").toString();

	return (!_remoteDeviceInfo->property("name").toString().isEmpty());
}

DeviceListing *BluetoothLe::deviceListing() const
{
    return _deviceListing;
}

LocalDeviceInfo *BluetoothLe::localDeviceInfo() const
{
    return _localDeviceInfo;
}

RemoteDeviceInfo *BluetoothLe::remoteDeviceInfo() const
{
    return _remoteDeviceInfo;
}

FindMeService *BluetoothLe::findMeService() const
{
    return _findMeService;
}

void BluetoothLe::toggleBluetoothActive()
{
    bt_ldev_set_power(!bt_ldev_get_power());
    emit bluetoothActiveChanged();

    _remoteDeviceInfo->reset();
    emit remoteDeviceSelectedChanged();
}

void BluetoothLe::toggleDiscoverableActive()
{
    if (bt_ldev_get_discoverable() == BT_DISCOVERABLE_GIAC) {
        bt_ldev_set_discoverable(BT_DISCOVERABLE_CONNECTABLE);
    } else {
        bt_ldev_set_discoverable(BT_DISCOVERABLE_GIAC);
    }

    emit discoverableActiveChanged();
}

void BluetoothLe::setRemoteDevice(const QString &address)
{
    qDebug() << "XXXX BlueToothLe::setRemoteDevice() " << address;

    _remoteDeviceInfo->update(address);
    emit remoteDeviceSelectedChanged();
    if (deviceIsPaired()) {
    	emit deviceIsPairedChanged();
    }
}

bool BluetoothLe::deviceIsPaired() const
{
    qDebug() << "XXXX BlueToothLe::deviceIsPaired()";

    bool isPaired = false;
    errno = 0;
    bt_remote_device_t *remoteDevice = bt_rdev_get_device(_remoteDeviceInfo->property("address").toString().toAscii().constData());
    if (remoteDevice) {
        qDebug() << "XXXX BlueToothLe::deviceIsPaired() - bt_rdev_get_device() success";

        errno = 0;
        if (!((bt_rdev_is_paired(remoteDevice, &isPaired) == EOK) && isPaired)) {
            qDebug() << "XXXX BlueToothLe::pairDevice() - device not paired";
        } else {
            qDebug() << "XXXX BlueToothLe::pairDevice() - device is paired";
        }
    } else {
        qDebug() << "XXXX BlueToothLe::deviceIsPaired() - bt_rdev_get_device() failed errno=(" << errno << ") " << strerror(errno);
    }
    bt_rdev_free(remoteDevice);

    return isPaired;
}

void BluetoothLe::pairDevice()
{
    qDebug() << "XXXX BlueToothLe::pairDevice() - " << _remoteDeviceInfo->property("address").toString();

    errno = 0;
    bt_remote_device_t *remoteDevice = bt_rdev_get_device(_remoteDeviceInfo->property("address").toString().toAscii().constData());
    if (remoteDevice) {

    	/// quick sanity check
    	char buffer[128];
        bt_rdev_get_address(remoteDevice, buffer);

    	qDebug() << "XXXX BlueToothLe::pairDevice() - bt_rdev_get_device() success - address=" << buffer;

        if (!deviceIsPaired()) {
            qDebug() << "XXXX BlueToothLe::pairDevice() - need to pair device";
            errno = 0;
            if ((bt_rdev_pair(remoteDevice) == EOK)) {
                qDebug() << "XXXX BlueToothLe::pairDevice() - bt_rdev_pair() request success";
            } else {
                qDebug() << "XXXX BlueToothLe::pairDevice() - bt_rdev_pair() failed errno=(" << errno << ") " << strerror(errno);
            }
        } else {
            qDebug() << "XXXX BlueToothLe::pairDevice() - device already paired";
        }
    } else {
        qDebug() << "XXXX BlueToothLe::pairDevice() - bt_rdev_get_device() failed errno=(" << errno << ") " << strerror(errno);
    }
    bt_rdev_free(remoteDevice);
}

bool BluetoothLe::serviceIsInitialised() const
{
    qDebug() << "XXXX BlueToothLe::serviceIsInitialised() - " << _findMeService->serviceIsInitialised();
    return _findMeService->serviceIsInitialised();
}

void BluetoothLe::initialiseService()
{
    qDebug() << "XXXX BlueToothLe::initialiseService()";
    _findMeService->initialiseService();
}

void BluetoothLe::deInitialiseService()
{
    qDebug() << "XXXX BlueToothLe::deInitialiseService()";
    _findMeService->deInitialiseService();
}

bool BluetoothLe::findMeServiceIsConnected() const
{
    qDebug() << "XXXX BlueToothLe::findMeServiceIsConnected() - " << _findMeService->findMeServiceIsConnected();
    return _findMeService->findMeServiceIsConnected();
}

void BluetoothLe::connectToImmediateAlertService() {
    qDebug() << "XXXX BlueToothLe::connectToImmediateAlertService()";

	_findMeService->setRemoteDevice(_remoteDeviceInfo->property("address").toString());
	_findMeService->setAlertLevel(FindMeService::Off);
	_findMeService->connectToImmediateAlertService();
}

void BluetoothLe::disconnnectFromImmediateAlertService() {
    qDebug() << "XXXX BlueToothLe::disconnnectFromFindMeService()";

    _findMeService->disconnectFromImmediateAlertService();
}

void BluetoothLe::sendAlertOff()
{
    qDebug() << "XXXX BlueToothLe::sendAlertOff()";
    _findMeService->setAlertLevel(FindMeService::Off);
    _findMeService->sendAlertRequest();
}

void BluetoothLe::sendAlertMild()
{
    qDebug() << "XXXX BlueToothLe::sendAlertMild()";
    _findMeService->setAlertLevel(FindMeService::Mild);
    _findMeService->sendAlertRequest();
}

void BluetoothLe::sendAlertHigh()
{
    qDebug() << "XXXX BlueToothLe::sendAlertHigh()";
    _findMeService->setAlertLevel(FindMeService::High);
    _findMeService->sendAlertRequest();
}

