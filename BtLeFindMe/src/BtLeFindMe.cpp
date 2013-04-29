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
#include "BtLeFindMe.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

BtLeFindMe::BtLeFindMe(bb::cascades::Application *app)
	: QObject(app)
	, _bluetoothLe(new BluetoothLe(this))
{
	qmlRegisterType<DeviceListing>();
	qmlRegisterType<Timer>("CustomTimer", 1, 0, "Timer");

    QmlDocument *_qml = QmlDocument::create("asset:///main.qml").parent(this);
    _qml->setContextProperty("_btLe", _bluetoothLe);
    AbstractPane *_root = _qml->createRootObject<AbstractPane>();

    app->setScene(_root);

	qDebug() << "XXXX running on OS version " << Utilities::getOSVersion();
}
