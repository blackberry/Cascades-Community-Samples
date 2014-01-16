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
#include "Utilities.hpp"
#include <QDebug>
#include <QSettings>
#include <float.h>

DataContainer* DataContainer::_instance;
QString DataContainer::KEY_DEVICE_NAME = "device_name";
QString DataContainer::KEY_DEVICE_ADDR = "device_addr";
QString DataContainer::KEY_MIN_RSSI_NEAR = "min_rssi_near";
QString DataContainer::KEY_MIN_RSSI_MEDIUM = "min_rssi_medium";
QString DataContainer::KEY_RSSI_FLUCTUATION_SENSITIVITY = "rssi_fluctuation_sensitivity";

// FLAGS field bit mask values

DataContainer::DataContainer()
{
    QSettings settings;
    _current_device_name = settings.value(KEY_DEVICE_NAME, "").toString();
    _current_device_addr = settings.value(KEY_DEVICE_ADDR, "").toString();
    bool ok;
    _min_rssi_near = settings.value(KEY_MIN_RSSI_NEAR, "-70").toInt(&ok);
    _min_rssi_medium = settings.value(KEY_MIN_RSSI_MEDIUM, "-85").toInt(&ok);
    _rssi_fluctuation_sensitivity = settings.value(KEY_RSSI_FLUCTUATION_SENSITIVITY, "5").toInt(&ok);
    _immediate_alert_level_handle = 0;
    _immediate_alert_level_value_handle = 0;
    _alert_service_instance = 0;
    _link_loss_alert_level_handle = 0;
    _link_loss_alert_level_value_handle = 0;
    _link_loss_service_instance = 0;
    _tx_power_level_dbm = 0;
    _rssi = 0;
    _proximity_indicator = -1;
}

DataContainer* DataContainer::getInstance()
{
    if (_instance == 0) {
        _instance = new DataContainer;
    }
    return _instance;
}

void DataContainer::addProximityDevice(char* device_name, char* device_addr, int device_class, int device_type, bool paired, bool encrypted)
{

    _proximity_device["device_name"] = device_name;
    _proximity_device["device_addr"] = device_addr;
    _proximity_device["device_class"] = device_class;
    _proximity_device["device_type"] = device_type;
    _proximity_device["paired"] = paired;
    _proximity_device["encrypted"] = encrypted;
    _list_of_proximity_devices.append(_proximity_device);

}

void DataContainer::clearDeviceList()
{
    _list_of_proximity_devices.clear();
}

QList<QVariantList> DataContainer::getDeviceIdList()
{

    QList<QVariantList> list_of_proximity_devices;

    QListIterator<QMap<QString, QVariant> > i(_list_of_proximity_devices);

    while (i.hasNext()) {
        QMap<QString, QVariant> device_details = i.next();
        QString device_name = device_details.value("device_name").toString();
        QString device_addr = device_details.value("device_addr").toString();
        qDebug() << QString("YYYY device_name:%1 device_addr:%2").arg(device_name).arg(device_addr);
        QList<QVariant> device_info;
        device_info.append(device_details.value("device_name"));
        device_info.append(device_details.value("device_addr"));
        list_of_proximity_devices.append(device_info);
    }
    return list_of_proximity_devices;
}

QList<QVariantMap> DataContainer::getDeviceList()
{

    QList<QVariantMap> list_of_proximity_devices;
    QListIterator<QMap<QString, QVariant> > i(_list_of_proximity_devices);

    while (i.hasNext()) {
        QMap<QString, QVariant> device_details = i.next();
        QString device_name = device_details.value("device_name").toString();
        QString device_addr = device_details.value("device_addr").toString();
        QVariantMap aDevice;
        aDevice["device_name"] = device_name;
        aDevice["device_addr"] = device_addr;
        list_of_proximity_devices.append(aDevice);
    }
    return list_of_proximity_devices;
}

QString DataContainer::getCurrentDeviceName()
{
    return _current_device_name;
}

QString DataContainer::getCurrentDeviceAddr()
{
    return _current_device_addr;
}

void DataContainer::setCurrentDeviceName(QString device_name)
{
    _current_device_name = device_name;
    QSettings settings;
    settings.setValue(KEY_DEVICE_NAME, device_name);
}

void DataContainer::setCurrentDeviceAddr(QString device_addr)
{
    _current_device_addr = device_addr;
    QSettings settings;
    settings.setValue(KEY_DEVICE_ADDR, device_addr);
}


