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

#include "applicationui.hpp"

using namespace bb::cascades;
using namespace bb::system;

ApplicationUI::ApplicationUI()
    : QObject()
    , _translator(new QTranslator(this))
    , _localeHandler(new LocaleHandler(this))
    , _invokeManager(new InvokeManager(this))
    , _socket(new QLocalSocket(this))
    , _connectedToServer(false)
    , _socketRetryInterval(WAKEME_HEADLESS_SOCKET_SERVER_RETRY_INTERVAL_MS)
//  , _socketServerPath(WAKEME_HEADLESS_SOCKET_SERVER_NAME)
//  , _socketServerPath(QDir::tempPath().append("/").append(WAKEME_HEADLESS_SOCKET_SERVER_NAME))
    , _socketServerPath(QString().append("/tmp/").append(WAKEME_HEADLESS_SOCKET_SERVER_NAME)) // TODO: permissions issue
    , _scanning(false)
{
    if (!QObject::connect(_localeHandler, SIGNAL(systemLanguageChanged()),
                                    this, SLOT(onSystemLanguageChanged()))) {
        qWarning() << "Recovering from a failed connect()";
        qWarning() << "XXXX ApplicationUI::ApplicationUI() - connect failed - onSystemLanguageChanged" << strerror(errno) << endl;
    }

    onSystemLanguageChanged();
    _qml = QmlDocument::create("asset:///main.qml").parent(this);
    _qml->setContextProperty("app", this);
    _root = _qml->createRootObject<AbstractPane>();

    _mainPage = _root->findChild<QObject*>("mainPage");

    // ============== Signals to be sent to page

    QObject::connect(     this, SIGNAL(message(QVariant)),
                     _mainPage, SLOT(onMessage(QVariant)));

    QObject::connect(     this, SIGNAL(scanStateChanged(QVariant)),
                     _mainPage, SLOT(onScanStateChanged(QVariant)));

    QObject::connect(     this, SIGNAL(startedStateChanged(QVariant)),
                     _mainPage, SLOT(onStartedStateChanged(QVariant)));

    QObject::connect(     this, SIGNAL(iBeaconEnteredRange(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)),
                     _mainPage, SLOT(onIBeaconEnteredRange(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)));

    QObject::connect(     this, SIGNAL(iBeaconExitedRange(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)),
                     _mainPage, SLOT(onIBeaconExitedRange(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)));

    QObject::connect(     this, SIGNAL(altBeaconEnteredRange(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)),
                     _mainPage, SLOT(onAltBeaconEnteredRange(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)));

    QObject::connect(     this, SIGNAL(altBeaconExitedRange(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)),
                     _mainPage, SLOT(onAltBeaconExitedRange(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant)));

    // ============== Hook up signals from QML

    QObject::connect(_mainPage, SIGNAL(startHeadlessService()),
                          this, SLOT(onStartHeadlessService()));

    QObject::connect(_mainPage, SIGNAL(stopHeadlessService()),
                          this, SLOT(onStopHeadlessService()));

    QObject::connect(_mainPage, SIGNAL(enableScanInvokeRequest()),
                          this, SLOT(onEnableScanInvokeRequest()));

    QObject::connect(_mainPage, SIGNAL(disableScanInvokeRequest()),
                          this, SLOT(onDisableScanInvokeRequest()));

    QObject::connect(_mainPage, SIGNAL(addScanRequest(QString)),
                          this, SLOT(onAddScanRequest(QString)));

    QObject::connect(_mainPage, SIGNAL(removeScanRequest(QString)),
                          this, SLOT(onRemoveScanRequest(QString)));

    // ============== Other signals

    QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
                                   this, SLOT(onInvoked(const bb::system::InvokeRequest&)));
    connectSocketSignals();
    onStartHeadlessService();

    Application::instance()->setScene(_root);
}

ApplicationUI::~ApplicationUI()
{
    if (_socket) {
        disconnectSocketSignals();
        _connectedToServer = false;
        _socket->close();
        _socket->deleteLater();
    }
}

