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

#include "BtleAdvertData.hpp"

BtleAdvertData::BtleAdvertData(QObject *obj)
	: QObject(obj)
	, _advertData()
	, _beaconUuid()
	, _beaconMajor(0)
	, _beaconMinor(0)
	, _calibratedStrength(0)
	, _hasIBeaconData(false)
    , _hasAltBeaconData(false)
    , _beaconMfgCompanyCode(0)
    , _beaconId()
    , _beaconReserved(0)
    , _edTxPower(0)
    , _hasEddystoneBeaconUIDdata(false)
    , _edNamespace()
    , _edInstanceId()
    , _hasEddystoneBeaconURLdata(false)
    , _edUrlScheme(0)
    , _edUrlBody()
    , _hasEddystoneBeaconTLMdata(false)
    , _edTlmVersion(0)
    , _edTlmBatteryVoltage(0)
    , _edTlmBeaconTemp(0)
    , _edTlmPduCount(0)
    , _edTlmTimeSinceReboot(0)
{
}

BtleAdvertData::~BtleAdvertData()
{
}

/*
	Sample data (iBeacon)

	Estimote data = 02 01 05 1a ff 4c 00 02 15 b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d fa 46 b8 1a c2
	Sensor Tag               1A FF 4C 00 02 15 FA 5F 55 D9 BC 63 40 2E A2 54 09 1B 8F E8 C9 91 00 01 00 01 C5
	02 (AD Structure length)
	    01  (AD Type - flags)
	        05  (AD Data)
	1a  (AD Structure length)
	    ff  (AD Type - Manufacturer Specific Data)
	        4c 00 ( Apple Specific data  - little endian of 0x004c)
	        02 (iBeacon data follows)
	            15  ( 21 (dec) bytes)
	                b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d ( BeconRegion UUID - fixed field 16 bytes)
	                fa 46  (Major Number - fixed field 16 bit integer - (big endian 64070)
	                b8 1a  (Minor Number - fixed field 16 bit integer - (big endian 47130)
	                c2 (Calibrated Strength -- power in dBm at 1m from device)


    AltBeacon (RIM MANF code) data = 1b ff 3c 00 be ac 02 15 b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d fa 46 c2 00

    1b  (AD Structure length)
        ff  (AD Type - Manufacturer Specific Data)
            3c 00 ( RIM Specific data - little endian of 0x003c)
            be ac (Alt Beacon data follows big endian of 0xbeac)
               02 15 b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d fa 46 ( Beacon Id 20 bytes )
               c2 (Reference -- Calibrated Strength -- power in dBm at 1m from device)
               00 (Reserved)

    Eddystone UID example advertising frame

    02              length 2
        01          type ( Flags )
            06      flags data

    03              length 3
        03          type ( Complete list of 16-bit Service UUIDs data type value )
            aafe  Eddystone 16-bit uuid (FEAA)

    15              length 21
        16          Service Data Type
            aafe    Eddystone 16-bit uuid
            00                                      Eddystone UID frame (FEAA)
            ce                                      Tx Power at 1m
            edd1ebeac04e5defa017    10 byte Namespace
            ce5a65329a61                    6 byte instance Id

    Eddystone URL example advertising frame

    02              length 2
        01          type ( Flags )
            06      flags data

    03              length 3
        03          type ( Complete list of 16-bit Service UUIDs data type value )
            aafe  Eddystone 16-bit uuid (FEAA)

    11              length 17
        16          Service Data Type
            aafe    Eddystone 16-bit uuid (FEAA)
            10                                      Eddystone URL frame
            ce                                      Tx Power at 1m
            00                                      1 byte url scheme (http://www.)
            'blackberry'                    0-17 byte encode url ...
            00                                      suffis '.com/'

    Eddystone TLM example advertising frame

    02              length 2
        01          type ( Flags )
            06      flags data

    03              length 3
        03          type ( Complete list of 16-bit Service UUIDs data type value )
            aafe  Eddystone 16-bit uuid (FEAA)

    0f              length 15
        16          Service Data Type
            aafe    Eddystone 16-bit uuid (FEAA)
            20                                      Eddystone TLM frame
            00                                      Version (0x00)
            2010                                    Battery Voltage ( BE 0x2010 mV )
            01020304                            Advertising PDU Count ( BE 0x01020304 )
            01020304                            Time since p/o or reboot ( BE 0x01020304 )

*/

