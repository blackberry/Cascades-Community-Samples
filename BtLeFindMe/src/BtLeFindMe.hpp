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
#ifndef BtLeFindMe_HPP_
#define BtLeFindMe_HPP_

#include <QObject>

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <BluetoothLe.hpp>
#include "Timer.hpp"
#include "Utilities.hpp"

using namespace bb::cascades;

namespace bb { namespace cascades { class Application; }}

class BtLeFindMe : public QObject
{
    Q_OBJECT

public:
    BtLeFindMe(bb::cascades::Application *app);
    virtual ~BtLeFindMe() {}

private:
    QmlDocument *_qml;
    AbstractPane *_root;
    BluetoothLe *_bluetoothLe;
//  QObject *_mainPage;
};

#endif /* BtLeFindMe_HPP_ */
