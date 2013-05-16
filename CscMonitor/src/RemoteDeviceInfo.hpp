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

#ifndef REMOTEDEVICEINFO_HPP
#define REMOTEDEVICEINFO_HPP

#include <QObject>

#include <bb/cascades/GroupDataModel>

class RemoteDeviceInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::DataModel* model READ model CONSTANT)
    Q_PROPERTY(QString name READ name NOTIFY changed)
    Q_PROPERTY(QString address READ address NOTIFY changed)
    Q_PROPERTY(QString deviceClass READ deviceClass NOTIFY changed)
    Q_PROPERTY(QString deviceType READ deviceType NOTIFY changed)
    Q_PROPERTY(QString encrypted READ encrypted NOTIFY changed)
    Q_PROPERTY(QString paired READ paired NOTIFY changed)
    Q_PROPERTY(QString trusted READ trusted NOTIFY changed)
    Q_PROPERTY(QString rssi READ rssi NOTIFY changed)
    Q_PROPERTY(QString minimumConnectionInterval READ minimumConnectionInterval NOTIFY changed)
    Q_PROPERTY(QString maximumConnectionInterval READ maximumConnectionInterval NOTIFY changed)
    Q_PROPERTY(QString latency READ latency NOTIFY changed)
    Q_PROPERTY(QString supervisoryTimeout READ supervisoryTimeout NOTIFY changed)
    Q_PROPERTY(QString appearance READ appearance NOTIFY changed)
    Q_PROPERTY(QString flags READ flags NOTIFY changed)
    Q_PROPERTY(QString connectable READ connectable NOTIFY changed)
    Q_PROPERTY(int deviceClassInt READ deviceClassInt NOTIFY changed)
    Q_PROPERTY(int deviceTypeInt READ deviceTypeInt NOTIFY changed)
    Q_PROPERTY(bool encryptedBool READ encryptedBool NOTIFY changed)
    Q_PROPERTY(bool pairedBool READ pairedBool NOTIFY changed)

public:
    RemoteDeviceInfo(QObject *parent = 0);
    void update(const QString &deviceAddress);
    void reset();

Q_SIGNALS:
    void changed();

private:
    bb::cascades::DataModel* model() const;

    QString name() const;
    QString address() const;
    QString deviceClass() const;
    QString deviceType() const;
    QString encrypted() const;
    QString paired() const;
    QString trusted() const;
    QString rssi() const;
    QString minimumConnectionInterval() const;
    QString maximumConnectionInterval() const;
    QString latency() const;
    QString supervisoryTimeout() const;
    QString appearance() const;
    QString flags() const;
    QString connectable() const;

    int deviceClassInt() const;
    int deviceTypeInt() const;
    bool encryptedBool() const;
    bool pairedBool() const;

    bb::cascades::GroupDataModel* _model;
    QString _name;
    QString _address;
    QString _deviceClass;
    QString _deviceType;
    QString _encrypted;
    QString _paired;
    QString _trusted;
    QString _rssi;
    QString _minimumConnectionInterval;
    QString _maximumConnectionInterval;
    QString _latency;
    QString _supervisoryTimeout;
    QString _appearance;
    QString _flags;
    QString _connectable;

    int _deviceClassInt;
    int _deviceTypeInt;
    bool _encryptedBool;
    bool _pairedBool;

    QString serviceDescription(const QString &uuid);
};

#endif