bool BtleAdvertData::parse(const QByteArray &advertData)
{

    qDebug() << "BBBB parse [" << advertData.toHex() << "]";

    _hasIBeaconData = false;
    _hasAltBeaconData = false;
    _hasEddystoneBeaconURLdata = false;
    _hasEddystoneBeaconUIDdata = false;
    _hasEddystoneBeaconTLMdata = false;

	int i = 0;
	do {
		uint8_t entryLen = advertData.at(i);
		uint8_t entryType = advertData.at(i+1);

		if (entryType == 0xff) {
			int j = i+2;
			if (((uint8_t)advertData.at(j) == 0x4c) && ((uint8_t)advertData.at(j+1) == 0x00) && ((uint8_t)advertData.at(j+2) == 0x02)&& ((uint8_t)advertData.at(j+3) == 0x15)) {
				_hasIBeaconData = true;
				_beaconUuid.clear();
				for (int k=0; k<16; k++) {
					_beaconUuid.append(advertData.at(k+j+4));
				}
				j += 20;
				_beaconMajor = 0;
				_beaconMajor  = (uint16_t)((uint8_t)advertData.at(j+1) & 0xff);
				_beaconMajor += (uint16_t)(((uint8_t)advertData.at(j) & 0xff) << 8);
				j += 2;
				_beaconMinor = 0;
				_beaconMinor  = (uint16_t)((uint8_t)advertData.at(j+1) & 0xff);
				_beaconMinor += (uint16_t)(((uint8_t)advertData.at(j) & 0xff) << 8);
				j += 2;
				_calibratedStrength = 0;
				_calibratedStrength = (int8_t) (advertData.at(j) & 0xff);

				return true;

			} else if (((uint8_t)advertData.at(j+2) == 0xbe)&& ((uint8_t)advertData.at(j+3) == 0xac)) {
                _hasAltBeaconData = true;
                _beaconMfgCompanyCode  = (uint16_t)((uint8_t)advertData.at(j) & 0xff);
                _beaconMfgCompanyCode += (uint16_t)(((uint8_t)advertData.at(j+1) & 0xff) << 8);
                _beaconId.clear();
                for (int k=0; k<20; k++) {
                    _beaconId.append(advertData.at(k+j+4));
                }
                j += 24;
                _calibratedStrength = 0;
                _calibratedStrength = (int8_t) (advertData.at(j) & 0xff);
                j += 1;
                _beaconReserved = (int8_t) (advertData.at(j) & 0xff);

                return true;
            }
		} else if (entryType == 0x16) {
            int j = i+2;
            if (((uint8_t)advertData.at(j) == 0xaa) && ((uint8_t)advertData.at(j+1) == 0xfe)) {
                j += 2;
                switch ((uint8_t)advertData.at(j)) {
                    case 0x00:
                        _hasEddystoneBeaconUIDdata = true;
                        j += 1;
                        _edTxPower = (int8_t) (advertData.at(j) & 0xff);
                        j += 1;
                        _edNamespace.clear();
                        for (int k=0; k<10; k++) {
                            _edNamespace.append(advertData.at(k+j));
                        }
                        j += 10;
                        _edInstanceId.clear();
                        for (int k=0; k<6; k++) {
                            _edInstanceId.append(advertData.at(k+j));
                        }
                        return true;
                        break;
                    case 0x10:
                        _hasEddystoneBeaconURLdata = true;
                         j += 1;

                         _edTxPower = (int8_t) (advertData.at(j) & 0xff);
                         j += 1;

                         _edUrlScheme = (int8_t) (advertData.at(j) & 0xff);
                         j += 1;

                         _edUrlBody.clear();
                         for (int k=0; k<(entryLen-(j-i)+1); k++) {
                             _edUrlBody.append(advertData.at(k+j));
                         }
                         return true;
                         break;
                    case 0x20:
                        _hasEddystoneBeaconTLMdata = true;
                        j += 1;

                        _edTlmVersion = (int8_t) (advertData.at(j) & 0xff);
                        j += 1;

                        _edTlmBatteryVoltage  = 0;
                        _edTlmBatteryVoltage  = (uint16_t)((uint8_t)advertData.at(j+1) & 0xff);
                        _edTlmBatteryVoltage += (uint16_t)(((uint8_t)advertData.at(j) & 0xff) << 8);
                        j += 2;

                        _edTlmBeaconTemp  = 0;
                        _edTlmBeaconTemp  = (int16_t)((uint8_t)advertData.at(j+1) & 0xff);
                        _edTlmBeaconTemp += (int16_t)(((uint8_t)advertData.at(j) & 0xff) << 8);
                        j += 2;

                        _edTlmPduCount  = 0;
                        _edTlmPduCount  = (uint32_t)((uint8_t)advertData.at(j+3) & 0xff);
                        _edTlmPduCount += (uint32_t)(((uint8_t)advertData.at(j+2) & 0xff) << 8);
                        _edTlmPduCount += (uint32_t)(((uint8_t)advertData.at(j+1) & 0xff) << 16);
                        _edTlmPduCount += (uint32_t)(((uint8_t)advertData.at(j) & 0xff) << 24);
                        j += 4;

                        _edTlmTimeSinceReboot  = 0;
                        _edTlmTimeSinceReboot  = (uint32_t)((uint8_t)advertData.at(j+3) & 0xff);
                        _edTlmTimeSinceReboot += (uint32_t)(((uint8_t)advertData.at(j+2) & 0xff) << 8);
                        _edTlmTimeSinceReboot += (uint32_t)(((uint8_t)advertData.at(j+1) & 0xff) << 16);
                        _edTlmTimeSinceReboot += (uint32_t)(((uint8_t)advertData.at(j) & 0xff) << 24);
                        return true;
                        break;
                    default:
                        qDebug() << "BBBB Eddystone Type " << (uint8_t)advertData.at(j) << endl;
                        break;
                }
            }
		}
		i += (entryLen+1);
	} while (i < advertData.length());

	return true;
}

