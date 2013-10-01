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

#ifndef BtDeviceInfo_HPP_
#define BtDeviceInfo_HPP_

#include <QObject>
#include "BluetoothHandler.hpp"

namespace bb
{
    namespace cascades
    {
        class Application;
    }
}

class BtDeviceInfo: public QObject
{
Q_OBJECT
public:
    BtDeviceInfo(bb::cascades::Application *app);
    virtual ~BtDeviceInfo()
    {
    }

    Q_INVOKABLE
    void getInfo(QString device_addr, QString device_name);Q_INVOKABLE

public slots:
    void onLinkEstablished();
    void onLostLink();
    void onInfoMessage(QString message);
    void onLogMessage(QString msg);

private:
    BluetoothHandler* _handler;

};

#endif /* BtDeviceInfo_HPP_ */
