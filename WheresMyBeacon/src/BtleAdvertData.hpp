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

#ifndef BTLEADVERTDATA_HPP_
#define BTLEADVERTDATA_HPP_

#include <QObject>
#include <QDebug>
#include <stdint.h>

class BtleAdvertData: public QObject {

	Q_OBJECT

public:
	BtleAdvertData(QObject *obj = 0);
	virtual ~BtleAdvertData();

	bool parse(const QByteArray &advertData);
	bool hasBeaconData();
    bool hasIBeaconData();
    bool hasAltBeaconData();
    bool hasEddystoneBeaconData();
    bool hasEddystoneBeaconUIDdata();
    bool hasEddystoneBeaconURLdata();
    bool hasEddystoneBeaconTLMdata();
	QByteArray & beaconUuid();
    QByteArray & beaconId();
	QString beaconUuidAsString();
    QString beaconIdAsString();
	int beaconMajor();
	int beaconMinor();
	int calibratedStrength();
	void clear();
	int companyCode();
	int altBeaconReserved();
    QString companyCodeAsString();

    int edTxPower();
    QByteArray & edNamespace();
    QString edNamespaceAsString();
    QByteArray & edInstanceId();
    QString edInstanceIdAsString();
    QString edUrlAsString();
    int edTlmVersion();
    int edTlmBatteryVoltage();
    float edTlmBatteryVoltageAsFloat();
    int edTlmBeaconTemp();
    float edTlmBeaconTempAsFloat();
    int edTlmPduCount();
    int edTlmTimeSinceReboot();

private:
	/**
	 * iBeacon attributes
	 */
	QByteArray _advertData;
	QByteArray _beaconUuid;
	uint16_t _beaconMajor;
	uint16_t _beaconMinor;
	int8_t _calibratedStrength;
	bool _hasIBeaconData;
    /**
     * AltBeacon attributes
     */
    bool _hasAltBeaconData;
    uint16_t _beaconMfgCompanyCode;
    QByteArray _beaconId;
    uint8_t _beaconReserved;
    /**
     * Eddystone common attributes
     */
    int8_t _edTxPower;
    /**
     * Eddystone UID attributes
     */
    bool _hasEddystoneBeaconUIDdata;
    QByteArray _edNamespace;
    QByteArray _edInstanceId;
    /**
     * Eddystone URL attributes
     */
    bool _hasEddystoneBeaconURLdata;
    uint8_t _edUrlScheme;
    QByteArray _edUrlBody;
    /**
     * Eddystone TLM attributes
     */
    bool _hasEddystoneBeaconTLMdata;
    uint8_t _edTlmVersion;
    uint16_t _edTlmBatteryVoltage;
    int16_t _edTlmBeaconTemp;
    uint32_t _edTlmPduCount;
    uint32_t _edTlmTimeSinceReboot;
};

#endif /* BTLEADVERTDATA_HPP_ */
