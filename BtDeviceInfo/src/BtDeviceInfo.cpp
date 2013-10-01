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

#include "BtDeviceInfo.hpp"
#include "DataContainer.hpp"
#include "Utilities.hpp"
#include "BluetoothHandler.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

static AbstractPane *_root = 0;

BtDeviceInfo::BtDeviceInfo(bb::cascades::Application *app)
: QObject(app)
, _handler(new BluetoothHandler(this))
{

	qmlRegisterType<DeviceListing>();

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	qml->setContextProperty("_bdi", this);
	qml->setContextProperty("_bt", _handler);

    QObject::connect(_handler, SIGNAL(signalLinkEstablished()), this, SLOT(onLinkEstablished()), Qt::QueuedConnection);
    QObject::connect(_handler, SIGNAL(signalLostLink()), this, SLOT(onLostLink()), Qt::QueuedConnection);
    QObject::connect(_handler, SIGNAL(signalInfoMessage(QString)), this, SLOT(onInfoMessage(QString)), Qt::QueuedConnection);
    QObject::connect(_handler, SIGNAL(signalLogMessage(QString)), this, SLOT(onLogMessage(QString)), Qt::QueuedConnection);

	_root = qml->createRootObject<AbstractPane>();

    app->setScene(_root);

}

void BtDeviceInfo::onLinkEstablished() {
	qDebug() << "XXXX onLinkEstablished";
    onInfoMessage("Connected to Device Information Service");
}

void BtDeviceInfo::onLostLink() {
    qDebug() << "XXXX onLostLink";
    onInfoMessage("Lost link to device");
}


void BtDeviceInfo::onInfoMessage(QString message) {
    qDebug() << "XXXX onInfoMessage";
    QMetaObject::invokeMethod(_root, "infoMessage", Q_ARG(QVariant, message));
}

void BtDeviceInfo::onLogMessage(QString msg)
{
    qDebug() << "XXXX " << msg;
    QMetaObject::invokeMethod(_root, "logMessage", Q_ARG(QVariant, msg));
}


void BtDeviceInfo::getInfo(QString device_addr, QString device_name) {
	qDebug() << "YYYY getting info from device " << device_addr;

	DataContainer* dc = DataContainer::getInstance();
	dc->setCurrentDeviceAddr(device_addr);
	dc->setCurrentDeviceName(device_name);

	_handler->getInfo();

}
