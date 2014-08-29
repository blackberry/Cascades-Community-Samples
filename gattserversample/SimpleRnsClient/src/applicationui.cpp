/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

#include "applicationui.hpp"

// Service UUID
#define SERVICE_UUID "0x3759E92B-4E2C-4EFD-97B5-AABBCCDDEEAA"

// Characteristic UUID
#define RANDOM_NUMBER_UUID "0x3759E92B-4E2C-4EFD-97B5-000000000000"

// Subset of Service UUIDs found on BB10 when advertising
#define ALERT_NOTIFICATION_SERVICE "0x1811"
#define TX_POWER_SERVICE "0x1804"
#define BATTERY_SERVICE "0x180F"
#define DEVICE_INFORMATION_SERVICE "0x180A"
#define LOCATION_AND_NAVIGATION_SERVICE "0x1819"
#define NEXT_DST_CHANGE_SERVICE "0x1807"
#define CURRENT_TIME_SERVICE "0x1805"
#define LINK_LOSS_SERVICE "0x1803"

void notifications_cb(int instance, uint16_t handle, const uint8_t *val, uint16_t len, void *userData) {

    Q_UNUSED(instance)

    qDebug() << "XXXX notifications call back received";

    ApplicationUI *app = (ApplicationUI *)userData;

    if (app == NULL) {
        qDebug() << "XXXX app == NULL - returning prematurely" << endl;
        return;
    }

    if (val == NULL) {
        qDebug() << "XXXX val == NULL - returning prematurely" << endl;
        return;
    }

    if (len != 1) {
        qDebug() << "XXXX len != 1 - returning prematurely" << endl;
        return;
    }

    if (handle == app->_value_handle) {
        app->logMessage(QString("Received Random Number: %1").arg(*val));
    }
}

void gatt_service_connected(const char *bdaddr, const char *service, int instance, int err, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData) {

    Q_UNUSED(superTimeout)
    Q_UNUSED(latency)
    Q_UNUSED(connInt)
    Q_UNUSED(err)

    qDebug() << "XXXX gatt_service_connected";

    ApplicationUI *app = (ApplicationUI *)userData;

    if (app == NULL) {
        qDebug() << "XXXX app == NULL - returning prematurely" << endl;
        return;
    }

    app->emitConnectionStateChanged(true);

    QString bdaddr_str = QString(bdaddr);
    QString service_str = QString(service);

    qDebug() << "XXXX registering for notifications";
    errno = 0;
    int rc = bt_gatt_reg_notifications(instance, notifications_cb);
    qDebug() << "XXXX rc from bt_gatt_reg_notifications=" << rc;
    if (rc != 0) {
        qDebug() << "XXXX bt_gatt_reg_notifications errno=" << errno;
        qDebug() << "XXXX bt_gatt_reg_notifications errno=" << strerror(errno);
        Utilities::alert("bt_gatt_reg_notifications errno=" + QString::number(errno));
        return;
    } else {
        qDebug() << "XXXX bt_gatt_reg_notifications was presumably OK";
    }

    bt_gatt_characteristic_t* random_number_measurement;

    random_number_measurement = (bt_gatt_characteristic_t*) malloc(sizeof(bt_gatt_characteristic_t));
    if (NULL == random_number_measurement) {
        qDebug() << "XXXX GATT characteristics: Not enough memory";
        bt_gatt_disconnect_instance(instance);
        return;
    }

    qDebug() << "XXXX Allocated memory for notifications";

    int num_characteristics = bt_gatt_characteristics_count(instance);

    qDebug() << "XXXX # characteristics=" << num_characteristics;

    if (num_characteristics > -1) {
        qDebug() << QString("XXXX # characteristics: %1").arg(num_characteristics);
        bt_gatt_characteristic_t *characteristicList;

        characteristicList = (bt_gatt_characteristic_t*) malloc(num_characteristics * sizeof(bt_gatt_characteristic_t));
        if (NULL == characteristicList) {
            qDebug() << QString("XXXX GATT characteristics: Not enough memory");
            bt_gatt_disconnect_instance(instance);
            return;
        }

        /* BEGIN WORKAROUND - Temporary fix to address race condition */
        int number = 0;
        do {
            number = bt_gatt_characteristics(instance, characteristicList, num_characteristics);
        } while ((number == -1) && (errno== EBUSY));

        int characteristicListSize = number;

        for (int i = 0; i < characteristicListSize; i++) {
            qDebug() << "XXXX characteristic: uuid,handle,value_handle:" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
            if (strcmp(characteristicList[i].uuid, RANDOM_NUMBER_UUID) == 0) {
                qDebug() << "XXXX RNS characteristic available";
                app->_handle = characteristicList[i].handle;
                app->_value_handle = characteristicList[i].value_handle;
                // enable=1 switches the notification on for the specified characteristic
                qDebug() << "XXXX registering for RNS notification. uuid,handle,value_handle=" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
                errno= 0;

                rc = bt_gatt_enable_notify(instance, &characteristicList[i], (uint8_t)1);
                if (rc != 0) {
                    qDebug() << "XXXX bt_gatt_enable_notify errno=" << errno;
                    qDebug() << "XXXX bt_gatt_enable_notify errno=" << strerror(errno);
                } else {
                    qDebug() << "XXXX bt_gatt_enable_notify was presumably OK";
                }
            } else {
                qDebug() << "XXXX other characteristic: uuid,handle,value_handle=" << characteristicList[i].uuid << "," << characteristicList[i].handle << "," << characteristicList[i].value_handle;
            }
        }

        if (characteristicList != NULL) {
            free(characteristicList);
            characteristicList = NULL;
        }

        /* END WORKAROUND */

        qDebug() << "XXXX done registering for RNS notifications";

    }
}

