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

#include "CharacteristicsManager.hpp"

CharacteristicsManager* CharacteristicsManager::_instance;

QString CharacteristicsManager::KEY_CHARACTERISTIC_UUID = "characteristic_uuid";
QString CharacteristicsManager::KEY_CHARACTERISTIC_HANDLE = "characteristic_handle";
QString CharacteristicsManager::KEY_CHARACTERISTIC_VALUEHANDLE = "characteristic_value_handle";
QString CharacteristicsManager::KEY_CHARACTERISTIC_HEX_VALUE = "characteristic_hex_value";
QString CharacteristicsManager::KEY_CHARACTERISTIC_DESCRIPTION = "characteristic_description";
QString CharacteristicsManager::KEY_DESCRIPTOR_UUID = "descriptor_uuid";
QString CharacteristicsManager::KEY_DESCRIPTOR_DESCRIPTION = "descriptor_description";
QString CharacteristicsManager::KEY_CHARACTERISTIC_PROP_BROADCAST = "characteristic_prop_broadcast";
QString CharacteristicsManager::KEY_CHARACTERISTIC_PROP_READ = "characteristic_prop_read";
QString CharacteristicsManager::KEY_CHARACTERISTIC_PROP_WRITE_NORESP = "characteristic_prop_write_noresp";
QString CharacteristicsManager::KEY_CHARACTERISTIC_PROP_WRITE = "characteristic_prop_write";
QString CharacteristicsManager::KEY_CHARACTERISTIC_PROP_NOTIFY = "characteristic_prop_notify";
QString CharacteristicsManager::KEY_CHARACTERISTIC_PROP_INDICATE = "characteristic_prop_indicate";
QString CharacteristicsManager::KEY_CHARACTERISTIC_PROP_WRITE_SIGNED = "characteristic_write_signed";
QString CharacteristicsManager::KEY_CHARACTERISTIC_PROP_EXT_PROP = "characteristic_prop_ext_prop";

static CharacteristicsManager *thisInstancCallbackHook = NULL;

void gattServiceConnected(const char *bdaddr, const char *service, int instance, int err, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{
    if (thisInstancCallbackHook) {
        thisInstancCallbackHook->emitGattServiceConnected(bdaddr, service, instance, err, connInt, latency, superTimeout, userData);
    }
}

void gattServiceDisconnected(const char *bdaddr, const char *service, int instance, int reason, void *userData)
{
    if (thisInstancCallbackHook) {
        thisInstancCallbackHook->emitGattServiceDisconnected(bdaddr, service, instance, reason, userData);
    }
}

void gattServiceUpdated(const char *bdaddr, int instance, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{
    if (thisInstancCallbackHook) {
        thisInstancCallbackHook->emitGattServiceUpdated(bdaddr, instance, connInt, latency, superTimeout, userData);
    }
}

bt_gatt_callbacks_t gattCallbacks = { gattServiceConnected, gattServiceDisconnected, gattServiceUpdated };

CharacteristicsManager::CharacteristicsManager(QObject *parent) :
        QObject(parent), _serviceUuid(QString("")), _serviceDescription(QString("")), _model(
                new GroupDataModel(QStringList() << KEY_CHARACTERISTIC_UUID << KEY_CHARACTERISTIC_DESCRIPTION << KEY_CHARACTERISTIC_HANDLE << KEY_CHARACTERISTIC_VALUEHANDLE, this)), _selectedServiceInstance(
                0)
{
    qRegisterMetaType<CharacteristicsList_t>("CharacteristicsList");
    qRegisterMetaType<DescriptorList_t>("DescriptorList");
    qRegisterMetaType<uint16_t>("uint16_t");

    createWellKnownCharacteristicsList();
    createWellKnownDescriptorList();

    _model->setSortingKeys(QStringList() << KEY_CHARACTERISTIC_UUID << KEY_CHARACTERISTIC_HANDLE);
    _model->setGrouping(ItemGrouping::None);

    initialiseGatt();

    QObject::connect(this, SIGNAL(gattServiceConnected(QString, QString, int, int, uint16_t, uint16_t, uint16_t, void *)), this,
            SLOT(handleGattServiceConnected(QString, QString, int, int, uint16_t, uint16_t, uint16_t, void *)));

    QObject::connect(this, SIGNAL(gattServiceDisconnected(QString, QString, int, int, void *)), this, SLOT(handleGattServiceDisconnected(QString, QString, int, int, void *)));

    QObject::connect(this, SIGNAL(gattServiceUpdated(QString, int, uint16_t, uint16_t, uint16_t, void *)), this, SLOT(handleGattServiceUpdated(QString, int, uint16_t, uint16_t, uint16_t, void *)));

    QObject::connect(ServicesManager::getInstance(), SIGNAL(serviceSelected(const QString)), this, SLOT(serviceSelected(const QString)));
}

CharacteristicsManager::~CharacteristicsManager()
{
    terminateGatt();
    _instance = 0;
}

CharacteristicsManager* CharacteristicsManager::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new CharacteristicsManager(parent);
    }

    return _instance;
}