bool DataContainer::isDeviceConnected() {
    return _device_connected;
}

void DataContainer::setDeviceConnected(bool state) {
    _device_connected = state;
}

uint16_t DataContainer::getImmediateAlertLevelHandle()
{
    return _immediate_alert_level_handle;
}

void DataContainer::setImmediateAlertLevelHandle(uint16_t handle)
{
    _immediate_alert_level_handle = handle;
}

uint16_t DataContainer::getImmediateAlertLevelValueHandle()
{
    return _immediate_alert_level_value_handle;
}

void DataContainer::setImmediateAlertLevelValueHandle(uint16_t handle)
{
    _immediate_alert_level_value_handle = handle;
}

uint16_t DataContainer::getLinkLossAlertLevelHandle()
{
    return _link_loss_alert_level_handle;
}

void DataContainer::setLinkLossAlertLevelHandle(uint16_t handle)
{
    _link_loss_alert_level_handle = handle;
}

uint16_t DataContainer::getLinkLossAlertLevelValueHandle()
{
    return _link_loss_alert_level_value_handle;
}

void DataContainer::setLinkLossAlertLevelValueHandle(uint16_t handle)
{
    _link_loss_alert_level_value_handle = handle;
}


uint16_t DataContainer::getTxPowerLevelHandle()
{
    return _tx_power_level_handle;
}

void DataContainer::setTxPowerLevelHandle(uint16_t handle)
{
    _tx_power_level_handle = handle;
}

uint16_t DataContainer::getTxPowerLevelValueHandle()
{
    return _tx_power_level_value_handle;
}

void DataContainer::setTxPowerLevelValueHandle(uint16_t handle)
{
    _tx_power_level_value_handle = handle;
}





QString DataContainer::intToHex(int decimal)
{
    QString hexadecimal;
    hexadecimal.setNum(decimal, 16);
    return hexadecimal;
}

void DataContainer::setAlertServiceInstance(int instance)
{
    _alert_service_instance = instance;
}

int DataContainer::getAlertServiceInstance()
{
    return _alert_service_instance;
}

void DataContainer::setLinkLossServiceInstance(int instance)
{
    _link_loss_service_instance = instance;
}

int DataContainer::getLinkLossServiceInstance()
{
    return _link_loss_service_instance;
}

void DataContainer::setTxPowerServiceInstance(int instance)
{
    _tx_power_service_instance = instance;
}

int DataContainer::getTxPowerServiceInstance()
{
    return _tx_power_service_instance;
}

void DataContainer::setTxPowerLevel(int level) {
    _tx_power_level_dbm = level;
}

int DataContainer::getTxPowerLevel() {
    return _tx_power_level_dbm;
}

void DataContainer::setRssi(int level) {
    _rssi = level;
}

int DataContainer::getRssi() {
    return _rssi;
}

void DataContainer::setProximityIndicator(int proximity_indicator) {
    _proximity_indicator = proximity_indicator;
}

int DataContainer::getProximityIndicator() {
    return _proximity_indicator;
}

bt_remote_device_t* DataContainer::getCurrentDevice() {
    return _current_device;
}

void DataContainer::setCurrentDevice(bt_remote_device_t* device) {
    _current_device = device;
}

void DataContainer::setMinRssiNear(int rssi_threshold) {
    _min_rssi_near = rssi_threshold;
    QSettings settings;
    settings.setValue(KEY_MIN_RSSI_NEAR, rssi_threshold);
}

int DataContainer::getMinRssiNear() {
    return _min_rssi_near;
}

void DataContainer::setMinRssiMedium(int rssi_threshold) {
    _min_rssi_medium = rssi_threshold;
    QSettings settings;
    settings.setValue(KEY_MIN_RSSI_MEDIUM, rssi_threshold);
}

int DataContainer::getMinRssiMedium() {
    return _min_rssi_medium;
}

void DataContainer::setRssiFluctuationSensitivity(int sensitivity) {
    _rssi_fluctuation_sensitivity = sensitivity;
    QSettings settings;
    settings.setValue(KEY_RSSI_FLUCTUATION_SENSITIVITY, sensitivity);
}

int DataContainer::getRssiFluctuationSensitivity() {
    return _rssi_fluctuation_sensitivity;
}