void gatt_service_disconnected(const char *bdaddr, const char *service, int instance, int reason, void *userData) {

    Q_UNUSED(instance)
    Q_UNUSED(reason)

    ApplicationUI *app = (ApplicationUI *)userData;

    if (app == NULL) {
        qDebug() << "XXXX app == NULL - returning prematurely" << endl;
        return;
    }

    app->emitConnectionStateChanged(false);

    QString bdaddr_str = QString(bdaddr);
    QString service_str = QString(service);
    qDebug() << QString("XXXX gatt_service_disconnected: bdaddr:%1 : %2 - %3").arg(bdaddr_str).arg(service_str).arg(service);
}

void gatt_service_updated(const char *bdaddr, int instance, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData) {

    Q_UNUSED(superTimeout)
    Q_UNUSED(latency)
    Q_UNUSED(connInt)
    Q_UNUSED(instance)

    ApplicationUI *app = (ApplicationUI *)userData;

    if (app == NULL) {
        qDebug() << "XXXX app == NULL - returning prematurely" << endl;
        return;
    }

    QString bdaddr_str = QString(bdaddr);
    qDebug() << QString("XXXX gatt_service_updated: bdaddr:%1").arg(bdaddr_str);
}

bt_gatt_callbacks_t gatt_callbacks = { gatt_service_connected, gatt_service_disconnected, gatt_service_updated };