void ApplicationUI::onInvoked(const bb::system::InvokeRequest &request)
{
    qDebug() << "XXXX Received invoke=" << request.action();

    if (request.action().compare(WAKEME_INVOKE_ACTION_STARTAPP) == 0) {

        qDebug() << "XXXX Application Started started via Invoke with data: " << request.data().data() << endl;

        JsonDataAccess jda;
        QVariant parsedPayload = jda.loadFromBuffer(request.data());
        if (!jda.hasError()) {
            handleBeaconEvent(parsedPayload.toMap());
        } else {
            qWarning() << "XXXX ApplicationUI::onInvoked() - JSON parse error" << request.data() << endl;
        }

    } else {
        qWarning() << "XXXX received invocation request we don't handle:" << request.action() << endl;
    }
}

void ApplicationUI::handleBeaconEvent(const QVariantMap &payload)
{
    QString type = payload["TYPE"].toString();

    qDebug() << "XXXX Handling Beacon Event: " << type << endl;

    bool beaconEnterEvent = (type.compare("BEACON-ENTER-RANGE") == 0);
    bool beaconExitEvent = (type.compare("BEACON-EXIT-RANGE") == 0);

    if ( beaconEnterEvent || beaconExitEvent) {

        // Common Beacon attributes
        QString beaconClass = qvariant_cast<QString>(payload["CLASS"]);
        QDateTime timeStamp = qvariant_cast<QDateTime>(payload["TIME"]);
        QString macAddress = qvariant_cast<QString>(payload["MAC"]);
        int rssi = qvariant_cast<int>(payload["RSSI"]);
        int loss = qvariant_cast<int>(payload["LOSS"]);

        bool isIBeacon = (beaconClass.compare("IBEACON") == 0);
        bool isAltBeacon = (beaconClass.compare("ALTBEACON") == 0);

        // iBeacon specific attributes
        QString uuid = "";
        int major = 0;
        int minor = 0;

        // AltBeacon specific attributes
        QString beaconId = "";
        QString companyName = "";
        int companyCode = 0;
        int reserved = 0;

        qDebug() << "XXXX TimeStamp: " << timeStamp << endl;
        qDebug() << "XXXX Event: " << type << endl;
        qDebug() << "XXXX macAddress: " << macAddress << endl;

        if (isIBeacon) {

            uuid = qvariant_cast<QString>(payload["UUID"]);
            major = qvariant_cast<int>(payload["MAJOR"]);
            minor = qvariant_cast<int>(payload["MINOR"]);

            qDebug() << "XXXX uuid: " << uuid << endl;
            qDebug() << "XXXX major: " << major << endl;
            qDebug() << "XXXX minor: " << minor << endl;

        } else if(isAltBeacon) {

            companyCode = qvariant_cast<int>(payload["COMPANY"]);
            companyName = qvariant_cast<QString>(payload["COMPANYNAME"]);
            beaconId = qvariant_cast<QString>(payload["ID"]);
            reserved = qvariant_cast<int>(payload["RESV"]);

            qDebug() << "XXXX companyCode: " << companyCode << endl;
            qDebug() << "XXXX companyName: " << companyName << endl;
            qDebug() << "XXXX beaconId: " << beaconId << endl;
            qDebug() << "XXXX reserved: " << reserved << endl;

        } else {
            qDebug() << "XXXX Unrecognised Beacon Specification" << endl;
        }

        qDebug() << "XXXX rssi: " << rssi << endl;
        qDebug() << "XXXX loss: " << loss << endl;

        if (beaconEnterEvent) {
            if (isIBeacon && monitoringUuid(uuid, major, minor)) {
                qDebug() << "XXXX - iBeacon - Emiting beaconEnteredRange signal" << endl;
                emit iBeaconEnteredRange(timeStamp, macAddress, uuid, major, minor, rssi, loss);
            }
            if (isAltBeacon && monitoringAltBeacon(companyCode, beaconId, reserved)) {
                qDebug() << "XXXX - AltBeacon - Emiting beaconEnteredRange signal" << endl;
                emit altBeaconEnteredRange(timeStamp, macAddress, companyCode, companyName, beaconId, reserved, rssi, loss);
            }
        }
        if (beaconExitEvent) {
            if (isIBeacon && monitoringUuid(uuid, major, minor)) {
                qDebug() << "XXXX - iBeacon - Emiting beaconExitedRange signal" << endl;
                emit iBeaconExitedRange(timeStamp, macAddress, uuid, major, minor, rssi, loss);
            }
            if (isAltBeacon && monitoringAltBeacon(companyCode, beaconId, reserved)) {
                qDebug() << "XXXX - AltBeacon - Emiting beaconExitedRange signal" << endl;
                emit altBeaconExitedRange(timeStamp, macAddress, companyCode, companyName, beaconId, reserved, rssi, loss);
            }
        }
    }
}

