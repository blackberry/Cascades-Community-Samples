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
#ifndef DATACONTAINER
#define DATACONTAINER
#include <QMap>
#include <QQueue>
#include <QObject>
#include <QVariant>
#include <QString>
#include <QByteArray>
#include <btapi/btgatt.h>
#include <btapi/btdevice.h>

/**
 * Characteristics which may be available:
 *
 * Name: Manufacturer Name String
 * Type: org.bluetooth.characteristic.manufacturer_name_string
 * Assigned Number: 0x2A29
 *
 * Name: Model Number String
 * Type: org.bluetooth.characteristic.model_number_string
 * Assigned Number: 0x2A24
 *
 * Name: Serial Number String
 * Type: org.bluetooth.characteristic.serial_number_string
 * Assigned Number: 0x2A25
 *
 * Name: Hardware Revision String
 * Type: org.bluetooth.characteristic.hardware_revision_string
 * Assigned Number: 0x2A27
 *
 * Name: Firmware Revision String
 * Type: org.bluetooth.characteristic.firmware_revision_string
 * Assigned Number: 0x2A26
 *
 * Name: Software Revision String
 * Type: org.bluetooth.characteristic.software_revision_string
 * Assigned Number: 0x2A28
 *
 * Name: System ID
 * Type: org.bluetooth.characteristic.system_id
 * Assigned Number: 0x2A23
 *
 * Name: IEEE 11073-20601 Regulatory Certification Data List
 * Type: org.bluetooth.characteristic.ieee_11073-20601_regulatory_certification_data_list
 * Assigned Number: 0x2A2A
 *
 * Name: PnP ID
 * Type: org.bluetooth.characteristic.pnp_id
 * Assigned Number: 0x2A50
 *
 */

class DataContainer: public QObject {
	Q_OBJECT

private:
	DataContainer();
	static DataContainer* _instance;
	DataContainer* _dc;
	unsigned long getSysTimeMs();
	static QString KEY_DEVICE_NAME;
	static QString KEY_DEVICE_ADDR;

public:
	static DataContainer* getInstance();
	QString _current_device_name;
	QString _current_device_addr;
	bt_remote_device_t* _current_device;
	bool _device_connected;
    int _device_information_service_instance;
	uint16_t _manufacturer_name_handle;
	uint16_t _manufacturer_name_value_handle;
    uint16_t _model_number_handle;
    uint16_t _model_number_value_handle;
    uint16_t _serial_number_handle;
    uint16_t _serial_number_value_handle;
    uint16_t _hardware_revision_handle;
    uint16_t _hardware_revision_value_handle;
    uint16_t _firmware_revision_handle;
    uint16_t _firmware_revision_value_handle;
    uint16_t _software_revision_handle;
    uint16_t _software_revision_value_handle;
    uint16_t _system_id_handle;
    uint16_t _system_id_value_handle;
    uint16_t _certification_data_handle;
    uint16_t _certification_data_value_handle;
    uint16_t _pnp_id_handle;
    uint16_t _pnp_id_value_handle;
	QMap<QString, QVariant> _device;
	QList<QMap<QString, QVariant> > _list_of_devices;
	void addDevice(char* device_name, char* device_addr, int device_class, int device_type, bool paired, bool encrypted);
	QList<QVariantList> getDeviceIdList();
	QList<QVariantMap> getDeviceList();
	QString _manufacturer_name;
	QString _model_number;
	QString _serial_number;
	QString _hardware_revision;
    QString _firmware_revision;
    QString _software_revision;
    uint32_t _org_unique_identifier;
    uint64_t _manufacturer_identifier;
    QByteArray _certification_data;
    QByteArray _pnp_id;

	bool isDeviceConnected();
	void setDeviceConnected(bool state);


	QString intToHex(int decimal);

// device data
	void clearDeviceList();
	Q_INVOKABLE	QString getCurrentDeviceName();
	Q_INVOKABLE	QString getCurrentDeviceAddr();
	Q_INVOKABLE	void setCurrentDeviceName(QString device_name);
	Q_INVOKABLE	void setCurrentDeviceAddr(QString device_addr);

	bt_remote_device_t* getCurrentDevice();
	void setCurrentDevice(bt_remote_device_t* device);
    QByteArray getCertificationData();
    void setCertificationData(const QByteArray certificationData);
    uint16_t getCertificationDataHandle();
    void setCertificationDataHandle(uint16_t certificationDataHandle);
    uint16_t getCertificationDataValueHandle();
    void setCertificationDataValueHandle(uint16_t certificationDataValueHandle);
    int getDeviceInformationServiceInstance();
    void setDeviceInformationServiceInstance(int deviceInformationServiceInstance);
    const QString getFirmwareRevision();
    void setFirmwareRevision(const QString firmwareRevision);
    uint16_t getFirmwareRevisionHandle();
    void setFirmwareRevisionHandle(uint16_t firmwareRevisionHandle);
    uint16_t getFirmwareRevisionValueHandle();
    void setFirmwareRevisionValueHandle(uint16_t firmwareRevisionValueHandle);
    const QString getHardwareRevision();
    void setHardwareRevision(const QString hardwareRevision);
    uint16_t getHardwareRevisionHandle();
    void setHardwareRevisionHandle(uint16_t hardwareRevisionHandle);
    uint16_t getHardwareRevisionValueHandle();
    void setHardwareRevisionValueHandle(uint16_t hardwareRevisionValueHandle);
    const QString getManufacturerName();
    void setManufacturerName(const QString manufacturerName);
    uint16_t getManufacturerNameHandle();
    void setManufacturerNameHandle(uint16_t manufacturerNameHandle);
    uint16_t getManufacturerNameValueHandle();
    void setManufacturerNameValueHandle(uint16_t manufacturerNameValueHandle);
    const QString getModelNumber();
    void setModelNumber(const QString modelNumber);
    uint16_t getModelNumberHandle();
    void setModelNumberHandle(uint16_t modelNumberHandle);
    uint16_t getModelNumberValueHandle();
    void setModelNumberValueHandle(uint16_t modelNumberValueHandle);
    uint16_t getPnpIdHandle();
    void setPnpIdHandle(uint16_t pnpIdHandle);
    uint16_t getPnpIdValueHandle();
    void setPnpIdValueHandle(uint16_t pnpIdValueHandle);
    const QByteArray getPnpId();
    void setPnpId(const QByteArray pnpId);
    const QString getSerialNumber();
    void setSerialNumber(const QString serialNumber);
    uint16_t getSerialNumberHandle();
    void setSerialNumberHandle(uint16_t serialNumberHandle);
    uint16_t getSerialNumberValueHandle();
    void setSerialNumberValueHandle(uint16_t serialNumberValueHandle);
    const QString getSoftwareRevision();
    void setSoftwareRevision(QString softwareRevision);
    uint16_t getSoftwareRevisionHandle();
    void setSoftwareRevisionHandle(uint16_t softwareRevisionHandle);
    uint16_t getSoftwareRevisionValueHandle();
    void setSoftwareRevisionValueHandle(uint16_t softwareRevisionValueHandle);
    uint64_t getManufacturerIdentifier();
    void setManufacturerIdentifier(const uint64_t manufacturer_identifier);
    uint32_t getOrgUniqueIdentifier();
    void setOrgUniqueIdentifier(const uint32_t organizationally_unique_identifier);
    uint16_t getSystemIdHandle();
    void setSystemIdHandle(uint16_t systemIdHandle);
    uint16_t getSystemIdValueHandle();
    void setSystemIdValueHandle(uint16_t systemIdValueHandle);
};

#endif // ifndef DATACONTAINER