QByteArray & BtleAdvertData::beaconUuid()
{
	return _beaconUuid;
}

QString BtleAdvertData::beaconUuidAsString()
{
	QString uuid("");

	QByteArray estimote = QByteArray::fromHex("b9407f30f5f8466eaff925556b57fe6d");

	if (_beaconUuid == estimote) {
		return "Default Estimote UUID";
	}

	for (int i = 0; i < _beaconUuid.length(); i++) {
	    uuid.append(QString("%1").arg((uint8_t)_beaconUuid.at(i), 0, 16));
	}
	return uuid;
}

int BtleAdvertData::beaconMajor()
{
	return (int)_beaconMajor;
}

int BtleAdvertData::beaconMinor()
{
	return (int)_beaconMinor;
}

int BtleAdvertData::calibratedStrength()
{
	return (int)_calibratedStrength;
}

bool BtleAdvertData::hasBeaconData()
{
	return hasIBeaconData() || hasAltBeaconData() || hasEddystoneBeaconData();
}

bool BtleAdvertData::hasIBeaconData()
{
    return _hasIBeaconData;
}

void BtleAdvertData::clear()
{
	_advertData.clear();

	_beaconUuid.clear();
	_beaconMajor = 0;
	_beaconMinor = 0;
	_calibratedStrength = 0;
	_hasIBeaconData = false;

    _hasAltBeaconData = false;
    _beaconMfgCompanyCode = 0;
    _beaconId.clear();
    _beaconReserved = 0;

    _edTxPower = 0;
    _hasEddystoneBeaconUIDdata = false;
    _edNamespace.clear();
    _edInstanceId.clear();
    _hasEddystoneBeaconURLdata = false;
    _edUrlScheme = 0;
    _edUrlBody.clear();
    _hasEddystoneBeaconTLMdata = false;
    _edTlmVersion = 0;
    _edTlmBatteryVoltage = 0;
    _edTlmBeaconTemp = 0;
    _edTlmPduCount = 0;
    _edTlmTimeSinceReboot = 0;
}

QByteArray & BtleAdvertData::beaconId()
{
    return _beaconId;
}

bool BtleAdvertData::hasAltBeaconData()
{
    return _hasAltBeaconData;
}

bool BtleAdvertData::hasEddystoneBeaconData()
{
    return hasEddystoneBeaconUIDdata() || hasEddystoneBeaconURLdata() || hasEddystoneBeaconTLMdata();
}

bool BtleAdvertData::hasEddystoneBeaconUIDdata()
{
    return _hasEddystoneBeaconUIDdata;
}

bool BtleAdvertData::hasEddystoneBeaconURLdata()
{
    return _hasEddystoneBeaconURLdata;
}

