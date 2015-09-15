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
// Characteristic UUIDs
#define RANDOM_NUMBER_UUID "0x3759E92B-4E2C-4EFD-97B5-000000000000"
#define NOTIFICATION_DESCRIPTOR_UUID "0x2902"
#define NOTIFICATION_HANDLE (2)

ApplicationUI *ApplicationUI::_app = NULL;

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
    qDebug() << "XXXX bluetooth event:" << event_name << "In Thread" << (int) QThread::currentThreadId();

    if (ApplicationUI::_app == NULL) {
        qDebug() << "XXXX ApplicationUI::_app == NULL - returning prematurely" << endl;
        return;
    }

    if (event_data != NULL) {
        QString event_data_str = QString(*event_data);
        qDebug() << "XXXX BT event_data=" << event_data_str;
    } else {
        qDebug() << "XXXX BT event_data=NULL";
    }

    if (event == BT_EVT_LE_DEVICE_CONNECTED) {
        ApplicationUI::_app->logMessage(QString("Device has connected"));
        return;
    }

    if (event == BT_EVT_LE_DEVICE_DISCONNECTED) {
        ApplicationUI::_app->logMessage(QString("Device has disconnected"));
        return;
    }
}

void clientConnected_cb(int instance, const char *bdaddr, uint8_t type, uint16_t connInt, uint16_t latency, uint16_t superTimeout, void *userData)
{
    qDebug() << "XXXX clientConnected_cb " << bdaddr << "In Thread" << (int) QThread::currentThreadId();

    ApplicationUI *app = (ApplicationUI *)userData;

    if (app == NULL) {
        qDebug() << "XXXX app == NULL - returning prematurely" << endl;
        return;
    }

    app->_instance = instance;
    app->_device_addr = (char *)bdaddr;
    app->_device_type = type;
    app->_connected = true;
    app->emitClientConnectedSignal(instance, bdaddr, type, connInt, latency, superTimeout, userData);
}

void clientDisconnected_cb(int instance, const char *bdaddr, uint8_t type, void *userData)
{
    qDebug() << "XXXX clientDisconnected_cb " << bdaddr << "In Thread" << (int) QThread::currentThreadId();

    ApplicationUI *app = (ApplicationUI *)userData;

    if (app == NULL) {
        qDebug() << "XXXX app == NULL - returning prematurely" << endl;
        return;
    }

    app->_connected = false;
    app->emitClientDisconnectedSignal(instance, bdaddr, type, userData);
}

#if BBNDK_VERSION_AT_LEAST(10,3,0)
void clientWriteRequest_cb(int instance, const char *bdaddr, uint8_t type, uint16_t handle, uint8_t offset, bool needRsp, uint8_t *val, uint16_t len, void *userData)
#else
void clientWriteRequest_cb(int instance, const char *bdaddr, uint8_t type, uint16_t handle, uint8_t *val, uint16_t len, void *userData)
#endif
{

#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX clientWriteRequest_cb " << bdaddr << " handle:" << handle << " value:" << *val << " len:" << len << " offset:" << offset  << " needRsp:" << needRsp << "In Thread" << (int) QThread::currentThreadId();
#else
    qDebug() << "XXXX clientWriteRequest_cb " << bdaddr << " handle:" << handle << " value:" << *val << " len:" << len << "In Thread" << (int) QThread::currentThreadId();

    bool needRsp = true;
#endif

    ApplicationUI *app = (ApplicationUI *)userData;

    if (app == NULL) {
        qDebug() << "XXXX app == NULL - returning prematurely" << endl;
        return;
    }

    bool valid = false;

    uint16_t value = 0;

    if (handle == 3 && len == 2) {
        valid = true;
        value |= *(val) & 0xff;
        value |= (*(val+1) << 8) & 0xff;
    }

    if (value == 0 || value == 1) {
        valid = true;
    }

    if (needRsp) {
        if (valid) {
            app->logMessage(QString("Attribute %1 updated to %2").arg(handle).arg(value));
            bt_gatt_srv_send_write_rsp(instance, bdaddr, type, handle, BT_ATT_ERROR_NONE);
        } else {
            app->logMessage(QString("Attribute write rejected: handle: %1  value: %2").arg(handle).arg(value));
            bt_gatt_srv_send_write_rsp(instance, bdaddr, type, handle, BT_ATT_ERROR_GENERIC_APPLICATION_ERROR);
            return;
        }
    }

    if (!valid) {
        return;
    }

    if (value == 1) {
        app->_notifications_enabled = true;
        app->emitStartRandomNumberNotificationSignal();
    } else {
        app->_notifications_enabled = false;
        app->emitStopRandomNumberNotificationSignal();
    }
}