bool ApplicationUI::monitoringUuid(const QString & uuid, int major, int minor)
{
	Q_UNUSED(uuid);
	Q_UNUSED(major);
	Q_UNUSED(minor);

	return true;
}

bool ApplicationUI::monitoringAltBeacon(int companyCode, const QString & beaconId, int reserved)
{
    Q_UNUSED(companyCode);
    Q_UNUSED(beaconId);
    Q_UNUSED(reserved);

    return true;
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(_translator);
    QString localeString = QLocale().name();
    QString file_name = QString("WakeMeByBeacon_%1").arg(localeString);
    if (_translator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(_translator);
    }
}

void ApplicationUI::resendNotification()
{
    InvokeRequest request;
    request.setTarget(WAKEME_INVOKE_HEADLESS_SERVICE);
    request.setAction(WAKEME_INVOKE_ACTION_RESET);
    _invokeManager->invoke(request);
    Application::instance()->minimize();
}

void ApplicationUI::onAddScanRequest(const QString &address) {
    InvokeRequest request;
    QByteArray data(address.toAscii().data());
    request.setTarget(WAKEME_INVOKE_HEADLESS_SERVICE);
    request.setAction(WAKEME_INVOKE_ACTION_ADDSCAN);
    request.setData(data);
    _invokeManager->invoke(request);
}

void ApplicationUI::onEnableScanInvokeRequest() {
    InvokeRequest request;
    request.setTarget(WAKEME_INVOKE_HEADLESS_SERVICE);
    request.setAction(WAKEME_INVOKE_ACTION_ENABLESCANINVOKE);
    _invokeManager->invoke(request);
}

void ApplicationUI::onDisableScanInvokeRequest() {
    InvokeRequest request;
    request.setTarget(WAKEME_INVOKE_HEADLESS_SERVICE);
    request.setAction(WAKEME_INVOKE_ACTION_DISABLESCANINVOKE);
    _invokeManager->invoke(request);
}

void ApplicationUI::onRemoveScanRequest(const QString &address) {
    QByteArray data(address.toAscii().data());
    InvokeRequest request;
    request.setTarget(WAKEME_INVOKE_HEADLESS_SERVICE);
    request.setAction(WAKEME_INVOKE_ACTION_REMOVESCAN);
    request.setData(data);
    _invokeManager->invoke(request);
}

void ApplicationUI::onStartHeadlessService()
{
    InvokeRequest request;
    request.setTarget(WAKEME_INVOKE_HEADLESS_SERVICE);
    request.setAction(WAKEME_INVOKE_ACTION_STARTSERVICE);
    _invokeManager->invoke(request);

    connectToHeadless();
}

void ApplicationUI::onStopHeadlessService() {
    InvokeRequest request;
    request.setTarget(WAKEME_INVOKE_HEADLESS_SERVICE);
    request.setAction(WAKEME_INVOKE_ACTION_STOPSERVICE);
    _invokeManager->invoke(request);
}

void ApplicationUI::connectToHeadless()
{
    if (!_connectedToServer) {
        qDebug() << "XXXX Connecting to Headless" << endl;
        qDebug() << "XXXX ApplicationUI::connectToHeadless() - _socketServerPath " << _socketServerPath << endl;
        _socket->connectToServer(_socketServerPath);
    } else {
        qDebug() << "XXXX Server connected - unable to connect" << endl;
    }
}

void ApplicationUI::connectSocketSignals()
{
    qDebug() << "XXXX Connecting socket SIGNAL()s" << endl;
    QObject::connect(_socket, SIGNAL(connected()),
                        this, SLOT(onConnected()));

    QObject::connect(_socket, SIGNAL(disconnected()),
                        this, SLOT(onDisconnected()));

    QObject::connect(_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
                        this, SLOT(onError(QLocalSocket::LocalSocketError)));

    QObject::connect(_socket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
                        this, SLOT(onStateChanged(QLocalSocket::LocalSocketState)));

    QObject::connect(_socket, SIGNAL(readyRead()),
                        this, SLOT(onReadyRead()));
}

