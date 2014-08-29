/*
 * Copyright (c) 2014 BlackBerry Limited.
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

#ifndef SmaatoADSample_HPP_
#define SmaatoADSample_HPP_

#include <QtCore/QObject>
#include <bb/cascades/Image>
#include <bb/ImageData>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeTargetReply>
#include <bb/cascades/Invocation>
#include <bb/cascades/InvokeQuery>
#include <bb/system/CardDoneMessage.hpp>

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class SmaatoADSample : public QObject
{
    Q_OBJECT

public:
    SmaatoADSample(bb::cascades::Application *app);
    virtual ~SmaatoADSample() {}
    bb::cascades::Image image() const;
    void initFullUI();


public Q_SLOTS:


Q_SIGNALS:
    // The change notification signal of the properties


private Q_SLOTS:
    // This slot is called whenever an invocation request is received

private:
    // The accessor methods of the properties
};


#endif /* SmaatoADSample_HPP_ */