#if BBNDK_VERSION_AT_LEAST(10,3,0)
void clientReadRequest_cb(int instance, const char *bdaddr, uint8_t type, uint16_t handle, uint16_t offset, uint16_t len, void *userData)
#else
void clientReadRequest_cb(int instance, const char *bdaddr, uint8_t type, uint16_t handle, uint16_t len, void *userData)
#endif
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
    qDebug() << "XXXX clientReadRequest_cb " << bdaddr << " type:" << type << " handle:" << handle << " offset:" << offset << " len:" << len << "In Thread" << (int) QThread::currentThreadId();
#else
    qDebug() << "XXXX clientReadRequest_cb " << bdaddr << " type:" << type << " handle:" << handle << " len:" << len << "In Thread" << (int) QThread::currentThreadId();
#endif

    ApplicationUI *app = (ApplicationUI *)userData;

    if (app == NULL) {
        qDebug() << "XXXX app == NULL - returning prematurely" << endl;
        return;
    }

    bt_gatt_srv_att_error_t returnedError = BT_ATT_ERROR_NONE;

    if (handle != 3 || len != 2) {
        returnedError = BT_ATT_ERROR_GENERIC_APPLICATION_ERROR;
    }

    uint8_t notifyOn[]  = {0x01, 0x00};
    uint8_t notifyOff[] = {0x00, 0x00};

    uint8_t *value = notifyOff;

    if (app->_notifications_enabled) {
        value = notifyOn;
    }

    bt_gatt_srv_send_read_rsp (instance, bdaddr, type, handle, returnedError, value, sizeof(notifyOn));
}

#if BBNDK_VERSION_AT_LEAST(10,3,0)
bt_gatt_srv_service_cb _service_cb = {
    clientConnected_cb,     // a remote device has connected to the server
    NULL,                   // a remote device has updated link parameters to the server
    clientDisconnected_cb,  // a remote device has disconnected from the server
    NULL,                   // a remote device has written to an attribute
    NULL,                   // a remote device is requesting to write to an attribute (deprecated in 10.3)
    NULL,                   // a remote device has read an attribute
    NULL,                   // a remote device is requesting to read an attribute (deprecated in 10.3)
    clientWriteRequest_cb,  // a remote device is requesting to write to an attribute (new in 10.3)
    clientReadRequest_cb    // a remote device is requesting to read an attribute (new in 10.3)
};
#else
bt_gatt_srv_service_cb _service_cb = {
    clientConnected_cb,     // a remote device has connected to the server
    NULL,                   // a remote device has updated link parameters to the server
    clientDisconnected_cb,  // a remote device has disconnected from the server
    NULL,                   // a remote device has written to an attribute
    clientWriteRequest_cb,  // a remote device is requesting to write to an attribute
    NULL,                   // a remote device has read an attribute
    clientReadRequest_cb    // a remote device is requesting to read an attribute
};
#endif

