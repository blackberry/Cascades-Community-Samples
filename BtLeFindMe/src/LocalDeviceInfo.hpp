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

#ifndef LOCALDEVICEINFO_HPP
#define LOCALDEVICEINFO_HPP

#include <QObject>

class LocalDeviceInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY changed)
    Q_PROPERTY(QString address READ address NOTIFY changed)
    Q_PROPERTY(QString deviceClass READ deviceClass NOTIFY changed)
    Q_PROPERTY(QString discoverable READ discoverable NOTIFY changed)
    Q_PROPERTY(QString enabled READ enabled NOTIFY changed)
    Q_PROPERTY(QString deviceType READ deviceType NOTIFY changed)
    Q_PROPERTY(QString apiVersion READ apiVersion NOTIFY changed)
    Q_PROPERTY(QString masterSlaveAllowed READ masterSlaveAllowed NOTIFY changed)
    Q_PROPERTY(QString maximumConnections READ maximumConnections NOTIFY changed)
    Q_PROPERTY(QString maximumL2capMtu READ maximumL2capMtu NOTIFY changed)
    Q_PROPERTY(QString maximumRfcommMtu READ maximumRfcommMtu NOTIFY changed)
    Q_PROPERTY(QString incomingInquiryScan READ incomingInquiryScan NOTIFY changed)
    Q_PROPERTY(QString incomingPageScan READ incomingPageScan NOTIFY changed)
    Q_PROPERTY(QString outgoingInquiryScan READ outgoingInquiryScan NOTIFY changed)
    Q_PROPERTY(QString outgoingPageScan READ outgoingPageScan NOTIFY changed)

public:
    LocalDeviceInfo(QObject *parent = 0);
    void update();

Q_SIGNALS:
    void changed();

private:
    QString name() const;
    QString address() const;
    QString deviceClass() const;
    QString discoverable() const;
    QString enabled() const;
    QString deviceType() const;
    QString apiVersion() const;
    QString masterSlaveAllowed() const;
    QString maximumConnections() const;
    QString maximumL2capMtu() const;
    QString maximumRfcommMtu() const;
    QString incomingInquiryScan() const;
    QString incomingPageScan() const;
    QString outgoingInquiryScan() const;
    QString outgoingPageScan() const;

    QString _name;
    QString _address;
    QString _deviceClass;
    QString _discoverable;
    QString _enabled;
    QString _deviceType;
    QString _apiVersion;
    QString _masterSlaveAllowed;
    QString _maximumConnections;
    QString _maximumL2capMtu;
    QString _maximumRfcommMtu;
    QString _incomingInquiryScan;
    QString _incomingPageScan;
    QString _outgoingInquiryScan;
    QString _outgoingPageScan;
};

#endif