bool BtleAdvertData::hasEddystoneBeaconTLMdata()
{
    return _hasEddystoneBeaconTLMdata;
}

int BtleAdvertData::companyCode()
{
    return (int)_beaconMfgCompanyCode;
}

int BtleAdvertData::altBeaconReserved()
{
    return (int)_beaconReserved;
}

QString BtleAdvertData::beaconIdAsString()
{
    QString id("");
    for (int i = 0; i < _beaconId.length(); i++) {
        id.append(QString("%1").arg((uint8_t)_beaconId.at(i), 0, 16));
    }
    return id;
}

QString BtleAdvertData::companyCodeAsString()
{
    QString company = "";

    switch (companyCode()) {
        case 60:
            company = "BlackBerry Ltd.";
            break;
        case 76:
            company = "Apple Inc.";
            break;
        default:
            break;
    }

    return company;
}

int BtleAdvertData::edTxPower()
{
    return (int)_edTxPower;
}

QByteArray & BtleAdvertData::edNamespace()
{
    return _edNamespace;
}

QString BtleAdvertData::edNamespaceAsString()
{
    QString result("");

    for (int i = 0; i < _edNamespace.length(); i++) {
        result.append(QString("%1").arg((uint8_t)_edNamespace.at(i), 0, 16));
    }
    return result;
}

QByteArray & BtleAdvertData::edInstanceId()
{
    return _edInstanceId;
}

QString BtleAdvertData::edInstanceIdAsString()
{
    QString result("");

    for (int i = 0; i < _edInstanceId.length(); i++) {
        result.append(QString("%1").arg((uint8_t)_edInstanceId.at(i), 0, 16));
    }
    return result;
}

QString BtleAdvertData::edUrlAsString()
{
    QString result("");

    switch (_edUrlScheme) {
        case 0x00:
            result.append("http://www.");
            break;
        case 0x01:
            result.append("https://www.");
            break;
        case 0x02:
            result.append("http://");
            break;
        case 0x03:
            result.append("https://");
            break;
        default:
            qDebug() << "BBBB Invalid Url Scheme Byte " << _edUrlScheme << endl;
            break;
    }

    for (int i = 0; i < _edUrlBody.length(); i++) {
        if (((uint8_t)_edUrlBody.at(i)) > (uint8_t)0x0d) {
            result.append(_edUrlBody.at(i));
        } else {
            switch ((uint8_t)_edUrlBody.at(i)) {
                case 0x00:
                    result.append(".com/");
                    break;
                case 0x01:
                    result.append(".org/");
                    break;
                case 0x02:
                    result.append(".edu/");
                    break;
                case 0x03:
                    result.append(".net/");
                    break;
                case 0x04:
                    result.append(".info/");
                    break;
                case 0x05:
                    result.append(".biz/");
                    break;
                case 0x06:
                    result.append(".gov/");
                    break;
                case 0x07:
                    result.append(".com");
                    break;
                case 0x08:
                    result.append(".org");
                    break;
                case 0x09:
                    result.append(".edu");
                    break;
                case 0x0a:
                    result.append(".net");
                    break;
                case 0x0b:
                    result.append(".info");
                    break;
                case 0x0c:
                    result.append(".biz");
                    break;
                case 0x0d:
                    result.append(".gov");
                    break;
                default:
                    qDebug() << "BBBB Invalid Url Sufix Byte " << _edUrlScheme << endl;
                    break;
            }
        }
    }

    return result;
}

int BtleAdvertData::edTlmVersion()
{
    return (int)_edTlmVersion;
}

int BtleAdvertData::edTlmBatteryVoltage()
{
    return (int)_edTlmBatteryVoltage;
}

float BtleAdvertData::edTlmBatteryVoltageAsFloat()
{
    return (float)(edTlmBatteryVoltage()/1000.0);
}

int BtleAdvertData::edTlmBeaconTemp()
{
    return (int)_edTlmBeaconTemp;
}

float BtleAdvertData::edTlmBeaconTempAsFloat()
{
    return ((float)((int)_edTlmBeaconTemp)/256.0);
}

int BtleAdvertData::edTlmPduCount()
{
    return (int)_edTlmPduCount;
}

int BtleAdvertData::edTlmTimeSinceReboot()
{
    return (int)_edTlmTimeSinceReboot;
}