ApplicationUI::ApplicationUI(bb::cascades::Application *app)
    : QObject(app)
{
    _translator = new QTranslator(this);
    _localeHandler = new LocaleHandler(this);
    _app = this;
    _notifications_enabled = false;
    _connected = false;
    _stopRequested = false;
    _notification_timer = new QTimer(this);

    bool res = QObject::connect(_localeHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    Q_ASSERT(res);
    Q_UNUSED(res);

    onSystemLanguageChanged();
    _qml = QmlDocument::create("asset:///main.qml").parent(this);
    _root = _qml->createRootObject<AbstractPane>();
    _mainPage = _root->findChild<QObject*>("mainPage");

    // Set created root object as the application scene
    Application::instance()->setScene(_root);

    qRegisterMetaType<uint16_t>("uint16_t");
    qRegisterMetaType<uint8_t>("uint8_t");

    QObject::connect(this, SIGNAL(clientConnectedSignal(int, QString, uint8_t, uint16_t, uint16_t, uint16_t, void *)),
                     this, SLOT(handleClientConnectedSignal(int, QString, uint8_t, uint16_t, uint16_t, uint16_t, void *)));

    QObject::connect(this, SIGNAL(clientDisconnectedSignal(int, QString, uint8_t, void *)),
                     this, SLOT(handleClientDisconnectedSignal(int, QString, uint8_t, void *)));

    QObject::connect(this, SIGNAL(clientWriteRequestSignal(int, QString, uint8_t, uint16_t, uint8_t *, uint16_t, void *)),
                     this, SLOT(handleClientWriteRequestSignal(int, QString, uint8_t, uint16_t, uint8_t *, uint16_t, void *)));

    QObject::connect(this, SIGNAL(startRandomNumberNotificationSignal()),
                     this, SLOT(handleStartRandomNumberNotificationSignal()));

    QObject::connect(this, SIGNAL(stopRandomNumberNotificationSignal()),
                     this, SLOT(handleStopRandomNumberNotificationSignal()));

    QObject::connect(this, SIGNAL(startGattService()),
                     this, SLOT(onStartGattService()));

    QObject::connect(this, SIGNAL(stopGattService()),
                     this, SLOT(onStopGattService()));

    QObject::connect(_notification_timer, SIGNAL(timeout()),
                                    this, SLOT(sendNotification()));

    // ============== Signals to be sent to page

    QObject::connect(     this, SIGNAL(message(QVariant)),
                     _mainPage, SLOT(onMessage(QVariant)));

    QObject::connect(     this, SIGNAL(startedStateChanged(QVariant)),
                     _mainPage, SLOT(onStartedStateChanged(QVariant)));

    QObject::connect(     this, SIGNAL(connectionStateChanged(QVariant)),
                     _mainPage, SLOT(onConnectionStateChanged(QVariant)));

    // ============== Hook up signals from QML

    QObject::connect(_mainPage, SIGNAL(startGattService()),
                          this, SLOT(onStartGattService()));

    QObject::connect(_mainPage, SIGNAL(stopGattService()),
                          this, SLOT(onStopGattService()));

    qDebug() << "XXXX In ApplicationUI() in Thread:" << (int) QThread::currentThreadId();
}

ApplicationUI::~ApplicationUI() {
    stopAdvertising();
    deregisterService();
    bt_gatt_srv_deinit();
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(_translator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("GattServerRandom_%1").arg(locale_string);
    if (_translator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(_translator);
    }
}

void ApplicationUI::onStartGattService()
{
    _stopRequested = false;

    startGattServer();
}

void ApplicationUI::onStopGattService()
{
    _stopRequested = true;

    if (_connected) {
        int rc = bt_gatt_disconnect_instance(_instance);
        if (rc == -1) {
            qDebug() << QString("XXXX Error on bt_gatt_disconnect_instance() instance=%1, rc=%2, errno=%3").arg(_instance).arg(rc).arg(strerror(errno));
        }
    }
    stopGattServer();
}

void ApplicationUI::restartGattServer()
{
    qDebug() << "XXXX attempting to restart GATT random number server";

    emit stopGattService();
    emit startGattService();
}

void ApplicationUI::startGattServer()
{
    qDebug() << "XXXX starting GATT random number server now";
    logMessage("GATT server starting");

    int rc;

    // make sure the Bluetooth radio is switched on
    if (!bt_ldev_get_power()) {
        bt_ldev_set_power(true);
        logMessage(QString("Bluetooth switched on"));
    } else {
        logMessage(QString("Bluetooth is on"));
    }

    rc = bt_device_init(btEvent);
    if (rc == -1) {
        qDebug() << QString("XXXX Error Initialising device rc=%1, errno=%2").arg(rc).arg(strerror(errno));
        logMessage(QString("Error Initialising device rc=%1, errno=%2").arg(rc).arg(strerror(errno)));
        return;
    } else {
        logMessage(QString("Bluetooth initialised"));
    }

    rc = bt_le_init(NULL);
    if (rc == -1) {
        qDebug() << QString("XXXX Error LE Initialising device rc=%1, errno=%2").arg(rc).arg(strerror(errno));
        logMessage(QString("Error LE Initialising device rc=%1, errno=%2").arg(rc).arg(strerror(errno)));
        return;
    } else {
        logMessage(QString("Bluetooth LE initialised"));
    }

    rc = bt_gatt_srv_init();
    if (rc == -1) {
        qDebug() << QString("XXXX Error initialising GATT server rc=%1, errno=%2").arg(rc).arg(strerror(errno));
        logMessage(QString("Error initialising GATT server rc=%1, errno=%2").arg(rc).arg(strerror(errno)));
        return;
    } else {
        logMessage(QString("GATT server initialised"));
    }

    char *bt_addr;
    bt_addr = (char*) malloc(18);
    rc = bt_ldev_get_address(bt_addr);
    logMessage(QString("Local device address:%1").arg(bt_addr));

    registerService();
    startAdvertising();
}

void ApplicationUI::stopGattServer()
{
    stopAdvertising();
    deregisterService();
    bt_gatt_srv_deinit();
    bt_le_deinit();
    bt_device_deinit();
}

void ApplicationUI::registerService()
{
    bt_gatt_srv_attrvalue_t _myService[4];

    strlcpy(_myService[0].service, SERVICE_UUID, sizeof(_myService[0].service));
    _myService[0].handle = 0;
    _myService[0].attrType = BT_GATT_SRV_ATT_SERVICE;
    _myService[0].dcl.service.cnt = 4;
    _myService[0].dcl.service.startHandle = 0;
    _myService[0].dcl.service.numHandles = 5;
    _myService[0].dcl.service.type = BT_GATT_SRV_ATT_PRIMARY_SERVICE;
    _myService[0].dcl.service.useSDP = 1;
    _myService[0].valueType = BT_GATT_SRV_ATT_VALUE_STATIC;
    _myService[0].value.sta.valueLen = 21;
    _myService[0].value.sta.value = (uint8_t *) "Random Number Service";

    strlcpy(_myService[1].service, RANDOM_NUMBER_UUID, sizeof(_myService[1].service));
    _myService[1].handle = 1;
    _myService[1].attrType = BT_GATT_SRV_ATT_CHARACTERISTICS;
    _myService[1].dcl.characteristic.properties = BT_GATT_SRV_ATT_PROPERTY_NOTIFY;
    _myService[1].dcl.characteristic.permission = BT_GATT_SRV_ATT_PERMISSION_NONE;
    _myService[1].dcl.characteristic.encryptKeySize = 0;
    _myService[1].dcl.characteristic.valueHandle = NOTIFICATION_HANDLE;
    _myService[1].valueType = BT_GATT_SRV_ATT_VALUE_DYNAMIC;
    _myService[1].value.dyn.valueLen = 1;
    _myService[1].value.dyn.maxValueLen = 1;

    strlcpy(_myService[2].service, NOTIFICATION_DESCRIPTOR_UUID, sizeof(_myService[2].service));
    _myService[2].handle = 3;
    _myService[2].attrType = BT_GATT_SRV_ATT_DESCRIPTOR;
    _myService[2].dcl.descriptor.permission = BT_GATT_SRV_ATT_PERMISSION_WRITEABLE | BT_GATT_SRV_ATT_PERMISSION_READABLE;
    _myService[2].dcl.descriptor.encryptKeySize = 0;
    _myService[2].valueType = BT_GATT_SRV_ATT_VALUE_DYNAMIC;
    _myService[2].value.dyn.valueLen = 2;
    _myService[2].value.dyn.maxValueLen = 2;

    strlcpy(_myService[3].service, "0x2901", sizeof(_myService[3].service));
    _myService[3].handle = 4;
    _myService[3].attrType = BT_GATT_SRV_ATT_DESCRIPTOR;
    _myService[3].dcl.descriptor.permission = BT_GATT_SRV_ATT_PERMISSION_READABLE;
    _myService[3].dcl.descriptor.encryptKeySize = 0;
    _myService[3].valueType = BT_GATT_SRV_ATT_VALUE_VOLATILE;
    _myService[3].value.vol.valueLen = 14;
    _myService[3].value.vol.maxValueLen = 14;
    _myService[3].value.vol.value = (uint8_t *) "Random Number";

    char* err_string = (char*) calloc(80, sizeof(char));
    int valid = bt_gatt_srv_validate_service(_myService, err_string, 80);
    if (valid == EOK) {
        logMessage(QString("Service definition is valid"));
    } else {
        logMessage(QString("Invalid service definition:"));
        logMessage(QString(err_string));
        return;
    }

    errno = 0;
    _serviceInstance = bt_gatt_srv_register_service(_myService, &_service_cb, this);

    if (_serviceInstance != -1) {
        qDebug() << QString("XXXX Registered service instance=%1").arg(_serviceInstance);
    } else {
        qDebug() << QString("XXXX Unable to register Service: %1").arg(strerror(errno));
        emit logMessage(QString("Unable to register Service: %1").arg(strerror(errno)));
        return;
    }
    logMessage("GATT service registered OK");
}

void ApplicationUI::deregisterService()
{
    bt_gatt_srv_deregister_service(_serviceInstance);
}

void ApplicationUI::startAdvertising()
{
    // start advertising
    bt_le_advert_parm_t advertParms;
    uint16_t intMin = 0;
    uint16_t intMax = 0;

    advertParms.minAdvertInterval1 = 0x0020;
    advertParms.maxAdvertInterval1 = 0x0030;
    advertParms.time1 = GATT_TIME_INFINITY;
    advertParms.minAdvertInterval2 = 0x0640;
    advertParms.maxAdvertInterval2 = 0x0FA0;
    advertParms.time2 = GATT_TIME_INFINITY;

    int rc = bt_le_start_advertisement(&advertParms, &intMin, &intMax);
    if (rc == -1) {
        qDebug() << QString("XXXX Error starting advertisements rc=%1, errno=%2").arg(rc).arg(strerror(errno));
        logMessage(QString("Error starting advertisements rc=%1, errno=%2").arg(rc).arg(strerror(errno)));
        return;
    }

    logMessage("Started advertising");
    emit startedStateChanged(true);
}

void ApplicationUI::stopAdvertising()
{
    bt_le_stop_advertisement();
    logMessage("Stopped advertising");
    emit startedStateChanged(false);
}

void ApplicationUI::emitClientConnectedSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t connInt, uint16_t latency, uint16_t superTimeout,
        void *userData)
{
    emit clientConnectedSignal(instance, bdaddr, type, connInt, latency, superTimeout, userData);
}

void ApplicationUI::handleClientConnectedSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t connInt, uint16_t latency, uint16_t superTimeout,
        void *userData)
{
    Q_UNUSED(instance)
    Q_UNUSED(bdaddr)
    Q_UNUSED(type)
    Q_UNUSED(connInt)
    Q_UNUSED(latency)
    Q_UNUSED(superTimeout)
    Q_UNUSED(userData)

    emit connectionStateChanged(true);

    _notifications_enabled = false;

    logMessage(QString("Client connected: %1").arg(bdaddr));
}

