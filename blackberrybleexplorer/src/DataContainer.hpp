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
#include <bb/cascades/GroupDataModel>

class DataContainer: public QObject {
	Q_OBJECT

private:
	DataContainer();
	static DataContainer* _instance;
	DataContainer* _dc;
	unsigned long getSysTimeMs();
	static QString KEY_DEVICE_NAME;
	static QString KEY_DEVICE_ADDR;

	bool known;
	bool paired;

public:
	static DataContainer* getInstance();
	int _device_count;
	QString _current_device_name;
	QString _current_device_addr;
	bt_remote_device_t* _current_device;
	QMap<QString, QVariant> _device;
	QList<QMap<QString, QVariant> > _list_of_devices;
    void addDevice(char* device_name, char* device_addr, int device_class, int device_type, bool paired, bool encrypted, bool known);
	QList<QVariantList> getDeviceIdList();
	QList<QVariantMap> getDeviceList();

	bb::cascades::DataModel* model() const;
    bb::cascades::GroupDataModel* _model;

	QString intToHex(int decimal);

// device data
	void clearDeviceList();
	Q_INVOKABLE	QString getDeviceName(int device_inx);
	Q_INVOKABLE	QString getDeviceAddr(int device_inx);
    Q_INVOKABLE int getDeviceCount();

	bt_remote_device_t* getCurrentDevice();
	void setCurrentDevice(bt_remote_device_t* device);
	void setDeviceCount(int dc);
	void setCurentDeviceInx(int inx);
	void setKnown(int device_inx, bool known);
	void setPaired(int device_inx, bool paired);
	Q_INVOKABLE bool isKnown(int device_inx);
	Q_INVOKABLE bool isPaired(int device_inx);

};

#endif // ifndef DATACONTAINER
