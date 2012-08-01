/* Copyright (c) 2012 Research In Motion Limited.
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
#ifndef STARSHIPSETTINGSAPP_H
#define STARSHIPSETTINGSAPP_H

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Navigationpane>
#include <QSettings>
#include "LocationEngine.h"

/**
 * StarshipSettings
 *
 * This sample application illustrates how to keep your applications current state
 * via QSettings.
 *
 * Feature summary
 * - setting up and communicating with QSettings
 */

using namespace bb::cascades;

class LocationDiagnosticsApp : public QObject
{
    Q_OBJECT

private:
    NavigationPane *appPage;

public:
    LocationDiagnosticsApp();
    ~LocationDiagnosticsApp();

    /**
     * Invokable functions that we can call from QML.
     * This function get's a value from the QSettings,
     * if that value does not exist in the QSettings database, that data will be added.
     *
     * @param objectName Index path to the item.
     * @param defaultValue is used to create that data in the database when created.
     */
    Q_INVOKABLE QString getValueFor(const QString &objectName, const QString &defaultValue);

    /**
       * Invokable functions that we can call from QML.
       * This function sets a value in the QSettings database, it's supposed to be called
       * when a datavalue has been updated from QML
       *
       * @param objectName Index path to the item.
       * @param inputValue new value to the QSettings database.
       */

    Q_INVOKABLE void saveValueFor(const QString &objectName, const QString &inputValue);

    Q_INVOKABLE void startLocationSession(bool trackingMode);
};

#endif // ifndef STARSHIPSETTINGSAPP_H