void ApplicationUI::emitClientDisconnectedSignal(int instance, const QString &bdaddr, uint8_t type, void *userData)
{
    emit clientDisconnectedSignal(instance, bdaddr, type, userData);
}

void ApplicationUI::handleClientDisconnectedSignal(int instance, const QString &bdaddr, uint8_t type, void *userData)
{

    Q_UNUSED(instance)
    Q_UNUSED(bdaddr)
    Q_UNUSED(type)
    Q_UNUSED(userData)

    emit connectionStateChanged(false);

    logMessage(QString("Client disconnected: %1").arg(bdaddr));

    if (_notifications_enabled) {
        _notifications_enabled = false;
        emit stopRandomNumberNotificationSignal();
    }

    if (_stopRequested) {
        emit stopGattService();
    } else {
        restartGattServer();
    }
}

void ApplicationUI::emitClientWriteRequestSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t handle, uint8_t *val, uint16_t len, void *userData)
{
    emit clientWriteRequestSignal(instance, bdaddr, type, handle, val, len, userData);
}

void ApplicationUI::handleClientWriteRequestSignal(int instance, const QString &bdaddr, uint8_t type, uint16_t handle, uint8_t *val, uint16_t len,
        void *userData)
{
    Q_UNUSED(instance)
    Q_UNUSED(type)
    Q_UNUSED(handle)
    Q_UNUSED(val)
    Q_UNUSED(len)
    Q_UNUSED(userData)

    logMessage(QString("Write request: %1").arg(bdaddr));
}

