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

DataContainer::DataContainer()
{
    QSettings settings;
    _current_device_name = settings.value(KEY_DEVICE_NAME, "").toString();
    _current_device_addr = settings.value(KEY_DEVICE_ADDR, "").toString();
    bool ok;
    _device_information_service_instance = 0;
    _manufacturer_name_handle = 0;
    _manufacturer_name_value_handle = 0;
    _model_number_handle = 0;
    _model_number_value_handle = 0;
    _serial_number_handle = 0;
    _serial_number_value_handle = 0;
    _hardware_revision_handle = 0;
    _hardware_revision_value_handle = 0;
    _firmware_revision_handle = 0;
    _firmware_revision_value_handle = 0;
    _software_revision_handle = 0;
    _software_revision_value_handle = 0;
    _system_id_handle = 0;
    _system_id_value_handle = 0;
    _certification_data_handle = 0;
    _certification_data_value_handle = 0;
    _pnp_id_handle = 0;
    _pnp_id_value_handle = 0;
    _org_unique_identifier = 0;

}

DataContainer* DataContainer::getInstance()
{
    if (_instance == 0) {
        _instance = new DataContainer;
    }
    return _instance;
}


void DataContainer::addDevice(char* device_name, char* device_addr, int device_class, int device_type, bool paired, bool encrypted)
{

    _device["device_name"] = device_name;
    _device["device_addr"] = device_addr;
    _device["device_class"] = device_class;
    _device["device_type"] = device_type;
    _device["paired"] = paired;
    _device["encrypted"] = encrypted;
    _list_of_devices.append(_device);

}

