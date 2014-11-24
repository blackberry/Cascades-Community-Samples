/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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
#include <bb/system/InvokeManager>

#include <QTimer>


Service::Service() :
        QObject()
{
    qDebug() << "DeferredAutoStart sample headless started!";

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start(5000);

}


void Service::onTimeout()
{
    qDebug() << "DeferredAutoStart headless service is running...";
}