void ApplicationUI::logMessage(QString msg)
{
    emit message(msg);
}

void ApplicationUI::emitStartRandomNumberNotificationSignal()
{
    emit startRandomNumberNotificationSignal();
}

void ApplicationUI::emitStopRandomNumberNotificationSignal()
{
    emit stopRandomNumberNotificationSignal();
}

void ApplicationUI::handleStartRandomNumberNotificationSignal()
{
    qDebug() << "XXXX handleStartRandomNumberNotificationSignal()";

    _notification_timer->stop();

    qDebug() << "XXXX starting notifications";
    logMessage(QString("starting notifications"));
    qsrand(QDateTime::currentDateTime().toTime_t());

    _notification_timer->start(1000);
}

void ApplicationUI::handleStopRandomNumberNotificationSignal()
{
    qDebug() << "XXXX handleStartRandomNumberNotificationSignal()";

    qDebug() << "XXXX stopping notifications";
    logMessage(QString("stopping notifications"));
    _notification_timer->stop();
}

void ApplicationUI::sendNotification()
{
    int random_int = qrand();
    uint8_t random_byte = random_int % 256;
    qDebug() << "XXXX Serving random value:" << random_byte;
    logMessage(QString("Serving random value: %1").arg(random_byte));

    bt_gatt_srv_send_notification(_instance, _device_addr, _device_type, NOTIFICATION_HANDLE, &random_byte, 1);

}