void CharacteristicsManager::initialiseGatt()
{
    thisInstancCallbackHook = this;
    bt_gatt_init(&gattCallbacks);
}

void CharacteristicsManager::terminateGatt()
{
    bt_gatt_deinit();
    thisInstancCallbackHook = NULL;
}

void CharacteristicsManager::connectToSelectedService(const QString &serviceUuid)
{
    bool ok = false;

    qDebug() << "XXXX CharacteristicsManager::connectToSelectedService() - serviceUuid=" << serviceUuid << endl;
    qDebug() << "XXXX CharacteristicsManager::connectToSelectedService() - _selectedServiceInstance=" << _selectedServiceInstance << endl;

    bt_gatt_conn_parm_t conParm;
    conParm.minConn = 0x30;
    conParm.maxConn = 0x50;
    conParm.latency = 0;
    conParm.superTimeout = 50;

    errno= 0;
    if (!_selectedServiceInstance) {
        qDebug() << "XXXX CharacteristicsManager::connectToSelectedService() - calling bt_gatt_connect_service()" << endl;
        ok = (bt_gatt_connect_service(ServicesManager::getInstance()->peripheralAddress().toAscii().constData(), serviceUuid.toAscii().constData(), NULL, &conParm, this) == EOK);
        emit scanStarted(ServicesManager::getInstance()->peripheralName(), serviceDescription());

        if (ok) {
            qDebug() << "XXXX CharacteristicsManager::connectToSelectedService() - connection request OK" << endl;
        } else {
            qDebug() << "XXXX CharacteristicsManager::connectToSelectedService() - connect request failed - errno=(" << errno<< ") :" << strerror(errno) << endl;
        }
    } else {
        qDebug() << "XXXX CharacteristicsManager::connectToSelectedService() - invalid service instance" << endl;
    }
}

void CharacteristicsManager::disconnectFromSelectedService()
{
    bool ok = false;
    errno= 0;
    if (_selectedServiceInstance) {
        ok = (bt_gatt_disconnect_instance(_selectedServiceInstance) == EOK);
        if (ok) {
            qDebug() << "XXXX CharacteristicsManager::disconnectFromSelectedService() - disconnected OK" << endl;
        } else {
            qDebug() << "XXXX CharacteristicsManager::disconnectFromSelectedService() - disconnect failed - errno=(" << errno<< ") :" << strerror(errno) << endl;
        }
        _selectedServiceInstance = 0;
        emit selectedServiceDisconnected();
    } else {
        qDebug() << "XXXX CharacteristicsManager::disconnectFromSelectedService() - invalid service instance" << endl;
    }
}

