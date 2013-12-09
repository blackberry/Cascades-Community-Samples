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

#include "DataContainer.hpp"
#include <QDebug>
#include <QSettings>
#include <float.h>

DataContainer* DataContainer::_instance;

DataContainer::DataContainer()
{
    QSettings settings;
    bool ok;
}

DataContainer* DataContainer::getInstance()
{
    if (_instance == 0) {
        _instance = new DataContainer;
    }
    return _instance;
}

void DataContainer::addDevice(char* device_name, char* device_addr, int device_class, int device_type, bool paired, bool encrypted, bool known)
{

    _device["device_name"] = device_name;
    _device["device_addr"] = device_addr;
    _device["device_class"] = device_class;
    _device["device_type"] = device_type;
    _device["paired"] = paired;
    _device["encrypted"] = encrypted;
    _device["known"] = known;
    _list_of_devices.append(_device);

}

void DataContainer::clearDeviceList()
{
    _device_count = 0;
    _list_of_devices.clear();
}

QList<QVariantList> DataContainer::getDeviceIdList()
{

    QList<QVariantList> list_of_devices;

    QListIterator<QMap<QString, QVariant> > i(_list_of_devices);

    while (i.hasNext()) {
        QMap<QString, QVariant> device_details = i.next();
        QString device_name = device_details.value("device_name").toString();
        QString device_addr = device_details.value("device_addr").toString();
        qDebug() << QString("YYYY device_name:%1 device_addr:%2").arg(device_name).arg(device_addr);
        QList<QVariant> device_info;
        device_info.append(device_details.value("device_name"));
        device_info.append(device_details.value("device_addr"));
        list_of_devices.append(device_info);
    }
    return list_of_devices;
}

QList<QVariantMap> DataContainer::getDeviceList()
{

    QList<QVariantMap> list_of_devices;
    QListIterator<QMap<QString, QVariant> > i(_list_of_devices);

    while (i.hasNext()) {
        QMap<QString, QVariant> device_details = i.next();
        QString device_name = device_details.value("device_name").toString();
        QString device_addr = device_details.value("device_addr").toString();
        QVariantMap aDevice;
        aDevice["device_name"] = device_name;
        aDevice["device_addr"] = device_addr;
        list_of_devices.append(aDevice);
    }
    return list_of_devices;
}

QString DataContainer::intToHex(int decimal)
{
    QString hexadecimal;
    hexadecimal.setNum(decimal, 16);
    return hexadecimal;
}

void DataContainer::setDeviceCount(int dc)
{
    qDebug() << "XXXX found " << dc << " devices";
    _device_count = dc;
}


QString DataContainer::getDeviceName(int device_inx) {
    if (_device_count > 0 && device_inx >=0 && device_inx < _device_count) {
        QMap<QString, QVariant> device_details = _list_of_devices.at(device_inx);
        QVariant device_name = device_details.value("device_name");
        return device_name.toString();
    } else {
        return QString("");
    }
}

QString DataContainer::getDeviceAddr(int device_inx) {
    if (_device_count > 0 && device_inx >=0 && device_inx < _device_count) {
        QMap<QString, QVariant> device_details = _list_of_devices.at(device_inx);
        QVariant device_addr = device_details.value("device_addr");
        return device_addr.toString();
    } else {
        return QString("");
    }
}


int DataContainer::getDeviceCount() {
    return _device_count;
}

void DataContainer::setKnown(int device_inx,bool known) {
    if (_device_count > 0 && device_inx >=0 && device_inx < _device_count) {
        QMap<QString, QVariant> device_details = _list_of_devices.at(device_inx);
        device_details["known"] = known;
        _list_of_devices.replace(device_inx,device_details);
        qDebug() << "XXXX changed known state of device " << device_inx << " to " << known;
    } else {
        qDebug() << "XXXX could not establish device to set 'known' state for";
    }
}

void DataContainer::setPaired(int device_inx,bool paired) {
    if (_device_count > 0 && device_inx >=0 && device_inx < _device_count) {
        QMap<QString, QVariant> device_details = _list_of_devices.at(device_inx);
        device_details["paired"] = paired;
        _list_of_devices.replace(device_inx,device_details);
        qDebug() << "XXXX changed paired state of device " << device_inx << " to " << paired;
    } else {
        qDebug() << "XXXX could not establish device to set 'paired' state for";
    }
}

bool DataContainer::isKnown(int device_inx) {
    if (_device_count > 0 && device_inx >=0 && device_inx < _device_count) {
        QMap<QString, QVariant> device_details = _list_of_devices.at(device_inx);
        QVariant addr = device_details.value("device_addr");
        QVariant known = device_details.value("known");
        return known.toBool();
    } else {
        qDebug() << "XXXX could not establish 'known' state";
        return false;
    }
}

bool DataContainer::isPaired(int device_inx) {
    if (_device_count > 0 && device_inx >=0 && device_inx < _device_count) {
        QMap<QString, QVariant> device_details = _list_of_devices.at(device_inx);
        QVariant known = device_details.value("paired");
        return known.toBool();
    } else {
        qDebug() << "XXXX could not establish 'paired' state";
        return false;
    }
}