void DataContainer::clearDeviceList()
{
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


QString DataContainer::intToHex(int decimal)
{
    QString hexadecimal;
    hexadecimal.setNum(decimal, 16);
    return hexadecimal;
}

QByteArray DataContainer::getCertificationData() {
    return _certification_data;
}

void DataContainer::setCertificationData(const QByteArray certificationData) {
    _certification_data = certificationData;
}

uint16_t DataContainer::getCertificationDataHandle() {
    return _certification_data_handle;
}

void DataContainer::setCertificationDataHandle(uint16_t certificationDataHandle) {
    _certification_data_handle = certificationDataHandle;
}

uint16_t DataContainer::getCertificationDataValueHandle() {
    return _certification_data_value_handle;
}

void DataContainer::setCertificationDataValueHandle(uint16_t certificationDataValueHandle) {
    _certification_data_value_handle = certificationDataValueHandle;
}

int DataContainer::getDeviceInformationServiceInstance() {
    return _device_information_service_instance;
}

void DataContainer::setDeviceInformationServiceInstance(int deviceInformationServiceInstance) {
    _device_information_service_instance = deviceInformationServiceInstance;
}

const QString DataContainer::getFirmwareRevision() {
    return _firmware_revision;
}

void DataContainer::setFirmwareRevision(const QString firmwareRevision) {
    _firmware_revision = firmwareRevision;
}

uint16_t DataContainer::getFirmwareRevisionHandle() {
    return _firmware_revision_handle;
}

void DataContainer::setFirmwareRevisionHandle(uint16_t firmwareRevisionHandle) {
    _firmware_revision_handle = firmwareRevisionHandle;
}

uint16_t DataContainer::getFirmwareRevisionValueHandle() {
    return _firmware_revision_value_handle;
}

void DataContainer::setFirmwareRevisionValueHandle(uint16_t firmwareRevisionValueHandle) {
    _firmware_revision_value_handle = firmwareRevisionValueHandle;
}

const QString DataContainer::getHardwareRevision() {
    return _hardware_revision;
}

void DataContainer::setHardwareRevision(const QString hardwareRevision) {
    _hardware_revision = hardwareRevision;
}

uint16_t DataContainer::getHardwareRevisionHandle() {
    return _hardware_revision_handle;
}

void DataContainer::setHardwareRevisionHandle(uint16_t hardwareRevisionHandle) {
    _hardware_revision_handle = hardwareRevisionHandle;
}

uint16_t DataContainer::getHardwareRevisionValueHandle() {
    return _hardware_revision_value_handle;

}

void DataContainer::setHardwareRevisionValueHandle(uint16_t hardwareRevisionValueHandle) {
    _hardware_revision_value_handle = hardwareRevisionValueHandle;
}

const QString DataContainer::getManufacturerName() {
    return _manufacturer_name;
}

void DataContainer::setManufacturerName(const QString manufacturerName) {
    _manufacturer_name = manufacturerName;
}

uint16_t DataContainer::getManufacturerNameHandle() {
    return _manufacturer_name_handle;
}

void DataContainer::setManufacturerNameHandle(uint16_t manufacturerNameHandle) {
    _manufacturer_name_handle = manufacturerNameHandle;
}

uint16_t DataContainer::getManufacturerNameValueHandle() {
    return _manufacturer_name_value_handle;
}

void DataContainer::setManufacturerNameValueHandle(uint16_t manufacturerNameValueHandle) {
    _manufacturer_name_value_handle = manufacturerNameValueHandle;
}

const QString DataContainer::getModelNumber() {
    return _model_number;
}

void DataContainer::setModelNumber(const QString modelNumber) {
    _model_number = modelNumber;
}

uint16_t DataContainer::getModelNumberHandle() {
    return _model_number_handle;
}

void DataContainer::setModelNumberHandle(uint16_t modelNumberHandle) {
    _model_number_handle = modelNumberHandle;
}

uint16_t DataContainer::getModelNumberValueHandle() {
    return _model_number_value_handle;
}

void DataContainer::setModelNumberValueHandle(uint16_t modelNumberValueHandle) {
    _model_number_value_handle = modelNumberValueHandle;
}

const QByteArray DataContainer::getPnpId() {
    return _pnp_id;
}

void DataContainer::setPnpId(const QByteArray pnpId) {
    _pnp_id = pnpId;
}

uint16_t DataContainer::getPnpIdHandle() {
    return _pnp_id_handle;
}

void DataContainer::setPnpIdHandle(uint16_t pnpIdHandle) {
    _pnp_id_handle = pnpIdHandle;
}

uint16_t DataContainer::getPnpIdValueHandle() {
    return _pnp_id_value_handle;
}

void DataContainer::setPnpIdValueHandle(uint16_t pnpIdValueHandle) {
    _pnp_id_value_handle = pnpIdValueHandle;
}

const QString DataContainer::getSerialNumber() {
    return _serial_number;
}

void DataContainer::setSerialNumber(const QString serialNumber) {
    _serial_number = serialNumber;
}

uint16_t DataContainer::getSerialNumberHandle() {
    return _serial_number_handle;
}

void DataContainer::setSerialNumberHandle(uint16_t serialNumberHandle) {
    _serial_number_handle = serialNumberHandle;
}

uint16_t DataContainer::getSerialNumberValueHandle() {
    return _serial_number_value_handle;
}

void DataContainer::setSerialNumberValueHandle(uint16_t serialNumberValueHandle) {
    _serial_number_value_handle = serialNumberValueHandle;
}

const QString DataContainer::getSoftwareRevision() {
    return _software_revision;
}

void DataContainer::setSoftwareRevision(QString softwareRevision) {
    _software_revision = softwareRevision;
}

uint16_t DataContainer::getSoftwareRevisionHandle() {
    return _software_revision_handle;
}

void DataContainer::setSoftwareRevisionHandle(uint16_t softwareRevisionHandle) {
    _software_revision_handle = softwareRevisionHandle;
}

uint16_t DataContainer::getSoftwareRevisionValueHandle() {
    return _software_revision_value_handle;
}

void DataContainer::setSoftwareRevisionValueHandle(uint16_t softwareRevisionValueHandle) {
    _software_revision_value_handle = softwareRevisionValueHandle;
}

uint16_t DataContainer::getSystemIdHandle() {
    return _system_id_handle;
}

void DataContainer::setSystemIdHandle(uint16_t systemIdHandle) {
    _system_id_handle = systemIdHandle;
}

uint16_t DataContainer::getSystemIdValueHandle() {
    return _system_id_value_handle;
}

void DataContainer::setSystemIdValueHandle(uint16_t systemIdValueHandle) {
    _system_id_value_handle = systemIdValueHandle;
}

uint64_t DataContainer::getManufacturerIdentifier() {
    return _manufacturer_identifier;
}

void DataContainer::setManufacturerIdentifier(const uint64_t manufacturer_identifier) {
    _manufacturer_identifier = manufacturer_identifier;
}

void DataContainer::setOrgUniqueIdentifier(const uint32_t org_unique_identifier) {
    _org_unique_identifier = org_unique_identifier;
}

uint32_t DataContainer::getOrgUniqueIdentifier() {
    return _org_unique_identifier;
}