void CharacteristicsManager::emitGattServiceConnected(const QString &bdaddr, const QString &service, int instance, int err, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{
    emit gattServiceConnected(bdaddr, service, instance, err, connInt, latency, superTimeout, userData);
}

void CharacteristicsManager::emitGattServiceDisconnected(const QString &bdaddr, const QString &service, int instance, int reason, void *userData)
{
    emit gattServiceDisconnected(bdaddr, service, instance, reason, userData);
}

void CharacteristicsManager::emitGattServiceUpdated(const QString &bdaddr, int instance, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{
    emit gattServiceUpdated(bdaddr, instance, connInt, latency, superTimeout, userData);
}

void CharacteristicsManager::handleGattServiceConnected(const QString &bdaddr, const QString &service, int instance, int err, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{
    Q_UNUSED(bdaddr)
    Q_UNUSED(service)
    Q_UNUSED(instance)
    Q_UNUSED(err)
    Q_UNUSED(connInt)
    Q_UNUSED(latency)
    Q_UNUSED(superTimeout)
    Q_UNUSED(userData)

    qDebug() << "XXXX CharacteristicsManager::handleGattServiceConnected() - " << instance << ", " << bdaddr << ", " << service << endl;

    emit scanStopped();

    if (err == EOK) {
        _selectedServiceInstance = instance;
        emit selectedServiceConnected();

        int numCharacteristics = bt_gatt_characteristics_count(instance);
        if (numCharacteristics > -1) {
            qDebug() << "XXXX CharacteristicsManager::handleGattServiceConnected() - #characteristics=" << numCharacteristics << endl;

            bt_gatt_characteristic_t *characteristicList;

            characteristicList = (bt_gatt_characteristic_t*) malloc(numCharacteristics * sizeof(bt_gatt_characteristic_t));
            if (!characteristicList) {
                qDebug() << "XXXX CharacteristicsManager::handleGattServiceConnected() - malloc fail" << endl;
                return;
            }

            /* BEGIN WORKAROUND - Temporary fix to address race condition */

            int number = 0;
            do {
                number = bt_gatt_characteristics(instance, characteristicList, numCharacteristics);
            } while ((number == -1) && (errno== EBUSY));

            int characteristicListSize = number;

            qDebug() << "XXXX CharacteristicsManager::handleGattServiceConnected() - Characteristics:" << endl;
            for (int i = 0; i < characteristicListSize; i++) {
                qDebug() << "XXXX characteristic name: " << characteristicDescription(characteristicList[i].uuid) << endl;
                qDebug() << "XXXX characteristic UUID: " << characteristicList[i].uuid << endl;
                qDebug() << "XXXX characteristic handle: " << characteristicList[i].handle << endl;
                qDebug() << "XXXX characteristic value_handle: " << characteristicList[i].value_handle << endl;
                qDebug() << "XXXX characteristic properties: " << characteristicList[i].properties << endl;
                addCharacteristic(characteristicList[i].uuid, characteristicList[i].handle, characteristicList[i].value_handle, characteristicList[i].properties);
            }

            /* END WORKAROUND */

            if (characteristicList) {
                free(characteristicList);
                characteristicList = NULL;
            }

            disconnectFromSelectedService();

        } else {
            qDebug() << "XXXX CharacteristicsManager::handleGattServiceConnected() Failed to determine number of characteristics" << endl;
            qDebug() << "XXXX CharacteristicsManager::handleGattServiceConnected() errno=" << errno<< endl;
            qDebug() << "XXXX CharacteristicsManager::handleGattServiceConnected() errno=" << strerror(errno) << endl;
        }
    } else {
        _selectedServiceInstance = 0;
        qDebug() << "XXXX CharacteristicsManager::handleGattServiceConnected() - not connected - err=" << strerror(err) << endl;
        QString errorMessage = QString("Unable to connect to selected Bluetooth LE service (\"%1\") ... please ensure the device is powered on and try again").arg(strerror(err));

        bb::system::SystemToast toast;
        toast.setBody(errorMessage);
        toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
        toast.exec();

        emit connectionError(errorMessage);
    }
}

void CharacteristicsManager::handleGattServiceDisconnected(const QString &bdaddr, const QString &service, int instance, int reason, void *userData)
{
    Q_UNUSED(userData)

    emit scanStopped();

    qDebug() << "XXXX CharacteristicsManager::handleGattServiceDisconnected() - " << instance << ", " << bdaddr << ", " << service << endl;

    if (reason == EOK) {
        qDebug() << "XXXX CharacteristicsManager::handleGattServiceDisconnected() - a service disconnected" << endl;
    } else {
        qDebug() << "XXXX CharacteristicsManager::handleGattServiceDisconnected() - error during disconnection - reason=" << strerror(reason) << endl;
    }
    if (instance == _selectedServiceInstance) {
        qDebug() << "XXXX CharacteristicsManager::handleGattServiceDisconnected() - selected Service disconnected" << endl;
        _selectedServiceInstance = 0;
        emit selectedServiceDisconnected();
    }
}

void CharacteristicsManager::handleGattServiceUpdated(const QString &bdaddr, int instance, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{
    Q_UNUSED(connInt)
    Q_UNUSED(latency)
    Q_UNUSED(superTimeout)
    Q_UNUSED(userData)

    qDebug() << "XXXX CharacteristicsManager::handleGattServiceUpdated()" << endl;
    qDebug() << "XXXX CharacteristicsManager::handleGattServiceUpdated() - " << instance << ", " << bdaddr << endl;

    emit selectedServiceUpdated();
}

DataModel* CharacteristicsManager::model() const
{
    return _model;
}

void CharacteristicsManager::setServiceUuid(const QString &uuid)
{
    _serviceUuid = uuid;

    emit serviceUuidChanged();
}

void CharacteristicsManager::addCharacteristic(const QString &uuid, uint16_t handle, uint16_t valueHandle, bt_gatt_char_prop_mask properties)
{
    QVariantMap map;

    map[KEY_CHARACTERISTIC_UUID] = uuid;
    map[KEY_CHARACTERISTIC_DESCRIPTION] = characteristicDescription(uuid);
    map[KEY_CHARACTERISTIC_HANDLE] = handle;
    map[KEY_CHARACTERISTIC_VALUEHANDLE] = valueHandle;

    QString hex_value = getCharacteristicHexValue(valueHandle);
    map[KEY_CHARACTERISTIC_HEX_VALUE] = hex_value;


    if (properties & BT_GATT_CHARACTERISTIC_PROP_BROADCAST) {
        /**
         * Allow for the broadcasting of Generic Attributes (GATT) characteristic values.
         */
        map[KEY_CHARACTERISTIC_PROP_BROADCAST] = true;
    } else {
        map[KEY_CHARACTERISTIC_PROP_BROADCAST] = false;
    }

    if (properties & BT_GATT_CHARACTERISTIC_PROP_READ) {
        /**
         * Allow the characteristic values to be read.
         */
        map[KEY_CHARACTERISTIC_PROP_READ] = true;
    } else {
        map[KEY_CHARACTERISTIC_PROP_READ] = false;
    }

    if (properties & BT_GATT_CHARACTERISTIC_PROP_WRITE_NORESP) {
        /**
         * Allow characteristic values without responses to be written.
         */
        map[KEY_CHARACTERISTIC_PROP_WRITE_NORESP] = true;
    } else {
        map[KEY_CHARACTERISTIC_PROP_WRITE_NORESP] = false;
    }

    if (properties & BT_GATT_CHARACTERISTIC_PROP_WRITE) {
        /**
         * Allow characteristic values to be written.
         */
        map[KEY_CHARACTERISTIC_PROP_WRITE] = true;
    } else {
        map[KEY_CHARACTERISTIC_PROP_WRITE] = false;
    }

    if (properties & BT_GATT_CHARACTERISTIC_PROP_NOTIFY) {
        /**
         * Permits notification of characteristic values.
         */
        map[KEY_CHARACTERISTIC_PROP_NOTIFY] = true;
    } else {
        map[KEY_CHARACTERISTIC_PROP_NOTIFY] = false;
    }

    if (properties & BT_GATT_CHARACTERISTIC_PROP_INDICATE) {
        /**
         * Permits indications of characteristic values.
         */
        map[KEY_CHARACTERISTIC_PROP_INDICATE] = true;
    } else {
        map[KEY_CHARACTERISTIC_PROP_INDICATE] = false;
    }

    if (properties & BT_GATT_CHARACTERISTIC_PROP_WRITE_SIGNED) {
        /**
         * Permits signed writes of the GATT characteristic values.
         */
        map[KEY_CHARACTERISTIC_PROP_WRITE_SIGNED] = true;
    } else {
        map[KEY_CHARACTERISTIC_PROP_WRITE_SIGNED] = false;
    }

    if (properties & BT_GATT_CHARACTERISTIC_PROP_EXT_PROP) {
        /**
         * Additional characteristic properties are defined in the characteristic
         * extended properties descriptor.
         */
        map[KEY_CHARACTERISTIC_PROP_EXT_PROP] = true;
    } else {
        map[KEY_CHARACTERISTIC_PROP_EXT_PROP] = false;
    }

    _model->insert(map);
}

void CharacteristicsManager::setServiceDescription(const QString &description)
{
    _serviceDescription = description;

    emit serviceDescriptionChanged();
}

QString CharacteristicsManager::serviceUuid() const
{
    return _serviceUuid;
}

QString CharacteristicsManager::serviceDescription() const
{
    return _serviceDescription;
}

void CharacteristicsManager::selectCharacteristic(const QString &characteristicUuid, const QVariant indexPath)
{
    emit characteristicSelected(characteristicUuid, indexPath);
}

void CharacteristicsManager::reset()
{
    _serviceUuid = "";
    _serviceDescription = "";

    _model->clear();

    emit serviceUuidChanged();
    emit serviceDescriptionChanged();
}

void CharacteristicsManager::serviceSelected(const QString &uuid)
{
    _model->clear();

    _serviceUuid = uuid;
    _serviceDescription = ServicesManager::getInstance()->serviceDescription(uuid);

    connectToSelectedService(uuid);
}

QString CharacteristicsManager::characteristicDescription(const QString &uuid)
{
    QListIterator<CharacteristicsItem_t> cit(_wellKnownCharacteristics);

    while (cit.hasNext()) {
        CharacteristicsItem_t ci = cit.next();
        QString wkUuid = ci.value(KEY_CHARACTERISTIC_UUID).toString();

        if (matchesWellKnownUuid(wkUuid, uuid))
            return ci.value(KEY_CHARACTERISTIC_DESCRIPTION).toString();
    }

    return "Proprietary Characteristic";
}

QString CharacteristicsManager::descriptorDescription(const QString &uuid)
{
    QListIterator<DescriptorItem_t> dit(_wellKnownDescriptors);

    while (dit.hasNext()) {
        DescriptorItem_t di = dit.next();
        QString wkUuid = di.value(KEY_DESCRIPTOR_UUID).toString();

        if (matchesWellKnownUuid(wkUuid, uuid))
            return di.value(KEY_DESCRIPTOR_DESCRIPTION).toString();
    }

    return "Proprietary Descriptor";
}

bool CharacteristicsManager::isWellKnownCharacteristic(const QString &uuid)
{
    QListIterator<CharacteristicsItem_t> cit(_wellKnownCharacteristics);

    while (cit.hasNext()) {
        CharacteristicsItem_t ci = cit.next();
        QString wkUuid = ci.value(KEY_CHARACTERISTIC_UUID).toString();

        if (matchesWellKnownUuid(wkUuid, uuid))
            return true;
    }

    return false;
}

bool CharacteristicsManager::isWellKnownDescriptor(const QString &uuid)
{
    QListIterator<DescriptorItem_t> dit(_wellKnownDescriptors);

    while (dit.hasNext()) {
        DescriptorItem_t di = dit.next();
        QString wkUuid = di.value(KEY_DESCRIPTOR_UUID).toString();

        if (matchesWellKnownUuid(wkUuid, uuid))
            return true;
    }

    return false;
}

bool CharacteristicsManager::matchesWellKnownUuid(const QString &wellKnownUuid, const QString &uuidToCheck)
{
    QString wellKnownPrepend0x(wellKnownUuid);
    wellKnownPrepend0x.prepend("0x");

    return ((uuidToCheck.compare(wellKnownUuid, Qt::CaseInsensitive) == 0) || (uuidToCheck.compare(wellKnownPrepend0x, Qt::CaseInsensitive) == 0));
}

void CharacteristicsManager::createWellKnownCharacteristicsList()
{
    CharacteristicsItem_t ci;

    ci[KEY_CHARACTERISTIC_UUID] = "2A43";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Alert Category Id";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A42";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Alert Category Id Bit Mask";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A06";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Alert Level";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A44";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Alert Notification Control Point";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A3F";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Alert Status";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A01";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Gap Appearance";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A19";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Battery Level";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A49";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Blood Pressure Feature";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A35";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Blood Pressure Measurement";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A38";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Body Sensor Location";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A22";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Boot Keyboard Input Report";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A32";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Boot Keyboard Output Report";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A33";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Boot Mouse Input Report";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A2B";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Current Time";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A08";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Date Time";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A0A";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Day Date Time";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A09";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Day Of Week";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A00";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "GAP Device Name";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A0D";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "DST Offset";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A0C";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Exact Time 256";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A26";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Firmware Revision String";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A51";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Glucose Feature";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A18";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Glucose Measurement";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A34";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Glucose Measurement Context";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A27";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Hardware Revision String";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A39";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Heart Rate Control Point";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A37";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Heart Rate Measurement";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A4C";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "HID Control Point";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A4A";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "HID Information";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A2A";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "IEEE 11073 20601 Regulatory Certification Data List";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A36";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Intermediate Blood Pressure";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A1E";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Intermediate Temperature";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A0F";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Local Time Information";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A29";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Manufacturer Name String";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A21";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Measurement Interval";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A24";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Model Number String";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A46";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "New Alert";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A04";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "GAP Peripheral Preferred Connection Parameters";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A02";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "GAP Peripheral Privacy Flag";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A50";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "PNP Id";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A4E";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Protocol Mode";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A03";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "GAP Reconnection Address";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A52";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Record Access Control Point";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A14";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Reference Time Information";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A4D";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Report";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A4B";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Report Map";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A40";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Ringer Control Point";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A41";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Ringer Setting";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A4F";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Scan Interval Window";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A31";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Scan Refresh";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A25";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Serial Number String";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A05";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "GATT Service Changed";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A28";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Software Revision String";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A47";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Supported New Alert Category";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A48";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Supported Unread Alert Category";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A23";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "System Id";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A1C";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Temperature Measurement";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A1D";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Temperature Type";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A12";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Time Accuracy";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A13";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Time Source";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A16";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Time Update Control Point";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A17";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Time Update State";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A11";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Time with DST";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A0E";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Time Zone";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A07";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "TX Power";
    _wellKnownCharacteristics.append(ci);

    ci[KEY_CHARACTERISTIC_UUID] = "2A45";
    ci[KEY_CHARACTERISTIC_DESCRIPTION] = "Unread Alert Status";
    _wellKnownCharacteristics.append(ci);
}

void CharacteristicsManager::createWellKnownDescriptorList()
{
    DescriptorItem_t di;

    di[KEY_DESCRIPTOR_UUID] = "2905";
    di[KEY_DESCRIPTOR_DESCRIPTION] = "Characteristic Aggregate Format";
    _wellKnownDescriptors.append(di);

    di[KEY_DESCRIPTOR_UUID] = "2900";
    di[KEY_DESCRIPTOR_DESCRIPTION] = "Characteristic Extended Properties";
    _wellKnownDescriptors.append(di);

    di[KEY_DESCRIPTOR_UUID] = "2904";
    di[KEY_DESCRIPTOR_DESCRIPTION] = "Characteristic Presentation Format";
    _wellKnownDescriptors.append(di);

    di[KEY_DESCRIPTOR_UUID] = "2901";
    di[KEY_DESCRIPTOR_DESCRIPTION] = "Characteristic User Description";
    _wellKnownDescriptors.append(di);

    di[KEY_DESCRIPTOR_UUID] = "2902";
    di[KEY_DESCRIPTOR_DESCRIPTION] = "Client Characteristic Configuration";
    _wellKnownDescriptors.append(di);

    di[KEY_DESCRIPTOR_UUID] = "2907";
    di[KEY_DESCRIPTOR_DESCRIPTION] = "External Report Reference";
    _wellKnownDescriptors.append(di);

    di[KEY_DESCRIPTOR_UUID] = "2908";
    di[KEY_DESCRIPTOR_DESCRIPTION] = "Report Reference";
    _wellKnownDescriptors.append(di);

    di[KEY_DESCRIPTOR_UUID] = "2903";
    di[KEY_DESCRIPTOR_DESCRIPTION] = "Server Characteristic Configuration";
    _wellKnownDescriptors.append(di);

    di[KEY_DESCRIPTOR_UUID] = "2906";
    di[KEY_DESCRIPTOR_DESCRIPTION] = "Valid Range";
    _wellKnownDescriptors.append(di);
}

QString CharacteristicsManager::getCharacteristicHexValue(uint16_t handle)
{
    int max_len = 256;
    QString hex_value("");
    uint8_t *characteristic_bytes;
    characteristic_bytes = (uint8_t*) calloc(max_len, sizeof(uint8_t));
    errno= 0;
    int bytes_read = bt_gatt_read_value(_selectedServiceInstance, handle, 0, characteristic_bytes, max_len, 0);
    if (bytes_read < 0) {
        qDebug() << "XXXX bt_gatt_read_value - errno=(" << errno<< ") :" << strerror(errno);
    } else {
        QByteArray ba = QByteArray::fromRawData(reinterpret_cast<const char *>(characteristic_bytes), max_len);
        hex_value = QString::fromAscii(ba.toHex());
        hex_value = hex_value.mid(0, bytes_read * 2);
    }
    free(characteristic_bytes);
    return hex_value;
}
