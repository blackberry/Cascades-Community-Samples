/* Copyright (c) 2013 BlackBerry.
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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QtLocationSubset/QGeoPositionInfo>

namespace bb { namespace cascades { class Application; }}
using namespace QtMobilitySubset;

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
private:
    // This slot will be used to notify the app when the geolocation is found
    Q_SLOT void positionUpdated (const QGeoPositionInfo &update);
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() {}

    // To make life a bit easier I have written some Q_INVOKABLE functions so the Flurry
    // methods can be called from QML
    Q_INVOKABLE void logEvent(const QString & eventName, bool timedEvent = false);
    Q_INVOKABLE void endTimedEvent(const QString & eventName);
    Q_INVOKABLE void setAge(int age);
    Q_INVOKABLE void setGender(const QString & gender);
    Q_INVOKABLE void setUserId(const QString & userId);
};


#endif /* ApplicationUI_HPP_ */
