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

#include "HeartMonitor.hpp"
#include "BluetoothHandler.hpp"
#include "HrDataContainer.hpp"
#include "Timer.hpp"
#include "Utilities.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <btapi/btdevice.h>
#include <btapi/btgatt.h>
#include <btapi/btle.h>
#include <btapi/btspp.h>
#include <bb/cascades/databinding/groupdatamodel.h>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/ListView>
#include <QSettings>

using namespace bb::cascades;

static AbstractPane *_root = 0;
QString HeartMonitor::KEY_MQTT_HOST = "mqtt_host";
QString HeartMonitor::KEY_MQTT_PORT = "mqtt_port";

HeartMonitor::HeartMonitor(bb::cascades::Application *app) :
        QObject(app), _app(app), _handler(new BluetoothHandler(this)), _mqttClient(new MqttClient())
{
    qmlRegisterType<DeviceListing>();
    qmlRegisterType<LocalDeviceInfo>();
    qmlRegisterType<RemoteDeviceInfo>();
    qmlRegisterType<Timer>("CustomTimer", 1, 0, "Timer");

    _sessions = new SessionListing();

    HrDataContainer* hrdc = HrDataContainer::getInstance();

    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("_hrdc", hrdc);
    qml->setContextProperty("_hrm", this);
    qml->setContextProperty("_scan", _handler);
    qml->setContextProperty("_sessions", _sessions);

    // create root object for the UI
    _root = qml->createRootObject<AbstractPane>();

    QObject::connect(hrdc, SIGNAL(heartRateData(QVariant)), this, SLOT(logHeartRate(QVariant)), Qt::QueuedConnection);
    QObject::connect(_handler->_deviceListing, SIGNAL(startMonitoring()), this, SLOT(monitorHeartRate()), Qt::QueuedConnection);

    // set created root object as a scene
    app->setScene(_root);

    QString os_version = Utilities::getOSVersion();

    qDebug() << "XXXX running on OS version " << os_version;

    QSettings settings;
    setMqttHost((settings.value(KEY_MQTT_HOST, "test.mosquitto.org").toString()));
    setMqttPort((settings.value(KEY_MQTT_PORT, "1883").toString()));

    qDebug() << "XXXX initial MQTT host value:" << getMqttHost();
    qDebug() << "XXXX initial MQTT port value:" << getMqttPort();

    if (hrdc->getCurrentDeviceAddr() != NULL) {
        monitorHeartRate();
    }


    logHeartRate("000");

}

HeartMonitor::~HeartMonitor()
{
}

void HeartMonitor::monitorHeartRate()
{
    qDebug() << "YYYY monitoring heart rate";
    _future = new QFuture<void>;
    _watcher = new QFutureWatcher<void>;
    qDebug() << "YYYY locking notification receiver thread mutex";
    _mutex.lock();
    qDebug() << "YYYY notification receiver thread mutex locked OK";
    *_future = QtConcurrent::run(_handler, &BluetoothHandler::receiveHrNotifications);
    _watcher->setFuture(*_future);
    QObject::connect(_watcher, SIGNAL(finished()), this, SLOT(finishedReceiving()));

    if (!_mqttClient->isConnected()) {
        MqttError connectError("");
        qDebug() << "XXXX connecting to " << getMqttHost() << ":" << getMqttPort();
        _mqttClient->setHostname(getMqttHost().toLatin1().data());
        _mqttClient->setPortNumber(getMqttPort().toLatin1().data());
        _mqttClient->connect(QString("MyId-").append(QString::number(qrand() % 30000)).toLatin1().data(), connectError);
        if (connectError.getCode() == MqttError::PASS_CODE) {
            qDebug() << "YYYY MQTT Connected OK";
        } else {
            qDebug() << "YYYY MQTT Connection error: " << connectError.getDescription().data();
        }
    } else {
        qDebug() << "YYYY MQTT Already Connected";
    }

}

void HeartMonitor::stopMonitoringHeartRate()
{
    qDebug() << "YYYY stop monitoring heart rate requested";
    _handler->stopHrNotifications();
    if (_mqttClient->isConnected()) {
        MqttError disconnectError("");
        _mqttClient->disconnect(disconnectError);
        if (disconnectError.getCode() == MqttError::PASS_CODE) {
            qDebug() << "YYYY MQTT Disconnected OK";
        } else {
            qDebug() << "YYYY MQTT Disconnection error: " << disconnectError.getDescription().data();
        }
    }
}

void HeartMonitor::finishedReceiving()
{
    qDebug() << "YYYY notification receiver thread has finished running";
    _mutex.unlock();
}

void HeartMonitor::logHeartRate(const QVariant &rate)
{
    qDebug() << "YYYY logging heart rate now " << rate;
    QMetaObject::invokeMethod(_root, "logHeartRate", Q_ARG(QVariant, rate));
    if (_mqttClient->isConnected()) {
        MqttError publishError("");
        _mqttClient->publish("test/hrm/rate", rate.toByteArray().data(), publishError);
        if (publishError.getCode() == MqttError::PASS_CODE) {
            qDebug() << "YYYY MQTT Published OK";
        } else {
            qDebug() << "YYYY MQTT Publish error: " << publishError.getDescription().data();
        }
    }
}

void HeartMonitor::hrDevicesFound()
{
    qDebug() << "YYYY HR devices were apparently found!";
}

void HeartMonitor::setConnectionArgs(QString hostName, QString portNumber)
{
    qDebug() << "XXXX setConnectionArgs(" << hostName << "," << portNumber << ")";

    setMqttHost(hostName);
    setMqttPort(portNumber);

    _mqttClient->setHostname(hostName.toLatin1().data());
    _mqttClient->setPortNumber(portNumber.toLatin1().data());

    QSettings settings;
    settings.setValue(KEY_MQTT_HOST, hostName);
    settings.setValue(KEY_MQTT_PORT, portNumber);

    if (_mqttClient->isConnected()) {
        MqttError disconnectError("");
        _mqttClient->disconnect(disconnectError);
        if (disconnectError.getCode() == MqttError::PASS_CODE) {
            qDebug() << "YYYY MQTT disconnected OK";
            MqttError connectError("");
            _mqttClient->connect(QString("MyId-").append(QString::number(qrand() % 30000)).toLatin1().data(), connectError);
            if (connectError.getCode() == MqttError::PASS_CODE) {
                qDebug() << "YYYY MQTT Connected OK to " << hostName << ":" << portNumber;
            } else {
                qDebug() << "YYYY MQTT Connection error: " << connectError.getDescription().data();
            }
        } else {
            qDebug() << "YYYY MQTT disconnect error: " << disconnectError.getDescription().data();
        }
    }

}

QString HeartMonitor::getMqttHost()
{
    return _mqtt_host;
}

void HeartMonitor::setMqttHost(QString host)
{
    _mqtt_host = host;
    emit signalMqttHostChanged();
}

QString HeartMonitor::getMqttPort()
{
    return _mqtt_port;
}

void HeartMonitor::setMqttPort(QString port)
{
    _mqtt_port = port;
    emit signalMqttPortChanged();
}
