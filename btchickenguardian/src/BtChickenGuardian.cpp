/* Copyright (c) 2013 BlackBerry Limited.
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

#include "BtChickenGuardian.hpp"
#include "Timer.hpp"
#include "DataContainer.hpp"
#include "Utilities.hpp"
#include "BluetoothHandler.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

static AbstractPane *_root = 0;

BtChickenGuardian::BtChickenGuardian(bb::cascades::Application *app)
: QObject(app)
, _handler(new BluetoothHandler(this))
{

	qmlRegisterType<DeviceListing>();
    qmlRegisterType<Timer>("CustomTimer", 1, 0, "Timer");

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	qml->setContextProperty("_bcg", this);
	qml->setContextProperty("_bt", _handler);

    QObject::connect(_handler, SIGNAL(signalLinkEstablished()), this, SLOT(onLinkEstablished()), Qt::QueuedConnection);
    QObject::connect(_handler, SIGNAL(signalLostLink()), this, SLOT(onLostLink()), Qt::QueuedConnection);
    QObject::connect(_handler, SIGNAL(signalSetMessage(QString)), this, SLOT(onSetMessage(QString)), Qt::QueuedConnection);
    QObject::connect(_handler, SIGNAL(signalRssi(QVariant,QVariant)), this, SLOT(onRssi(QVariant,QVariant)), Qt::QueuedConnection);
    QObject::connect(_handler, SIGNAL(signalProximityChange(QVariant)), this, SLOT(onProximityChange(QVariant)), Qt::QueuedConnection);
    QObject::connect(_handler, SIGNAL(signalEnableProximityVisualisation()), this, SLOT(onEnableProximityVisualisation()), Qt::QueuedConnection);
    QObject::connect(_handler, SIGNAL(signalDisableProximityVisualisation()), this, SLOT(onDisableProximityVisualisation()), Qt::QueuedConnection);

	_root = qml->createRootObject<AbstractPane>();

    app->setScene(_root);

}

void BtChickenGuardian::onLinkEstablished() {
	qDebug() << "XXXX onLinkEstablished";
	QMetaObject::invokeMethod(_root, "linkEstablished");
}

void BtChickenGuardian::onLostLink() {
    qDebug() << "XXXX onLostLink";
    DataContainer* dc = DataContainer::getInstance();
    dc->setProximityIndicator(-1);
    QMetaObject::invokeMethod(_root, "lostLink");
}

void BtChickenGuardian::onEnableProximityVisualisation() {
    qDebug() << "XXXX onEnableProximityVisualisation";
    QMetaObject::invokeMethod(_root, "enableProximityVisualisation");
}

void BtChickenGuardian::onDisableProximityVisualisation() {
    qDebug() << "XXXX onDisableProximityVisualisation";
    QMetaObject::invokeMethod(_root, "disableProximityVisualisation");
}

void BtChickenGuardian::onSetMessage(QString message) {
    qDebug() << "XXXX onSetMessage";
    QMetaObject::invokeMethod(_root, "setMessage", Q_ARG(QVariant, message));
}

void BtChickenGuardian::onRssi(QVariant proximity_indicator, QVariant rssi) {
    qDebug() << "XXXX onRssi";
    QMetaObject::invokeMethod(_root, "onRssi", Q_ARG(QVariant, proximity_indicator), Q_ARG(QVariant, rssi));
}

void BtChickenGuardian::onProximityChange(QVariant proximity_indicator) {
    qDebug() << "XXXX onProximityChange";
    QMetaObject::invokeMethod(_root, "onProximityChange", Q_ARG(QVariant, proximity_indicator));
}

void BtChickenGuardian::monitorProximity(QString device_addr, QString device_name) {
	qDebug() << "YYYY monitoring proximity of device " << device_addr;

	DataContainer* dc = DataContainer::getInstance();
	dc->setCurrentDeviceAddr(device_addr);
	dc->setCurrentDeviceName(device_name);

	_handler->startProximityMonitoring();

}

void BtChickenGuardian::cleanUpOnExit() {
    qDebug() << "XXXX Clean up on application exit";
    _handler->quit();
}

void BtChickenGuardian::saveSettings(QString rssiNear,QString rssiMedium, QString rssiFluctuationSensitivity) {
    qDebug() << "XXXX saveSettings:" << rssiNear << "," << rssiMedium;
    DataContainer* dc = DataContainer::getInstance();
    dc->setMinRssiNear(rssiNear.toInt());
    dc->setMinRssiMedium(rssiMedium.toInt());
    dc->setRssiFluctuationSensitivity(rssiFluctuationSensitivity.toInt());
}

int BtChickenGuardian::getRssiNear() {
    qDebug() << "XXXX getRssiNear";
    DataContainer* dc = DataContainer::getInstance();
    return dc->getMinRssiNear();
}

int BtChickenGuardian::getRssiMedium() {
    qDebug() << "XXXX getRssiMedium";
    DataContainer* dc = DataContainer::getInstance();
    return dc->getMinRssiMedium();
}

int BtChickenGuardian::getRssiFluctuationSensitivity() {
    qDebug() << "XXXX getRssiFluctuationSensitivity";
    DataContainer* dc = DataContainer::getInstance();
    return dc->getRssiFluctuationSensitivity();
}
