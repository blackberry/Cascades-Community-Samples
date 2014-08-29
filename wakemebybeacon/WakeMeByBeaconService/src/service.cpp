/*
 * Copyright (c) 2013-2014 BlackBerry Limited.
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

#include "service.hpp"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>
#include <btapi/btdevice.h>
#include <btapi/btle.h>
#include <QTimer>

using namespace bb::platform;
using namespace bb::system;

static void btEvent(const int event, const char *btAddr, const char *eventData);
static void btLeAdvertisementCbExt(const char *bdaddr, int8_t rssi, bt_le_advert_packet_event_t eventType, const char *data, int len, void *userData);
static Service *gServiceInstance = 0;

static void btLeAdvertisementCbExt(const char *bdaddr, int8_t rssi, bt_le_advert_packet_event_t eventType, const char *data, int len, void *userData)
{
    Q_UNUSED(userData)
    Q_UNUSED(eventType)

    if (gServiceInstance) {
        gServiceInstance->parseBeaconData(data, len, rssi, bdaddr);
    }
}

static void btEvent(const int event, const char *btAddr, const char *eventData)
{
    Q_UNUSED(btAddr)

    QString eventName("**UNSET**");

    if (gServiceInstance) {
        eventName = gServiceInstance->btEventName(event);
    }

    qDebug() << "SSSS BT event_name=" << eventName << endl;

    if (eventData != NULL) {
        QString eventDataStr = QString(*eventData);
        qDebug() << "SSSS BT event_data=" << eventDataStr;
    } else {
        qDebug() << "SSSS BT event_data=NULL";
    }

    if (event == BT_EVT_RADIO_INIT) {
        if (gServiceInstance) {
            gServiceInstance->_btRadioOn = true;
            gServiceInstance->sendStatus();
        }
    } else if (event == BT_EVT_RADIO_SHUTDOWN) {
        if (gServiceInstance) {
            gServiceInstance->_btRadioOn = false;
            gServiceInstance->sendStatus();
        }
    }
}

bt_le_callbacks_t leCallbacks = { NULL, NULL, NULL, btLeAdvertisementCbExt };

Service::Service()
    : QObject()
    , _btRadioOn(false)
    , _notification(new Notification(this))
    , _invokeManager(new InvokeManager(this))
    , _btInitialised(false)
    , _model(new GroupDataModel(this))
    , _socketListener(SocketListener::getInstance(this))
    , _scanning(false)
    , _guiConnected(false)
    , _expireTimer(new QTimer(this))
{
    gServiceInstance = this;

    if (!QObject::connect( bb::Application::instance(), SIGNAL(processStateChanged(bb::ProcessState::Type)),
                                                  this, SLOT(onProcessStateChanged(bb::ProcessState::Type)))) {
        qWarning() << "SSSS Service::Service() - connect failed - onProcessStateChanged" << strerror(errno) << endl;
    }

    if(!QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
                                   this, SLOT(onInvoked(const bb::system::InvokeRequest&)))) {
        qWarning() << "SSSS Service::Service() - connect failed - onInvoked" << strerror(errno) << endl;
    }

    if(!QObject::connect(_socketListener, SIGNAL(guiConnected()),
                                   this, SLOT(onGuiConnected()))) {
        qWarning() << "SSSS Service::Service() - connect failed - onGuiConnected" << strerror(errno) << endl;
    }

    if(!QObject::connect(_socketListener, SIGNAL(guiDisconnected()),
                                   this, SLOT(onGuiDisconnected()))) {
        qWarning() << "SSSS Service::Service() - connect failed - onGuiDisconnected" << strerror(errno) << endl;
    }

    if(!QObject::connect(           this, SIGNAL(messageForGui(const QVariant &)),
                         _socketListener, SLOT(onMessageForGui(const QVariant &)))) {
        qWarning() << "SSSS Service::Service() - connect failed - onMessageForUi" << strerror(errno) << endl;
    }

    if(!QObject::connect(_expireTimer, SIGNAL(timeout()),
                                 this, SLOT(onTimeout()))) {
        qWarning() << "SSSS Service::Service() - connect failed - onTimeout" << strerror(errno) << endl;
    }

    Application::instance()->setAutoExit(true);

    _expireTimer->setInterval(BEACON_EXPIRY_SCAN_INTERVAL_MS);
    _expireTimer->start();

    NotificationDefaultApplicationSettings notificationSettings;
    notificationSettings.setPreview(NotificationPriorityPolicy::Allow);
    notificationSettings.apply();

    _notification->setTitle("WakeMeByBeacon Service");
    _notification->setBody("WakeMeByBeacon service requires attention");

    InvokeRequest request;
    request.setTarget(WAKEME_INVOKE_GUI);
    request.setAction(WAKEME_INVOKE_ACTION_STARTSERVICE);
    _notification->setInvokeRequest(request);

    initBluetooth();

    onTimeout();
}

Service::~Service()
{
    _expireTimer->stop();
    deinitBluetooth();
}

void Service::onTimeout()
{
    expireBeaconData(BEACON_EXPIRY_INTERVAL_SECONDS);
}

void Service::initBluetooth()
{
    if (_btInitialised) return;

    _model->clear();

    if (bt_device_init(btEvent) == EOK) {
        qDebug() << "SSSS bt_device_init() OK" << endl;
    } else {
        qDebug() << "SSSS bt_device_init() FAIL " << strerror(errno) << endl;
        return;
    }

    if (!bt_ldev_get_power()) {
        qDebug() << "SSSS Bluetooth radio is not on -- calling bt_ldev_set_power(true)" << endl;
        _btRadioOn = false;
        bt_ldev_set_power(true);
    } else {
        qDebug() << "SSSS Bluetooth radio is on" << endl;
        _btRadioOn = true;
    }

    if (bt_ldev_set_filters(BT_EVT_ALL_EVENT, 0) == EOK) {
        qDebug() << "SSSS bt_ldev_set_filters() OK" << endl;
    } else {
        qDebug() << "SSSS bt_ldev_set_filters() FAIL " << strerror(errno) << endl;
        return;
    }

    if (bt_le_init(&leCallbacks) == EOK) {
        qDebug() << "SSSS bt_le_init() OK" << endl;
    } else {
        qDebug() << "SSSS bt_le_init() FAIL " << strerror(errno) << endl;
        return;
    }

    _btInitialised = true;

}

void Service::deinitBluetooth()
{
    if (!_btInitialised) return;

    qDebug() << "SSSS Calling bt_le_deinit()" << endl;
    bt_le_deinit();
    qDebug() << "SSSS Calling bt_device_deinit()" << endl;
    bt_device_deinit();

    _btInitialised = false;
}

void Service::parseBeaconData(const char *data, int len, int8_t rssi, const char *bdaddr)
{
    BtleAdvertData advertData;
    if (advertData.parse(QByteArray(data, len))) {
        qDebug() << "SSSS advert data parsed OK" << endl;
        if (advertData.hasBeaconData()) {
            QVariantMap entry;

            entry["MAC"] = QString(bdaddr);
            entry["TIME"] = QDateTime::currentDateTime();

            if (advertData.hasIBeaconData()) {
                qDebug() << "BBBB iBeacon data present in advert data" << endl;
                qDebug() << "BBBB beacon UUID    " << advertData.beaconUuid().toHex() << endl;
                qDebug() << "BBBB beacon Major   " << advertData.beaconMajor() << endl;
                qDebug() << "BBBB beacon Minor   " << advertData.beaconMinor() << endl;
                qDebug() << "BBBB beacon Strength" << advertData.calibratedStrength() << "dBm" << endl;
                qDebug() << "BBBB beacon path loss" << advertData.calibratedStrength() - rssi << "dBm" << endl;

                entry["CLASS"] = "IBEACON";
                entry["UUID"] = advertData.beaconUuidAsString();
                entry["MAJOR"] = advertData.beaconMajor();
                entry["MINOR"] = advertData.beaconMinor();

            } else if(advertData.hasAltBeaconData()) {
                qDebug() << "BBBB Alt Beacon data present in advert data" << endl;
                qDebug() << "BBBB beacon Id    " << advertData.beaconId().toHex() << endl;
                qDebug() << "BBBB beacon Strength" << advertData.calibratedStrength() << "dBm" << endl;
                qDebug() << "BBBB beacon path loss" << advertData.calibratedStrength() - rssi << "dBm" << endl;

                entry["CLASS"] = "ALTBEACON";
                entry["COMPANY"] = advertData.companyCode();
                entry["COMPANYNAME"] = advertData.companyCodeAsString();
                entry["ID"] = advertData.beaconIdAsString();
                entry["RESV"] = advertData.altBeaconReserved();
            }

            entry["RSSI"] = advertData.calibratedStrength();
            entry["LOSS"] = advertData.calibratedStrength() - rssi;

            qDebug() << "SSSS entry[\"MAC\"]" << entry["MAC"] << endl;

            if (!_model->isEmpty()) {
                bool duplicate = false;
                for (QVariantList indexPath = _model->first(); !indexPath.isEmpty(); indexPath = _model->after(indexPath)) {
                    QVariantMap targetEntry = _model->data(indexPath).toMap();
                    if (targetEntry["MAC"].toString().compare(entry["MAC"].toString()) == 0) {
                        qDebug() << "SSSS duplicate found in model" << endl;
                        duplicate = true;
                        _model->updateItem(indexPath, entry);
                        break;
                    }
                }
                if (!duplicate) {
                    qDebug() << "SSSS Inserting new item in model" << endl;
                    _model->insert(entry);
                    beaconEnteredRange(entry);
                }
            } else {
                qDebug() << "SSSS Adding first item in model" << endl;
                _model->insert(entry);
                beaconEnteredRange(entry);
            }
        } else {
            qDebug() << "SSSS no beacon present data in advert data" << endl;
        }
    } else {
        qDebug() << "SSSS advert data parse FAIL" << endl;
    }

    expireBeaconData(BEACON_EXPIRY_INTERVAL_SECONDS);
}

void Service::expireBeaconData(int seconds)
{
    if (!_model->isEmpty()) {
        for (QVariantList indexPath = _model->first(); !indexPath.isEmpty(); indexPath = _model->after(indexPath)) {
            QVariantMap targetEntry = _model->data(indexPath).toMap();
            QDateTime beaconLastSeen = qvariant_cast<QDateTime>(targetEntry["TIME"]);
            if (beaconLastSeen.secsTo(QDateTime::currentDateTime()) > seconds) {
                _model->removeAt(indexPath);
                beaconExitedRange(targetEntry);
            }
        }
    }
}

QString Service::btEventName(int event)
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
        case BT_EVT_FAULT:
            return QString("BT_EVT_FAULT");
        case BT_EVT_NAME_UPDATED:
            return QString("BT_EVT_NAME_UPDATED");
        case BT_EVT_UNDEFINED_EVENT:
            return QString("BT_EVT_UNDEFINED_EVENT");
        default:
            return QString("UNKNOWN EVENT:%1").arg(event);
    }
}

void Service::onInvoked(const bb::system::InvokeRequest &request)
{
    qDebug() << "SSSS Received invoke=" << request.action();

    if (request.action().compare(WAKEME_INVOKE_ACTION_RESET) == 0) {

        triggerNotification();

    } else if (request.action().compare(WAKEME_INVOKE_ACTION_STOPSERVICE) == 0) {

        bb::Application::instance()->requestExit();

    } else if (request.action().compare(WAKEME_INVOKE_ACTION_ADDSCAN) == 0) {

        bt_le_add_scan_device(request.data().data(), this);

    } else if (request.action().compare(WAKEME_INVOKE_ACTION_REMOVESCAN) == 0) {

        bt_le_remove_scan_device(request.data().data());

    } else if (request.action().compare(WAKEME_INVOKE_ACTION_ENABLESCANINVOKE) == 0) {

        bt_le_set_scan_params(1000, 100, BT_LE_ADVERT_SCAN_PASSIVE);
        bt_le_enable_scan_invoke(WAKEME_INVOKE_HEADLESS_SERVICE);
        _scanning = true;
        sendStatus();

    } else if (request.action().compare(WAKEME_INVOKE_ACTION_DISABLESCANINVOKE) == 0) {

        bt_le_disable_scan_invoke();
        _scanning = false;
        sendStatus();

    } else if (request.action().compare(WAKEME_INVOKE_ACTION_SCANRESULT) == 0) {

        const char *bdaddr = NULL;
        int8_t rssi = -127;
        const char *data = NULL;
        int len = 0;

        bt_le_invoke_decode_scan_result(request.data().data(), request.data().length(), &bdaddr, &rssi, &data, &len);
        parseBeaconData(data, len, rssi, bdaddr);

    } else if (request.action().compare(WAKEME_INVOKE_ACTION_STARTSERVICE) == 0) {

        qDebug() << "SSSS Headless service started" << endl;

    } else {
        qWarning() << "SSSS received invocation request we don't handle:" << request.action() << endl;
    }
}

void Service::triggerNotification()
{
    // Timeout is to give time for UI to minimize
    QTimer::singleShot(2000, this, SLOT(onGuiTimeout()));
}

void Service::onGuiTimeout()
{
    Notification::clearEffectsForAll();
    Notification::deleteAllFromInbox();
    _notification->notify();
}

void Service::onProcessStateChanged(bb::ProcessState::Type processState)
{
    switch (processState) {
        case ProcessState::Foreground:
            qDebug() << "SSSS Process state changes to Foreground" << endl;
            break;
        case ProcessState::Background:
            qDebug() << "SSSS Process state changes to Background" << endl;
            break;
        case ProcessState::Stopping:
            qDebug() << "SSSS Process state changes to Stopping" << endl;
            break;
        case ProcessState::Unknown:
            qDebug() << "SSSS Process state changes to Unknown" << endl;
            break;
        default:
            qDebug() << "SSSS Process state changes to unspecified value" << processState << endl;
            break;
    }
}

void Service::onGuiConnected()
{
    qDebug() << "SSSS GUI Connected" << endl;
    _guiConnected = true;
    sendStatus();
}

void Service::onGuiDisconnected()
{
    qDebug() << "SSSS GUI Disconnected" << endl;
    _guiConnected = false;
}

void Service::sendStatus()
{
    qDebug() << "SSSS Sending Status" << endl;

    QVariantMap entry;

    entry["TYPE"] = "STATUS";
    entry["TIME_STAMP"] = QDateTime::currentDateTime();
    entry["SERVER_UP"] = true;
    entry["SCANNING"] = _scanning;
    entry["BT_INIT"] = _btInitialised;
    entry["BT_RADIO_ON"] = _btRadioOn;

    emit messageForGui(constructJsonPayload(entry));
}

const QString Service::constructJsonPayload(QVariantMap parameters)
{
    QVariant payload = QVariant(parameters);
    QString jsonPayload;
    JsonDataAccess jda;

    jda.saveToBuffer(payload, &jsonPayload);
    if (!jda.hasError()) {
        return jsonPayload;
    } else {
        return QString("");
    }
}

void Service::beaconEnteredRange(const QVariantMap &entry)
{
    QVariantMap beacon(entry);
    QString beaconClass = qvariant_cast<QString>(beacon["CLASS"]);

    beacon["TYPE"] = "BEACON-ENTER-RANGE";

    bool isIBeacon = (beaconClass.compare("IBEACON") == 0);
    bool isAltBeacon = (beaconClass.compare("ALTBEACON") == 0);

    if (_guiConnected) {
        qDebug() << "SSSS Beacon Entered range message sent to GUI over socket" << endl;
        emit messageForGui(constructJsonPayload(beacon));
    } else {
        qDebug() << "SSSS Beacon Entered range message sent to GUI via notification" << endl;

        _notification->setTitle("WakeMeByBeacon Service");

        if (isIBeacon) {
            _notification->setBody(QString("Entered range of iBeacon\nUUID: %1\nMajor: %2\nMinor: %3")
                    .arg(qvariant_cast<QString>(beacon["UUID"]))
                    .arg(qvariant_cast<int>(beacon["MAJOR"]))
                    .arg(qvariant_cast<int>(beacon["MINOR"])));

        } else if (isAltBeacon) {
            if (qvariant_cast<QString>(beacon["COMPANYNAME"]).compare("") == 0) {
            _notification->setBody(QString("Entered range of AltBeacon\nID: %1\nCompany Code: %2\nReserved: %3")
                    .arg(qvariant_cast<QString>(beacon["ID"]))
                    .arg(qvariant_cast<int>(beacon["COMPANY"]))
                    .arg(qvariant_cast<int>(beacon["RESV"])));
            } else {
                _notification->setBody(QString("Entered range of AltBeacon\nID: %1\nCompany Name: %2\nReserved: %3")
                        .arg(qvariant_cast<QString>(beacon["ID"]))
                        .arg(qvariant_cast<int>(beacon["COMPANYNAME"]))
                        .arg(qvariant_cast<int>(beacon["RESV"])));
            }
        }

        InvokeRequest request;
        request.setTarget(WAKEME_INVOKE_GUI);
        request.setAction(WAKEME_INVOKE_ACTION_STARTSERVICE);
        request.setData(constructJsonPayload(beacon).toAscii().data());
        _notification->setInvokeRequest(request);
        _notification->notify();
    }
}

void Service::beaconExitedRange(const QVariantMap &entry)
{
    QVariantMap beacon(entry);
    QString beaconClass = qvariant_cast<QString>(beacon["CLASS"]);

    beacon["TYPE"] = "BEACON-EXIT-RANGE";

    bool isIBeacon = (beaconClass.compare("IBEACON") == 0);
    bool isAltBeacon = (beaconClass.compare("ALTBEACON") == 0);

    if (_guiConnected) {
        qDebug() << "SSSS Beacon Exited range message sent to GUI over socket" << endl;
        emit messageForGui(constructJsonPayload(beacon));
    } else {
        qDebug() << "SSSS Beacon Exited range message sent to GUI via notification" << endl;

        if (isIBeacon) {
            _notification->setBody(QString("Exited range of iBeacon\nUUID: %1\nMajor: %2\nMinor: %3")
                    .arg(qvariant_cast<QString>(beacon["UUID"]))
                    .arg(qvariant_cast<int>(beacon["MAJOR"]))
                    .arg(qvariant_cast<int>(beacon["MINOR"])));

        } else if (isAltBeacon) {
            if (qvariant_cast<QString>(beacon["COMPANYNAME"]).compare("") == 0) {
            _notification->setBody(QString("Exited range of AltBeacon\nID: %1\nCompany Code: %2\nReserved: %3")
                    .arg(qvariant_cast<QString>(beacon["ID"]))
                    .arg(qvariant_cast<int>(beacon["COMPANY"]))
                    .arg(qvariant_cast<int>(beacon["RESV"])));
            } else {
                _notification->setBody(QString("Exited range of AltBeacon\nID: %1\nCompany Name: %2\nReserved: %3")
                        .arg(qvariant_cast<QString>(beacon["ID"]))
                        .arg(qvariant_cast<int>(beacon["COMPANYNAME"]))
                        .arg(qvariant_cast<int>(beacon["RESV"])));
            }
        }

        InvokeRequest request;
        request.setTarget(WAKEME_INVOKE_GUI);
        request.setAction(WAKEME_INVOKE_ACTION_STARTSERVICE);
        request.setData(constructJsonPayload(beacon).toAscii().data());
        _notification->setInvokeRequest(request);
        _notification->notify();
    }
}

