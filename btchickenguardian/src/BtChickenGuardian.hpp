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

#ifndef BtChickenGuardian_HPP_
#define BtChickenGuardian_HPP_

#include <QObject>
#include "BluetoothHandler.hpp"

namespace bb
{
    namespace cascades
    {
        class Application;
    }
}

class BtChickenGuardian: public QObject
{
Q_OBJECT
public:
    BtChickenGuardian(bb::cascades::Application *app);
    virtual ~BtChickenGuardian()
    {
    }

    Q_INVOKABLE
    void monitorProximity(QString device_addr, QString device_name);Q_INVOKABLE
    Q_INVOKABLE
    void saveSettings(QString rssiNear,QString rssiMedium,QString rssiFluctuationSensitivity);
    Q_INVOKABLE
    int getRssiNear();
    Q_INVOKABLE
    int getRssiMedium();
    Q_INVOKABLE
    int getRssiFluctuationSensitivity();

public slots:
    void onLinkEstablished();
    void onLostLink();
    void onEnableProximityVisualisation();
    void onDisableProximityVisualisation();
    void onSetMessage(QString message);
    void cleanUpOnExit();
    void onRssi(QVariant proximity_indicator, QVariant rssi);
    void onProximityChange(QVariant proximity_indicator);

private:
    BluetoothHandler* _handler;

};

#endif /* BtChickenGuardian_HPP_ */