void ApplicationUI::disconnectSocketSignals()
{
    qDebug() << "XXXX Disconnecting socket SIGNAL()s" << endl;
    QObject::disconnect(_socket, SIGNAL(connected()),
                           this, SLOT(onConnected()));

    QObject::disconnect(_socket, SIGNAL(disconnected()),
                           this, SLOT(onDisconnected()));

    QObject::disconnect(_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
                           this, SLOT(onError(QLocalSocket::LocalSocketError)));

    QObject::disconnect(_socket, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
                           this, SLOT(onStateChanged(QLocalSocket::LocalSocketState)));
}

void ApplicationUI::onConnected() {
    qDebug() << "XXXX Connected to server" << endl;
    _connectedToServer = true;
    sendToHeadless("*** Message from GUI: Connected to Server ***");
    emit startedStateChanged(_connectedToServer);
}

void ApplicationUI::onDisconnected() {
    qDebug() << "XXXX Disconnected from server" << endl;
    _connectedToServer = false;
    _socketRetryInterval = WAKEME_HEADLESS_SOCKET_SERVER_RETRY_INTERVAL_MS;
    _socket->close();
    emit startedStateChanged(_connectedToServer);
}

void ApplicationUI::onError(QLocalSocket::LocalSocketError socketError) {
    qDebug() << "XXXX Socket error from server" << socketError << endl;
    if (!_connectedToServer) {
        qDebug() << "XXXX Socket not connected so retrying" << endl;
        triggerRetryConnection();
    }
}

void ApplicationUI::triggerRetryConnection() {
    qDebug() << "XXXX Socket retry scheduled for " << _socketRetryInterval << "mseconds" << endl;
    // Timeout is to give time for Headless socket server to start
    QTimer::singleShot(_socketRetryInterval, this, SLOT(onTimeout()));
}

void ApplicationUI::onTimeout() {
    qDebug() << "XXXX retry timeout expiry - retrying connection to socket server" << endl;
    _socketRetryInterval *= 2;
    if (_socketRetryInterval > WAKEME_HEADLESS_SOCKET_SERVER_MAX_RETRY_INTERVAL_MS) {
        _socketRetryInterval = WAKEME_HEADLESS_SOCKET_SERVER_MAX_RETRY_INTERVAL_MS;
    }
    connectToHeadless();
}

void ApplicationUI::onStateChanged(QLocalSocket::LocalSocketState socketState) {
    qDebug() << "XXXX Socket state changed" << socketState << endl;
}

void ApplicationUI::onReadyRead() {
    QByteArray readData = _socket->readAll();
    qDebug() << "XXXX Read: " << readData.constData() << endl;

    JsonDataAccess jda;
    QVariant parsedPayload = jda.loadFromBuffer(readData);
    if (!jda.hasError()) {
        QVariantMap payload = parsedPayload.toMap();
        QString type = payload["TYPE"].toString();

        if (type.compare("STATUS") == 0) {
            QDateTime timeStamp = qvariant_cast<QDateTime>(payload["TIME_STAMP"]);

            bool serverUp = qvariant_cast<bool>(payload["SERVER_UP"]);
            bool scanning = qvariant_cast<bool>(payload["SCANNING"]);
            bool btInitialised = qvariant_cast<bool>(payload["BT_INIT"]);
            bool btRadioOn = qvariant_cast<bool>(payload["BT_RADIO_ON"]);

            qDebug() << "XXXX timeStamp: " << timeStamp << endl;
            qDebug() << "XXXX serverUp: " << serverUp << endl;
            qDebug() << "XXXX scanning: " << scanning << endl;
            qDebug() << "XXXX BT Initialised: " << btInitialised << endl;
            qDebug() << "XXXX BT Radio On: " << btRadioOn << endl;

            _scanning = scanning;
            emit scanStateChanged(_scanning);

            if (btRadioOn) {
                emit message("Bluetooth Radio is ON");
            } else {
                emit message("Bluetooth Radio is OFF -- please enable it");
            }

        } else if ((type.compare("BEACON-ENTER-RANGE") == 0) || (type.compare("BEACON-EXIT-RANGE") == 0)) {
        	handleBeaconEvent(payload);
        }
    } else {
        qWarning() << "XXXX ApplicationUI::onReadyRead() - JSON parse error" << readData.constData() << endl;
    }
}

void ApplicationUI::sendToHeadless(const QVariant &text)
{
    if (_connectedToServer) {
        qDebug() << "XXXX sending to headless " << text.toString() << endl;
        _socket->write(text.toByteArray());
        _socket->flush();
    } else {
        qDebug() << "XXXX Server not connected unable to send " << text.toString() << endl;
    }
}
