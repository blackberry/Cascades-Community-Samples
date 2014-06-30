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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include "BtleAdvertData.hpp"

#include <QObject>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/LocaleHandler>
#include <btapi/btdevice.h>
#include <btapi/btle.h>
#include <errno.h>
#include <bb/cascades/GroupDataModel>
#include <bbndk.h>

using namespace bb::cascades;

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

class QTranslator;

class ApplicationUI : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::DataModel* model READ model CONSTANT)
    Q_PROPERTY(bb::cascades::DataModel* config_model READ configModel CONSTANT)
    Q_PROPERTY(int state READ state NOTIFY stateChanged);
    Q_PROPERTY(int scan_state READ scanState NOTIFY scanStateChanged);

public:
    ApplicationUI();
    virtual ~ApplicationUI();

    void emitMessageForUi(const QVariant &text);
	void parseBeaconData(const char *data, int len, int8_t rssi, const char *bdaddr);
	QString btEventName(int event);
    Q_INVOKABLE void save(QString mac, QString name, QVariant type);
    Q_INVOKABLE void updateBeaconAttributes(QString uuid, int major, int minor, int power, int interval, int pin);
    Q_INVOKABLE QString getBeaconType(QString mac);
    Q_INVOKABLE QString getBeaconName(QString mac);
    Q_INVOKABLE QString getBeaconUuid();
    Q_INVOKABLE int getBeaconMajor();
    Q_INVOKABLE int getBeaconMinor();
    Q_INVOKABLE int getBeaconPower();
    Q_INVOKABLE int getBeaconInterval();
    Q_INVOKABLE int getBeaconPin();
    Q_INVOKABLE void startScanning();
    Q_INVOKABLE void readBeaconCharacteristics(QString mac);
    Q_INVOKABLE bool isBeaconCharacteristicsAvailable();
    void emitOpenConfig(QString mac);

private slots:
    void onSystemLanguageChanged();
    void initBluetooth();
    void deinitBluetooth();
    void prepForAdvertisements();

public slots:
    void onStartListening();
    void onStopListening();

signals:
    void message(const QVariant &text);
    void stateChanged();
    void scanStateChanged();
    void openConfig(QVariant mac);

private:
    bb::cascades::DataModel* model() const;
    bb::cascades::DataModel* configModel() const;

    QTranslator* _pTranslator;
    bb::cascades::LocaleHandler* _pLocaleHandler;
    QmlDocument *_qml;
    QObject *_mainPage;
    GroupDataModel *_model;
    GroupDataModel *_config_model;

    static const int STATE_STOPPED = 1;
    static const int STATE_STARTED = 2;
    int _state;
    int _scan_state;
    const QString _orgName;
    const QString _appName;
    bool _bt_initialised;
    bt_remote_device_t *_selected_device;

    int state();
    int scanState();
    void updateSensorTag(QString mac, QString uuid, int major, int minor, int power, int interval);

};

#endif /* ApplicationUI_HPP_ */
