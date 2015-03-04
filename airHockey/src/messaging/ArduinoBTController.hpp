/* Copyright (c) 2012, 2013  BlackBerry Limited.
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

#ifndef ARDUINOBTCONTROLLER_HPP
#define ARDUINOBTCONTROLLER_HPP

#include "ChatManager.hpp"

#include "KeepAliveThread.h"
#include <QObject>


class ArduinoBTController : public QObject
{
    Q_OBJECT


public:
    /**
     * Creates a new BTController object.
     *
     * @param parent The parent object.
     */
    ArduinoBTController(QObject* parent);

    // Destroys the BTController object
    virtual ~ArduinoBTController();

    Q_INVOKABLE
//    void sendMyMessage(const QString &msg);
    void sendMyMessage(const char *msg);


public Q_SLOTS:

//void onWantsToMoveArm(int direction);

Q_SIGNALS:


private Q_SLOTS:


private:
SPPThread m_sppDataThread;
KeepAliveThread m_keepAliveThread;

};


#endif