ApplicationUI::ApplicationUI(bb::cascades::Application *app)
    : QObject(app)
    , _bt_initialised(false)
{
    _handle = 0;
    _value_handle = 0;

    _translator = new QTranslator(this);
    _localeHandler = new LocaleHandler(this);

    bool res = QObject::connect(_localeHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    Q_ASSERT(res);
    Q_UNUSED(res);

    onSystemLanguageChanged();

    _qml = QmlDocument::create("asset:///main.qml").parent(this);

    _root = _qml->createRootObject<AbstractPane>();
    _mainPage = _root->findChild<QObject*>("mainPage");

    Application::instance()->setScene(_root);


    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<uint8_t>("uint8_t");

    // ============== Signals to be sent to page

    QObject::connect(     this, SIGNAL(message(QVariant)),
                     _mainPage, SLOT(onMessage(QVariant)));

    QObject::connect(     this, SIGNAL(connectionStateChanged(QVariant)),
                     _mainPage, SLOT(onConnectionStateChanged(QVariant)));

    QObject::connect(     this, SIGNAL(enableUi(QVariant)),
                     _mainPage, SLOT(onEnableUi(QVariant)));

    // ============== Hook up signals from QML

    QObject::connect(_mainPage, SIGNAL(connectToGattService()),
                          this, SLOT(onConnectToGattService()));

    QObject::connect(_mainPage, SIGNAL(disconnectFromGattService()),
                          this, SLOT(onDisconnectFromGattService()));

    qDebug() << "XXXX In ApplicationUI() in Thread:" << (int) QThread::currentThreadId();

    initBluetooth();
}

ApplicationUI::~ApplicationUI()
{
    deinitBluetooth();
}

void ApplicationUI::onConnectToGattService()
{
    startRandomNumbers();
}

void ApplicationUI::onDisconnectFromGattService()
{
    stopRandomNumbers();
}

void ApplicationUI::emitConnectionStateChanged(bool isConnected)
{
    emit connectionStateChanged(isConnected);
}

void ApplicationUI::initBluetooth()
{
    if (bt_device_init(NULL) == EOK) {
        qDebug() << "XXXX bt_device_init() OK" << endl;
    } else {
        qDebug() << "XXXX bt_device_init() FAIL " << strerror(errno) << endl;
        return;
    }

    if (bt_le_init(NULL) == EOK) {
        qDebug() << "XXXX bt_le_init() OK" << endl;
    } else {
        qDebug() << "XXXX bt_le_init() FAIL " << strerror(errno) << endl;
        return;
    }

    if (bt_le_set_scan_params(1000, 100, BT_LE_ADVERT_SCAN_ACTIVE) == EOK) {
        qDebug() << "XXXX bt_le_set_scan_params() OK" << endl;
    } else {
        qDebug() << "XXXX bt_le_set_scan_params() FAIL " << strerror(errno) << endl;
        return;
    }

    if (bt_gatt_init(&gatt_callbacks) == EOK) {
        qDebug() << "XXXX bt_gatt_init() OK" << endl;
    } else {
        qDebug() << "XXXX bt_gatt_init() FAIL " << strerror(errno) << endl;
        return;
    }

    _bt_initialised = true;
}

void ApplicationUI::deinitBluetooth()
{
    bt_gatt_deinit();
    bt_le_deinit();
    bt_device_deinit();

    _bt_initialised = false;
}


void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(_translator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("SimpleRnsClient_%1").arg(locale_string);
    if (_translator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(_translator);
    }
}

void ApplicationUI::startRandomNumbers()
{
    logMessage("Starting random number client");
    if(findRnsDevice()) {
        logMessage("Found a candidate RNS Device");
        logMessage("Requesting Random Numbers");
        if (startReceivingRandomNumbers()) {
            logMessage("Starting to receive random numbers");
        } else {
            logMessage("Failure to start receiving random numbers");
        }
    } else {
        logMessage("Couldn't find an RNS device");
    }

}

void ApplicationUI::stopRandomNumbers()
{
    if (bt_gatt_disconnect_service(_rns_device_addr, SERVICE_UUID) == EOK) {
        logMessage("Disconnection from RNS service OK");
        emitConnectionStateChanged(false);
    } else {
        qDebug() << "XXXX requested disconnection from RNS service OK";
        logMessage("GATT disconnect service request failed");
        qDebug() << "XXXX GATT disconnect service request failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
    }
}

void ApplicationUI::logMessage(QString msg)
{
    emit message(msg);
}

bool ApplicationUI::startReceivingRandomNumbers()
{
    bt_gatt_conn_parm_t conParm;
    conParm.minConn = 0x30;
    conParm.maxConn = 0x50;
    conParm.latency = 0;
    conParm.superTimeout = 50;

    errno= 0;
    qDebug() << "XXXX GATT connecting to service: " << SERVICE_UUID << ", on device:" << _rns_device_addr << endl;

    if (bt_gatt_connect_service(_rns_device_addr, SERVICE_UUID, NULL, &conParm, this) == EOK) {
        qDebug() << "XXXX requested connection to RNS service OK";
    } else {
        qDebug() << "XXXX GATT connect service request failed: " + QString::number(errno) + " (" + QString(strerror(errno)) + ")";
        // there's a known issue where sometimes we get ERRNO=EBUSY (16) when this is not the case and we've connected to the service OK. So for now we ignore this errno value.
        if (errno != 16) {
            Utilities::alert("GATT connect service request failed: " + QString::number(errno));
        }
    }

    return true;
}

bool ApplicationUI::findRnsDevice()
{
    logMessage("Searching for candidate devices");

    emit enableUi(false); // stop users from clicking "connect" repeatedly whilst searching :-)

    bool found_rns=false;
    bb::system::SystemToast toast;
    toast.setBody(tr("Searching for Bluetooth Random Number servers ... please wait..."));
    toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast.exec();

    bt_disc_start_inquiry(BT_INQUIRY_GIAC);
    bt_disc_cancel_inquiry();
    toast.setBody(tr("Search completed!"));
    toast.exec();

    emit enableUi(true);

    bt_remote_device_t *nextRemoteDevice = 0;
    bt_remote_device_t **remoteDeviceArray = bt_disc_retrieve_devices(BT_DISCOVERY_ALL, 0);

    if (remoteDeviceArray) {
        qDebug() << "XXXX found devices";

        for (int i = 0; (nextRemoteDevice = remoteDeviceArray[i]); ++i) {
            char buffer[128];
            const int bufferSize = sizeof(buffer);
            qDebug() << "XXXX checking device";
            if (hasRandomNumberService(nextRemoteDevice)) {
                qDebug() << "XXXX found Random Number device";
                bt_rdev_get_friendly_name(nextRemoteDevice, buffer, bufferSize);
                qDebug() << "XXXX name=" << QString::fromLatin1(buffer);
                bt_rdev_get_address(nextRemoteDevice, buffer);
                qDebug() << "XXXX address=" << QString::fromLatin1(buffer);
                strncpy(_rns_device_addr, buffer, sizeof(_rns_device_addr));
                logMessage(QString("Found RNS device: %1").arg(_rns_device_addr));
                found_rns=true;
                break;
            } else {
                qDebug() << "XXXX not a Random Number device";
            }
        }
        qDebug() << "XXXX freeing buffer";

        bt_rdev_free_array(remoteDeviceArray);
    }
    qDebug() << "XXXX Found RNS device:" << _rns_device_addr << endl;

    return found_rns;
}

bool ApplicationUI::hasRandomNumberService(bt_remote_device_t *remoteDevice)
{

    qDebug() << "XXXX hasRandomNumberService() - entering";

    bool foundService = true;

    if (!remoteDevice) {
        qDebug() << "XXXX hasRandomNumberService() - no devices - exiting";
        return false;
    }

    char buffer[256];
    if ( bt_rdev_get_address(remoteDevice, buffer) != -1) {
        qDebug() << "XXXX Checking device address:" << buffer << endl;
    } else {
        qDebug() << "XXXX Unable to determine device address" << endl;
    }

    if ( bt_rdev_get_friendly_name(remoteDevice, buffer,sizeof(buffer)) != -1) {
        qDebug() << "XXXX Checking Friendly Name:" << buffer << endl;
    } else {
        qDebug() << "XXXX Unable to determine Friendly Name" << endl;
    }

    qDebug() << "XXXX hasRandomNumberService() - checking device type";

    const int deviceType = bt_rdev_get_type(remoteDevice);
    qDebug() << "XXXX hasRandomNumberService() - deviceType=" << deviceType;
    if ((deviceType == BT_DEVICE_TYPE_LE_PUBLIC) || (deviceType == BT_DEVICE_TYPE_LE_PRIVATE)) {
        qDebug() << "XXXX hasRandomNumberService() - BT_DEVICE_TYPE_LE_PUBLIC or  BT_DEVICE_TYPE_LE_PRIVATE";
        char **servicesArray = bt_rdev_get_services_gatt(remoteDevice);
        qDebug() << "XXXX hasRandomNumberService() - checking services";
        if (servicesArray) {
            qDebug() << "XXXX hasRandomNumberService() - found services";
            for (int i = 0; servicesArray[i]; i++) {
                qDebug() << "XXXX hasRandomNumberService() - Checking type = " << servicesArray[i];

                /*
                 * Bit of a hack really -- The UUID I'm looking for: 0x3759E92B-4E2C-4EFD-97B5-AABBCCDDEEAA
                 * doesn't appear in the advertisement list I've scanned so I'm going to test the set of exposed
                 * services against the finger-print of a BB10 device with an app running  GATT service. It should
                 * be running these services as well:
                 *
                 * 0x1811 - Alert Notification Service
                 * 0x1804 - Tx Power Service
                 * 0x180F - Battery Service
                 * 0x180A - Device information Service
                 * 0x1819 - Location and Navigation Service
                 * 0x1807 - Next DST Change Service
                 * 0x1805 - Current Time Service
                 * 0x1803 - Link Loss Service
                 *
                 * Of course this list may change so it's not necessarily reliable. I'll consider it a minimal
                 * set to assume the device is a BB10 one with a GATT service running in an app.
                 */

                if (isOneOfFingerPrintServices(servicesArray[i])) {
                    qDebug() << "XXXX hasRandomNumberService() - Fingerprint service match";
                } else {
                    qDebug() << "XXXX hasRandomNumberService() - Fingerprint service mismatch";
                    foundService = false;
                }
            }
            qDebug() << "XXXX hasRandomNumberService() - freeing buffer";
            bt_rdev_free_services(servicesArray);
        } else {
            qDebug() << "XXXX hasRandomNumberService() - no services found";
            foundService = false;
        }
    } else {
        qDebug() << "XXXX hasRandomNumberService() - not BT_DEVICE_TYPE_LE_PUBLIC nor BT_DEVICE_TYPE_LE_PRIVATE";
        foundService = false;
    }
    qDebug() << "XXXX hasRandomNumberService() - returning " << foundService << endl;

    return foundService;
}

bool ApplicationUI::isOneOfFingerPrintServices(const char *serviceUuid)
{
    struct ServicePropertyMap
    {
        const char *serviceUuid;
        const char *description;
    } servicePropertyMap[] = {
            { ALERT_NOTIFICATION_SERVICE, "ALERT NOTIFICATION SERVICE" },
            { TX_POWER_SERVICE, "TX POWER SERVICE" },
            { BATTERY_SERVICE, "BATTERY SERVICE" },
            { DEVICE_INFORMATION_SERVICE, "DEVICE INFORMATION SERVICE" },
            { LOCATION_AND_NAVIGATION_SERVICE, "LOCATION AND_NAVIGATION SERVICE" },
            { NEXT_DST_CHANGE_SERVICE, "NEXT DST CHANGE SERVICE" },
            { CURRENT_TIME_SERVICE, "CURRENT TIME SERVICE" },
            { LINK_LOSS_SERVICE, "LINK LOSS SERVICE" },
    };

    for (uint i = 0; i < (sizeof(servicePropertyMap) / sizeof(struct ServicePropertyMap)); i++) {
        if (strcmp(servicePropertyMap[i].serviceUuid, serviceUuid) == 0) {
            qDebug() << "XXXX Device hosts:" << servicePropertyMap[i].description << endl;
            return true;
        }
    }
    return false;
}
